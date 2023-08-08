

#pragma once

#include "Design.h"

namespace RBJ {
	typedef LowPass LoPass;
	typedef HighPass HiPass;
	typedef LowShelf LoShelf;
	typedef HighShelf HiShelf;
}
/*

// Design Types

	LowPass, HighPass,
	AllPass, // Matches the Phase change of Lo/Hi Pass
	BandPass1, //Constant Skirt Gain, peak gain ∝ Q
	BandPass2, //Constant Peak Gain
	BandStop,
	PeakingEQ,
	LowShelf, HighShelf

   Q/BW/S parameter:
   
   LowPass, HighPass, AllPass:
	   Q is the classical EE definition.
	   Q Ranges from 0 to infinity, where values above 1 give resonance, and values below 1 give more gradual slopes. 0 yields silence.
	   
   BandPass1, BandPass2, Notch, PeakingEQ:
	   Bandwidth is in Octaves
	   
   LowShelf, HighShelf:
	   S is (0 and 1], where 0 is silence, .1 is gradual, and  1 is the maximum possible steepness.
   
	Resonance is an optional way to set Q for Lo/HiPass filters.
		Acceptable values are from 0 to 1, where 0 is no slope and 1 is maximum slope.

		Code attempts to set resonance for other filters, but jasserts false where that doesn't make sense.

*/


#include "Forms.h"

namespace RBJ {

	typedef DirectForm1 DF1;
	typedef DirectForm2 DF2;
	typedef TransposedDirectForm2 TDF2;	
	
	typedef DirectForm2 Speed;
	typedef TransposedDirectForm2 Smooth;

}
/*
// State Variable Types



	Direct Form II:
			Most lightweight
			Shouldn't cause problems if filter isn't moving
			Uses least amount of variables

	Transposed Direct Form II:
			Should be more resilient to fast changes
			Can still blow up under extreme changes

*/


#include "Fixed.h"
#include "Semi.h"

/*
// Changing Param Implementations


	Fixed:
		Most lightweight
		Snaps coefficients
		Can change, but will cause slight "digital" sounds
		
	Semi:
		Linearly approaches coefficients
		Smooth changes
		Resolution can be decreased for better speed
		
 
	SemiExp:
		Continuously approaches set coefficients
		Much smoother for changes
		Continuous alpha function weighs on CPU
		Causes "bloops" when rapidly increasing frequency
		
 
	




*/
