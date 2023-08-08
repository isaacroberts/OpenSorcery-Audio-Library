//
//  NoteGridComponent.cpp
//  PianoRollEditor - App
//
//  Created by Samuel Hunt on 16/08/2019.
//

//#include "NoteGridComponent.hpp"

//#ifndef LIB_VERSION
//#include "DataLoggerRoot.h"
//#endif


#define RETURN_IF_EDITING_DISABLED if(styleSheet->disableEditing) { return; }

NoteGridComponent::NoteGridComponent (LiveMidi* midi_, NoteGridStyleSheet::Ptr ss)
: midi(midi_), styleSheet(ss)
{
    addChildComponent(&selectorBox);
    addKeyListener(this);
    setWantsKeyboardFocus(true);
    currentQValue = 16;
    lastNoteLength = 1/8.;

	midi->addListener(this, true);
	setOpaque(true);
}
NoteGridComponent::~NoteGridComponent ()
{
	midi->removeListener(this);
}

void NoteGridComponent::noteUpdated(LiveMidi::Message::Ptr msg, bool position, LiveMidi* seq)
{
	for (int n=0; n < noteComps.size(); ++n)
	{
		if (noteComps[n]->getModel()==msg)
		{
			if (position)
			{
				updateComp(noteComps[n]);
			}
			else
				noteComps[n]->update();
			return;
		}
	}
}
void NoteGridComponent::noteAdded  (LiveMidi::Message::Ptr msg, LiveMidi*)
{
	newNoteComponent(msg);
}
void NoteGridComponent::noteRemoved(int ix, LiveMidi::Message::Ptr msg, LiveMidi*)
{
	if (noteComps[ix]->getModel()==msg)
	{
		noteComps.remove(ix);
		return;
	}
	else{
		for (int n=0; n < noteComps.size(); ++n)
		{
			if (noteComps[n]->getModel()==msg)
			{
				noteComps.remove(n);
				return;
			}
		}
	}
}
void NoteGridComponent::allNotesUpdated(LiveMidi*)
{
	noteComps.clear();
	for (int n=0; n < midi->getNumEvents(); ++n)
	{
		newNoteComponent(midi->getMessage(n));
	}
	repaint();
}

PNoteComponent* NoteGridComponent::newNoteComponent(LiveMidi::Message::Ptr msg)
{
	PNoteComponent * nn = new PNoteComponent(this, msg, styleSheet);
	noteComps.add(nn);
//	nn->onNoteSelect = [this](PNoteComponent * n, const juce::MouseEvent& e) {
//		this->noteCompSelected(n, e);
//	};
//	nn->onPositionMoved = [this](PNoteComponent * n) {
//		this->noteCompPositionMoved(n);
//	};
//	nn->onLegnthChange = [this](PNoteComponent * n, int diff) {
//		this->noteCompLengthChanged(n, diff);
//	};
//	nn->onDragging = [this](PNoteComponent * n, const juce::MouseEvent & e) {
//		this->noteCompDragging(n, e);
//	};
	addAndMakeVisible(nn);
	updateComp(nn);
	return nn;
}
void NoteGridComponent::updateComp(PNoteComponent* p)
{
	p->updateSize();
}

void NoteGridComponent::paint (juce::Graphics & g)
{
    g.fillAll(juce::Colours::darkgrey);
    
//    const int totalBars = (getWidth() / pixelsPerBar) + 1;
    
    
    //draw piano roll background first.
	float line = 0;//noteCompHeight;
        
	for (int i = 127; i >= 0; i--) {
		if (styleSheet->drawAsWhite(i))
			g.setColour(styleSheet->getWhiteRowColor());
		else
			g.setColour(styleSheet->getBlackRowColor());
				 
		g.fillRect(0, (int)line, getWidth(), (int)noteCompHeight);
		
		line += noteCompHeight;
	}
	
	line=0;
	g.setColour(juce::Colour::grey(.1));
	for (int i = 127; i >= 0; i--) {
		line += noteCompHeight;
		g.drawLine(0, line, getWidth(), line);
	}

    //again this is assuming 4/4
    const float increment = pixelsPerBar / currentQValue;
    line = increment;
//    g.setColour(juce::Colours::lightgrey);
	
    for (int i = 1; line < getWidth() ; i++) {
        float lineThickness=1.0f;
        if (i % 16 == 0) { //bar marker
			g.setColour(juce::Colour::grey(.95));
            lineThickness = 3.0;
        }
        else if (i % 4 == 0) { //1/4 div
			g.setColour(juce::Colour::grey(.85));
            lineThickness = 2.0;
        }
		else
			g.setColour(juce::Colour::grey(.8));
			
        g.drawLine(line, 0, line, getHeight(), lineThickness);

        line += increment;
    }
}
void NoteGridComponent::resized ()
{
	noteCompHeight = getHeight()/128.;
    for (auto component : noteComps) {
//        if (component->coordiantesDiffer) {
//            noteCompPositionMoved(component);
//        }
        // convert from model representation into component representation (translation and scale)
		updateComp(component);
    }
}

int NoteGridComponent::getStartYPosition()
{
	if (noteComps.size() > 0)
	{
		return noteComps[0]->getY() + noteComps[0]->getHeight()/2;
	}
	else
	{
		return getHeight()/2;
	}
}
int NoteGridComponent::getCenterYPosition(int viewH)
{
	if (noteComps.size() > 0)
	{
		float H = noteComps[0]->getHeight();
		float firstY = noteComps[0]->getY() - H/2;
		float centerY = 0;
		for (int n=0; n < noteComps.size(); ++n)
		{
			centerY += noteComps[n]->getY();
		}
		centerY /= noteComps.size();
		centerY -= H/2;
		
		float gap = (viewH/2-H/2);
		float dif = abs(centerY-firstY);
		if (dif < gap)
		{
			return centerY;
		}
		else
		{
			if (centerY > firstY)
				return firstY+gap;
			else
				return firstY-gap;
		}
	}
	else
	{
		return getHeight()/2;
	}
}
float NoteGridComponent::getNoteCompHeightToViewAll(int viewHeight, int& yStart, float minHeight)
{
	if (noteComps.size() > 0)
	{
		int min=127;
		int max=0;
		
		for (int n=0; n < noteComps.size(); ++n)
		{
			min = std::min(min, noteComps[n]->getNote());
			max = std::max(max, noteComps[n]->getNote());
		}
		if (max < 127)
			max++;
		if (min > 0)
			min--;
		int diff = max-min+1;
//		float centern = (max-min+1)/2.f;
		
		float height = viewHeight/diff;
		
		yStart = (127 - max) * height;// - viewHeight/2;
		
		if (minHeight > 0 && height < minHeight)
		{
			yStart += (minHeight-height)*127./2;
			height = minHeight;
		}
		
		
		return height;
	}
	else
	{
		yStart = -1;
		return 64*noteCompHeight;
	}

}

void NoteGridComponent::zoomNoteCompHeight(float zoom)
{
	float newH = zoom * noteCompHeight;
	if (newH < 5) newH = 5;
	if (newH > 30) newH = 30;
	if (fabs(noteCompHeight-newH) > .001)
	{
		setNoteCompHeight(newH);
	}
}

void NoteGridComponent::setNoteCompHeight(float set)
{
	if (fabs(set- noteCompHeight) > .01)
	{
//		std::cout<<noteCompHeight<<std::endl;
		noteCompHeight = set;
		setSize(getWidth(), noteCompHeight*128);
	}
}
void NoteGridComponent::setupGrid (float px, float compHeight, const int bars)
{
    pixelsPerBar = px;
    noteCompHeight = compHeight;
    setSize(pixelsPerBar * bars, compHeight * 128); //we have 128 slots for notes
}

void NoteGridComponent::setQuantization (const int val)
{
	currentQValue = val;
}


//callback from PNoteComponent
void NoteGridComponent::noteCompSelected (PNoteComponent * nc, const juce::MouseEvent& e)
{
    RETURN_IF_EDITING_DISABLED
    
	int dragMove=0;
    for (auto component : noteComps) {
        if (component->isMultiDrag) {
            dragMove++;
        }
    }
//    std::cout << "Drag: " << dragMove << "\n";
    
    for (auto component : noteComps) {
        if (component == nc) {
            component->setSelected(true);
            component->toFront(true);
        }
        /*
         This complicated if statement exists because if the user is dragging multiple notes around we don't want to clear the selection.
         We only want so switch the selected note when the user selects another note
         */
        else if (component->isSelected() && !e.mods.isShiftDown() && (dragMove==0)) {
            component->setSelected(false);
        }
    }
    // need to reset the multidrag
    for (auto component : noteComps) {
        if (component->isMultiDrag) {
            component->isMultiDrag = false;
        }
    }
    sendEdit();
}
void NoteGridComponent::noteCompPositionMoved (PNoteComponent * comp, bool callResize)
{
    RETURN_IF_EDITING_DISABLED
    
	// we want to move all the components...
	_sub_noteCompPositionMoved(comp);
	
	for (auto n : noteComps) {
		if (n != comp && n->isSelected()) {
			_sub_noteCompPositionMoved(n);
		}
	}
	
    if (callResize) {
        resized();
    }
    sendEdit();
}
void NoteGridComponent::_sub_noteCompPositionMoved (PNoteComponent* comp)
{
	
	//could do with refactoring this code here..
	float xPos = (comp->getX() / ((float)pixelsPerBar));
	int note = 127 - (comp->getY() / noteCompHeight);
	if (note > 127) {
		note = 127;
	}
	else if (note < 0) {
		note = 0;
	}
	
	if (xPos <= 0) {
		xPos = 0;
	}
	
	const float len = (comp->getWidth() / ((float)pixelsPerBar));
	auto nm = comp->getModel();
	//TODO: Streamline calls
	//TODO: Move to component
	nm->setNote(note, this);
	nm->setStart(xPos, this);
	nm->setLength(len, this);
	nm->quantize(currentQValue, this);
	//    nm.sendChange = sendChange;
	
	//todo: could make this toggleable behaviour
	lastNoteLength = nm->getNoteLength();
	
	comp->startY = -1;
	comp->startX = -1;
	comp->setValues(nm);
}


void NoteGridComponent::noteCompLengthChanged (PNoteComponent * original, int diff)
{
    RETURN_IF_EDITING_DISABLED
    
    for (auto n : noteComps) {
        if (n->isSelected() || n == original) {
            if (n->startWidth == -1) {
                n->startWidth = n->getWidth();
            }
		
            const int newWidth = n->startWidth - diff;
			n->setAndUpdateLength(std::max(20, newWidth));
        }
    }
    sendEdit();
}

void NoteGridComponent::noteCompDragging (PNoteComponent* original, const juce::MouseEvent& event)
{
    RETURN_IF_EDITING_DISABLED
    
    for (auto n : noteComps) {
        if (n->isSelected() && n != original) {
            
            const int movedX = event.getDistanceFromDragStartX();// (event.getx - original->startX);
            const int movedY = event.getDistanceFromDragStartY(); //(original->getY() - original->startY);
            
            if (n->startY == -1) {
                n->startX = n->getX();
                n->startY = n->getY();
            }
            
            /*
            std::cout << "Started at: " << n->startX << " - " << n->startY << "\n";
            std::cout << n->getBounds().toString() << "\n";
             */
            
            const int newX = n->startX + movedX;
            const int newY = n->startY + movedY;
            const int xDif = abs(newX - n->startX);
            const int yDif = abs(newY - n->startY);
            if (xDif > 2 || yDif > 2) { //ingnore a small amount of jitter.
                n->setTopLeftPosition(newX, newY);
                n->isMultiDrag = true;
            }
        }
    }
    

    /*
     This enables the notes to be triggered while dragging.
     */
    int note = 127 - (original->getY() / noteCompHeight);
    if (note > 127) { note = 127; }
    else if (note < 0) { note = 0; }
    
}
void NoteGridComponent::setPositions ()
{
    //unused..
}

void NoteGridComponent::mouseDown (const juce::MouseEvent&)
{
    RETURN_IF_EDITING_DISABLED
    
    for (PNoteComponent * component : noteComps) {
        component->setSelected(false);
    }
    sendEdit();
}
void NoteGridComponent::mouseDrag (const juce::MouseEvent& e)
{
    RETURN_IF_EDITING_DISABLED
    
    if (!selectorBox.isVisible()) {
        selectorBox.setVisible(true);
        selectorBox.toFront(false);
        
        selectorBox.setTopLeftPosition(e.getPosition());
        selectorBox.startX = e.getPosition().x;
        selectorBox.startY = e.getPosition().y;
    }
    else {
        int xDir = e.getPosition().x - selectorBox.startX;
        int yDir = e.getPosition().y - selectorBox.startY;
        
        //work out which way to draw the selection box
        if (xDir < 0 && yDir < 0) { //top left
            selectorBox.setTopLeftPosition(e.getPosition().x, e.getPosition().y);
            selectorBox.setSize(selectorBox.startX - e.getPosition().getX(), selectorBox.startY - e.getPosition().getY());
        }
        else if (xDir > 0 && yDir < 0) { //top right
            selectorBox.setTopLeftPosition(selectorBox.startX, e.getPosition().y);
            selectorBox.setSize(e.getPosition().getX() - selectorBox.startX, selectorBox.startY - e.getPosition().getY());
        }
        else if (xDir < 0 && yDir > 0) { //bottom left
            selectorBox.setTopLeftPosition(e.getPosition().x, selectorBox.startY);
            selectorBox.setSize(selectorBox.startX - e.getPosition().getX(), e.getPosition().getY() -  selectorBox.startY);
        }
        else { //bottom right
            selectorBox.setSize(e.getPosition().getX() - selectorBox.getX(), e.getPosition().getY() - selectorBox.getY());
        }
    }
}
void NoteGridComponent::mouseUp (const juce::MouseEvent&)
{
    RETURN_IF_EDITING_DISABLED
    
    if (selectorBox.isVisible()) {
        for (PNoteComponent * component : noteComps) {
            if (component->getBounds().intersects(selectorBox.getBounds())) {
                component->setSelected(true);
            }
            else {
                component->setSelected(false);
            }
        }
        selectorBox.setVisible(false);
        selectorBox.toFront(false);
        selectorBox.setSize(1,1);
    }
    
    sendEdit();
    
}

void NoteGridComponent::mouseDoubleClick (const juce::MouseEvent& e)
{
    RETURN_IF_EDITING_DISABLED
    
    const float xPos = (e.getMouseDownX() / ((float)pixelsPerBar));
    const int yIn = ((float)e.getMouseDownY() / noteCompHeight);
    const int note = 127 - yIn;
	
    jassert(note >= 0 && note <= 127);
        
	// Create new Note
	const float defaultVelocity = 100/127.f;
	//	Message::Ptr addNote(int note, float start, float length=.25, float velo=1.0f, Listener* src=NULL);

	auto newNote = midi->addNote(note, xPos, lastNoteLength, defaultVelocity, this);
	newNote->quantize(currentQValue);
	
	newNoteComponent(newNote);
    
    resized();
    repaint();
    sendEdit();
}


bool NoteGridComponent::keyPressed (const juce::KeyPress& key, juce::Component* originatingComponent)
{
    
#ifndef LIB_VERSION
    LOG_KEY_PRESS(key.getKeyCode(), 1, key.getModifiers().getRawFlags());
#endif
    
    if (styleSheet->disableEditing) {
        return true;
    }
    if (key == juce::KeyPress::backspaceKey) {
        //
        deleteAllSelected();
        sendEdit();
        return true;
    }
    else if (key == juce::KeyPress::upKey || key == juce::KeyPress::downKey) {
        for (auto nComp : noteComps) {
            if (nComp->isSelected()) {
				nComp->getModel()->moveN((key == juce::KeyPress::upKey) ? 1 : -1, this);
				updateComp(nComp);
				return true;
            }
        }
        
    }
    else if (key == juce::KeyPress::leftKey || key == juce::KeyPress::rightKey) {
        const int nudgeAmount = currentQValue;
        for (auto nComp : noteComps) {
            if (nComp->isSelected()) {
				nComp->getModel()->moveT((key == juce::KeyPress::rightKey) ? nudgeAmount : -nudgeAmount, this);
				updateComp(nComp);
				return true;
            }
        }
    }
    return false;
}

void NoteGridComponent::deleteAllSelected ()
{
	for (int i = noteComps.size()-1; i>=0; i--) {
        if (noteComps[i]->isSelected()) {
			noteComps[i]->getModel()->deleteMessage(this);
            noteComps.remove(i);
        }
    }
}
float NoteGridComponent::getNoteCompHeight ()
{
    return noteCompHeight;
}
float NoteGridComponent::getPixelsPerBar ()
{
    return pixelsPerBar;
}
std::vector<LiveMidi::Message::Ptr> NoteGridComponent::getSelectedModels ()
{
    std::vector<LiveMidi::Message::Ptr> noteModels;
    for (auto comp : noteComps) {
        if (comp->isSelected()) {
           noteModels.push_back(comp->getModelPtr());
        }
    }
    return noteModels;
}

void NoteGridComponent::sendEdit ()
{
    if (this->onEdit != nullptr) {
        this->onEdit();
    }
}
