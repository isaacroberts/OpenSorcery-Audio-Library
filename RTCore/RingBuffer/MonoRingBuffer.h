


class MonoRingBuffer
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

	void function(float* data, int N);
	void function(float* data, int start, int N);
*/
public:
	MonoRingBuffer(int size);

	MonoRingBuffer();
	
	~MonoRingBuffer();
	
	void setLength(int set);
	
	void applyRamp(float start, float end, int N);
	
	//Returns an AudioBlock suitable for writing; write to Block1 and then Block2; check each block has greater than 0 samples
	juce::dsp::AudioBlock<float> getWriteBlock1();
	juce::dsp::AudioBlock<float> getWriteBlock2();

	//Does not advance write pointers
	//Both may return size 0 block
	juce::dsp::AudioBlock<float> getWriteBlock1(int start, int len);
	juce::dsp::AudioBlock<float> getWriteBlock2(int start, int len);

	
	inline int numC() const { return 1; }
	inline int size() const { return size_;} //buffer.getNumSamples(); }
	inline int length() const { return size_; }
	
	float rms();
	float max();
	float average();
	
	void clear();
	void clear(int numSamples);
	void clearSample();
	
	void fill(float value, int n);
	void fillAll(float value);

	inline float& atRead() {
		return *(buffer.getWritePointer(0)+readPtr);
	}
	inline float& atWrite() {
		return *(buffer.getWritePointer(0)+writePtr);
	}

	inline float& atRead(int offset) {
		return *(buffer.getWritePointer(0)+wrap(readPtr+offset));
	}
	inline float& atWrite(int offset) {
		return *(buffer.getWritePointer(0)+wrap(writePtr+offset));
	}

	inline float atReadInterp(float offset) {
		return interpolateSample(readPtr + offset);
	}
	inline float atWriteInterp(float offset) {
		return interpolateSample(writePtr + offset);
	}
	
private:
//	typedef std::function<void(int c, float* data, int numSamples)> RAudioCallback;
//	typedef std::function<void(int c, const float* input, const float* output, int numSamples)> NRAudioCallback;


public:
	//NumSamples = bufferSize
//	void delayAndProcess(dsp::AudioBlock<float> input, dsp::AudioBlock<float> output, RAudioCallback, NRAudioCallback);
	
	
	
	inline void push(float x) {
		buffer.setSample(0, writePtr, x);
		incWritePtr();
	}
	inline float pop() {
		float x = buffer.getSample(0, readPtr);
		incReadPtr();
		return x;
	}
	
	void writeAdd(const float* data, int numSamples);
//	void writeAddFirst(int c, const float* data, int numSamples);
	
	void writeAddLastWithGain(const float* data, int numSamples, float gain);
	
	void writeReplace(const float* data, int numSamples);
//	void writeReplaceFirst(int c, const float* data, int numSamples);
	
	void readReplaceFirst(float* data, int numSamples=0);
	void readReplaceLast(float* data, int numSamples=0);

	void readAddFirst(float* data, int numSamples=0);
	void readAddLast(float* data, int numSamples=0);

	void readAddAndClear(float* data, int numSamples=0);
	
//	void readToBlockWithDb(MonoRingBuffer& gainDb, dsp::AudioBlock<float>& output);
	
	void assertAllPointersMatch();

	void catchupRead();
	void catchupWrite();
	
	inline void setReadToWrite() { catchupRead(); }
	inline void setWriteToRead() { catchupWrite(); }
	
	void debugAllPointersMatch();
	
	
	
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
		jassert(input.getNumChannels() == 1); \
		name(input[0]+start, numSamples fnv); \
	} \
	void name(juce::AudioBuffer<float>& input, int start, int numSamples fnp) { \
		jassert(input.getNumChannels() == 1); \
		name(input.getWritePointer(0)+start, numSamples fnv); \
	} \
	inline void name(float** input, int numSamples fnp) { \
		name(input, 0, numSamples fnv); \
	} \
	void name(float** input, int start, int numSamples fnp) { \
		name(input[0]+start, numSamples fnv); \
	} \
	void name(float* input, int start, int numSamples fnp) { \
		name(input+start, numSamples fnv); \
	} \

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
		jassert(input.getNumChannels() == 1); \
		name(input.getChannelPointer(0)+start, numSamples fnv); \
	} \
	void name(const juce::AudioBuffer<const float>& input, int start, int numSamples fnp) { \
		jassert(input.getNumChannels() == 1); \
		name(input.getReadPointer(0)+start, numSamples fnv); \
	} \
	inline void name(const float** input, int numSamples fnp) { \
		name(input, 0, numSamples fnv); \
	} \
	void name(const float** input, int start, int numSamples fnp) { \
		name(input[0]+start, numSamples fnv); \
	} \
	void name(const float* input, int start, int numSamples fnp) { \
	   name(input+start, numSamples fnv); \
	} \
	
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
	void advanceWritePtr(int n);
	
	void advanceReadPtr(int n);

	void incWritePtr();
	
	void incReadPtr();

	void setReadPtr(int n);
	void setWritePtr(int n);
	

	inline void setReadPtrs(int n) {setReadPtr(n); }
	inline void setWritePtrs(int n){ setWritePtr(n); }
	
	void setAllPointers(int p);
	
	
protected:
	AudioBuffer<float> buffer;
//	int numChannels;
	int size_;
	int writePtr=0;
	int readPtr=0;
	
	inline int wrap(int x) const {
		if (x >= 0)
			return x % size();
		else
			return ((x % size()) + size()) % size();
	}
	
protected:
//	typename std::enable_if <std::is_same <T, DelayLineInterpolationTypes::Linear>::value, SampleType>::type
	float interpolateSample (float pos) const
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
		
		auto value1 = buffer.getSample (0, index1);
		auto value2 = buffer.getSample (0, index2);

		return value1 + delayFrac * (value2 - value1);
	}

};
