#pragma once
#include <bits/stdc++.h>
#include "definition.hpp"

thread_local std::random_device d_rnd;
thread_local std::mt19937 rnd;

// HACK: static initialization
struct _init_rand{_init_rand(){
	rnd.seed(d_rnd());
}};
thread_local _init_rand _init_rand_instance_;

auto uniformRandom = []() -> Real{
	std::uniform_real_distribution<Real> randReal(0, 1);
	return randReal(rnd);
};

auto normalRandom = [](double sigma_sq) -> Complex {
	std::uniform_real_distribution<Real> randReal(0, 1);
	Real u1 = randReal(rnd), u2 = randReal(rnd);
	Real ab = std::sqrt(-sigma_sq * std::log(u1));
	return ab * Complex(cos(2*M_PI*u2), sin(2*M_PI*u2));
};

auto normalRandomReal = [](double sigma_sq) -> Real {
	return normalRandom(sigma_sq).real();
};

auto AWGN_impl = [](const auto& input, double Eb_N0) -> ComplexPacket {
	Real CNR = Eb_N0 + 3;
	Real sigma_sq = std::pow<Real>(10., -CNR/10);
	// std::cerr << "sigma_sq = " << sigma_sq << std::endl;
	ComplexPacket res;
	res.reserve(input.size());
	for(Complex elem : input){
		res.push_back(elem + normalRandom(sigma_sq));
	}
	return res;
};
