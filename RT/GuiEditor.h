//
//  GuiEditor.h
//  
//
//  Created by Isaac Roberts on 3/22/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "BasicFXProcessor.h"

using namespace juce;

class GuiEditor : public juce::AudioProcessorEditor
{
public:
	GuiEditor(BasicFXProcessor& b)
	: AudioProcessorEditor(b),
	basicProcessor(b),
	finishedConstructing(false),
	connectedKnobs(false)
	{
		
	}
	GuiEditor(BasicFXProcessor* b)
	: AudioProcessorEditor(b),
	basicProcessor(*b),
	finishedConstructing(false),
	connectedKnobs(false)
	{
		
	}
	
protected:
	
	Point<int> pluginMargin()
	{
		//Not exact, better for these to be higher than lower
		Point<int> marg;
		//Consider mac vs windows 
		if (Format::isStandalone())
			marg.setXY(0, 50);
		if (Format::getHost().isLogic())
			marg.setXY(0, 200);
		else if (Format::getHost().isReaper())
			marg.setXY(250, 30);
		else if (Format::getHost().isFruityLoops())
			marg.setXY(200, 160);
		else if (Format::getHost().isProTools())
			marg.setXY(0, 200);
		else
			marg.setXY(300, 200);
		return marg;
	}
	
	void completeSetup()
	{
		auto* disp = Desktop::getInstance().getDisplays().getPrimaryDisplay();
		if (disp)
		{
			float w = getWidth();
			float h = getHeight();
			if (w != 0 && h != 0)
			{
				Rectangle<int> s = disp->userArea;
				
				Point<int> marg = pluginMargin();
				marg += Point<int>(10, 20);
				
				float wm = w + marg.getX();
				float hm = h + marg.getY();
				if (wm > s.getWidth() || hm > s.getHeight())
				{
					float ratio = jmin(1.f, s.getWidth()/wm, s.getHeight()/hm);
					setSize(w*ratio, h*ratio);
				}
			}
			else jassertfalse;
		}

		finishedConstructing=true;
		if (basicProcessor.hasPreset())
			doConnectKnobs();
		
		if (basicProcessor.hasUiXml())
		{
			auto xml = basicProcessor.getUiXml();
			loadState(&*xml);
		}
	}
	void setRatio(float w_over_h, float width=1000)
	{
		setResizable(true, true);
		getConstrainer()->setFixedAspectRatio(w_over_h);
		setSize (width, width/w_over_h);
	}
	
//	void saveThisXmlForLater(
	
public:
	void connectFromProcessor()
	{
		if (finishedConstructing && !connectedKnobs)
		{
			connectKnobs();
			connectedKnobs=true;
		}
	}
	
	void doConnectKnobs() {
		if (!connectedKnobs) {
			connectKnobs();
			connectedKnobs=true;
		}
	}
	void markKnobsDisconnected() {
		connectedKnobs=false;
	}
protected:
	virtual void connectKnobs() = 0;
public:
	
	virtual void saveState (XmlElement* xml) {}
	virtual void loadState (XmlElement* xml) {}
	
	
	virtual ~GuiEditor()
	{
		//You forgot to call completeSetup()!
		jassert(finishedConstructing);
		basicProcessor.clearUiCallbacks();
		
		//TODO: Do not save in all cases, for example if FXProcessor has already saved
		if (true)
		{
			XmlElement* xml = basicProcessor.createUiXml();
			saveState(xml);
			basicProcessor.setUiXml(xml);
		}
	}
	
	BasicFXProcessor& getBasicProcessor() {return basicProcessor; }
	bool hasFinishedConstructing() { return finishedConstructing; }
	
protected:
	BasicFXProcessor& basicProcessor;
	bool finishedConstructing=false;
	bool connectedKnobs=false;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiEditor);

};
