//
//  BufferManager.cpp
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/3/20.
//

// Created by Isaac Roberts


void Utility::copy(juce::dsp::AudioBlock<float>& to, InputBlock &from)
{
	to.copyFrom(from);
}
void Utility::copy(AudioBuffer<float>& to, const AudioBuffer<float>& from)
{
	if (to.getNumSamples() < from.getNumSamples())
 		to.clear();
 	int sampleCt = (int)std::min(from.getNumSamples(), to.getNumSamples());
 	if (from.getNumChannels() == 0)
 		return;
 	else if (from.getNumChannels() == 1)
 	{
 		for (int c=0; c< to.getNumChannels(); ++c)
 			to.copyFrom(c, 0, from, 0, 0, sampleCt);
 	}
 	else if (from.getNumChannels() >= to.getNumChannels())
 	{
 		for (int c=0; c< to.getNumChannels(); ++c)
 			to.copyFrom(c, 0, from, c, 0, sampleCt);
 	}
 	else if (from.getNumChannels() < to.getNumChannels())
 	{
 		for (int c=0; c< from.getNumChannels(); ++c)
 			to.copyFrom(c, 0, from, c, 0, sampleCt);
 		for (int c=from.getNumChannels(); c < to.getNumChannels(); ++c)
 		{
 			//Manual clear
 			for (int n=0; n < to.getNumSamples(); ++n)
 			{
 				to.setSample(c, n, 0);
 			}
 		}
 	}
}

void Utility::add(ABlock &a, const ABlock &b)
{
	a.add(b);
//	int sampleCt = min(a.getNumSamples(), b.getNumSamples());
//
//	if (b.getNumChannels() == 1)
//	{
//		a.applyGain(mixA);
//		for (int c=0;c<a.getNumChannels(); c++)
//		{
//			a.addFrom(c, 0, b, 0, 0, sampleCt);
//		}
//	}
//	else
//	{
//		int channels = min(a.getNumChannels(), b.getNumChannels());
//		for (int c=0;c<channels; c++)
//		{
//			a.addFrom(c, 0, b, c, 0, sampleCt);
//		}
//	}
}

void Utility::add(ABlock &a, const ABlock &b, float mixA, float mixB)
{
	a.multiplyBy(mixA);
	a.addProductOf(b, mixB);
//	int sampleCt = std::min(a.getNumSamples(), b.getNumSamples());
//
//	if (b.getNumChannels() == 1)
//	{
//		a.applyGain(mixA);
//		for (int c=0;c<a.getNumChannels(); c++)
//		{
//			a.addFrom(c, 0, b, 0, 0, sampleCt, mixB);
//		}
//	}
//	else// if (b.getNumChannels() >= a.getNumChannels())
//	{
//		int channels = std::min(a.getNumChannels(), b.getNumChannels());
//		a.applyGain(mixA);
//		for (int c=0;c<channels; c++)
//		{
//			a.addFrom(c, 0, b, c, 0, sampleCt, mixB);
//		}
//	}
}
void Utility::addProduct(ABlock& a, const ABlock& b, float product)
{
	a.addProductOf(b, product);
//	int sampleCt = std::min(a.getNumSamples(), b.getNumSamples());
//
//	if (b.getNumChannels() == 1)
//	{
//		a.applyGain(mixA);
//		for (int c=0;c<a.getNumChannels(); c++)
//		{
//			a.addFrom(c, 0, b, 0, 0, sampleCt, mixB);
//		}
//	}
//	else// if (b.getNumChannels() >= a.getNumChannels())
//	{
//		int channels = std::min(a.getNumChannels(), b.getNumChannels());
//		a.applyGain(mixA);
//		for (int c=0;c<channels; c++)
//		{
//			a.addFrom(c, 0, b, c, 0, sampleCt, mixB);
//		}
//	}
}

float Utility::getBlockMax(InputBlock& block)
{
	float max = 0;
	for (int c=0; c < block.getNumChannels(); ++c)
	{
		Range<float> r = FloatVectorOperations::findMinAndMax(block[c], (int)block.getNumSamples());
		max = jmax(fabs(r.getStart()), max, fabs(r.getEnd()));
	}
	return max;
}


void Utility::sanitize(AudioBuffer<float>& buffer)
{
	for (int c=0; c < buffer.getNumChannels(); ++c)
	{
		auto d = buffer.getWritePointer(c);
		
		for (int n=0; n < buffer.getNumSamples(); ++n)
		{
			if (std::isinf(d[n]))
			{
				d[n] = d[n]>0 ? 1 : -1;
			}
			else if (std::isnan(d[n]))
			{
				d[n]=0;
			}
		}
	}
}
void Utility::normalizeRMS(AudioBuffer<float>& buffer, float targetLevel)
{
	float rms=0;
	for (int c=0; c < buffer.getNumChannels(); ++c)
	{
		rms += buffer.getRMSLevel(c, 0, buffer.getNumSamples());
	}
	rms /= buffer.getNumChannels();
	if (!std::isfinite(rms))
	{
		sanitize(buffer);
		//Recalculate
		rms=0;
		for (int c=0; c < buffer.getNumChannels(); ++c)
		{
			rms += buffer.getRMSLevel(c, 0, buffer.getNumSamples());
		}
		rms /= buffer.getNumChannels();
	}
	if (rms > 0)
		buffer.applyGain(0, buffer.getNumSamples(), targetLevel/rms);
}
void Utility::normalizePeak(AudioBuffer<float>& buffer, float targetLevel)
{
	float mag = buffer.getMagnitude(0, buffer.getNumSamples());
	if (!std::isfinite(mag))
	{
		sanitize(buffer);
		//Recalculate
		mag = buffer.getMagnitude(0, buffer.getNumSamples());
	}
	if (mag > 0)
		buffer.applyGain(0, buffer.getNumSamples(), targetLevel/mag);
}
