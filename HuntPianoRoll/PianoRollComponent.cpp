//
//  PianoRollComponent.cpp
//  PianoRollEditor - App
//
//  Created by Samuel Hunt on 05/02/2020.
//

//#include "PianoRollComponent.hpp"


//==============================================================================
PianoRollComponent::PianoRollComponent(LiveMidi* m, NoteGridStyleSheet::Ptr sheet)
: styleSheet(sheet),
noteGrid(m, styleSheet), keyboardComp(styleSheet),
controlPannel(noteGrid, styleSheet),
playhead(NULL)
{
 
    //--- grid
    addAndMakeVisible(viewportGrid);
 
    viewportGrid.setViewedComponent(&noteGrid, false);
    viewportGrid.setScrollBarsShown(true, true);
    viewportGrid.setScrollBarThickness(10);
 
    
    //--- timeline
    viewportTimeline.setViewedComponent(&timelineComp, false);
    viewportTimeline.setScrollBarsShown(false, false);
    addAndMakeVisible(viewportTimeline);
    
    //--- keyboard
    viewportPiano.setViewedComponent(&keyboardComp, false);
    viewportPiano.setScrollBarsShown(false, false);
    addAndMakeVisible(viewportPiano);
    
	
    //once the piano roll component is scrolled then it updates the others manually
    viewportGrid.positionMoved = [this](int x, int y)
    {
        viewportTimeline.setViewPosition(x, y);
        viewportPiano.setViewPosition(x, y);
    };
    
    addAndMakeVisible(controlPannel);
    controlPannel.configureGrid = [this](int pixelsPerBar, int noteHeight)
    {
        setup(10, pixelsPerBar, noteHeight);
    };
    
    noteGrid.onEdit = [this]() //pass up the chain.
    {
        if (this->onEdit != nullptr) {
            this->onEdit();
        }
    };
    noteGrid.sendChange = [this](int note, int vel)
    {
        if (this->sendChange != nullptr) {
            this->sendChange(note, vel);
        }
    };
	
	setOpaque(true);
//    showPlaybackMarker = false;
}

void PianoRollComponent::addPlayhead(PlayheadComponent* p)
{
	jassert (!playhead);
	playhead = p;
	playhead->setParentBounds(viewportGrid.getBounds());
	playhead->setPixelsPerBar(noteGrid.getPixelsPerBar());
	addAndMakeVisible(playhead);
}
void PianoRollComponent::deletePlayhead()
{
	delete playhead;
	playhead=NULL;
}
PianoRollComponent::~PianoRollComponent()
{
}

//==============================================================================
void PianoRollComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey.darker());

}
//void PianoRollComponent::paintOverChildren (juce::Graphics& g)
//{
//	
//	
//    const int x = noteGrid.getPixelsPerBar() * (playbackTicks / (4.0 * PRE::defaultResolution));
//    const int xAbsolute = viewportGrid.getViewPosition().getX();
//    
//    g.setColour(juce::Colours::greenyellow);
//    g.drawLine(x - xAbsolute, 0, x - xAbsolute, getHeight(), 5.0);
//}
void PianoRollComponent::resized()
{
    viewportGrid.setBounds(80, 50, getWidth()-90, controlPannel.isVisible() ? getHeight()-180 : getHeight() - 55);
    viewportTimeline.setBounds(viewportGrid.getX(), 5, viewportGrid.getWidth()-10, viewportGrid.getY() - 5);
    viewportPiano.setBounds(5, viewportGrid.getY(), 70, viewportGrid.getHeight()- 10);
    
    noteGrid.setBounds(0,0,4000, 20*128);
    noteGrid.setupGrid(900, 20, 10);

	if (playhead)
	{
		playhead->setParentBounds(viewportGrid.getBounds());
		playhead->setPixelsPerBar(noteGrid.getPixelsPerBar());
	}
	
    timelineComp.setBounds(0, 0, 100, viewportTimeline.getHeight());
    timelineComp.setup(10, 900);
    keyboardComp.setBounds(0, 0, viewportPiano.getWidth(), noteGrid.getHeight());
    
    controlPannel.setBounds(5, viewportGrid.getBottom() + 5, getWidth() - 10, 140);
    
    
}

void PianoRollComponent::showControlPanel (bool state)
{
    controlPannel.setVisible(state);
}
//void PianoRollComponent::setStyleSheet (NoteGridStyleSheet style)
//{
//
//}
void PianoRollComponent::setup (const int bars, const int pixelsPerBar, const int noteHeight)
{
    
    if (bars > 1 && bars < 1000) { // sensible limits..

        noteGrid.setupGrid(pixelsPerBar, noteHeight, bars);
        timelineComp.setup(bars, pixelsPerBar);
        keyboardComp.setSize(viewportPiano.getWidth(), noteGrid.getHeight());
    }
    else {
        // you might be able to have a 1000 bars but do you really need too!?
        jassertfalse;
    }
}

void PianoRollComponent::updateBars (const int newNumberOfBars)
{
    if (newNumberOfBars > 1 && newNumberOfBars < 1000) { // sensible limits..
        const float pPb = noteGrid.getPixelsPerBar();
        const float nH = noteGrid.getNoteCompHeight();
        
        noteGrid.setupGrid(pPb, nH, newNumberOfBars);
        timelineComp.setup(newNumberOfBars, pPb);
        keyboardComp.setSize(viewportPiano.getWidth(), noteGrid.getHeight());
    }
    else {
        jassertfalse;
    }
}

void PianoRollComponent::autobound()
{
	int centerY = noteGrid.getCenterYPosition(viewportGrid.getHeight()) - viewportGrid.getHeight()/2;
	viewportGrid.setViewPosition(0, centerY);
}

void PianoRollComponent::setScroll (double x, double y)
{
    viewportGrid.setViewPositionProportionately(x, y);
}


//void CustomViewport::mouseMagnify (const juce::MouseEvent &event, float scaleFactor)
//{
//	mouseMagnified(scaleFactor);
//}


//void PianoRollComponent::setPlaybackMarkerPosition (const st_int ticks, bool isVisable)
//{
//    showPlaybackMarker = isVisable;
//    playbackTicks = ticks;
//    repaint();
//    
//}
void PianoRollComponent::disableEditing (bool value)
{
    styleSheet->disableEditing = value;
    noteGrid.repaint();
}

NoteGridControlPanel & PianoRollComponent::getControlPanel ()
{
    return controlPannel;
}
void PianoRollComponent::zoomVertical(float amt)
{
	//c = (Y/H + h/2)
	//c-h/2 = Y/H
	//(c-h/2)H = Y
	int h = viewportGrid.getHeight();
	float hf = float(h) / 2.f;
	float center = float(viewportGrid.getViewPositionY()+hf)/noteGrid.getHeight();
	noteGrid.zoomNoteCompHeight(amt);
	keyboardComp.setSize(keyboardComp.getWidth(), noteGrid.getHeight());
	int pos = (center) * noteGrid.getHeight()-hf;
	pos = jlimit(0, noteGrid.getHeight()-h, pos);
	viewportGrid.setViewPosition(0, pos);
}

//PianoRollComponent::ExternalModelEditor PianoRollComponent::getSelectedNoteModels ()
//{
//    ExternalModelEditor mEdit;
//    mEdit.update = [this]()
//    {
//        noteGrid.resized();
//        noteGrid.repaint();
//    };
//    mEdit.models = noteGrid.getSelectedModels();
//    return mEdit;
//}
