# firdn

An (almost*) header only downsampling library in plain C++.
It can downsample multiplexed data of any type (that implements
`operator*`) after filtering it with a FIR filter of your choice.

After constructing the downsampler, no further allocations and almost
no copies of your data are made. Most compilers should be smart enough to use
SSE/AVX intrinsics.

# Limitations

No C++ code to construct a lowpass FIR filter is included, but the included
Python script can create and export a filter of your choice.

Downsampling is only possible in integer ratios, e.g. 100->50Hz, but not
100->75Hz.

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
