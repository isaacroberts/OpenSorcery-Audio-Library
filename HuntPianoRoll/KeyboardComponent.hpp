//
//  KeyboardComponent.hpp
//  PianoRollEditor - App
//
//  Created by Samuel Hunt on 04/02/2020.
//

//#ifndef KeyboardComponent_hpp
//#define KeyboardComponent_hpp

//#include "PConstants.h"

class KeyboardComponent : public juce::Component {
public:
    
    KeyboardComponent (NoteGridStyleSheet::Ptr);
    void paint (juce::Graphics & g);
    
	void drawAsPiano(juce::Graphics& g);
	void drawAsFlat(juce::Graphics& g);
	
protected:
	NoteGridStyleSheet::Ptr style;
};

//#endif /* KeyboardComponent_hpp */
