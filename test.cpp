#include "firdn.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void resample_and_print(
	int *data, unsigned int downrate, unsigned int nsample, unsigned int nchan) {
	int coef1[] = {1, 1, 0};
	auto r(firdn::makeDownsampler<int>(downrate, nchan, coef1, 3));
	// Downsampler<int, double, int> r(2, nchan, coef1, 3);
	const auto nout = nsample * nchan / downrate;
	std::vector<int> out, out2;
	r(data, nsample, out);
	for (auto i = 0u; i < out.size(); i++)
		std::cout << out[i] << ((i + 1) % nchan != 0 ? '\t' : '\n');
	r.resetState();

	r(data, nsample / downrate, out2);
	r(data + nout, nsample / downrate, out2);
	std::cout << '\n';
	for (auto i = 0u; i < nout; ++i) std::cout << out2[i] << ((i + 1) % nchan ? '\t' : '\n');
	assert(out == out2);
}

int main(int argc, char **argv) {
	// clang-format off
	int data1[] = {
		1, 13, 25,
		1, 13, 25,
		2, 14, 26,
		2, 14, 26,
		3, 15, 27,
		3, 15, 27
	};
	int data2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
	// clang-format on
	resample_and_print(data1, 2, 6, 3);
	std::cout << "\nData2\n";
	resample_and_print(data2, 2, 12, 1);
	std::cout << std::endl;
}
