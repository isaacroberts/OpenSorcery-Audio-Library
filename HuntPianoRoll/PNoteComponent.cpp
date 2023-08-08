//
//  NoteComponent.cpp
//  PianoRollEditor - App
//
//  Created by Samuel Hunt on 16/08/2019.
//

//#include "PNoteComponent.hpp"

#define CHECK_EDIT if(styleSheet->disableEditing) { return; }

PNoteComponent::PNoteComponent (NoteGridComponent* notegrid, LiveMidi::Message::Ptr msg, NoteGridStyleSheet::Ptr ss)
: model(msg),
styleSheet(ss),
parent(notegrid),
edgeResizer(this, nullptr, juce::ResizableEdgeComponent::Edge::rightEdge)
{
    mouseOver = useCustomColour = false;
    addAndMakeVisible(edgeResizer);
    setMouseCursor(normal);
    startWidth = startX = startY = -1;
    isMultiDrag = false;
    selected = false;
    
    setCustomColour(juce::Colours::green);
}
PNoteComponent::~PNoteComponent ()
{
    
}

void PNoteComponent::paint (juce::Graphics & g)
{
	if (getWidth() < 2 || getHeight() < 2)
		return;
	
	juce::Colour cToUse = styleSheet->getNoteColor();
    
    g.fillAll(juce::Colours::darkgrey); //border...
//    if (useCustomColour && model->flags.isGenerative) {
//        cToUse = customColour;
//    }
    
    if (selected || mouseOver) {
        cToUse = cToUse.brighter().brighter();
    }
    g.setColour(cToUse);
    
    //draw middle box.
    g.fillRect(1, 1, getWidth() - 2, getHeight() - 2);

    
    //draw velocity
    if (getWidth() > 10 && styleSheet->shouldDrawVelocity()) {
        g.setColour(cToUse.brighter());
        const int lineMax = getWidth() - 5;
        
        g.drawLine(5, getHeight() * 0.5 - 2, lineMax * (model->getFloatVelo()), getHeight() * 0.5 - 2, 4);
    }
	
	if (getWidth() > 6 && getHeight() > 6)
	{
		juce::String toDraw = styleSheet->getNoteLabel(model->getNote(), model->getFloatVelo());
		
		if (toDraw.isNotEmpty())
		{
			g.setColour(juce::Colours::white);
			g.drawText(juce::String(toDraw), 3, 3, getWidth() - 6, getHeight() - 6, juce::Justification::centred);
		}
	}
}
void PNoteComponent::deleteMessage()
{
	model->deleteMessage();
	model = nullptr;
}
void PNoteComponent::resized ()
{
//    edgeResizer.setBounds(getWidth() - 10, getHeight(), 10, getHeight());
}
void PNoteComponent::setCustomColour (juce::Colour c)
{
    customColour = c;
    useCustomColour = true;
}

void PNoteComponent::setValues (LiveMidi::Message::Ptr m)
{
    model = m;
    repaint();

}
void PNoteComponent::update()
{
	repaint();
}
void PNoteComponent::updateSize()
{
	int parentH = parent->getHeight();
	const float xPos = (model->getStartTime()) * parent->pixelsPerBar;
	const float yPos = (parentH - (model->getNote() * parent->noteCompHeight)) - parent->noteCompHeight;
	
	int len = std::round((model->getNoteLength()) * parent->pixelsPerBar);
	
	setBounds(xPos, yPos, len, std::ceil(parent->noteCompHeight));

//	repaint();
}

void PNoteComponent::setAndUpdateLength(float inPx)
{
	float newL = inPx / parent->pixelsPerBar;
	model->setLength(newL, parent);
	setBounds(getX(), getY(), inPx, getHeight());
}

LiveMidi::Message::Ptr PNoteComponent::getModel ()
{
    return model;
}
LiveMidi::Message::Ptr PNoteComponent::getModelPtr ()
{
    return model;
}

void PNoteComponent::setSelected (bool set)
{
	selected = set;
    repaint();
}
bool PNoteComponent::isSelected ()
{
	return selected;
}

void PNoteComponent::mouseEnter (const juce::MouseEvent&)
{
    CHECK_EDIT
    mouseOver = true;
    repaint();
}
void PNoteComponent::mouseExit  (const juce::MouseEvent&)
{
    CHECK_EDIT
    mouseOver = false;
    setMouseCursor(juce::MouseCursor::NormalCursor);
    repaint();
    
}
void PNoteComponent::mouseDown (const juce::MouseEvent& e)
{
    CHECK_EDIT

    if (e.mods.isShiftDown()) {
        velocityEnabled = true;
        startVelocity = model->getVelocity();
    }
    else if (getWidth() - e.getMouseDownX() < 10) {
        resizeEnabled = true;
        startWidth = getWidth();
    }
    else {
        startDraggingComponent(this, e);
        
    }
    if (!resizeEnabled) {
        
    }
}
void PNoteComponent::mouseUp (const juce::MouseEvent& e)
{
    CHECK_EDIT
    if (parent) {
        parent->noteCompSelected(this, e);
		parent->noteCompPositionMoved(this);
    }
    startWidth = startX = startY -1;
    mouseOver = false;
    resizeEnabled = false;
    velocityEnabled = false;
    repaint();
    isMultiDrag = false;
    
}
void PNoteComponent::mouseDrag  (const juce::MouseEvent& e)
{
    CHECK_EDIT
    //velocityEnabled
    if (resizeEnabled) {
        if (parent) {
			parent->noteCompLengthChanged(this, startWidth-e.getPosition().getX());
        }

    }
    else if (velocityEnabled) {
        int velocityDiff = e.getDistanceFromDragStartY() * -0.5;
        int newVelocity = startVelocity + velocityDiff;
        if (newVelocity < 1) {
            newVelocity = 1;
        }
//        else if (newVelocity > 127) {
//            newVelocity = 127;
//        }
        model->setVelocity(newVelocity);
        repaint();
//        std::cout << velocityDiff << "\n";
        
    }
    else {
        setMouseCursor(juce::MouseCursor::DraggingHandCursor);
        dragComponent(this, e, nullptr);
        
        if (parent ) { //&& isMultiDrag
            parent->noteCompDragging(this, e);
        }
    }
    
}
void PNoteComponent::mouseMove  (const juce::MouseEvent& e)
{
    CHECK_EDIT
    if (getWidth() - e.getMouseDownX() < 10) {
        setMouseCursor(juce::MouseCursor::RightEdgeResizeCursor);
    }
    else {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
}

