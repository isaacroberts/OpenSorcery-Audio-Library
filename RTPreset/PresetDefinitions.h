//
//  PresetDefinitions.h
//  
//
//  Created by Isaac Roberts on 4/7/21.
//

#ifndef PresetDefinitions_h
#define PresetDefinitions_h

//#include <JuceHeader.h>


#ifndef JUCE_AUDIO_PROCESSORS_H_INCLUDED
class RangedAudioParameter {
	virtual float getValue() const =0;
	virtual void setValue(float nrm) {}
	virtual float getDefaultValue() const =0;
	virtual String getName(int maxLength) const =0;
	
	virtual String getLabel() const =0;
	
	virtual const NormalisableRange<float>& getNormalisableRange() const =0;
	
	//	int getNumSteps() const override { return 500; }
//	inline float convertTo0to1(float v) { return type->normalize(v);}
//	inline float convertFrom0to1(float v) {return type->denormalize(v);}
	
	virtual bool isDiscrete() const  =0;
	virtual bool isBoolean() const =0;
	virtual bool isAutomatable() =0;
	virtual float getValueForText(const String& text) const =0;

};
#endif

namespace RTPreset
{
	enum class SliderResetMethod {
		Ignore=0, DoubleOrAlt=1, AltOrDouble=1, DoubleOnly=2, DoubleClick=2, AltOnly, LeftClick,
		ShiftOnly, ShiftOrDouble,
		ShiftOrAlt,
		LeftClickOrDouble,
		None
	};
	
	void setSliderResetMethod(juce::Slider* slider, SliderResetMethod method, double def);
	
	
	
	inline ParameterID paramId(juce::String s)
	{
		return ParameterID(s, 1);
	}
	
};
typedef RTPreset::SliderResetMethod SliderResetMethod;

#endif /* PresetDefinitions_h */
