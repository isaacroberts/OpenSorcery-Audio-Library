//
//  RTGuiBasicInclude.h
//  
//
//  Created by Isaac Roberts on 5/11/21.
//

#include "Sliders/ComboSlider.h"
#include "Sliders/TwoValueCommon.h"
#include "Sliders/TwoValueSlider.h"
#include "Sliders/TwoValueSliderLookAndFeel.h"
#include "Sliders/SliderIncDecBottom.h"
#include "Sliders/FlagBox.h"
#include "Sliders/PopupSelector.h"

#include "RTMeter.h"

#include "RTLookAndFeel.h"

/*
 
 
 JUCE UPDATE:
 
	Goto juce_gui_basics > juce_gui_basics.h
	right before the #include LAFs section

	 #ifdef RT_INCLUDED
	 //Adds Extended LAF Methods
	 #include "RTGuiBasics/RTGuiBasicInclude.h"
	 #endif
 
	Then add
 
	 #ifdef RT_INCLUDED
	 #include "RTGuiBasics/RTGuiBasicInclude.cpp"
	 #endif

	to the juce_gui_basics.cpp file next to the other includes

 
	goto juce_gui_basics > lookandfeel > juce_LookAndFeel.h
		and add
			#ifdef RT_INCLUDED
			, public RTLookAndFeel
			#endif 
		to the end of the LookAndFeel constructor (line ~108)
 
 
 
 */
