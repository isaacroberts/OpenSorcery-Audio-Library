


/*

class  Env
{
public:
    Env()
    {
    }

    // Returns true if the envelope is in its attack, decay, sustain or release stage.
    bool isActive() const noexcept
    {
    
	}

    // Sets the sample rate that will be used for the envelope.
    void setSampleRate (double newSampleRate) noexcept
    {
        jassert (newSampleRate > 0.0);
        sampleRate = newSampleRate;
    }

    // Resets the envelope to an idle state.
    void reset() noexcept
    {
    }

    // Starts the attack phase of the envelope.
    void noteOn() noexcept
    {
    }

    // Starts the release phase of the envelope.
    void noteOff() noexcept
    {
    }

    // Returns the next sample value for an Env object.
    float getNextSample() noexcept
    {
    }

	void advanceSamples(int numSamples)
	{
	}
private:
    double sampleRate = 44100.0;
};

*/


/*
	JUCE UPDATE
	
	Go to juce_audio_basics > utilities > juce_ADSR and add this:
			
	void advanceSamples(int numSamples)
	{
		for (int n=0; n < numSamples; ++n)
		{
			getNextSample();
		}
		return;
		
		//TODO: Remove the above and test this
		
		if (state==State::idle)
		{
			return;
		}

		int remaining = numSamples;
		if ( state == State::attack)
		{
			float end = envelopeVal + attackRate * remaining;
			if (end > 1.0f)
			{
				int dif = (end-1.0f)/attackRate;
				remaining -= dif;
				envelopeVal = 1.0f;
				goToNextState();
			}
			else
			{
				envelopeVal = end;
				return;
			}
		}
		if (remaining <= 0) return;
		
		if (state== State::decay)
		{
			float end = envelopeVal - decayRate * remaining;
			
			if (end <= parameters.sustain)
			{
				int dif = (parameters.sustain - end)/decayRate;
				remaining -= dif;
				envelopeVal= parameters.sustain;
				goToNextState();
			}
			else
			{
				envelopeVal = end;
				return;
			}
		}
		if (remaining <= 0) return;

		if (state == State::sustain)
		{
			envelopeVal = parameters.sustain;
			return;
		}
		
		if (state == State::release)
		{
			float end = envelopeVal - releaseRate * remaining;
			
			if (end <= 0)
			{
//				int dif = (0-end)/releaseRate;
//				remaining -= dif;
				envelopeVal = 0;
				goToNextState();
			}
			else
			{
				envelopeVal = end;
			}
		}
	}

}


*/
