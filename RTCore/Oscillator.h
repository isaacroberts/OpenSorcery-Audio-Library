
#pragma once

namespace Osc {
	class Lator {
		
		float freq=10;
		float phase=0;
		float depth=1;
		
	public:
		Lator() {}
		Lator(float freq_, float depth_)
		: freq(freq_), depth(depth_)
		{
			jassert(freq > 0);
		}
		
		void setFreq(float set) {
			freq = set;
			jassert(freq > 0);
		}
		void setDepth(float set) {
			depth = set;
		}
		
		inline float tick(float rate) {
			float x = getValue(rate);
			advance(rate);
			return x;
		}
		
		
		inline float getValue(float rate) {
			return depth*sin(phase);
		}
		inline void advance(float rate) {
			phase += freq/rate;
		}
		
		
		void reset() {
			phase=0;
		}
	};
		
	class ChannelLator {
		
		float freq=10;
		std::vector<float> phase;
		float depth=1;
		
	public:
		ChannelLator() {}
		ChannelLator(float freq_, float depth_)
		: freq(freq_), depth(depth_)
		{
			jassert(freq > 0);
		}
		
		void setNumChannels(int set) {
			phase.resize(set);
		}
		
		void setFreq(float set) {
			freq = set;
			jassert(freq > 0);
		}
		void setDepth(float set) {
			depth = set;
		}
		
		inline float tick(int c, float rate) {
			float x = getValue(c, rate);
			advance(c, rate);
			return x;
		}
		
		
		inline float getValue(int c, float rate) {
			return depth*sin(phase[c]);
		}
		inline void advance(int c, float rate) {
			phase[c] += freq/rate;
		}
		
		
		void reset() {
			for (int c=0; c < phase.size(); ++c)
				phase[c]=0;
		}
	};

}
