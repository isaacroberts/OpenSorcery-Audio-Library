//
//  NoteGridComponent.hpp
//  PianoRollEditor - App
//
//  Created by Samuel Hunt on 16/08/2019.
//

//#ifndef NoteGridComponent_hpp
//#define NoteGridComponent_hpp

//#include "PNoteComponent.hpp"
//#include "NoteGridStyleSheet.hpp"

class SelectionBox : public juce::Component {
public:
    void paint (juce::Graphics & g)
    {
		juce::Colour c = juce::Colours::white;
        c = c.withAlpha((float)0.5);
        g.fillAll(c);
    }
    int startX, startY;
};

class NoteGridComponent :
public juce::Component,
public juce::KeyListener,
public LiveMidi::Listener
{
public:
    

    NoteGridComponent (LiveMidi* midi, NoteGridStyleSheet::Ptr styleSheet);
    ~NoteGridComponent ();
    
    void paint (juce::Graphics & g) override;
    void resized () override;
	
	void noteUpdated(LiveMidi::Message::Ptr msg, bool position, LiveMidi*) override;
	void noteAdded  (LiveMidi::Message::Ptr msg, LiveMidi*) override;
	void noteRemoved(int ix, LiveMidi::Message::Ptr msg, LiveMidi*) override;
	void allNotesUpdated(LiveMidi*) override;

    void noteCompSelected (PNoteComponent *, const juce::MouseEvent&);
    void noteCompPositionMoved (PNoteComponent *, bool callResize = true);
    void noteCompLengthChanged (PNoteComponent *, int diff);
    void noteCompDragging (PNoteComponent*, const juce::MouseEvent&);
    void setPositions ();
    
    /* optional
    void mouseEnter (const MouseEvent&);
    void mouseExit  (const MouseEvent&);
    void mouseMove  (const MouseEvent&);
    */
    void mouseDown  (const juce::MouseEvent&) override;
    void mouseDrag  (const juce::MouseEvent&) override;
    void mouseUp    (const juce::MouseEvent&) override;
    void mouseDoubleClick (const juce::MouseEvent&) override;
    
	void setNoteCompHeight(float compHeight);
	void zoomNoteCompHeight(float amt);

    void setupGrid (float pixelsPerBar, float compHeight, const int bars);
    void setQuantization (int val);
    
    
    bool keyPressed (const juce::KeyPress& key, juce::Component* originatingComponent) override;
    void deleteAllSelected ();
    
    // From here you could convert this into MIDI or any other custom musical encoding.
//    PRESequence getSequence ();
//    void loadSequence (PRESequence& sq);
    
    float getNoteCompHeight ();
    float getPixelsPerBar ();
    
	int getStartYPosition();
	int getCenterYPosition(int viewH);
	
	float getNoteCompHeightToViewAll(int height, int& yStart, float minCompHeight=7);
	
	LiveMidi* getMidi() { return midi; }
	
    std::vector<LiveMidi::Message::Ptr> getSelectedModels ();
    
    std::function<void(int note,int velocity)> sendChange;
    std::function<void()> onEdit;
		
private:
	friend class PNoteComponent;
	
	void _sub_noteCompPositionMoved (PNoteComponent *);

	PNoteComponent* newNoteComponent(LiveMidi::Message::Ptr msg);
	void updateComp(PNoteComponent* p);
    void sendEdit ();
    
	LiveMidi* midi;
	juce::OwnedArray<PNoteComponent> noteComps;
    NoteGridStyleSheet::Ptr styleSheet;
    SelectionBox selectorBox;
    
    float noteCompHeight;
    float pixelsPerBar;
//    st_int ticksPerTimeSignature;
    int currentQValue;
    float lastNoteLength;
};
//#endif /* NoteGridComponent_hpp */
