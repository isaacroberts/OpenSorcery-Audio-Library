// ==================================================================================
// Copyright (c) 2017 HiFi-LoFi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ==================================================================================

#include "MobileConvolver.h"

#include <cassert>
#include <cmath>

#if defined (FFTCONVOLVER_USE_SSE)
#include <xmmintrin.h>
#endif


#include <iostream>


namespace fftconvolver
{  
	
MobileConvolver::MobileConvolver() :
_blockSize(0),
_segSize(0),
_segCount(0),
_fftComplexSize(0),
_segments(),
_segmentsIR(),
_fftBuffer(),
_fft(),
_preMultiplied(),
_conv(),
_overlap(),
_current(0),
_inputBuffer(),
_inputBufferFill(0),
shouldExit(false)
{
}


MobileConvolver::~MobileConvolver()
{
	reset();
}


void MobileConvolver::reset()
{
	for (size_t i=0; i<_segCount; ++i)
	{
		delete _segments[i];
		delete _segmentsIR[i];
		delete _swapSegmentsIR[i];
	}
	_blockSize = 0;
	_segSize = 0;
	_segCount = 0;
	_fftComplexSize = 0;
	_segments.clear();
	_segmentsIR.clear();
	_fftBuffer.clear();
	_fft.init(0);
	_preMultiplied.clear();
	_conv.clear();
	_overlap.clear();
	_current = 0;
	_inputBuffer.clear();
	_inputBufferFill = 0;
	
	//	shouldExit=false;
}

void MobileConvolver::clearBuffer()
{
	//	_fftBuffer.setZero();
	_preMultiplied.setZero();
	_conv.setZero();
	_overlap.setZero();
	_current = 0;
	_inputBuffer.setZero();
	_inputBufferFill = 0;
	
	for (size_t i=0; i<_segCount; ++i)
	{
		_segments[i]->setZero();
	}
}

	
bool MobileConvolver::init(size_t blockSize, const Sample* ir, size_t irLen)
{
	reset();
	
	if (blockSize == 0)
	{
		return false;
	}
	this->irLen = irLen;
	
	_blockSize = NextPowerOf2(blockSize);
	_segSize = 2 * _blockSize;
	_segCount = static_cast<size_t>(::ceil(static_cast<float>(irLen) / static_cast<float>(_blockSize)));
	_fftComplexSize = audiofft::AudioFFT::ComplexSize(_segSize);
	
	// FFT
	_fft.init(_segSize);
	_fftBuffer.resize(_segSize);
	
	// Prepare segments
	for (size_t i=0; i<_segCount; ++i)
	{
		_segments.push_back(new SplitComplex(_fftComplexSize));
	}
	
	// Prepare IR
	for (size_t i=0; i<_segCount; ++i)
	{
		SplitComplex* segment = new SplitComplex(_fftComplexSize);
		const size_t remaining = irLen - (i * _blockSize);
		const size_t sizeCopy = (remaining >= _blockSize) ? _blockSize : remaining;
		CopyAndPad(_fftBuffer, &ir[i*_blockSize], sizeCopy);
		_fft.fft(_fftBuffer.data(), segment->re(), segment->im());
		_segmentsIR.push_back(segment);
	}
	// Prepare scratch space
	for (size_t i=0; i<_segCount; ++i)
	{
		SplitComplex* segment = new SplitComplex(_fftComplexSize);
		_swapSegmentsIR.push_back(segment);
	}
	
	// Prepare convolution buffers
	_preMultiplied.resize(_fftComplexSize);
	_conv.resize(_fftComplexSize);
	_overlap.resize(_blockSize);
	
	// Prepare input buffer
	_inputBuffer.resize(_blockSize);
	_inputBufferFill = 0;
	
	// Reset current position
	_current = 0;
	
	return true;
}
	
	
	
bool MobileConvolver::init(size_t blockSize, size_t irLen)
{
	reset();
	
	if (blockSize == 0)
	{
		return false;
	}
	this->irLen = irLen;
	
	_blockSize = NextPowerOf2(blockSize);
	_segSize = 2 * _blockSize;
	_segCount = static_cast<size_t>(::ceil(static_cast<float>(irLen) / static_cast<float>(_blockSize)));
	_fftComplexSize = audiofft::AudioFFT::ComplexSize(_segSize);
	
	// FFT
	_fft.init(_segSize);
	_fftBuffer.resize(_segSize);
	
	// Prepare segments
	for (size_t i=0; i<_segCount; ++i)
	{
		_segments.push_back(new SplitComplex(_fftComplexSize));
	}
	
	// Prepare IR
	for (size_t i=0; i<_segCount; ++i)
	{
		SplitComplex* segment = new SplitComplex(_fftComplexSize);
		
		_fftBuffer.setZero();
		if (i==0)
			_fftBuffer[0] = 1;
	
		_fft.fft(_fftBuffer.data(), segment->re(), segment->im());
		_segmentsIR.push_back(segment);
	}
	// Prepare scratch space
	for (size_t i=0; i<_segCount; ++i)
	{
		SplitComplex* segment = new SplitComplex(_fftComplexSize);
		_swapSegmentsIR.push_back(segment);
	}
	
	// Prepare convolution buffers
	_preMultiplied.resize(_fftComplexSize);
	_conv.resize(_fftComplexSize);
	_overlap.resize(_blockSize);
	
	// Prepare input buffer
	_inputBuffer.resize(_blockSize);
	_inputBufferFill = 0;
	
	// Reset current position
	_current = 0;
	
	return true;
}
	
	
bool MobileConvolver::setIR(const Sample* ir, size_t irLen, bool waitIfLocked)
{
	if (irLen > this->irLen)
	{
		jassertfalse;
		return false;
	}
	jassert(irLen == this->irLen);
	
	
	if (needsSwap)
		return false;
	
	if (waitIfLocked)
	{
		swapLock.lock();
	}
	else
	{
		if (!swapLock.try_lock())
			return false;
	}

	if (needsSwap)
	{
		swapLock.unlock();
		return false;
	}
	
	// Prepare IR
	for (size_t i=0; i<_segCount; ++i)
	{
		SplitComplex* segment = _swapSegmentsIR[i];
		
		const size_t remaining = irLen - (i * _blockSize);
		const size_t sizeCopy = (remaining >= _blockSize) ? _blockSize : remaining;
		jassert(i*_blockSize+ sizeCopy <= irLen);
		
		//The error is on this line - when re-copying the data the FFT passes the bounds check quicker
		CopyAndPad(_fftBuffer, &ir[i*_blockSize], sizeCopy);
		jassert(segment->size()==_fftComplexSize);
		_fft.fft(_fftBuffer.data(), segment->re(), segment->im());
		float reMax=0;
		for (int n=0; n < _fftComplexSize; ++n)
		{
			reMax = std::max(reMax, std::abs(segment->re()[n]));
			reMax = std::max(reMax, std::abs(segment->im()[n]));
		}
		int tries = 1;
		while (reMax > 2.f && tries < 5)
		{
			CopyAndPad(_fftBuffer, &ir[i*_blockSize], sizeCopy);
			_fft.fft(_fftBuffer.data(), segment->re(), segment->im());
			float prev = reMax;
			reMax=0;
			for (int n=0; n < _fftComplexSize; ++n)
			{
				reMax = std::max(reMax, std::abs(segment->re()[n]));
				reMax = std::max(reMax, std::abs(segment->im()[n]));
			}
#if DEBUG
			std::cout<<"Max after reload: ("<<tries<<") "<<prev<<"\t->\t"<<reMax<<std::endl;
#endif 
			tries++;
		}
	}
	
	needsSwap=true;
	swapLock.unlock();
	
	return true;
}
	
void MobileConvolver::process(const Sample* input, Sample* output, size_t len)
{
	if (_segCount == 0)
	{
		::memset(output, 0, len * sizeof(Sample));
		return;
	}
	
	if (needsSwap)
	{
		if (swapLock.try_lock())
		{
			swap(_segmentsIR, _swapSegmentsIR);
			_current=0;
			for (int n=0; n <_segCount; ++n)
			{
				_segments[n]->setZero();
			}
			needsSwap=false;
			swapLock.unlock();

			_fftBuffer.setZero();
			_preMultiplied.setZero();
			_conv.setZero();
			_overlap.setZero();
			_inputBuffer.setZero();
		}
	}
	
	size_t processed = 0;
	while (processed < len && !shouldExit)
	{
		const bool inputBufferWasEmpty = (_inputBufferFill == 0);
		const size_t processing = std::min(len-processed, _blockSize-_inputBufferFill);
		const size_t inputBufferPos = _inputBufferFill;
		::memcpy(_inputBuffer.data()+inputBufferPos, input+processed, processing * sizeof(Sample));
		
		// Forward FFT
		CopyAndPad(_fftBuffer, &_inputBuffer[0], _blockSize);
		_fft.fft(_fftBuffer.data(), _segments[_current]->re(), _segments[_current]->im());
		
		// Complex multiplication
		if (inputBufferWasEmpty)
		{
			_preMultiplied.setZero();
			for (size_t i=1; i<_segCount; ++i)
			{
				const size_t indexIr = i;
				const size_t indexAudio = (_current + i) % _segCount;
				ComplexMultiplyAccumulate(_preMultiplied, *_segmentsIR[indexIr], *_segments[indexAudio]);
			}
		}
		_conv.copyFrom(_preMultiplied);
		ComplexMultiplyAccumulate(_conv, *_segments[_current], *_segmentsIR[0]);
		
		// Backward FFT
		_fft.ifft(_fftBuffer.data(), _conv.re(), _conv.im());
		
		// Add overlap
		Sum(output+processed, _fftBuffer.data()+inputBufferPos, _overlap.data()+inputBufferPos, processing);
		
		// Input buffer full => Next block
		_inputBufferFill += processing;
		if (_inputBufferFill == _blockSize)
		{
			// Input buffer is empty again now
			_inputBuffer.setZero();
			_inputBufferFill = 0;
			
			// Save the overlap
			::memcpy(_overlap.data(), _fftBuffer.data()+_blockSize, _blockSize * sizeof(Sample));
			
			// Update current segment
			_current = (_current > 0) ? (_current - 1) : (_segCount - 1);
		}
		
		processed += processing;
	}
	shouldExit=false;
}
	
} // End of namespace fftconvolver
