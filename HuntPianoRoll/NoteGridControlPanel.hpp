//
//  NoteGridControlPanel.hpp
//  PianoRollEditor - App
//
//  Created by Samuel Hunt on 03/02/2020.
//

//#ifndef NoteGridControlPanel_hpp
//#define NoteGridControlPanel_hpp


//#include "PConstants.h"
//#include "NoteGridComponent.hpp"
//#include "NoteGridStyleSheet.hpp"

class NoteGridControlPanel : public juce::Component {
public:
    NoteGridControlPanel (NoteGridComponent & component, NoteGridStyleSheet::Ptr styleSheet);
    ~NoteGridControlPanel ();
    
    void resized ();
    void paint (juce::Graphics & g);
    
    void renderSequence ();
    std::function<void(int pixelsPerBar, int noteHeight)> configureGrid;
    
    void setQuantisation (int value);
private:
    
    NoteGridComponent & noteGrid;
    NoteGridStyleSheet::Ptr styleSheet;
    
	juce::Slider noteCompHeight, pixelsPerBar;
    
	juce::TextButton render;
	juce::ToggleButton drawMIDINotes, drawMIDIText, drawVelocity;
    
	juce::ComboBox quantisationVlaue;
};

//#endif /* NoteGridControlPanel_hpp */
