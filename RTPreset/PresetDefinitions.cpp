//
//  PresetDefinitions.cpp
//  
//
//  Created by Isaac Roberts on 4/7/21.
//

//#include "PresetDefinitions.h"


void RTPreset::setSliderResetMethod(juce::Slider* slider, SliderResetMethod method, double def)
{
	if (method == SliderResetMethod::Ignore)
		return;
	else if (method == SliderResetMethod::None)
	{
		slider->setDoubleClickReturnValue(false, def, ModifierKeys::noModifiers);
	}
	else if (method == SliderResetMethod::DoubleOrAlt)
	{
		slider->setDoubleClickReturnValue(true, def, ModifierKeys::altModifier);
	}
	else if (method == SliderResetMethod::DoubleOnly)
	{
		slider->setDoubleClickReturnValue(true, def, ModifierKeys::noModifiers);
	}
	else if (method == SliderResetMethod::AltOnly)
	{
		slider->setDoubleClickReturnValue(false, def, ModifierKeys::altModifier);
	}
	else if (method == SliderResetMethod::LeftClick)
	{
		slider->setDoubleClickReturnValue(false, def, ModifierKeys::rightButtonModifier);
	}
	else if (method == SliderResetMethod::ShiftOnly)
	{
		slider->setDoubleClickReturnValue(false, def, ModifierKeys::shiftModifier);
	}
	else if (method == SliderResetMethod::ShiftOrDouble)
	{
		slider->setDoubleClickReturnValue(true, def, ModifierKeys::shiftModifier);
	}
	else if (method == SliderResetMethod::ShiftOrAlt)
	{
		slider->setDoubleClickReturnValue(false, def, ModifierKeys::shiftModifier | ModifierKeys::altModifier);
	}
	else if (method == SliderResetMethod::LeftClickOrDouble)
	{
		slider->setDoubleClickReturnValue(true, def, ModifierKeys::rightButtonModifier);
	}
}
