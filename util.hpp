#pragma once
#include <bits/stdc++.h>
#include "definition.hpp"
#include "random.hpp"

auto generateN = [](auto func, int n, auto&&... args) {  // std::vector<T>
	std::vector<decltype(func(args...))> res;
	while(n --> 0)
		res.push_back(func(args...));
	return res;
};

auto showPacket_impl = [](const auto& vec) -> void {
	bool initial = true;
	for(auto&& elem : vec){
		if(initial) initial = false;
		else std::cout << ", ";
		std::cout << elem;
	}
	std::cout << std::endl;
	return vec;
};

auto errorRateOf(const IntPacket& input, const IntPacket& original) -> double{
	assert(input.size() == original.size());
	int errorCnt = 0;
	for(size_t i=0; i<input.size(); ++i)
		if(input.at(i) != original.at(i))
			++errorCnt;
	return errorCnt / double(input.size());
}

auto genPacket = [](int len) -> IntPacket {
	IntPacket res;
	res.reserve(len);
	while(len --> 0)
		res.push_back(rnd() & 1);
	return res;
};

std::map<std::string, std::ofstream> dump_ofs_map;
std::map<std::string, std::pair<std::string, std::string>> label_map;
std::mutex dump_mtx, plot_mtx, pa_mtx;

std::string makeFileName(std::string name){
	std::replace(name.begin(), name.end(), ' ', '_');
	return name;
}

auto dump_impl = [](const ComplexPacket& packet, std::string fileName, bool output = true) -> ComplexPacket {
	std::lock_guard lock{dump_mtx};
	if(!fileName.empty() && output){
		if(!dump_ofs_map.count(fileName))
			dump_ofs_map[fileName].open(makeFileName(fileName)+".txt");
		if(dump_ofs_map.at(fileName).fail())
			throw std::runtime_error("could not open: " + makeFileName(fileName+".txt"));
		for(auto p : packet)
			dump_ofs_map.at(fileName) << p.real() << "  " << p.imag() << std::endl;
	}
	return packet;
};

auto plotPoint = [](std::pair<Real, Real> pos, std::pair<std::string, std::string> label, std::string fileName) -> void {
	std::lock_guard lock{plot_mtx};
	if(!fileName.empty()){
		if(!dump_ofs_map.count(fileName))
			dump_ofs_map[fileName].open(makeFileName(fileName)+".txt");
		if(dump_ofs_map.at(fileName).fail())
			throw std::runtime_error("could not open: " + makeFileName(fileName+".txt"));
		if(!label_map.count(fileName))
			label_map[fileName] = label;
		dump_ofs_map.at(fileName) << pos.first << "  " << pos.second << std::endl;
	}
};

void make_plotAll(){
	using namespace std;
	std::lock_guard lock{pa_mtx};
	std::ofstream pa("plotAll.plot");
	for(auto&& [name, _] : dump_ofs_map){
		bool lmc = label_map.count(name);
		pa << "set terminal png size 1200,960 font \"Arial,10\"\n";
		if(lmc && label_map.at(name).second == "ErrorRate")
			pa << "set logscale y\n";
		else
			pa << "unset logscale y\n";
		pa << "set out \"" + name + ".png\"\n";
		pa << "set title \"" + name + "\"\n";
		pa << "set xlabel \"" + (lmc?label_map.at(name).first :"Real Part"s ) + "\"\n";
		pa << "set ylabel \"" + (lmc?label_map.at(name).second:"Imaginary Part"s ) + "\"\n";
		if(lmc){
			pa << "set xtics auto\n";
			pa << "set ytics auto\n";
			pa << "unset grid\n";
		}else{
			pa << "set xtics sqrt(2)/2\n";
			pa << "set ytics sqrt(2)/2\n";
			pa << "set grid lw 0.5 lc rgb \"blue\"\n";
		}
		pa << "set zeroaxis lt -1 lw 1.5\n";
		pa << "plot \"" + makeFileName(name) + ".txt\" using 1:2 with points title \"\"\n";
		// pa << "plot \"" + name + ".txt\" using 1:2 with points pt 6 title \"\"\n";
		pa << std::endl;
	}
}