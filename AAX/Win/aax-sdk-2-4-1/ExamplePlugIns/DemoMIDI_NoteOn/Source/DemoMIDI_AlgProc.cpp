/*================================================================================================*/
/*
 *	Copyright 2013-2017, 2019, 2021 by Avid Technology, Inc.
 *	All rights reserved.
 *
 *	CONFIDENTIAL: This document contains confidential information.  Do not 
 *	read or examine this document unless you are an Avid Technology employee
 *	or have signed a non-disclosure agreement with Avid Technology which protects
 *	the	confidentiality of this document.  DO NOT DISCLOSE ANY INFORMATION 
 *	CONTAINED IN THIS DOCUMENT TO ANY THIRD-PARTY WITHOUT THE PRIOR WRITTEN	CONSENT 
 *	OF Avid Technology, INC.
 */
/*================================================================================================*/

// Component includes
#include "DemoMIDI_Alg.h"

// AAX includes
#include "AAX.h"
#include "AAX_MIDIUtilities.h"

// Standard includes
#include <math.h>		// for fabsf(), sinf()
#include <algorithm>	// for max()


// Local helper functions
namespace {
	inline void ClearNoteBits(SDemoMIDI_NoteBits* oNoteBits)
	{
		if (oNoteBits)
		{
			oNoteBits->mIsOn[0] = 0x00000000;
			oNoteBits->mIsOn[1] = 0x00000000;
			oNoteBits->mIsOn[2] = 0x00000000;
			oNoteBits->mIsOn[3] = 0x00000000;
		}
	}
}

//==============================================================================
// Processing function definition
//==============================================================================

template<int kNumChannelsIn, int kNumChannelsOut> 
void
AAX_CALLBACK
DemoMIDI_AlgorithmProcessFunction (
								   SDemoMIDI_Alg_Context * const	inInstancesBegin [],
								   const void *					inInstancesEnd)
{
	// Get a pointer to the beginning of the memory block table
	SDemoMIDI_Alg_Context* AAX_RESTRICT instance = inInstancesBegin [0];
	
	
	//--------- Iterate over plug-in instances ---------//
	//
	for (SDemoMIDI_Alg_Context * const * walk = inInstancesBegin; walk < inInstancesEnd; ++walk)
	{
		instance = *walk;
		
		//--------- Retrieve instance-specific information ---------//
		//
		// Memory blocks
		const int32_t		bufferSize	= *instance->mBufferSizeP;
		const int32_t		bypass		= *instance->mCtrlBypassP;
		const float			gain		= *instance->mGainP;
		const bool			holdNotes	= (*instance->mCtrlHoldP != 0);
		float*				meterTaps	= *instance->mMetersPP;
		SDemoMIDI_NoteBits* const notes	= instance->mIsOn;
		
		// Setup MIDI In node pointers 
		AAX_IMIDINode* midiInNode = instance->mMIDINodeInP;
		const AAX_CMidiStream* midiInStream = midiInNode->GetNodeBuffer();
		const AAX_CMidiPacket* midiInPacketPtr = midiInStream->mBuffer;
		uint32_t inputPacketsRemaining = midiInStream->mBufferSize;
		
		// Setup MIDI Out node pointers
		AAX_IMIDINode* const midiOutPassThroughNode = instance->mMIDINodeOutPassThroughP;
		
		// Setup per-instance data
		const bool			holdChanged = (holdNotes != (notes->mHoldState != 0));
		
		// Handle parameter changes
		//
		// Example of MIDI output: turn all notes off when the note hold coefficient state switches from on to off
		if (holdChanged && !holdNotes)
		{
			ClearNoteBits(notes);
			
			// Individual NoteOffs
			for (int i = 0; i < 128; ++i)
			{
				AAX_CMidiPacket midiOutPacket;
				midiOutPacket.mTimestamp = 0;
				midiOutPacket.mLength = 3;
				midiOutPacket.mData[0] = AAX::eStatusNibble_NoteOff | 0x00;
				midiOutPacket.mData[1] = (unsigned char)i;
				midiOutPacket.mData[2] = 0x00;
				midiOutPacket.mData[3] = 0x00;
				midiOutPacket.mIsImmediate = 1;
								
				// Post the MIDI packet
				midiOutPassThroughNode->PostMIDIPacket(&midiOutPacket);
			}
		}
		
		//--------- Run processing loop over each channel ---------//
		//
		for (int ch = 0; ch < kNumChannelsIn; ch++)
		{
			//--------- Run processing loop over each sample ---------//
			//
			for (int32_t t = 0; t < bufferSize; t++)
			{
				// Set up audio sample pointers
				//
				// Audio channels
				float* const AAX_RESTRICT pdI = instance->mInputPP [0]; // First channel
				float* const AAX_RESTRICT pdO = instance->mOutputPP [0]; // First channel
				
				while ((0 < inputPacketsRemaining) &&
					   (NULL != midiInPacketPtr) &&
					   (midiInPacketPtr->mTimestamp <= t))
				{
					bool passPacket = false;
					
					// Handle the MIDI packet
					//
					// Switch note-is-on bit flags on or off in the SDemoMIDI_NoteBits bit
					// array as note on and note off messages are received.
					{
						const uint8_t data1 = midiInPacketPtr->mData[1];
						const uint8_t keySegment = data1/32; // (intentional int division) Segment in SDemoMIDI_NoteBits
						const uint8_t keyOffset = data1 % 32; // Offset in SDemoMIDI_NoteBits
						
						if (AAX::IsNoteOff(midiInPacketPtr))
						{
							// only send note off events if notes are not held
							if (!holdNotes)
							{
								passPacket = true;
								const uint32_t mask = ~(0x01 << keyOffset);
								notes->mIsOn[keySegment] &= mask;
							}
						}
						else if (AAX::IsNoteOn(midiInPacketPtr))
						{
							passPacket = true;
							const uint32_t mask = (0x01 << keyOffset);
							notes->mIsOn[keySegment] |= mask;
						}
						else if (AAX::IsAllNotesOff(midiInPacketPtr))
						{
							// pass an all notes off event even if notes are held
							passPacket = true;
							ClearNoteBits(notes);
						}
					}
					
					// Example of MIDI output: perform MIDI packet pass-through
					if (NULL != midiOutPassThroughNode && true == passPacket)
					{
						AAX_CMidiPacket midiOutPacket;
						
						// Copy the packet from the input MIDI node to the output MIDI node
						memcpy(&midiOutPacket, midiInPacketPtr, sizeof(AAX_CMidiPacket));
						
						// Post the MIDI packet
						midiOutPassThroughNode->PostMIDIPacket(&midiOutPacket);
					}
					
					// Increment to the next MIDI packet
					++midiInPacketPtr;
					--inputPacketsRemaining;
				}
				
				// Process Audio Samples
				const bool anyNoteIsOn = 0x00 != (notes->mIsOn[0] | notes->mIsOn[1] | notes->mIsOn[2] | notes->mIsOn[3]);
				if (anyNoteIsOn)
				{
					pdO[t] = 1.0f * gain;
				}
				else
				{
					pdO[t] = 0.0f;
				}
				
				// Handle bypass
				if (bypass) 
				{
					pdO[t] = pdI[t];
				}
				
				// Do metering
				//
				// Accumulate the max value for metering. This will get cleared for us by the shell
				// when it sends the accumulated value up to the host.
				meterTaps[eMeterTap_PreGain] = std::max(fabsf(instance->mInputPP [0] [t]), meterTaps[eMeterTap_PreGain]);
				meterTaps[eMeterTap_PostGain] = std::max(fabsf(instance->mOutputPP [0] [t]), meterTaps[eMeterTap_PostGain]);
				
			} // Go to the next sample
		} // Go to next channel
		
		// Update persistent state
		notes->mHoldState = holdNotes ? 1 : 0;
		
	} // End instance-iteration loop
}
