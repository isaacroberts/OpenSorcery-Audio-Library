//
//  PNoteComponent.hpp
//  PianoRollEditor - App
//
//  Created by Samuel Hunt on 16/08/2019.
//

//#ifndef PNoteComponent_hpp
//#define PNoteComponent_hpp


//#include "PConstants.h"
//#include "NoteModel.hpp"
//#include "NoteGridStyleSheet.hpp"

class NoteGridComponent;

class PNoteComponent :
public juce::Component,
public juce::ComponentDragger
{
public:
    
//    enum eState {
//        eNone,
//        eSelected,
//    };
    struct MultiEditState {
        int startWidth; //used when resizing the noteComponents length
        bool coordiantesDiffer; //sometimes the size of this component gets changed externally (for example on multi-resizing) set this flag to be true and at
		juce::Rectangle<int> startingBounds;
    };
    
    PNoteComponent (NoteGridComponent* parent, LiveMidi::Message::Ptr msg, NoteGridStyleSheet::Ptr styleSheet);

    ~PNoteComponent ();
    
    void paint (juce::Graphics & g) override;
    void resized () override;
    void setCustomColour (juce::Colour c);
	
	void deleteMessage();
    
	void setAndUpdateLength(float px);
    void setValues (LiveMidi::Message::Ptr model);
	void update();
	void updateSize();

	int getNote() { return model->getNote(); }

	LiveMidi::Message::Ptr getModel ();
	LiveMidi::Message::Ptr getModelPtr ();
    
//    void setState (eState state);
//    eState getState ();
	void setSelected(bool set);
	bool isSelected();
    
    void mouseEnter (const juce::MouseEvent&) override;
    void mouseExit  (const juce::MouseEvent&) override;
    void mouseDown  (const juce::MouseEvent&) override;
    void mouseUp    (const juce::MouseEvent&) override;
    void mouseDrag  (const juce::MouseEvent&) override;
    void mouseMove  (const juce::MouseEvent&) override;
    
    
//    void mouseDoubleClick (const MouseEvent&);
    
//    std::function<void(PNoteComponent*, const juce::MouseEvent&)> onNoteSelect;
//    std::function<void(PNoteComponent*)> onPositionMoved;
//    std::function<void(PNoteComponent*, const juce::MouseEvent&)> onDragging; //send the drag event to other components..
//    std::function<void(PNoteComponent*, int)> onLegnthChange; //sends the difference as this is relative for all components.
    
    int minWidth = 10;
    int startWidth; //used when resizing the noteComponents length
    int startX, startY;
    bool isMultiDrag;
    
private:
	LiveMidi::Message::Ptr model;
    NoteGridStyleSheet::Ptr styleSheet;
	NoteGridComponent* parent;
	juce::ResizableEdgeComponent edgeResizer;
    
    bool mouseOver, useCustomColour, resizeEnabled, velocityEnabled;
    int startVelocity;
    
    juce::Colour customColour;
//    NoteModel model;
	juce::MouseCursor normal;
//    eState state;
	bool selected;

};

//#endif /* NoteComponent_hpp */
