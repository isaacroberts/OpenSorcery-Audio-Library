/*================================================================================================*/
/*
 *	Copyright 2014-2017 by Avid Technology, Inc.
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
// Host Alg wrapper shim headers
#include "DemoGain_Alg.h"
#include "../TI/CCSv5/elf_linkage_aax_ccsv5.h"

extern "C"
TI_EXPORT
void AlgEntry_1(	SDemoGain_Alg_Context * const		inInstancesBegin [],
				const void *						inInstancesEnd)
{
	DemoGain_AlgorithmProcessFunction<1> (
		inInstancesBegin,
		inInstancesEnd);
}

extern "C"
TI_EXPORT
void AlgEntry_2(	SDemoGain_Alg_Context * const		inInstancesBegin [],
				const void *						inInstancesEnd)
{
	DemoGain_AlgorithmProcessFunction<2> (
		inInstancesBegin,
		inInstancesEnd);
}

extern "C"
TI_EXPORT
void AlgEntry_3(	SDemoGain_Alg_Context * const		inInstancesBegin [],
				const void *						inInstancesEnd)
{
	DemoGain_AlgorithmProcessFunction<3> (
		inInstancesBegin,
		inInstancesEnd);
}

extern "C"
TI_EXPORT
void AlgEntry_4(	SDemoGain_Alg_Context * const		inInstancesBegin [],
				const void *						inInstancesEnd)
{
	DemoGain_AlgorithmProcessFunction<4> (
		inInstancesBegin,
		inInstancesEnd);
}

extern "C"
TI_EXPORT
void AlgEntry_5(	SDemoGain_Alg_Context * const		inInstancesBegin [],
				const void *						inInstancesEnd)
{
	DemoGain_AlgorithmProcessFunction<5> (
		inInstancesBegin,
		inInstancesEnd);
}

extern "C"
TI_EXPORT
void AlgEntry_6(	SDemoGain_Alg_Context * const		inInstancesBegin [],
				const void *						inInstancesEnd)
{
	DemoGain_AlgorithmProcessFunction<6> (
		inInstancesBegin,
		inInstancesEnd);
}

extern "C"
TI_EXPORT
void AlgEntry_7(	SDemoGain_Alg_Context * const		inInstancesBegin [],
				const void *						inInstancesEnd)
{
	DemoGain_AlgorithmProcessFunction<7> (
		inInstancesBegin,
		inInstancesEnd);
}

extern "C"
TI_EXPORT
void AlgEntry_8(	SDemoGain_Alg_Context * const		inInstancesBegin [],
				const void *						inInstancesEnd)
{
	DemoGain_AlgorithmProcessFunction<8> (
		inInstancesBegin,
		inInstancesEnd);
}

extern "C"
TI_EXPORT
void AlgEntry_9(	SDemoGain_Alg_Context * const		inInstancesBegin [],
				const void *						inInstancesEnd)
{
	DemoGain_AlgorithmProcessFunction<9> (
		inInstancesBegin,
		inInstancesEnd);
}

extern "C"
TI_EXPORT
void AlgEntry_10(	SDemoGain_Alg_Context * const		inInstancesBegin [],
				const void *						inInstancesEnd)
{
	DemoGain_AlgorithmProcessFunction<10> (
		inInstancesBegin,
		inInstancesEnd);
}

extern "C"
TI_EXPORT
void AlgEntry_16(   SDemoGain_Alg_Context * const       inInstancesBegin [],
                const void *                        inInstancesEnd)
{
    DemoGain_AlgorithmProcessFunction<16> (
        inInstancesBegin,
        inInstancesEnd);
}
