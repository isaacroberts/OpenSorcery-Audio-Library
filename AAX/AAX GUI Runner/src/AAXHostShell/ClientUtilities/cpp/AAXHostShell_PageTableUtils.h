//
//
// Copyright 2017 by Avid Technology, Inc.
//
//

//
//  AAXHostShell_PageTableUtils.h
//  AAXHostShell Utilities
//
//

#ifndef __AAXHostShell_CppUtilities_PageTableUtils_h__
#define __AAXHostShell_CppUtilities_PageTableUtils_h__

// AAXHostShell Includes
#include "AAXHostShell_Types.h"


namespace AAXHostShell
{
	/** @brief PageTableType pair
	 */
	typedef struct
	{
		uint32_t	mType;
		int32_t		mSize;
	} SPageTableType;
	
	inline SPageTableType PageTableType(uint32_t inType, int32_t inSize)
	{
		SPageTableType type = {0, 0};
		type.mType = inType;
		type.mSize = inSize;
		return type;
	}
	
	inline SPageTableType PageTableType(uint32_t inType)
	{
		SPageTableType type = {0, 0};
		type.mType = inType;
		type.mSize = kAAXHShell_PageTableSize_Any;
		return type;
	}
	
	const uint32_t cDefaultPageTable			= 'PgTL'; ///< Default page table
	const uint32_t cMackiePageTable				= 'MkTL'; ///< Mackie page table
	const uint32_t cProControlPageTable			= 'PcTL'; ///< ProControl page table
	const uint32_t cControl24PageTable			= 'FrTL'; ///< Control 24 and C|24 page table
	const uint32_t cDigi002PageTable			= 'HgTL'; ///< 002 and 003 page table
	const uint32_t cICONChannelStripPageTable	= 'BkCS'; ///< ICON Channel Strip page table
	const uint32_t cICONSuperfaderPageTable		= 'BkSF'; ///< ICON expand mode page table
	const uint32_t cDigiEQPageTable				= 'DgEQ'; ///< Center-section EQ page table
	const uint32_t cDigiCompLimPageTable		= 'DgCP'; ///< Center-section Dynamics page table (comp/limiter)
	const uint32_t cDigiExpGatePageTable		= 'DgGT'; ///< Center-section Dynamics page table (expander/gate)
	
	const uint32_t cAvidEucon11Table			= 'Av11'; ///< Avid 1x1 Table to support Eucon and VENUE Surfaces (1 row, 1 columns). @details Used by VENUE 5.6 for special left-column parameter mappings for Waves SoundGrid plug-ins.
	const uint32_t cAvidEucon18Table			= 'Av18'; ///< Avid 1x8 Table to support Eucon Surfaces (1 row, 8 columns)
	const uint32_t cAvidEucon46Table			= 'Av46'; ///< Avid 4x6 Table to support Eucon and VENUE Surfaces (4 rows, 6 columns) @details Used by VENUE S6L
	const uint32_t cAvidEucon48Table			= 'Av48'; ///< Avid 4x8 Table to support Eucon Surfaces (4 rows, 8 columns)
}

inline bool operator< (const AAXHostShell::SPageTableType& inL, const AAXHostShell::SPageTableType& inR)
{
	if (inL.mType < inR.mType) { return true; }
	else if (inL.mType == inR.mType)
		if (inL.mSize < inR.mSize) { return true; }
	return false;
}


#endif /* defined(__AAXHostShell_CppUtilities_PageTableUtils_h__) */
