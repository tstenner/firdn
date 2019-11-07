# firdn

An (almost*) header only downsampling library in plain C++.
It can downsample multiplexed data of any type (that implements
`operator*`) after filtering it with a FIR filter of your choice.

After constructing the downsampler, no further allocations and almost
no copies of your data are made. Most compilers should be smart enough to use
SSE/AVX intrinsics.

# Example code

``` cpp
int data1[] = {
	1, 13, 24,
	1, 13, 26,
	2, 14, 28,
	2, 14, 30,
	3, 15, 32,
	3, 15, 34
};
int coef1[] = {1, 1, 0};
auto downrate = 2u, nchan = 3u, ncoef = 3u;
auto r(firdn::makeDownsampler<int>(downrate, nchan, coef1, ncoef));
std::vector<int> out;
// Downsample first half, one sample is left over
r(data, nsample / downrate, out);
// Downsample second half, continueing with the left over sample and appending
// to the `out` vector
r(data + nsample * nchan / downrate, nsample / downrate, out2);
```

# Limitations

No C++ code to construct a lowpass FIR filter is included, but the included
Python script can create and export a filter of your choice.

Downsampling is only possible in integer ratios, e.g. 100->50Hz, but not
100->75Hz. PRs to implement upsampling are welcome.

Currently, the input buffer needs to have at least as many samples as
filter has coefficients. This requirement will be removed in a later release.

# License

This code is based on the upfirdn polyphase FIR resampling package
(https://sourceforge.net/motorola/upfirdn), (c) 2009, Motorola, Inc.

Everything else is licensed under the LGPL 3.0.

In practice, this means that if you use the library you have to mention it
(e.g. in the about dialog or somewhere buried in a list of projects you're
using) and you can sell your program and keep its source code private.

Also, you have provide only the `firdn.hpp` header file to anyone you
distributed your compiled program to if they're asking politely for it.

For more information, see the [eigen licensing FAQ](http://eigen.tuxfamily.org/index.php?title=Licensing_FAQ#So_what_does_the_LGPL_require_me_to_do.3F).
