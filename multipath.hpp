#pragma once
#include <bits/stdc++.h>
#include "definition.hpp"
#include "random.hpp"

auto calc_h = [](
		int channelCnt, Real T_s, Real f_D_per_bps,
		RealPacket A, RealPacket phi, RealPacket theta
) -> Complex {
	Complex res = 0;
	for(int i=0; i<channelCnt; ++i){
		Real ar = 2*M_PI*f_D_per_bps*std::cos(theta.at(i))*Real(i+1)*T_s + phi.at(i);
		res += A.at(i) * Complex(std::cos(ar), std::sin(ar));
	}
	return res;
};

auto multipath_enc_impl = [](
		const ComplexPacket& input, int channelCnt, Real f_D_per_bps,
		RealPacket A, RealPacket phi, RealPacket theta
) -> ComplexPacket {
	ComplexPacket res;
	assert(A.size() == phi.size() && phi.size() == theta.size());
	res.reserve(input.size());
	for(int i=0; i<input.size(); ++i){
		Complex h = calc_h(channelCnt, i, f_D_per_bps, A, phi, theta);
		res.push_back(input.at(i) * h);
	}
	return res;
};

auto multipath_dec_impl = [](
		const ComplexPacket& input, int channelCnt, Real f_D_per_bps,
		RealPacket A, RealPacket phi, RealPacket theta
) -> ComplexPacket {
	ComplexPacket res;
	assert(A.size() == phi.size() && phi.size() == theta.size());
	res.reserve(input.size());
	for(int i=0; i<input.size(); ++i){
		Complex h = calc_h(channelCnt, i, f_D_per_bps, A, phi, theta);
		res.push_back(input.at(i) / h);
	}
	return res;
};
