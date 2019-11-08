#include "firdn.hpp"
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

std::vector<double> readfile(std::string filename) {
	std::ifstream in(filename, std::ios::binary);
	in.seekg(0, std::ios::seekdir::_S_end);
	const auto filesize = in.tellg();
	in.seekg(0, std::ios::seekdir::_S_beg);
	if(filesize%sizeof(double))
		throw std::runtime_error("invalid file size");
	std::vector<double> res(filesize/8, 0.);
	in.read(reinterpret_cast<char*>(res.data()), filesize);
	return res;
}

int main(int argc, char** argv) {
	if(argc < 5 || argc%2!=1) {
		std::cerr << "Usage: " << argv[0] << " infile nchan "
				  << "decimation_factor filter_file"
				  << "[decimation_factor filter_file]...\n";
		return 1;
	}
	std::vector<double> data{readfile(argv[1])}, buf;
	std::clog << "Read items: " << data.size() << std::endl;
	using Downsampler = firdn::Downsampler<double, double, double>;
	auto nchan = std::stoul(argv[2]);
	std::ofstream out(argv[1]+std::string(".out"));
	for(int i=3; i<argc; ++i) {
		auto downrate = std::stoul(argv[i++]);
		auto coefs = readfile(argv[i]);
		auto downsampler = firdn::makeDownsampler(downrate, nchan, coefs.data(), coefs.size());
		downsampler(data.data(), data.size()/nchan, buf);
		data.swap(buf);
		buf.clear();
	}
	out.write(reinterpret_cast<const char *>(data.data()), data.size()*sizeof(double));
	return 0;
}
