#pragma once
#include <bits/stdc++.h>
#include "definition.hpp"

auto toQPSK_impl = [](const IntPacket& input) -> ComplexPacket {
	constexpr long double invsqrt2 = 1/1.4142135623730950488;
	assert(input.size() % 2 == 0); //input size must be twice of integer
	ComplexPacket res;
	res.reserve(input.size());
	for(size_t i=0; i<input.size(); i+=2)
		res.emplace_back(
			(input.at(i)*2-1)*invsqrt2,
			(input.at(i+1)*2-1)*invsqrt2
		);
	return res;
};

auto fromQPSK_impl = [](const ComplexPacket& input) -> IntPacket {
	IntPacket res;
	res.reserve(input.size() * 2);
	for(auto&& elem : input){
		res.push_back(elem.real() > 0);
		res.push_back(elem.imag() > 0);
	}
	return res;
};
