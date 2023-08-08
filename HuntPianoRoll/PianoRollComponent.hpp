//
//  PianoRollEditorComponent.hpp
//  PianoRollEditor - App
//
//  Created by Samuel Hunt on 05/02/2020.
//

//#ifndef PianoRollEditorComponent_hpp
//#define PianoRollEditorComponent_hpp

//#include "PConstants.h"
//#include "NoteGridControlPanel.hpp"
//#include "TimelineComponent.hpp"
//#include "KeyboardComponent.hpp"

/*
 This custom viewport is used so that when the main piano roll is moved the others can be moved also, see diagram below
 */
class CustomViewport : public juce::Viewport
{
public:
    void visibleAreaChanged (const juce::Rectangle<int>& newVisibleArea) override
    {
		juce::Viewport::visibleAreaChanged(newVisibleArea);
        if (positionMoved) {
            positionMoved(getViewPositionX(), getViewPositionY());
        }
    }
	
//	void mouseMagnify (const juce::MouseEvent &event, float scaleFactor) override;

    
	std::function<void(int,int)> positionMoved;
//	std::function<void(float)> mouseMagnified;

};


/*
 
 |      |   timeline component
 |______|__________________________________________
 |      |
 |      |
 |keys  |   main viewport and note grid component
 |      |
 |      |
 |      |
 |      |
 |      |
 |      |
 |      |
 |      |
 |      |
 
 
 */

class PianoRollComponent : public juce::Component
{
public:
    
    //==============================================================================
    PianoRollComponent(LiveMidi*, NoteGridStyleSheet::Ptr styleSheet);
	PianoRollComponent(LiveMidi* m) : PianoRollComponent(m, std::make_shared<StdNoteGridStyleSheet>()) {}

    ~PianoRollComponent();
    //==============================================================================
    
    /*
     This needs to be called after the constructor and determines the size of the grid.
     
     Once setup the number of bars can be dynamically altered through @updateBars(..
     
     Todo: automatically resize the number of bars
     */
    void setup (const int bars, const int pixelsPerBar, const int noteHeight);
    void updateBars (const int newNumberOfBars);
	
	void zoomVertical(float amt);
	void zoomHorizontal(float amt) {
		jassertfalse;
	}
	
	void addPlayhead(PlayheadComponent* p);
	void deletePlayhead();
    
    void paint (juce::Graphics&) override;
//    void paintOverChildren (juce::Graphics&) override;

    void resized() override;
    
    void showControlPanel (bool state);
//    void setStyleSheet (NoteGridStyleSheet style);
    
	virtual void autobound();
	
    void setScroll (double x, double y);
//    void setPlaybackMarkerPosition (const st_int ticks, bool isVisable = true); 
    
    /*
     If you just want to view the editor then disable the grid.
     */
    void disableEditing (bool value);
    NoteGridControlPanel & getControlPanel ();
    
    /*
     Returns any notes that are selected used in IGME
     */
//    ExternalModelEditor getSelectedNoteModels ();
	auto getSelectedNoteModels() { return noteGrid.getSelectedModels(); }
	auto getStyleSheet() { return styleSheet; }
	LiveMidi* getMidi() { return noteGrid.getMidi(); }

    /*
     This is called when the grid is edited.
     */
    std::function<void()> onEdit;
    /*
     You can use this to implement simple MIDI synthesis when notes are being edited,
     when notes are edited this function will be called
     */
    std::function<void(int note,int velocity)> sendChange;

	
protected:
	
	
protected:
    //==============================================================================
    
    NoteGridStyleSheet::Ptr      styleSheet;
    
    /*
     These 3 essential components make up the piano roll editor.
     Each is stored in a customViewport instance, that are coupled to move in unison
     */
    NoteGridComponent       noteGrid; //the actual piano roll
    TimelineComponent       timelineComp; // the timeline marker at the top
    KeyboardComponent       keyboardComp; // the keyboard visualiser to the left
    
    CustomViewport          viewportGrid, viewportPiano, viewportTimeline;
    
    /*
     Optional control pannel
     */
    NoteGridControlPanel    controlPannel;
    
//    bool    showPlaybackMarker;
	PlayheadComponent* playhead;
    
    JUCE_DECLARE_NON_COPYABLE(PianoRollComponent)
//    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
    
    
};


//#endif /* PianoRollEditorComponent_hpp */
