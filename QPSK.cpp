#include <bits/stdc++.h>
#include "definition.hpp"
#include "filter.hpp"
#include "parameterized_filter.hpp"
#include "random.hpp"
#include "QPSK.hpp"
#include "multipath.hpp"
#include "util.hpp"
#define forAll(x) x.begin(), x.end()

const int packetSize = 1'000;
const int serialPacketCnt = 2'000;
const int theradCnt = 8;
const int packetCnt = serialPacketCnt * theradCnt;
const Real EbN0_init = 15;
const Real EbN0_end  = 0;
const Real EbN0_step = 0.1;

int main(){
	// define carrier channel functions
	auto modulation = Filter{toQPSK_impl};
	auto demodulation = Filter{fromQPSK_impl};
	auto show = Filter{showPacket_impl};
	auto multipath_enc = ParameterizedFilter{multipath_enc_impl};
	auto multipath_dec = ParameterizedFilter{multipath_dec_impl};
	auto AWGN = ParameterizedFilter{AWGN_impl};
	auto dump = ParameterizedFilter{dump_impl};

	// try various communication strength
	for(Real Eb_N0 = EbN0_init; Eb_N0 >= EbN0_end; Eb_N0 -= EbN0_step){
		// debug output
		if(int(Eb_N0 + EbN0_step) != int(Eb_N0))
			std::cerr << std::endl << "trying with Eb/N0 = " << Eb_N0 << " : " << std::flush;
		else
			std::cerr << "." << std::flush;

		std::vector<Real> berLog, berLogMp;
		berLog   .resize(packetCnt);
		berLogMp.resize(packetCnt);
		std::atomic_ulong globalPacketId = 0;

		for(int serialTryRemain = serialPacketCnt; serialTryRemain --> 0;){
			std::vector<std::thread> ths;
			for(int tc = theradCnt; tc --> 0;){
				ths.emplace_back([&]{
					int packetId = globalPacketId.fetch_add(1);
					bool initialEb     = (Eb_N0 == EbN0_init);
					bool initialPacket = (Eb_N0 == EbN0_init && packetId == 0);
					// define multipath propagation
					const int channelCnt = 8;
					auto A     = generateN(normalRandomReal,  channelCnt, 1./8);
					auto phi   = generateN(normalRandomReal,  channelCnt, 1./8);
					auto theta = generateN(uniformRandom, channelCnt);
					Real f_D_per_bps = 0;

					// generate packet
					IntPacket originalPacket = genPacket(packetSize);

					// communicate
					auto AWGN_recv =
						originalPacket
							| modulation 	| dump("AWGN Original",  initialEb)
							| AWGN(Eb_N0)	| dump("AWGN with noise",initialEb)
							| demodulation
							// | show
						;
					auto MP_AWGN_recv =
						originalPacket
							| modulation										| dump("MP Original", initialPacket)
							| multipath_enc(channelCnt,f_D_per_bps,A,phi,theta) | dump("MP Multipath",initialPacket)
							| AWGN(Eb_N0)										| dump("MP AWGN",     initialPacket)
							| multipath_dec(channelCnt,f_D_per_bps,A,phi,theta)
							| demodulation
							// | show
						;

					// calc error rate
					Real AWGN_errorRate    = errorRateOf(   AWGN_recv, originalPacket);
					Real MP_AWGN_errorRate = errorRateOf(MP_AWGN_recv, originalPacket);

					berLog  .at(packetId) =    AWGN_errorRate;
					berLogMp.at(packetId) = MP_AWGN_errorRate;
				});
			}
			for(auto&& th : ths)
				th.join();
		}
		// output
		std::sort(forAll(berLog));
		std::sort(forAll(berLogMp));
		plotPoint({Eb_N0, std::accumulate(forAll(berLog  ), Real{})/packetCnt}, {"E_b / N_0", "ErrorRate"}, "AWGN errorRate");
		plotPoint({Eb_N0, std::accumulate(forAll(berLogMp), Real{})/packetCnt}, {"E_b / N_0", "ErrorRate"}, "MultiPath AWGN errorRate");
	}
	std::cerr << std::endl;
	make_plotAll();
}

