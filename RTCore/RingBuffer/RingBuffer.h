
class MonoRingBuffer;

class RingBuffer
{
	
/**
	This class cannot handle asyncronous changing length, and will not handle changing length during playback very gracefully 
 
	Last: Writes last N samples of input, in effect overwriting extra
	First: Writes first N samples of input, returning when full
 
	Replace: Sets buffer value
	Add: Adds to buffer value
 
	Both AudioBlock and AudioBuffer inputs are accepted
 
	Write: Writes the input data to the buffer
	Read: Reads the buffer into the output
 *//*
	
Functions:
	clear
	fill

	writeAdd
	writeReplace
	//writeFirst
	
	readReplaceFirst
	readReplaceLast
	
	readAddFirst
	readAddLast
	
	readAddAndClear
	
	
	writeLastWithGain
	writeFirstWithGain
	
	
	
Function parameter variations:
	//All channels
	void function(AudioBlock<float>&  data, int N=data.size());
	void function(AudioBuffer<float>& data, int N=data.size());
	
	void function(AudioBlock<float>&  data, int start, int N);
	void function(AudioBuffer<float>& data, int start, int N);
	
	void function(float** data, int N);
	void function(float** data, int start, int N);

	//Channel specific
	void function(int c, float* data, int N);
	void function(int c, float* data, int start, int N);
	
	void function(int c, AudioBlock<float>&  data, int N=data.size());
	void function(int c, AudioBuffer<float>& data, int N=data.size());
	
	void function(int c, AudioBlock<float>&  data, int start, int N);
	void function(int c, AudioBuffer<float>& data, int start, int N);
*/
public:
	RingBuffer(int size);
	RingBuffer(int size, int numC);

	RingBuffer();
	
	~RingBuffer();
	
	void setChannelsAndLength(int c, int N);
	void setNumChannels(int set);
	void setLength(int set);
	
	void applyRamp(float start, float end, int N);
	
	//Returns an AudioBlock suitable for writing; write to Block1 and then Block2; check each block has greater than 0 samples
	juce::dsp::AudioBlock<float> getWriteBlock1();
	juce::dsp::AudioBlock<float> getWriteBlock2();

	//Does not advance write pointers
	//Both may return size 0 block
	juce::dsp::AudioBlock<float> getWriteBlock1(int start, int len);
	juce::dsp::AudioBlock<float> getWriteBlock2(int start, int len);

	
	inline int numC() const { return buffer.getNumChannels(); }
	inline int size() const { return size_;} //buffer.getNumSamples(); }
	inline int length() const { return size_; }
	
	float rms();
	float max();
	float average();
	
	void clear();
	void clearChannel(int c);
	void clear(int c, int numSamples);
	void clear(int numSamples);
	void clearSample();
	
	void fill(float value, int n);
	void fill(float value, int c, int n);
	void fillAll(float value, int c);
	void fillAll(float value);

	inline float& atRead(int c) {
		return *(buffer.getWritePointer(c)+readPtr[c]);
	}
	inline float& atWrite(int c) {
		return *(buffer.getWritePointer(c)+writePtr[c]);
	}

	inline float& atRead(int c, int offset) {
		return *(buffer.getWritePointer(c)+wrap(readPtr[c]+offset));
	}
	inline float& atWrite(int c, int offset) {
		return *(buffer.getWritePointer(c)+wrap(writePtr[c]+offset));
	}

	inline float atReadInterp(int c, float offset) {
		return interpolateSample(c, readPtr[c] + offset);
	}
	inline float atWriteInterp(int c, float offset) {
		return interpolateSample(c, writePtr[c] + offset);
	}
private:
//	typedef std::function<void(int c, float* data, int numSamples)> RAudioCallback;
//	typedef std::function<void(int c, const float* input, const float* output, int numSamples)> NRAudioCallback;


public:
	//NumSamples = bufferSize
//	void delayAndProcess(dsp::AudioBlock<float> input, dsp::AudioBlock<float> output, RAudioCallback, NRAudioCallback);
	
	
	
	inline void push(int c, float x) {
		buffer.setSample(c, writePtr[c], x);
		incWritePtr(c);
	}
	inline float pop(int c) {
		float x = buffer.getSample(c, readPtr[c]);
		incReadPtr(c);
		return x;
	}
	
	void writeAdd(int c, const float* data, int numSamples);
//	void writeAddFirst(int c, const float* data, int numSamples);
	
	void writeAddLastWithGain(int c, const float* data, int numSamples, float gain);
	
	void writeReplace(int c, const float* data, int numSamples);
//	void writeReplaceFirst(int c, const float* data, int numSamples);
	
	void readReplaceFirst(int c, float* data, int numSamples=0);
	void readReplaceLast(int c, float* data, int numSamples=0);

	void readAddFirst(int c, float* data, int numSamples=0);
	void readAddLast(int c, float* data, int numSamples=0);

	void readAddAndClear(int c, float* data, int numSamples=0);
	
//	void readToBlockWithDb(RingBuffer& gainDb, dsp::AudioBlock<float>& output);
//	void readToBlockWithDb(MonoRingBuffer& gainDb, dsp::AudioBlock<float>& output);

	void readToBlockWithDb(RingBuffer& gainDb, dsp::AudioBlock<float>& output, double extraDb=0);
	void readToBlockWithDb(MonoRingBuffer& gainDb, dsp::AudioBlock<float>& output, double extraDb=0);

	
	void assertAllPointersMatch();
	void assertWritePointersMatch();
	void assertReadPointersMatch();

	void catchupRead();
	void catchupWrite();
	
	inline void setReadToWrite() { catchupRead(); }
	inline void setWriteToRead() { catchupWrite(); }
	
	void debugAllPointersMatch();
	void debugWritePointersMatch();
	void debugReadPointersMatch();
	
	
	
#define COMMA ,
#define Empty
	
#define broadcast(name, fnp, fnv) \
	inline void name(juce::dsp::AudioBlock<float>& input fnp) { \
		name(input, 0, (int)input.getNumSamples() fnv); \
	} \
	inline void name(juce::AudioBuffer<float>& input fnp) { \
		name (input, 0, (int)input.getNumSamples() fnv); \
	} \
	inline void name(juce::dsp::AudioBlock<float>& input, int numSamples fnp) { \
		name(input, 0, numSamples fnv); \
	} \
	inline void name(juce::AudioBuffer<float>& input, int numSamples fnp) { \
		name (input, 0, numSamples fnv); \
	} \
	void name(juce::dsp::AudioBlock<float>& input, int start, int numSamples fnp) { \
		jassert(numC() == input.getNumChannels()); \
		for (int c=0; c < numC(); ++c) { \
			name(c, input[c]+start, numSamples fnv); \
		} \
	} \
	void name(juce::AudioBuffer<float>& input, int start, int numSamples fnp) { \
		jassert(numC() == input.getNumChannels()); \
		for (int c=0; c < numC(); ++c) { \
			name(c, input.getWritePointer(c)+start, numSamples fnv); \
		} \
	} \
	inline void name(float** input, int numSamples fnp) { \
		name(input, 0, numSamples fnv); \
	} \
	void name(float** input, int start, int numSamples fnp) { \
		for (int c=0; c < numC(); ++c) \
			name(c, input[c]+start, numSamples fnv); \
	} \
	inline void name(int c, juce::dsp::AudioBlock<float>& input fnp) { \
		name(c, input[c], (int)input.getNumSamples() fnv); \
	} \
	inline void name(int c, juce::AudioBuffer<float>& input fnp) { \
		name(c, input.getWritePointer(c), (int)input.getNumSamples() fnv); \
	} \
	void name(int c, juce::dsp::AudioBlock<float>& input, int numSamples fnp) { \
		name(c, input[c], numSamples fnv); \
	} \
	void name(int c, juce::AudioBuffer<float>& input, int numSamples fnp) { \
		name(c, input.getWritePointer(c), numSamples fnv); \
	} \
	void name(int c, juce::dsp::AudioBlock<float>& input, int start, int numSamples fnp) { \
		name(c, input[c]+start, numSamples fnv); \
	} \
	void name(int c, juce::AudioBuffer<float>& input, int start, int numSamples fnp) { \
		name(c, input.getWritePointer(c)+start, numSamples fnv); \
	}
	//	broadcast(name, fnp, fnv)
#define broadcast_const(name, fnp, fnv) \
	inline void name(const juce::dsp::AudioBlock<const float>& input fnp) { \
		name(input, 0, (int)input.getNumSamples() fnv); \
	} \
	inline void name(const juce::AudioBuffer<const float>& input fnp) { \
		name (input, 0, (int)input.getNumSamples() fnv); \
	} \
	inline void name(const juce::dsp::AudioBlock<const float>& input, int numSamples fnp) { \
		name(input, 0, numSamples fnv); \
	} \
	inline void name(const juce::AudioBuffer<const float>& input, int numSamples fnp) { \
		name (input, 0, numSamples fnv); \
	} \
	void name(const juce::dsp::AudioBlock<const float>& input, int start, int numSamples fnp) { \
		jassert(numC() == input.getNumChannels()); \
		for (int c=0; c < numC(); ++c) { \
			name(c, input.getChannelPointer(c)+start, numSamples fnv); \
		} \
	} \
	void name(const juce::AudioBuffer<const float>& input, int start, int numSamples fnp) { \
		jassert(numC() == input.getNumChannels()); \
		for (int c=0; c < numC(); ++c) { \
			name(c, input.getReadPointer(c)+start, numSamples fnv); \
		} \
	} \
	inline void name(const float** input, int numSamples fnp) { \
		name(input, 0, numSamples fnv); \
	} \
	void name(const float** input, int start, int numSamples fnp) { \
		for (int c=0; c < numC(); ++c) \
			name(c, input[c]+start, numSamples fnv); \
	} \
	inline void name(int c, const juce::dsp::AudioBlock<const float>& input fnp) { \
		name(c, input.getChannelPointer(c), (int)input.getNumSamples() fnv); \
	} \
	inline void name(int c, const juce::AudioBuffer<const float>& input fnp) { \
		name(c, input.getReadPointer(c), (int)input.getNumSamples() fnv); \
	} \
	void name(int c, const juce::dsp::AudioBlock<const float>& input, int numSamples fnp) { \
		name(c, input.getChannelPointer(c), numSamples fnv); \
	} \
	void name(int c, const juce::AudioBuffer<const float>& input, int numSamples fnp) { \
		name(c, input.getReadPointer(c), numSamples fnv); \
	} \
	void name(int c, const juce::dsp::AudioBlock<const float>& input, int start, int numSamples fnp) { \
		name(c, input.getChannelPointer(c)+start, numSamples fnv); \
	} \
	void name(int c, const juce::AudioBuffer<const float>& input, int start, int numSamples fnp) { \
		name(c, input.getReadPointer(c)+start, numSamples fnv); \
	}
	
	//To add extra parameters:
	//		First arg: COMMA float name COMMA int name2
	//		Second arg: COMMA name COMMA name2
	
//	broadcast_const(f2, COMMA float gain COMMA int num, COMMA gain COMMA num);
//	broadcast(f2, COMMA float gain COMMA int num, COMMA gain COMMA num);

	broadcast_const(writeAdd,Empty,Empty);
	broadcast_const(writeReplace,Empty,Empty);
	
	broadcast(writeAdd,Empty,Empty);
	broadcast(writeReplace,Empty,Empty);

//	broadcast_const(writeAddFirst,Empty,Empty);	
//	broadcast_const(writeReplaceFirst,Empty,Empty);
	
	broadcast(readReplaceFirst,Empty,Empty);
	broadcast(readReplaceLast,Empty,Empty);
	
	broadcast(readAddFirst,Empty,Empty);
	broadcast(readAddLast,Empty,Empty);
	
	broadcast(readAddAndClear,Empty,Empty);
	
#undef broadcast_const
#undef broadcast
#undef COMMA
#undef Empty
	
public:
	void advanceWritePtr(int c, int n);
	void advanceWritePtrs(int n);
	
	void advanceReadPtr(int c, int n);
	void advanceReadPtrs(int n);

	void incWritePtr(int c);
	void incWritePtrs();
	
	void incReadPtr(int c);
	void incReadPtrs();

	void setReadPtrs(int n);
	void setWritePtrs(int n);
	
	void setAllPointers(int p);
	
	
protected:
	AudioBuffer<float> buffer;
//	int numChannels;
	int size_;
	int* writePtr=NULL;
	int* readPtr=NULL;
	
	inline int wrap(int x) const {
		if (x >= 0)
			return x % size();
		else
			return ((x % size()) + size()) % size();
	}
	
protected:
//	typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Linear>::value, SampleType>::type
	float interpolateSample (int channel, float pos) const
	{
		
		auto index1 = int(pos);
		float delayFrac = pos-index1;

		if (index1 < 0) {
			index1 = wrap(index1);
		}

		auto index2 = index1 + 1;
		
		if (index2 >= size())
		{
			index1 %= size();
			index2 %= size();
		}
		
		auto value1 = buffer.getSample (channel, index1);
		auto value2 = buffer.getSample (channel, index2);

		return value1 + delayFrac * (value2 - value1);
	}

};
