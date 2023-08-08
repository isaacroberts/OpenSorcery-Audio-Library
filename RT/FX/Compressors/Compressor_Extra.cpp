
#include "CompressorCommon.h"


/*
		This is mostly stuff from the OTT that I put into the Common file because I have no understanding of scale


*/

	
void Compressors::Common::ffToDb(InputBlock& levelBlock, OutputBlock& dbBlock, float rate, bool mono)
{
	if (ratio >=1)
	{
		dbBlock.fill(pregain);
		return;
	}
	
	//	std::cout<<"common Proc"<<std::endl;
	int N = (int)levelBlock.getNumSamples();
	int C = (int)levelBlock.getNumChannels();
	
	jassert (!mono || C==dbBlock.getNumChannels());
	
	float slope = 1-ratio;
	
	float alphaAtt = alpha(attack, rate);
	float alphaRel = alpha(release, rate);
		
	float x_T;
	
	if (mono) {
		for (int n=0; n < N; ++n)
		{
			float level = peakSampleLevel(levelBlock, n);
			level += pregain;
			
			float overshoot = level - threshold;
			
			x_T = doShoot(overshoot, kneewidth, slope);
			y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);
			
			float control_db = pregain - y_t;
			
			dbBlock.setSample(0, n, control_db);
		}
	}
	else {
		for (int n=0; n < N; ++n)
		{
			for (int c=0; c< C; ++c)
			{
				float level = getSampleLevel(levelBlock[c][n]);
				level += pregain;
				
				float overshoot = level - threshold;
				
				x_T = doShoot(overshoot, kneewidth, slope);
				y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);
				
				float control_db = pregain - y_t;
				
				dbBlock.setSample(c, n, control_db);
			}
		}
	}
}
void Compressors::Common::ffToDb(AudioBuffer<float>& levelBlock, int writePos, OutputBlock& dbBlock, float rate, ReadType type, bool mono)
{
	//Pregain/postgain works differently on this function
	jassertfalse;
	
	if (ratio >=1)
	{
		dbBlock.fill(pregain+postgain);
		return;
	}
	
	//	std::cout<<"common Proc"<<std::endl;
	int BN = (int)levelBlock.getNumSamples();
	int N = (int)dbBlock.getNumSamples();
	int C = (int)levelBlock.getNumChannels();
	
	jassert (!mono || C==dbBlock.getNumChannels());
	
	float slope = 1-ratio;
	
	float alphaAtt = alpha(attack, rate);
	float alphaRel = alpha(release, rate);
			
	float x_T;
	
	for (int n=0; n < N; ++n)
	{
		float level = getSampleLevel(levelBlock, (n+writePos)%BN, type);
		level += pregain;
		
		float overshoot = level - threshold;
		
		x_T = doShoot(overshoot, kneewidth, slope);
		y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);

		float control_db = pregain + postgain - y_t;
								
		if (mono) {
			dbBlock.setSample(0, n, control_db);
		}
		else {
			for (int c=0;c< C; ++c)
			{
				dbBlock.setSample(c, n, control_db);
			}
		}
	}
}
void Compressors::Common::ffToDb(AudioBuffer<float>& levelBlock, AudioBuffer<float>& dbBlock, int writePos, int numSamples, float rate, ReadType type, bool mono)
{
	//Pregain/postgain works differently on this function
	jassertfalse;
	
	if (ratio >=1)
	{
		if (mono)
		{
			FloatVectorOperations::fill(dbBlock.getWritePointer(0), pregain+postgain, dbBlock.getNumSamples());
		}
		else {
			for (int c=0; c < dbBlock.getNumChannels(); ++c)
			{
				FloatVectorOperations::fill(dbBlock.getWritePointer(c), pregain+postgain, dbBlock.getNumSamples());
			}
		}
		return;
	}
	
	//	std::cout<<"common Proc"<<std::endl;
	int BN = (int)levelBlock.getNumSamples();
//		int N = (int)dbBlock.getNumSamples();
	int C = (int)levelBlock.getNumChannels();
	
	jassert (!mono || C==dbBlock.getNumChannels());
	
	float slope = 1-ratio;
	
	float alphaAtt = alpha(attack, rate);
	float alphaRel = alpha(release, rate);
			
	float x_T;
	
	for (int n=0; n < numSamples; ++n)
	{
		int i = (n+writePos) % BN;
		float level = getSampleLevel(levelBlock, i, type);
		level += pregain;
		
		float overshoot = level - threshold;
		
		x_T = doShoot(overshoot, kneewidth, slope);
		y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);

		float control_db = pregain + postgain - y_t;
								
		if (mono) {
			dbBlock.setSample(0, i, control_db);
		}
		else {
			for (int c=0;c< C; ++c)
			{
				dbBlock.setSample(c, i, control_db);
			}
		}
	}
}
void Compressors::Common::ffToDb(InputBlock& levelBlock, AudioBuffer<float>& dbBlock, int writePos, float rate, ReadType type, bool mono)
{
	//Pregain/postgain works differently on this function
	jassertfalse;
	
	if (ratio >=1)
	{
		if (mono)
		{
			FloatVectorOperations::fill(dbBlock.getWritePointer(0), pregain+postgain, dbBlock.getNumSamples());
		}
		else {
			for (int c=0; c < dbBlock.getNumChannels(); ++c)
			{
				FloatVectorOperations::fill(dbBlock.getWritePointer(c), pregain+postgain, dbBlock.getNumSamples());
			}
		}
		return;
	}
	
	//	std::cout<<"common Proc"<<std::endl;
	int BN = (int)dbBlock.getNumSamples();
	int N = (int)levelBlock.getNumSamples();
	int C = (int)levelBlock.getNumChannels();
	
	jassert (!mono || C==dbBlock.getNumChannels());
	
	float slope = 1-ratio;
	
	float alphaAtt = alpha(attack, rate);
	float alphaRel = alpha(release, rate);
			
	float x_T;
	
	for (int n=0; n < N; ++n)
	{
		int i = (n+writePos) % BN;

		float level = getSampleLevel(levelBlock, n, type);
		level += pregain;
		
		float overshoot = level - threshold;
		
		x_T = doShoot(overshoot, kneewidth, slope);
		y_t = switchStep(y_t, x_T, alphaAtt, alphaRel);

		float control_db = pregain + postgain - y_t;
								
		if (mono) {
			dbBlock.setSample(0, i, control_db);
		}
		else {
			for (int c=0;c< C; ++c)
			{
				dbBlock.setSample(c, i, control_db);
			}
		}
	}
}



void Compressors::rmsDbLookahead(InputBlock& input, AudioBuffer<float>& buffer, int writePos, OutputBlock& dbBlock)
{
	/* This function tracks a block of RMS samples
		by keeping a running sum, adding the next element
		and subtracting the previous element
		RMS = sqrt ( sum / amt );
		
		The sqrt has also been optimized out by moving it to the log function.
		log(sqrt(x)) = log(x)/2
	 
	 */
	jassert(input.getNumChannels() == buffer.getNumChannels());
	
	int C = (int)input.getNumChannels();
	int N = (int)input.getNumSamples();
	
	int delaySamples = buffer.getNumSamples();
	
	float* db = dbBlock[0];

	if (delaySamples==0)
	{
		const float MEANF = (1.f/C);
		for (int n=0; n < N; ++n)
		{
			db[n] = _meanSquared(input, n, MEANF);
		}
	}
	else
	{
		const float MEANF = 1.f/(C  * delaySamples);
		
		float curTotal=0;
		
		int remainingSamples = delaySamples;
		
		//DelaySamples worth of input
		{
			//First samples from input
			for (int n=0; n < remainingSamples && n < N; ++n)
				curTotal += _meanSquared(input, n, MEANF);
			remainingSamples -= N;
							
			const int bufferWrap = delaySamples - writePos;

			//First half of buffer
			for (int n = 0; n < bufferWrap && n < remainingSamples; ++n)
				curTotal += _meanSquared(buffer, n+writePos, MEANF);
			remainingSamples -= bufferWrap;
			//Second half of buffer
			jassert(remainingSamples<=writePos);
			for (int n=0; n < remainingSamples; ++n)
				curTotal += _meanSquared(buffer, n, MEANF);
		}
		//Set 0th element
		db[0] = (curTotal);
			
		//Where n+delay is within input
		for (int n=1; n+delaySamples < N; ++n)
		{
			curTotal += _meanSquared(input, n+delaySamples, MEANF);
			//Subtract previous
			curTotal -= _meanSquared(input, n-1, MEANF);
			
			db[n]= (curTotal);
		}
		//n = N-delaySamples
		//Where n+delay is within buffer
		for (int n=std::max(N-delaySamples, 1); n < N; ++n)
		{
			//(N+writePos + delay) % delaySamples = (N+writePos)%delaySamples
			curTotal += _meanSquared(buffer, (n+writePos)%delaySamples, MEANF);
			//Subtract previous
			curTotal -= _meanSquared(input, n-1, MEANF);
			
			db[n]= (curTotal);
		}
		
		
	}
	//Convert to db
	for (int n=0; n < N; ++n)
	{
		jassert(db[n]>=0);
		//log(sqrt(x)) = log(x)/2
		db[n] = 10 * log10(db[n]+1.e-44);
	}
}
void Compressors::peakDbLookahead(InputBlock& input, AudioBuffer<float>& buffer, int bufferWP, OutputBlock& dbBlock)
{
	const int delaySamples = buffer.getNumSamples();
	
	float* db = dbBlock[0];
	
	if (delaySamples == 0)
	{
		size_t N = input.getNumSamples();
		size_t C = input.getNumChannels();
		for (int n=0; n < N; ++n)
		{
			db[n] = 0;
			for (int c=0; c < C; ++c)
			{
				db[n] = std::max(fabs(input.getSample(c, n)), db[n]);
			}
		}
	}
	else
	{
		int writePtr = (int)input.getNumSamples()-1;
		int maxPtr= writePtr + delaySamples;
		float curMax=_getMaxSampleAcrossChannels(maxPtr, input, buffer, bufferWP);

		//TODO: Test
		while (writePtr >= 0)
		{
			float writeMax = _getMaxSampleAcrossChannels(writePtr, input, buffer, bufferWP);
			
			if (writeMax >= curMax) {
				curMax = writeMax;
				maxPtr = writePtr;
			}
			
			if (maxPtr > writePtr + delaySamples)//If max out of range
			{
				curMax = -1.f;
				while (curMax <= writeMax && maxPtr > writePtr) {
					--maxPtr;
					curMax = _getMaxSampleAcrossChannels(maxPtr, input, buffer, bufferWP);
				}
			}
			db[writePtr] = curMax;
			
			--writePtr;
		}
	}
	//Convert to db
	for (int n=0; n < input.getNumSamples(); ++n)
	{
		jassert(db[n]>=0);
		db[n] = 20 * log10(db[n]+1.e-44);
	}
}

void Compressors::rollingRMS(InputBlock& input, OutputBlock& dbBlock, OutputBlock& rollBuffer, int writePos)
{
	int C = (int)input.getNumChannels();
	int N = (int)input.getNumSamples();

	int N2 = (int)rollBuffer.getNumSamples();
	
	float* db = dbBlock[0];

	int width = (int)rollBuffer.getNumSamples();
	
	if (width == 0)
	{
//		const float MEAN2 = (1.f/C);
		for (int n=0; n < N; ++n)
		{
			db[n] = _meanSquared(input, n);
		}
		
		FloatVectorOperations::multiply(db, 1.f/(C), width);
	}
	else
	{
		//Total up rollbuffer
		float curTotal = 0;
		
		for (int n= 0; n < N2; ++n)
		{
			curTotal += _meanSquared(rollBuffer, n);
		}
		db[0]=curTotal;

		int prevN = std::min(width, N);
		//Subtract previous roll buffer and add new input
		for (int n=0; n < prevN; ++n)
		{
			//Subtract previous
			int rbPos = (writePos+n)%width;
			curTotal -= _meanSquared(rollBuffer, rbPos);
			curTotal += _meanSquared(input, n);

			if (curTotal < 0) curTotal = 0;

			db[n]= (curTotal);
		}

//		Add new input
		for (int n=prevN; n < N; ++n)
		{
			curTotal -= _meanSquared(input, n-width);
			curTotal += _meanSquared(input, n);

			if (curTotal < 0) curTotal = 0;

			db[n]= (curTotal);
		}

//		const float MEANF = 1.f/(C  * width);
		FloatVectorOperations::multiply(db, 1.f/(C  * width), width);
	}
	
	//Convert to db
	for (int n=0; n < N; ++n)
	{
		jassert(db[n]>=0);
		db[n] = 20 * log10(db[n]+1.e-44);
	}
}
float _getMaxSampleAcrossChannels2(int n, InputBlock& block, OutputBlock& buffer, int writePtr) {
	if (n > 0) {
		float max=0;
		for (int c=0; c < block.getNumChannels(); ++c)
			max = std::max(max, fabs(block.getSample(c, n)));
		return max;
	}
	else
	{
		int B = (int)buffer.getNumSamples();
		n = (n + writePtr) % B;
		//avoid negative
		n = (n + B) % B;
		float max=0;
		for (int c=0; c < buffer.getNumChannels(); ++c)
			max = std::max(max, fabs(buffer.getSample(c, n)));
		return max;
	}
}
void Compressors::rollingPeak(InputBlock& input, OutputBlock& dbBlock, OutputBlock& buffer, int writePos)
{
	int C = (int)input.getNumChannels();
	int N = (int)input.getNumSamples();

	int width = (int)buffer.getNumSamples();
//	int N2 = width;
//	int delaySamples = width;
	
	float* db = dbBlock[0];
		
	if (width == 0)
	{
		for (int n=0; n < N; ++n)
		{
			db[n] = 0;
			for (int c=0; c < C; ++c)
			{
				db[n] = std::max(fabs(input.getSample(c, n)), db[n]);
			}
		}
	}
	else
	{
//		int writePtr = N- 1;
//		int maxPtr= writePtr + delaySamples;
//		float curMax=_getMaxSampleAcrossChannels2(maxPtr, input, buffer, writePos);
//
//		while (writePtr >= 0)
//		{
//			float writeMax = _getMaxSampleAcrossChannels2(writePtr, input, buffer, writePos);
//
//			if (writeMax >= curMax) {
//				curMax = writeMax;
//				maxPtr = writePtr;
//			}
//
//			if (maxPtr > writePtr + delaySamples)//If OOB
//			{
//				curMax = -1;
//				while (curMax <= writeMax && maxPtr > writePtr) {
//					--maxPtr;
//					curMax = _getMaxSampleAcrossChannels2(maxPtr, input, buffer, writePos);
//				}
//			}
//			db[writePtr] = curMax;
//
//			--writePtr;
//		}
		
		//Theoretically this is correct
		int maxPtr= -width;
		int writePtr = 0;
		float curMax=_getMaxSampleAcrossChannels2(maxPtr, input, buffer, writePos);

		while (writePtr < N)
		{
			float writeMax = _getMaxSampleAcrossChannels2(writePtr, input, buffer, writePos);
			
			if (writeMax >= curMax) {
				curMax = writeMax;
				maxPtr = writePtr;
			}
			if (maxPtr > writePtr + width)//If max ptr out of range
			{
				curMax = -1.f;
				while (curMax <= writeMax && maxPtr < writePtr) {
					++maxPtr;
					curMax = _getMaxSampleAcrossChannels2(maxPtr, input, buffer, writePos);
				}
			}
			db[writePtr] = curMax;
			
			++writePtr;
		}
	}
	//Convert to db
	for (int n=0; n < input.getNumSamples(); ++n)
	{
		jassert(db[n]>=0);
		db[n] = 20 * log10(db[n]+1.e-44);
	}
}

//void Compressors::gainReductionStepFlat(const float* inDb, int numSamples, int delaySamples, float& downLevel, float& downStep, float& upLevel, float& upStep, float* outDb, bool breakOnPeak)
//{
////	return;
//
//	downStep = 40.f / delaySamples;
//
//	for (int index=numSamples-1; index >= 0; --index)
//	{
//		const float smpl = inDb[index];
//
//		if (smpl > downLevel)
//		{
//			if (outDb)
//			{
//				outDb[index] = downLevel;
//			}
//		}
//		else if (smpl < 0)//dn compression
//		{
////			if (
//			if (smpl < downLevel) // otherwise... (new peak)
//			{
//				//				downStep = - smpl / delaySamples; // calculate the new slope
//				downLevel = smpl; // and also the new ramp value
//			}
//		}
//		else //up compression
//		{
////			float x = smpl;
////			x++;
//		}
//
//		if (downLevel < 200)
//			downLevel += downStep; // and update the next ramp value
//	}
//}
//void Compressors::smoothGainReductionFlat(OutputBlock& gainDbBlock, int delaySamples, InputBlock& newDbBlock)
//{
//
//	/** The basic idea here is to look for high gain-reduction values in the signal, and apply a fade which starts exactly  `delayInSamples` many samples before that value appears. Depending on the value itself, the slope of the fade will vary.
//	 Some things to note:
//		 - as the samples are gain-reduction values in decibel, we actually look for negative peaks or local minima.
//		 - it's easier for us to time-reverse our search, so we start with the last sample
//		 - once we find a minimum, we calculate the slope, which will be called `step`
//		 - with that slope, we can calculate the next value of our fade-in `nextGainReductionValue`
//		 - once a value in the buffer is below our fade-in value, we found a new minimum, which might not be as deep as the previous one, but as it comes in earlier, it needs more attention, so we update our fade-in slope
//
//		 github.com/DanielRudrich/SimpleCompressor
//	 */
//
//	/*
//	 First, what if the new incoming samples have such a high gain-reduction value, that they need a fade-in? So we have to apply a gain-ramp even further into the past. And that is exactly the reason why we need lookahead, why we need to buffer our signal for a short amount of time: so we can apply that gain ramp for the first handful of gain-reduction samples.
//	 */
//
//	float downLevel = 0.0f;
//	float downStep = 0.0f;
//	float upLevel = 0.0f;
//	float upStep = 0.0f;
//
//	int N = (int)gainDbBlock.getNumSamples();
//
//	//For this, we only need to check `delayInSamples` many samples.
//	int lookaheadAmt = std::min(delaySamples, (int)newDbBlock.getNumSamples());
//
//	//Track incoming DB to account for incoming ramp
//	gainReductionStepFlat(newDbBlock[0], lookaheadAmt, delaySamples, downLevel, downStep, upLevel, upStep, NULL);
//
//	//Next, apply ramp to incoming db
//	gainReductionStepFlat(gainDbBlock[0], N, delaySamples, downLevel, downStep, upLevel, upStep, gainDbBlock[0]);
//
//	float* gainDb = gainDbBlock[0];
//
//	//Forward pass to ramp upwards compression
//	upLevel = std::max(gainDb[0], 0.f);
//	upStep=0;
//}

//void Compressors::smoothGainReduction(OutputBlock& gainDbBlock, int delaySamples, InputBlock& newDbBlock)
//{
//	float downLevel = 0.0f;
//	float downStep = 0.0f;
//
//	float upLevel, upStep;
//
//	int N = (int)gainDbBlock.getNumSamples();
//
//	//For this, we only need to check `delayInSamples` many samples.
//	//Currently not useful since blockSize = delaySamples
//	int lookaheadAmt = std::min(delaySamples, (int)newDbBlock.getNumSamples());
//
//	//Track new DB to account for incoming ramp
//	gainReductionStepVarying(newDbBlock[0], lookaheadAmt, delaySamples, downLevel, downStep, upLevel, upStep, NULL, false);
//
//	//Next, apply ramp to current db
//	gainReductionStepVarying(gainDbBlock[0], N, delaySamples, downLevel, downStep, upLevel, upStep, gainDbBlock[0], true);
//}

bool Compressors::gainReductionStep(const float* inDb, int numSamples, int delaySamples, float& downLevel, float& downStep, float* outDb, bool breakOnPeak, float gain)
{
	for (int index=numSamples-1; index >= 0; --index)
	{
		const float smpl = inDb[index];
		
		if (smpl > downLevel)
		{
			if (outDb)
			{
				outDb[index] = downLevel;
			}
		}
		else// otherwise... (new peak)
		{
			if (breakOnPeak) return true;
			downStep =  ((gain-smpl) / delaySamples); // calculate the new slope
			downLevel = smpl; // and also the new ramp value
		}
		downLevel += downStep; // and update the next ramp value
	}
	return false;
}
void Compressors::smoothGainReduction(AudioBuffer<float>& gainDbBlock, int writePos, int delaySamples, OutputBlock& newDbBlock, float gain)
{
	float downLevel = gain;
	float downStep = 0.0f;
	
	int N = (int)gainDbBlock.getNumSamples();
	
	//For this, we only need to check `delayInSamples` many samples.
	//	int lookaheadAmt = std::min(delaySamples, (int)newDbBlock.getNumSamples());
	int lookaheadAmt = (int)newDbBlock.getNumSamples();
	
	auto* wp = gainDbBlock.getWritePointer(0);
	
	//Track new DB to account for incoming ramp
	gainReductionStep(newDbBlock[0], lookaheadAmt, delaySamples, downLevel, downStep, newDbBlock[0], false, gain);
	
	//Next, apply ramp to current db
	bool brk = gainReductionStep(wp, writePos, delaySamples, downLevel, downStep, wp, true, gain);
	if (!brk)
		gainReductionStep(wp+writePos, N-writePos, delaySamples, downLevel, downStep, wp+writePos, true, gain);
}

void Compressors::smoothGainReduction(AudioBlock<float>& newDbBlock, MonoRingBuffer& dbRing, float gain)
{
	float downLevel = gain;
	float downStep = 0.0f;
	
	int ringSize = (int)dbRing.size();
	int newSmp = (int)newDbBlock.getNumSamples();
	
	auto* newDb = newDbBlock[0];
	
	//Track new DB to account for incoming ramp
	for (int index=newSmp-1; index >= 0; --index)
	{
		const float smpl = newDb[index];
		
		if (smpl > downLevel)
		{
			newDb[index] = downLevel;
		}
		else// otherwise... (new peak)
		{
			downStep =  ((gain-smpl) / ringSize); // calculate the new slope
			downLevel = smpl; // and also the new ramp value
		}
		downLevel += downStep; // and update the next ramp value
	}

//	//Next, apply ramp to current db
	for (int index=ringSize-1; index >= 0; --index)
	{
		const float smpl = dbRing.atWrite(index);
		
		if (smpl > downLevel)
		{
			dbRing.atWrite(index) = downLevel;
		}
		else// otherwise... (new peak)
		{
			//No need to continue propagating - this has already been propagated
			return;
		}
		downLevel += downStep; // and update the next ramp value
	}

	
	
}


void Compressors::_applyGainReplacingMono(float* db, OutputBlock& out, InputBlock& in, int o, int N)
{
	N = std::min(N, (int)out.getNumSamples() - o);
	if (N<=0) return;
	for (int n=0; n < N; ++n)
	{
		db[n] = pow(10.f, db[n]/20);
	}
	int C = (int)out.getNumChannels();
	for (int c=0; c < C; ++c)
	{
		FloatVectorOperations::multiply(out[c]+o, in[c]+o, db, N);
	}
}
void Compressors::_applyGainReplacingStereo(float* db, OutputBlock& out, InputBlock& in, int o, int N, int c)
{
	N = std::min(N, (int)out.getNumSamples() - o);
	if (N<=0) return;
	for (int n=0; n < N; ++n)
	{
		db[n] = pow(10.f, db[n]/20);
	}
	FloatVectorOperations::multiply(out[c]+o, in[c]+o, db, N);
}
void Compressors::_applyGainNRMono(const float* db, OutputBlock& out, InputBlock& in, int offset, int N)
{
	N = std::min(N, (int)out.getNumSamples() - offset);
	if (N<=0) return;
	int C = (int)out.getNumChannels();
	for (int n=0; n < N; ++n)
	{
		float gain = pow(10.f, db[n]/20);
		int no = n+offset;
		for (int c=0; c < C; ++c) {
			out[c][no] = in[c][no] * gain;
		}
	}
}
void Compressors::_applyGainNRStereo(const float* db, OutputBlock& out, InputBlock& in, int offset, int N, int c)
{
	N = std::min(N, (int)out.getNumSamples() - offset);
	if (N<=0) return;
	for (int n=0; n < N; ++n)
	{
		float gain = pow(10.f, db[n]/20);
		int no = n+offset;
		out[c][no] = in[c][no] * gain;
	}
}
void Compressors::applyGain(OutputBlock& out, InputBlock& in, AudioBuffer<float>& dbBlock, int writePos, InputBlock& newDb, bool mono)
{
	const int ON= (int)out.getNumSamples();
	const int BN= (int)dbBlock.getNumSamples();
	
	const int bn1 = BN-writePos;
	if (mono) {
		const float* db = dbBlock.getReadPointer(0);
		_applyGainNRMono(db+writePos, out, in, 0, bn1);
		_applyGainNRMono(db, out, in, bn1, writePos);
		_applyGainNRMono(newDb[0], out, in, BN, ON-BN);
	}
	else {
		const int C = (int)out.getNumChannels();
		for (int c=0; c < C; ++c)
		{
			const float* db = dbBlock.getReadPointer(c);
			_applyGainNRStereo(db+writePos, out, in, 0, bn1, c);
			_applyGainNRStereo(db, out, in, bn1, writePos, c);
			_applyGainNRStereo(newDb[c], out, in, BN, ON-BN, c);
		}
	}
}

void Compressors::applyGain(OutputBlock& out, InputBlock& in, OutputBlock& gain, bool mono)
{
	const int N = (int)out.getNumSamples();
	if (mono)
		_applyGainReplacingMono(gain[0], out, in, 0, N);
	else {
		const int C = (int)out.getNumChannels();
		for (int c=0; c < C; ++c)
			_applyGainReplacingStereo(gain[c], out, in, 0, N, c);
	}
}
