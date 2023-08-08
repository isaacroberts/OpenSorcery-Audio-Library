//
//  NoteGridStyleSheet.hpp
//  PianoRollEditor - App
//
//  Created by Samuel Hunt on 03/02/2020.
//

//#ifndef NoteGridStyleSheet_hpp
//#define NoteGridStyleSheet_hpp

//#include "PConstants.h"


class NoteGridStyleSheet {
public:
	typedef std::shared_ptr<NoteGridStyleSheet> Ptr;
    /*
     We use a friend class to ensure a one-way only relationship. The control pannel can set member varialbes easily. Saves adding loads of getters/setters
     */
    friend class NoteGridControlPanel;
    
	NoteGridStyleSheet () {}
	virtual ~NoteGridStyleSheet() {}
	
	bool disableEditing;
    /*
	 bool getDrawMIDINum () override;
	 bool getDrawMIDINoteStr () override;
	 bool getDrawVelocity () override;
	 
	 juce::Colour getNoteColor() override;

	 juce::Colour getWhiteRowColor() override;
	 juce::Colour getBlackRowColor() override;

	 juce::Colour getSidebarWhiteColor() override;
	 juce::Colour getSidebarBlackColor() override;
	 
	 juce::Colour getSidebarWhiteFontColor() override;
	 juce::Colour getSidebarBlackFontColor() override;
	 
	 */
	virtual juce::String getNoteLabel(int n, float velo) {
		return "";
	}
	virtual bool shouldDrawVelocity () = 0;
	
	virtual juce::String getRowLabel(int n) {
//		return getNoteName(n);
		return juce::String(n);
	}
	virtual juce::Justification getRowLabelJustification() {
		return juce::Justification::left;
	}
	
	virtual bool drawKeyboardAsPiano() { return true; }
	virtual bool drawAsWhite(int n) { return Scale::isPitchWhite(n); }
	
	virtual juce::Colour getWhiteRowColor() = 0;
	virtual juce::Colour getBlackRowColor() = 0;
	virtual juce::Colour getSidebarWhiteColor() = 0;
	virtual juce::Colour getSidebarBlackColor() = 0;
	virtual juce::Colour getSidebarWhiteFontColor() = 0;
	virtual juce::Colour getSidebarBlackFontColor() = 0;
	
	virtual juce::Colour getNoteColor() = 0;
    
protected:
	juce::String getNoteName(int n) {
		return juce::String(PRE::pitches_names[n % 12]) + juce::String(n / 12);
	}
	
};


class StdNoteGridStyleSheet : public NoteGridStyleSheet {
public:
	StdNoteGridStyleSheet ();
	virtual ~StdNoteGridStyleSheet() {}
	
	bool shouldDrawVelocity () override { return drawVelocity; }
	
	juce::Colour getWhiteRowColor() override;
	juce::Colour getBlackRowColor() override;
	
	juce::Colour getSidebarWhiteColor() override;
	juce::Colour getSidebarBlackColor() override;
	
	juce::Colour getSidebarWhiteFontColor() override;
	juce::Colour getSidebarBlackFontColor() override;
	
	juce::Colour getNoteColor() override;
		
public:
	bool drawVelocity;
};



//#endif /* NoteGridStyleSheet_hpp */
