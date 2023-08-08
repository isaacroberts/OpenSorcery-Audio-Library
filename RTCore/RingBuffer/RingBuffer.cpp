

#define for_c for (int c=0; c < numC(); ++c)
#define for_c1 for (int c=1; c < numC(); ++c)

RingBuffer::RingBuffer(int size)
: buffer(0, size)
{

}
RingBuffer::RingBuffer(int size, int numC)
: buffer(numC, size)
{

}
RingBuffer::RingBuffer()
: buffer()
{

}

RingBuffer::~RingBuffer()
{
	delete[] writePtr;
	delete[] readPtr;
}

void RingBuffer::setChannelsAndLength(int c, int N)
{
	if (c != numC())
	{
		delete[] writePtr;
		delete[] readPtr;
		writePtr = new int[c];
		readPtr = new int[c];
	}
	buffer.setSize(c, N);
	buffer.clear();
	size_ = N;
	
	for_c {
		writePtr[c]=0;
		readPtr[c]=0;
	}
}
void RingBuffer::setNumChannels(int set)
{
	if (set != numC())
	{
		delete[] writePtr;
		delete[] readPtr;
		writePtr = new int[set];
		readPtr = new int[set];
		
		buffer.setSize(set, size(), false);
		
		for_c {
			writePtr[c]=0;
			readPtr[c]=0;
		}
	}
	
}
void RingBuffer::setLength(int set)
{
	//Need new class with locks if you want to do this async
	for_c
	{//Avoid write/read being OOB
		writePtr[c]=0;
		readPtr[c]=0;
	}
	buffer.setSize(numC(), set, true, true, true);
	size_ = set;
}


float RingBuffer::rms()
{
	//TODO: This was being weird 
	return Util::rmsLevelBuffer(buffer);
}
float RingBuffer::max()
{
	float max = FloatVectorOperations::findMaximum(buffer[0], size());
	for (int c=1; c < numC(); ++c)
	{
		float m2 = max = FloatVectorOperations::findMaximum(buffer[c], size());
		if (m2 > max) {
			max = m2;
		}
	}
	return max;
}

float RingBuffer::average()
{
	float sum=0;
	for (int c=0; c < numC(); ++c)
	{
		auto data = buffer[c];
		for (int n=0; n < size(); ++n)
		{
			sum += data[n];
		}
	}
	sum /= size() * numC();
	return sum;
}
void RingBuffer::applyRamp(float start, float end, int N)
{
	int w=writePtr[0];
	
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

juce::dsp::AudioBlock<float> RingBuffer::getWriteBlock1()
{
	assertWritePointersMatch();
	//Advance writePointer by size; no effect
	return juce::dsp::AudioBlock<float> (buffer.getArrayOfWritePointers(), numC(), writePtr[0], size()-writePtr[0]);
}
juce::dsp::AudioBlock<float> RingBuffer::getWriteBlock2()
{
//	assertWritePointersMatch();
	//Have already advanced writePointer by size
	if (writePtr[0]==0)
	{
		//Return empty
		return juce::dsp::AudioBlock<float>();
	}
	return juce::dsp::AudioBlock<float> (buffer.getArrayOfWritePointers(), numC(), 0, writePtr[0]);
}

juce::dsp::AudioBlock<float> RingBuffer::getWriteBlock1(int start, int len)
{ //Does not advance write pointers
	jassert(len <= size());
	assertWritePointersMatch();
	int p1 = start + writePtr[0];
	p1 = p1 % size();
	int l1 = std::max(len, size()-p1);
	return juce::dsp::AudioBlock<float> (buffer.getArrayOfWritePointers(), numC(), p1, l1);
}
juce::dsp::AudioBlock<float> RingBuffer::getWriteBlock2(int start, int len)
{
	int end = writePtr[0] + start + len;
	if (end <= size())
	{
		//Return empty
		return juce::dsp::AudioBlock<float>();
	}
	else
	{
		end = end % size();
		jassert(end <= writePtr[0]);
		return juce::dsp::AudioBlock<float> (buffer.getArrayOfWritePointers(), numC(), 0, end);
	}
}


/*
	TODO: Rewrite all of these with FloatVectorOperations, since they're much faster 
 */
void RingBuffer::writeReplace(int c, const float* data, int numSamples)
{
	int start = std::min(0, size()-numSamples);
	auto* dest = buffer.getWritePointer(c);
	for (int n=start; n < numSamples; ++n)
	{
		dest[writePtr[c]++] = data[n];
		if (writePtr[c]>=size())
			writePtr[c]=0;
	}
}
//void RingBuffer::writeReplaceFirst(int c, const float* data, int numSamples)
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
void RingBuffer::writeAdd(int c, const float* data, int numSamples)
{
	int start = std::min(0, size()-numSamples);
	auto* dest = buffer.getWritePointer(c);
	for (int n=start; n < numSamples; ++n)
	{
		dest[writePtr[c]++] += data[n];
		if (writePtr[c]>=size())
			writePtr[c]=0;
	}
}
//void RingBuffer::writeAddFirst(int c, const float* data, int numSamples)
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

void RingBuffer::readReplaceFirst(int c, float* data, int numSamples)
{
	int start = std::min(0, size()-numSamples);
	auto* src = buffer.getWritePointer(c);
	for (int n=start; n < numSamples; ++n)
	{
		data[n] = src[readPtr[c]++];
		if (readPtr[c]>=size())
			readPtr[c]=0;
	}
}
void RingBuffer::readReplaceLast(int c, float* data, int numSamples)
{
	int end = std::min(size(), numSamples);
	auto* src = buffer.getWritePointer(c);
	for (int n=0; n < end; ++n)
	{
		data[n] = src[readPtr[c]++];
		if (readPtr[c]>=size())
			readPtr[c]=0;
	}
}

void RingBuffer::readAddFirst(int c, float* data, int numSamples)
{
	int start = std::min(0, size()-numSamples);
	auto* src = buffer.getWritePointer(c);
	for (int n=start; n < numSamples; ++n)
	{
		data[n] += src[readPtr[c]++];
		if (readPtr[c]>=size())
			readPtr[c]=0;
	}
}
void RingBuffer::readAddLast(int c, float* data, int numSamples)
{
	int end = std::min(size(), numSamples);
	auto* src = buffer.getWritePointer(c);
	for (int n=0; n < end; ++n)
	{
		data[n] += src[readPtr[c]++];
		if (readPtr[c]>=size())
			readPtr[c]=0;
	}
}

void RingBuffer::readAddAndClear(int c, float* dst, int numSamples)
{
	int amt = std::min(size(), numSamples);
//	const int d0 = 0;
//	const int d1 = amt;

	int s0 = readPtr[c];
	int s1 = readPtr[c] + amt;

	auto* src = buffer.getWritePointer(c);

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
	
	advanceReadPtr(c, numSamples);
}
void RingBuffer::readToBlockWithDb(RingBuffer& gainDb, dsp::AudioBlock<float>& output, double extraDb)
{
	int numSamples = (int)output.getNumSamples();
	int amt = std::min(size(), numSamples);
	jassert(gainDb.numC()==numC());
	jassert(gainDb.size()==size());
	for (int c=0; c < numC(); ++c)
	{
		for (int n=0; n <amt; ++n)
		{
			float db=gainDb.pop(c);
			float amp = Util::dbToAmp(db+extraDb);
			output[c][n] = pop(c) * amp;
		}
	}
}

void RingBuffer::readToBlockWithDb(MonoRingBuffer& gainDb, dsp::AudioBlock<float>& output, double extraDb)
{
	jassert(gainDb.size()==size());
	int numSamples = (int)output.getNumSamples();
	int amt = std::min(size(), numSamples);

	for (int n=0; n <amt; ++n)
	{
		float db=gainDb.pop();
		float amp = Util::dbToAmp(db+extraDb);
		for (int c=0; c < numC(); ++c)
		{
			output[c][n] = pop(c) * amp;
		}
	}
}

void RingBuffer::clear()
{
	buffer.clear();
}
void RingBuffer::clearChannel(int c)
{
	buffer.clear(c, 0, size());
}
void RingBuffer::clear(int c, int numSamples)
{
	if (numSamples >= size())
	{
		buffer.clear(c, 0, size());
	}
	else
	{
		int end = writePtr[c] + numSamples;
		
		if (end < size())
		{
			buffer.clear(c, writePtr[c], end);
		}
		else
		{
			buffer.clear(c, writePtr[c], size()-writePtr[c]);
			
			if (end > size())
			{
				buffer.clear(c, 0, end % size());
			}
		}
	}
	advanceWritePtr(c, numSamples);
}
void RingBuffer::fill(float value, int numSamples)
{
	
	
	jassertfalse;
}

void RingBuffer::fill(float value, int c, int n)
{
	jassertfalse;
}
void RingBuffer::fillAll(float value, int c)
{
	jassertfalse;
}
void RingBuffer::fillAll(float value)
{
	jassertfalse;
}

void RingBuffer::clear(int numSamples)
{
	debugWritePointersMatch();
	
	int w = writePtr[0];
	
	if (numSamples >= size())
	{
		for_c
			buffer.clear(c, 0, size());
	}
	else
	{
		int end = w + numSamples;
		
		if (end < size())
		{
			for_c
				buffer.clear(c, w, end);
		}
		else
		{
			for_c
				buffer.clear(c, w, size()-writePtr[c]);
			
			if (end > size())
			{
				for_c
					buffer.clear(c, 0, end % size());
			}
		}
	}
	advanceWritePtrs(numSamples);
}
void RingBuffer::clearSample()
{
	for_c
		buffer.setSample(c, writePtr[0], 0);
	writePtr[0]++;
	if (writePtr[0]>=size()) writePtr[0]=0;
	for_c1
		writePtr[c] = writePtr[0];
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



void RingBuffer::advanceWritePtr(int c, int n)
{
	writePtr[c]= (writePtr[c] + n) % size();
}
void RingBuffer::advanceWritePtrs(int n)
{
	writePtr[0]= (writePtr[0] + n) % size();
	
	for_c1
	{
		writePtr[c]=writePtr[0];
	}
}
void RingBuffer::advanceReadPtr(int c, int n)
{
	readPtr[c]= (readPtr[c] + n) % size();
}
void RingBuffer::advanceReadPtrs(int n)
{
	readPtr[0]= (readPtr[0] + n) % size();
	
	for_c1
	{
		readPtr[c]=readPtr[0];
	}
}

void RingBuffer::incWritePtr(int c)
{
	writePtr[c]= (writePtr[c] + 1) % size();
}
void RingBuffer::incWritePtrs()
{
	writePtr[0]= (writePtr[0] + 1) % size();
	for_c1
		writePtr[c] = writePtr[0];
}

void RingBuffer::incReadPtr(int c)
{
	readPtr[c]= (readPtr[c] + 1) % size();
}
void RingBuffer::incReadPtrs()
{
	readPtr[0]= (readPtr[0] + 1) % size();
	for_c1
		readPtr[c] = readPtr[0];

}


void RingBuffer::catchupRead()
{
	for_c
		readPtr[c] = writePtr[0];
}
void RingBuffer::catchupWrite()
{
	for_c
		writePtr[c] = readPtr[0];
}

void RingBuffer::setAllPointers(int p)
{
	for_c
	{
		writePtr[c] = p;
		readPtr[c]=p;
	}
}

void RingBuffer::setReadPtrs(int p)
{
	for_c
		readPtr[c]=p;
}
void RingBuffer::setWritePtrs(int p)
{
	for_c
		writePtr[c]=p;
}


void RingBuffer::assertAllPointersMatch()
{
	jassert(readPtr[0] == writePtr[0]);
	readPtr[0] = writePtr[0];
	for_c1
	{
		jassert(readPtr[c] == readPtr[0]);
		jassert(writePtr[c] == writePtr[0]);

		readPtr[c]  = writePtr[0];
		writePtr[c] = writePtr[0];
	}
}
void RingBuffer::assertWritePointersMatch()
{
	for_c1
	{
		jassert(writePtr[c] == writePtr[0]);
		writePtr[c]=writePtr[0];
	}
}
void RingBuffer::assertReadPointersMatch()
{
	for_c1
	{
		jassert(readPtr[c] == readPtr[0]);
		readPtr[c]=readPtr[0];
	}
}

void RingBuffer::debugWritePointersMatch()
{
#if DEBUG
	for_c1
	{
		jassert(writePtr[c] == writePtr[0]);
	}
#endif
}
void RingBuffer::debugReadPointersMatch()
{
#if DEBUG
	for_c1
	{
		jassert(readPtr[c] == readPtr[0]);
	}
#endif
}
void RingBuffer::debugAllPointersMatch()
{
#if DEBUG
	jassert(readPtr[0] == writePtr[0]);
	for_c1
	{
		jassert(readPtr[c] == readPtr[0]);
		jassert(writePtr[c] == writePtr[0]);
		jassert(readPtr[c] == writePtr[c]);
	}
//	std::cout<<"ringbuffer ptrs="<<readPtr[0]<<std::endl;
#endif
}


#undef for_c
#undef for_c1
