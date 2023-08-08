//
//  CompressorCommon.hpp
//  
//
//  Created by Isaac Roberts on 4/5/21.
//

#ifndef CompressorCommon_hpp
#define CompressorCommon_hpp

#include <stdio.h>

namespace Compressors {
	
	enum ReadType {
		Feedforward, RMS, Peak, Mean
	};
	
	enum Params : unsigned int  {
		//Ratio is amount compressed - Db let through is 1-Ratio
		Threshold, Ratio, Attack, Release, KneeWidth, PreGain, PostGain
	};
	
	void dbLevel(InputBlock& i, OutputBlock db);
	
	float getSampleLevel(InputBlock& i, int n, ReadType type);
	float getSampleLevel(AudioBuffer<float>& i, int n, ReadType type);
	
	float peakSampleLevel(InputBlock& i, int n);
	float peakSampleLevel(AudioBuffer<float>& i, int n);

	float getSampleLevel(float x);

	float getLevel(InputBlock& i, ReadType type);

	float peakLevel(InputBlock& i);
	float rmsLevel(InputBlock& i);
	float meanLevel(InputBlock& i);
	
	float alpha(float inputVar, float rate);
	
	float alpha(float inputVar, float rate);
	float alpha(float inputVar, float rate);
	
	float timeStep(float y_t, float x_t, float alpha);
	float doShoot(float shoot, float knee, float slope);
	float switchStep(float y_t, float x_t, float alphaAtt, float alphaRel);
	
	
	void applyGainReductionDb(InputBlock& db, InputBlock& input, OutputBlock& output, float extraGain);
	void applyGainReductionDb(InputBlock& db, OutputBlock& output, float extraGain);
	
	
	void applyGain(OutputBlock& out, InputBlock& in, OutputBlock& gain, bool mono=true);
	void applyGain(OutputBlock& out, InputBlock& in, AudioBuffer<float>& gain, int writePos, InputBlock& incomingGain, bool mono=true);

	class Common
	{
	protected:
		
		Common(float t, float r, float a, float rl, float kw, float pg, float postg) :
			threshold(t), ratio(r), attack(a), release(rl), kneewidth(kw), pregain(pg), postgain(postg),
			y_t(0) {}
		
		Common() : Common(0, .2, .003, .1, .2, 0, 0) {}
		
		virtual ~Common() {}
		
		void compressorProcess(InputBlock& levelBlock, InputBlock& input, OutputBlock& output, float rate, ReadType type);
		
//		void blockProcess(InputBlock& level, InputBlock& i, OutputBlock& b, float rate, ReadType type);

		void blockProcess(float level, InputBlock& i, OutputBlock& b, float rate);
		void rmsProcess(InputBlock& levelBlock, InputBlock& input, OutputBlock& output, float rate);
		void peakProcess(InputBlock& levelBlock, InputBlock& input, OutputBlock& output, float rate);
		void meanProcess(InputBlock& levelBlock, InputBlock& input, OutputBlock& output, float rate);
		void ffProcess(InputBlock& levelBlock, InputBlock& input, OutputBlock& output, float rate);
	
		float dbReductionSample(float level, float rate);
		float dbReductionSampleWithoutPostgain(float level, float rate);
		
		void compressorBypass(InputBlock& levelBlock, float rate, ReadType type);
		
		void blockBypass(float level, int numSamples, float rate);

		void rmsBypass(InputBlock& levelBlock, float rate);
		void peakBypass(InputBlock& levelBlock, float rate);
		void meanBypass(InputBlock& levelBlock, float rate);
		void ffBypass(InputBlock& levelBlock, float rate);
	
		
		
		void releaseNFrames(int frames, float rate) {
			if (y_t != 0)
			  y_t *= pow(alpha(release, rate), frames);
		}
		void resetState() {
			y_t = 0;
		}

		
		void doBypass(InputBlock& input, OutputBlock& block, float rate) {
			if (input != block)
			{
				block.copyFrom(input);
			}
			releaseNFrames((int)input.getNumSamples(), rate);
		}
		
		void setCompressorVar(int n, float set);
		float getCompressorVar(int n);

		float threshold, ratio, attack, release, kneewidth, pregain, postgain;
		float y_t;
	public:
		void setThreshold(float set) 	{ threshold=set; }
		void setRatio(float set) 		{ ratio = set; }
		void setAttack(float set) 		{ attack=set; }
		void setRelease(float set) 		{ release= set;}
		void setKnee(float set) 		{ kneewidth = set; }
		void setKneeWidth(float set) 	{ kneewidth = set; }
		void setPregain(float set) 		{ pregain = set; }
		void setPostgain(float set) 	{ postgain = set; }
		
		//Alternate names
		void setTimes(float atk, float rel) { attack = atk; release = rel; }
		void setAttackAndRelease(float atk, float rel) { attack = atk; release = rel; }
		void setPreGain(float set) 		{ pregain = set; }
		void setPostGain(float set) 	{ postgain = set; }
		
		//Alternate input methods
		void setSlope(float set) { ratio = 1-set;}
		void setRatioNumerator(float set) {
			if (set<=0)
				ratio = 0;
			else
				ratio = 1/set;
		}
		
		//"Presets" 
		void setLimiter(float thresh=0, float pregain_=0, float postgain_ = 0) {
			threshold = thresh;
			ratio = attack = release = kneewidth = 0;
			pregain = pregain_;
			postgain = postgain_;
		}
		
		
		void setParameters(float threshold, float ratio, float attack, float release, float kneewidth, float pregain, float postgain)
		{
			this->threshold = threshold;
			this->ratio = ratio;
			this->attack=attack;
			this->release = release;
			this->kneewidth = kneewidth;
			this->pregain = pregain;
			this->postgain = postgain;
		}
		void setParameters(float threshold, float ratio, float attack, float release, float kneewidth, float pregain)
		{
			this->threshold = threshold;
			this->ratio = ratio;
			this->attack=attack;
			this->release = release;
			this->kneewidth = kneewidth;
			this->pregain = pregain;
		}
		void setParameters(float threshold, float ratio, float attack, float release, float kneewidth)
		{
			this->threshold = threshold;
			this->ratio = ratio;
			this->attack=attack;
			this->release = release;
			this->kneewidth = kneewidth;
		}
		void setParameters(float threshold, float ratio, float attack, float release)
		{
			this->threshold = threshold;
			this->ratio = ratio;
			this->attack=attack;
			this->release = release;
		}
		
		
		inline constexpr int compressorVarAmt() { return 7; }
		
		float getCompressLevel()
		{
			return y_t;
		}
	protected:
		//OTT Stuff
		void ffToDb(InputBlock& levelBlock, OutputBlock& dbBlock, float rate, bool mono);
		void ffToDb(AudioBuffer<float>& levelBlock, int writePos, OutputBlock& dbBlock, float rate, ReadType type, bool mono);
		void ffToDb(AudioBuffer<float>& levelBlock, AudioBuffer<float>& dbBlock, int writePos, int numSamples, float rate, ReadType type, bool mono);
		void ffToDb(InputBlock& levelBlock, AudioBuffer<float>& dbBlock, int writePos, float rate, ReadType type, bool mono);

		
	};
	
	inline void sanitize(float& x)
	{
		if (std::isfinite(x))
		{
			return;
		}
		else if (std::isinf(x))
		{
			if (x > 0) {
				x=10;
			}
			else {
				x=-10;
			}
		}
		else {
			x=0;
		}
	}
	
	//OTT Stuff
	
	void rmsDbLookahead(InputBlock& audio, AudioBuffer<float>& buffer, int writePtr, OutputBlock& db);
	void peakDbLookahead(InputBlock& audio, AudioBuffer<float>& buffer, int writePtr, OutputBlock& db);
	
	void rollingRMS(InputBlock& audio, OutputBlock& db, OutputBlock& rollbuffer, int writePos);
	void rollingPeak(InputBlock& audio, OutputBlock& db, OutputBlock& rollbuffer, int writePos);

	float processSingleNoBallistics(float x, float threshDb, float ratio, float knee=0, float pregain=0, float postgain=0);
	
	bool gainReductionStep(const float* inDb, int numSamples, int delaySamples, float& downDbLevel, float& downStep, float* outDb, bool breakOnPeak, float gain);
//	void smoothGainReduction(OutputBlock& gainDb, int delaySamples, InputBlock& incomingDb);
	void smoothGainReduction(AudioBuffer<float>& gainDb, int writePos, int delaySamples, OutputBlock& incomingDb, float gain);

	void smoothGainReduction(AudioBlock<float>& incomingDb, MonoRingBuffer& dbRing, float gain);
	
	
	void _applyGainReplacingMono	(float* gain, 		OutputBlock& out, InputBlock& in,
									 int offset, int numSamples);
	void _applyGainReplacingStereo	(float* gain, 		OutputBlock& out, InputBlock& in,
									 int offset, int numSamples, int c);
	void _applyGainNRMono			(const float* gain, OutputBlock& out, InputBlock& in,
									 int offset, int numSamples);
	void _applyGainNRStereo			(const float* gain, OutputBlock& out, InputBlock& in,
									 int offset, int numSamples, int c);

	
	inline float _meanSquared(InputBlock& block, const int& n) {
		float total=0;
		for (int c=0; c < block.getNumChannels(); ++c)
		{
			total += pow(block[c][n], 2);
		}
		return total;
	}
	
	inline float _meanSquared(InputBlock& block, const int& n, const float& factor) {
		float total=0;
		for (int c=0; c < block.getNumChannels(); ++c)
		{
			total += pow(block[c][n], 2);
		}
		return total * factor;
	}
	inline float _getMaxSampleAcrossChannels(int n, InputBlock& block, AudioBuffer<float>& buffer, int writePtr) {
		if (n < block.getNumSamples()) {
			float max=0;
			for (int c=0; c < block.getNumChannels(); ++c)
			{
				max = std::max(max, fabs(block.getSample(c, n)));
			}
			return max;
		}
		else {
			n = (n-block.getNumSamples() + writePtr) % buffer.getNumSamples();
			float max=0;
			for (int c=0; c < buffer.getNumChannels(); ++c)
			{
				max = std::max(max, fabs(buffer.getSample(c, n)));
			}
			return max;
		}
	}
	
};


#endif /* CompressorCommon_hpp */
