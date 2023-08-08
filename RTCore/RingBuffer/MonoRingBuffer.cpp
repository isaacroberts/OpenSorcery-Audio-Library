

MonoRingBuffer::MonoRingBuffer(int size)
: buffer(1, size)
{

}
MonoRingBuffer::MonoRingBuffer()
: buffer(1, 0)
{

}

MonoRingBuffer::~MonoRingBuffer()
{
}

void MonoRingBuffer::setLength(int set)
{
	//Need new class with locks if you want to do this async
	writePtr=0;
	readPtr=0;
	buffer.setSize(1, set, true, true, true);
	size_ = set;
}


float MonoRingBuffer::rms()
{
	//TODO: This was being weird 
	return Util::rmsLevelBuffer(buffer);
}
float MonoRingBuffer::max()
{
	float max = FloatVectorOperations::findMaximum(buffer[0], size());
	return max;
}
float MonoRingBuffer::average()
{
	float sum=0;
	for (int n=0; n < buffer.getNumSamples(); ++n)
	{
		sum += buffer[0][n];
	}
	sum /= size();
	return sum;
}

void MonoRingBuffer::applyRamp(float start, float end, int N)
{
	int w=writePtr;
	
	if (w + N <= size())
	{
		buffer.applyGainRamp(w, N, start, end);
	}
	else
	{
		float midpt = float(size()-w)/N * (end-start) + start;
		buffer.applyGainRamp (w, size()-w, start, midpt);
		buffer.applyGainRamp (0, w, midpt, end);
	}
}

juce::dsp::AudioBlock<float> MonoRingBuffer::getWriteBlock1()
{
	//Advance writePointer by size; no effect
	return juce::dsp::AudioBlock<float> (buffer.getArrayOfWritePointers(), numC(), writePtr, size()-writePtr);
}
juce::dsp::AudioBlock<float> MonoRingBuffer::getWriteBlock2()
{
//	assertWritePointersMatch();
	//Have already advanced writePointer by size
	if (writePtr==0)
	{
		//Return empty
		return juce::dsp::AudioBlock<float>();
	}
	return juce::dsp::AudioBlock<float> (buffer.getArrayOfWritePointers(), numC(), 0, writePtr);
}

juce::dsp::AudioBlock<float> MonoRingBuffer::getWriteBlock1(int start, int len)
{ //Does not advance write pointers
	jassert(len <= size());
	int p1 = start + writePtr;
	p1 = p1 % size();
	int l1 = std::max(len, size()-p1);
	return juce::dsp::AudioBlock<float> (buffer.getArrayOfWritePointers(), numC(), p1, l1);
}
juce::dsp::AudioBlock<float> MonoRingBuffer::getWriteBlock2(int start, int len)
{
	int end = writePtr + start + len;
	if (end <= size())
	{
		//Return empty
		return juce::dsp::AudioBlock<float>();
	}
	else
	{
		end = end % size();
		jassert(end <= writePtr);
		return juce::dsp::AudioBlock<float> (buffer.getArrayOfWritePointers(), numC(), 0, end);
	}
}


/*
	TODO: Rewrite all of these with FloatVectorOperations, since they're much faster 
 */
void MonoRingBuffer::writeReplace(const float* data, int numSamples)
{
	int start = std::min(0, size()-numSamples);
	auto* dest = buffer.getWritePointer(0);
	for (int n=start; n < numSamples; ++n)
	{
		dest[writePtr++] = data[n];
		if (writePtr>=size())
			writePtr=0;
	}
}
//void MonoRingBuffer::writeReplaceFirst(int c, const float* data, int numSamples)
//{
//	int end = std::min(size(), numSamples);
//	auto* dest = buffer.getWritePointer(c);
//	for (int n=0; n < end; ++n)
//	{
//		dest[writePtr[c]++] = data[n];
//		if (writePtr[c]>=size())
//			writePtr[c]=0;
//	}
//}
void MonoRingBuffer::writeAdd(const float* data, int numSamples)
{
	int start = std::min(0, size()-numSamples);
	auto* dest = buffer.getWritePointer(0);
	for (int n=start; n < numSamples; ++n)
	{
		dest[writePtr++] += data[n];
		if (writePtr>=size())
			writePtr=0;
	}
}
//void MonoRingBuffer::writeAddFirst(int c, const float* data, int numSamples)
//{
//	int end = std::min(size(), numSamples);
//	auto* dest = buffer.getWritePointer(c);
//	for (int n=0; n < end; ++n)
//	{
//		dest[writePtr[c]++] += data[n];
//		if (writePtr[c]>=size())
//			writePtr[c]=0;
//	}
//}

void MonoRingBuffer::readReplaceFirst(float* data, int numSamples)
{
	int start = std::min(0, size()-numSamples);
	auto* src = buffer.getWritePointer(0);
	for (int n=start; n < numSamples; ++n)
	{
		data[n] = src[readPtr++];
		if (readPtr>=size())
			readPtr=0;
	}
}
void MonoRingBuffer::readReplaceLast(float* data, int numSamples)
{
	int end = std::min(size(), numSamples);
	auto* src = buffer.getWritePointer(0);
	for (int n=0; n < end; ++n)
	{
		data[n] = src[readPtr++];
		if (readPtr>=size())
			readPtr=0;
	}
}

void MonoRingBuffer::readAddFirst(float* data, int numSamples)
{
	int start = std::min(0, size()-numSamples);
	auto* src = buffer.getWritePointer(0);
	for (int n=start; n < numSamples; ++n)
	{
		data[n] += src[readPtr++];
		if (readPtr>=size())
			readPtr=0;
	}
}
void MonoRingBuffer::readAddLast(float* data, int numSamples)
{
	int end = std::min(size(), numSamples);
	auto* src = buffer.getWritePointer(0);
	for (int n=0; n < end; ++n)
	{
		data[n] += src[readPtr++];
		if (readPtr>=size())
			readPtr=0;
	}
}

void MonoRingBuffer::readAddAndClear(float* dst, int numSamples)
{
	int amt = std::min(size(), numSamples);
//	const int d0 = 0;
//	const int d1 = amt;

	int s0 = readPtr;
	int s1 = readPtr + amt;

	auto* src = buffer.getWritePointer(0);

	if (s1 < size())
	{
		FloatVectorOperations::add(dst, src, amt);
		FloatVectorOperations::clear(src, amt);
	}
	else
	{
		int a1 = size()-s0;
		int a2 = amt-a1;
		
		FloatVectorOperations::add(dst, src+s0, a1);
		FloatVectorOperations::clear(src+s0, a1);
		
		FloatVectorOperations::add(dst+a1, src, a2);
		FloatVectorOperations::clear(src, a2);
	}
	
	advanceReadPtr(numSamples);
}
//void MonoRingBuffer::readToBlockWithDb(MonoRingBuffer& gainDb, dsp::AudioBlock<float>& output)
//{
//	int numSamples = (int)output.getNumSamples();
//	int amt = std::min(size(), numSamples);
//
//	if (gainDb.numC()==1)
//	{
//		for (int n=0; n <amt; ++n)
//		{
//			float db=gainDb.pop(0);
//			float amp = Util::dbToAmp(db);
//			for (int c=0; c < numC(); ++c)
//			{
//				output[c][n] = pop(c) * amp;
//			}
//		}
//	}
//	else {
//		for (int c=0; c < numC(); ++c)
//		{
//			for (int n=0; n <amt; ++n)
//			{
//				float db=gainDb.pop(c);
//				float amp = Util::dbToAmp(db);
//				output[c][n] = pop(c) * amp;
//			}
//		}
//
//	}
//}

void MonoRingBuffer::clear()
{
	buffer.clear();
}

void MonoRingBuffer::clear(int numSamples)
{
	if (numSamples >= size())
	{
		buffer.clear(0, 0, size());
	}
	else
	{
		int end = writePtr + numSamples;
		
		if (end < size())
		{
			buffer.clear(0, writePtr, end);
		}
		else
		{
			buffer.clear(0, writePtr, size()-writePtr);
			
			if (end > size())
			{
				buffer.clear(0, 0, end % size());
			}
		}
	}
	advanceWritePtr(numSamples);
}
void MonoRingBuffer::fill(float value, int numSamples)
{
	
	
	jassertfalse;
}

void MonoRingBuffer::fillAll(float value)
{
	jassertfalse;
}

void MonoRingBuffer::clearSample()
{
	buffer.setSample(0, writePtr, 0);
	writePtr++;
	if (writePtr>=size()) writePtr=0;
}


/*
 void LookaheadCompressor::copyBufferToBlock(InputBlock& input, OutputBlock& block)
 {
	 //If block longer than buffer
	 if (buffer.getNumSamples()==0)
		 return;
	 else if (block.getNumSamples() > buffer.getNumSamples())
	 {
		 int framesNeeded = (int)(block.getNumSamples() - buffer.getNumSamples());
		 
		 Util::copyBlock(input, block, 0, buffer.getNumSamples(), framesNeeded);

		 int remaining = buffer.getNumSamples() - writePos;
		 block.copyFrom(buffer, writePos, 0, remaining);
		 if (writePos > 0)
		 {
			 block.copyFrom(buffer, 0, remaining, writePos);
		 }
	 }
	 else if (writePos + block.getNumSamples() > buffer.getNumSamples())
	 {
		 int remaining = buffer.getNumSamples() - writePos;
		 block.copyFrom(buffer, writePos, 0, remaining);
		 if (block.getNumSamples() > remaining)
		 {
			 block.copyFrom(buffer, 0, remaining, block.getNumSamples() - remaining);
		 }
	 }
	 else
	 {
		 block.copyFrom(buffer, writePos, 0, block.getNumSamples());
	 }
 }
 void LookaheadCompressor::copyInputToBuffer(BlockPtr& b)
 {
	 auto& input = *b;

	 if (buffer.getNumSamples() == 0)
		 return;
	 else if (input.getNumSamples() >= buffer.getNumSamples())
	 {
		 writePos = 0;
		 
		 int inputStart = (int)(input.getNumSamples() - buffer.getNumSamples());
		 
		 int remaining = buffer.getNumSamples() - writePos;

		 input.copyTo(buffer, inputStart, writePos, remaining);
 //		ffToDb(buffer, dbBuffer, writePos, buffer.getNumSamples(), rate, type, true);
	 }
	 else if (writePos + input.getNumSamples() > buffer.getNumSamples())
	 {
		 int remaining = buffer.getNumSamples() - writePos;
		 
		 input.copyTo(buffer, 0, writePos, remaining);
		 if (input.getNumSamples() > remaining)
		 {
			 input.copyTo(buffer, remaining, 0, input.getNumSamples() - remaining);
		 }
	 }
	 else
	 {
		 input.copyTo(buffer, 0, writePos, input.getNumSamples());
		 
	 }
 }
 void LookaheadCompressor::copyDbToBuffer(BlockPtr& b)
 {
	 auto& input = *b;

	 if (dbBuffer.getNumSamples()==0)
		 return;
	 else if (input.getNumSamples() >= dbBuffer.getNumSamples())
	 {
		 writePos = 0;
		 
		 int inputStart = (int)(input.getNumSamples() - dbBuffer.getNumSamples());
		 
		 int remaining = dbBuffer.getNumSamples() - writePos;

		 input.copyTo(dbBuffer, inputStart, writePos, remaining);
	 }
	 else if (writePos + input.getNumSamples() > dbBuffer.getNumSamples())
	 {
		 int remaining = dbBuffer.getNumSamples() - writePos;
		 
		 input.copyTo(dbBuffer, 0, writePos, remaining);
		 if (input.getNumSamples() > remaining)
		 {
			 input.copyTo(dbBuffer, remaining, 0, input.getNumSamples() - remaining);
		 }
	 }
	 else
	 {
		 input.copyTo(dbBuffer, 0, writePos, input.getNumSamples());
		 
	 }
 }
 */



void MonoRingBuffer::advanceWritePtr(int n)
{
	writePtr= (writePtr + n) % size();
}
void MonoRingBuffer::advanceReadPtr(int n)
{
	readPtr= (readPtr + n) % size();
}

void MonoRingBuffer::incWritePtr()
{
	writePtr= (writePtr + 1) % size();
}

void MonoRingBuffer::incReadPtr()
{
	readPtr= (readPtr + 1) % size();
}

void MonoRingBuffer::catchupRead()
{
	readPtr = writePtr;
}
void MonoRingBuffer::catchupWrite()
{
	writePtr = readPtr;
}

void MonoRingBuffer::setAllPointers(int p)
{
	writePtr = p;
	readPtr=p;
}

void MonoRingBuffer::setReadPtr(int p)
{
	readPtr=p;
}
void MonoRingBuffer::setWritePtr(int p)
{
	writePtr=p;
}


void MonoRingBuffer::assertAllPointersMatch()
{
	jassert(readPtr == writePtr);
	readPtr = writePtr;
}
void MonoRingBuffer::debugAllPointersMatch()
{
#if DEBUG
	jassert(readPtr == writePtr);
#endif
}

