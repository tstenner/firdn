#pragma once
/*

Copyright (c) 2019, Tristan Stenner (LGPL 3.0)

This code is based on the MIT licensed upfirdn polyphase FIR resampling package
(https://sourceforge.net/motorola/upfirdn)
Copyright (c) 2009, Motorola, Inc

All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

* Neither the name of Motorola nor the names of its contributors may be
used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace firdn {

using uint = uint_fast32_t;

/* By default, passed pointers may overlap so some optimizations
(especially for code like *out = *in * *coef
aren't possible. __restrict__ is a non-standard extension that promises the
compiler our arrays don't overlap*/
#define RESTRICT __restrict

template <class In, class Out, class Coef> class Downsampler {
public:
	using CoefIter = Coef const *RESTRICT;

	/* The coefficients are copied into local storage in a transposed, flipped
			arrangement. Since no upsampling is done, this is simply the reversed
			coefficient vector */
	Downsampler(uint downRate, uint nChan, Coef *coefs, uint coefCount)
		: _downRate(downRate), _nChan(nChan),
		  _coefs(std::make_reverse_iterator(coefs + coefCount), std::make_reverse_iterator(coefs)),
		  _state(nChan * (coefCount - 1), 0.) {}
	~Downsampler() noexcept = default;
	Downsampler(const Downsampler &) = default;
	Downsampler(Downsampler &&) = default;

	void operator()(In const *in, uint inCount, std::vector<Out> &out);
	// number of output samples produced for given input
	std::size_t nCoefs() const { return _coefs.size(); }
	void resetState() { std::fill(_state.begin(), _state.end(), 0.); }

private:
	const uint _downRate, _nChan;
	const std::vector<Coef> _coefs; // reorganized coefficient vector
	std::vector<In> _state;			// state vector (in case past data is needed for next sample)

	uint leftoverSamples{0}; // how many samples were left over from the last downsampling pass

	/** @brief Fused Multiply Add
	 * For each channel, calculate the sum of each sample with the
	 * corresponding coefficient.
	 * @param[in] in Pointer to the input data
	 * @param samplecount Number of samples, samplecount*nchan gives the length of the input array
	 * @param[in] h Pointer to coefficient vector
	 * @param[out] out Point to already allocated output array
	 * @return pointer to current position in the coefficient vector
	 */
	inline CoefIter fma(
		In const *RESTRICT in, uint samplecount, CoefIter h, Out *RESTRICT out) const {
		for (uint i = 0; i < samplecount; ++i) {
			const auto coef = *h;
			for (uint chan = 0; chan < _nChan; ++chan) { *(out + chan) += coef * *in++; }
			++h;
		}
		return h;
	}
};

template <class In = double, class Coef>
auto makeDownsampler(uint downRate, uint nChan, Coef const *coef, uint coefCount)
	-> Downsampler<In, decltype(In() * Coef()), Coef> {
	return Downsampler<In, decltype(In() * Coef()), Coef>(downRate, nChan, coef, coefCount);
}

template <class In, class Out, class Coef>
void Downsampler<In, Out, Coef>::operator()(In const *in, uint inSamples, std::vector<Out> &out) {
	const uint outSamples = (inSamples + leftoverSamples) / _downRate;
	if (inSamples < nCoefs())
		throw std::invalid_argument("Need more input samples than filter coefficients");
	auto *x = in - (leftoverSamples + nCoefs() - _downRate) * _nChan;
	auto oldOutSize = out.size();
	out.resize(out.size() + outSamples * _nChan, Out());
	auto y = out.begin() + oldOutSize;

	for (uint s = 0; s < outSamples; ++s) {
		CoefIter h = _coefs.data();

		const auto samp_from_state = std::max<int>((in - x) / (int)_nChan, 0);
		h = fma(&*_state.end() - samp_from_state * _nChan, samp_from_state, h, &*y);
		const int samp_from_x = nCoefs() - samp_from_state;
		fma(x + _nChan * samp_from_state, samp_from_x, h, &*y);
		y += _nChan;
		x += _downRate * _nChan;
	}
	leftoverSamples = (inSamples + leftoverSamples) % _downRate;

	// manage _state buffer
	auto *end = in + inSamples * _nChan;
	std::copy(end - _state.size(), end, _state.begin());
}
} // namespace firdn
#undef RESTRICT
