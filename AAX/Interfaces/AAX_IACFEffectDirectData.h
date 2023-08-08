/*================================================================================================*/
/*
 *
 *	Copyright 2013-2015 by Avid Technology, Inc.
 *	All rights reserved.
 *	
 *	CONFIDENTIAL: This document contains confidential information.  Do not 
 *	read or examine this document unless you are an Avid Technology employee
 *	or have signed a non-disclosure agreement with Avid Technology which protects
 *	the	confidentiality of this document.  DO NOT DISCLOSE ANY INFORMATION 
 *	CONTAINED IN THIS DOCUMENT TO ANY THIRD-PARTY WITHOUT THE PRIOR WRITTEN	CONSENT 
 *	OF Avid Technology, INC.
 *
 */

/**  
 *	\file  AAX_IACFEffectDirectData.h
 *
 *	\brief The direct data access interface that gets exposed to the host application
 *
 */ 
/*================================================================================================*/


#ifndef AAX_IACFEFFECTDIRECTDATA_H
#define AAX_IACFEFFECTDIRECTDATA_H

#include "AAX.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnon-virtual-dtor"
#endif

#include "acfunknown.h"


/** @brief	Optional interface for direct access to a plug-in's alg memory.
	
	@details
	Direct data access allows a plug-in to directly manipulate the data in its algorithm's
	private data blocks.  The callback methods in this interface provide a safe context
	from which this kind of access may be attempted.

	\ingroup AuxInterface_DirectData
*/
class AAX_IACFEffectDirectData : public IACFUnknown
{	
public:
	
	/** @name Initialization and uninitialization
	 */
	//@{
	/*!
	 *  \brief Main initialization
	 *  
	 *	Called when the interface is created
	 *
	 *  \param[in] iController 
	 *		A versioned reference that resolves to an AAX_IController interface
	 */
	virtual AAX_Result		Initialize ( IACFUnknown * iController ) = 0;
	/*!
	 *  \brief Main uninitialization
	 *
	 *	Called when the interface is destroyed.
	 *
	 */
	virtual AAX_Result		Uninitialize () = 0;
	//@}end Initialization and uninitialization

	
	/** @name Safe data update callbacks
	 *
	 *	These callbacks provide a safe context from which to directly access the
	 *	algorithm's private data blocks.  Each callback is called regularly with
	 *	roughly the schedule of its corresponding AAX_IEffectParameters counterpart.
	 *
	 *	\note Do not attempt to directly access the algorithm's data from outside
	 *	these callbacks.  Instead, use the packet system to route data to the
	 *	algorithm using the %AAX host's buffered data transfer facilities.
	 *	
	 */
	//@{
	/*!  
	 *	\brief Periodic wakeup callback for idle-time operations
	 *
	 *	Direct alg data updates must be triggered from this method.
	 *
	 *	This method is called from the host using a non-main thread.  In general, it should
	 *	be driven at approximately one call per 30 ms.  However, the wakeup is not guaranteed to
	 *	be called at any regular interval - for example, it could be held off by a high real-time
	 *	processing load - and there is no host contract regarding maximum latency between wakeup
	 *	calls.
	 *
	 *	This wakeup thread runs continuously and cannot be armed/disarmed or by the plug-in.
	 *
	 *	\param[in] iDataAccessInterface
	 *		Reference to the direct access interface.
	 *	
	 *			\note It is not safe to save this address or call
	 *			the methods in this interface from other threads.
	 */
	virtual AAX_Result		TimerWakeup (
		IACFUnknown *		iDataAccessInterface ) = 0;
	//@} end
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif //AAX_IACFEFFECTDIRECTDATA_H
