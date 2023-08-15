

//#include "ReverbCommon.cpp"
//
//#include "RTSingleReverb.cpp"
//#include "RTFastReverb.cpp"
#include "RTJuceReverb.cpp"
//#include "RTMobileReverb.cpp"
#include "SpectrumReverb.cpp"
//
#include "LofiReverb.cpp"


/*
JUCE UPDATE 

juce_dsp > frequency > juce_Convolution.h 

	In main class:

		template <typename ProcessContext,
				  std::enable_if_t<std::is_same<typename ProcessContext::SampleType, float>::value, int> = 0>
		void bypassProcess (const ProcessContext& context) noexcept
		{
			processSamples (context.getInputBlock(), context.getOutputBlock(), true);
		}


	in Mixer class:

			bool isTailing() const;



juce_Convolution.cpp:

	bool Convolution::Mixer::isTailing() const
	{
		if (!currentIsBypassed)
			return true;
		else
		{
			return volumeDry[0].isSmoothing();
		}
	}

	bool Convolution::isTailing() const
	{
		return mixer.isTailing();
	}


*/