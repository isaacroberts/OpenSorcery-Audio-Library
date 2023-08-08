//
//  DebugRig.h
//  
//
//  Created by Isaac Roberts on 8/7/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "TestRigBase.h"

using namespace juce;

template <class Processor>
class DebugRig : public Component, public TestRigBase, public juce::Timer
{
public:
	//Creates labels in resized function, HashMap prevents adding multiple labels per component
	juce::HashMap<Component*, Label*> labels;
	DebugRig(Processor& p)
	: Component(),
	proc(p)
	{
		//	Label display;
	//	TextButton random, reset;

		display.setText("Howdy", dontSendNotification);
		display.setJustificationType(Justification::centredLeft);
		
		random.setButtonText("Randomize");
		reset.setButtonText("Default");
		
		addAndMakeVisible(display);
		addAndMakeVisible(random);
		addAndMakeVisible(reset);
	}

	
	virtual void timerCallback() override {}
	virtual void extraConnect(Processor* p) {}
	
	void connectKnobs()
	{
		auto preset = proc.getPreset();
		
		random.onClick = [=]() { preset->randomize(); };
		reset.onClick = [=]() { preset->resetToDefaults(); };
	
		extraConnect(&proc);
	}
	
	int getRigDim() override 		{ return 100; }
	bool isSplitHoriz() override 		{ return false;}

	
	void paint(Graphics& g) override
	{
		g.fillAll(Colour::hex(0x666666));
	}

	void layoutCompsWithDisplay(Component** arr, int amt)
	{
		auto r = getBounds();
		r.setPosition(0, 0);
		
		float bwp = std::min(.2f, 1.f/(2+amt));
		int bw = int(getWidth() * bwp);
		
		for (int n=amt-1; n>=0; --n)
		{
			auto rr = r.removeFromRight(bw);
			arr[n]->setBounds(rr.reduced(10));
		}
		
		display.setBounds(r.reduced(10));
	}
	void layoutCompRow(Component** arr, int amt, int row, int totalRows)
	{
		float th = getHeight();
		Rectangle<int> r{0, int(row*th/totalRows), getWidth(), int(th/totalRows)};
		
		float bwp = 1.f/amt;
		int bw = int(getWidth() * bwp);
		
		for (int n=amt-1; n>=0; --n)
		{
			auto rr = r.removeFromRight(bw);
			arr[n]->setBounds(rr.reduced(10));
		}
	}
	void layoutCompRowWithLabels(Component** arr, int amt, int row, int totalRows)
	{
		float th = getHeight();
		Rectangle<int> r{0, int(row*th/totalRows), getWidth(), int(th/totalRows)};
		
		int lh = std::min(r.getHeight()*.2f, 40.f);
		float bwp = 1.f/amt;
		int bw = int(getWidth() * bwp);
		
		for (int n=amt-1; n>=0; --n)
		{
			auto rr = r.removeFromRight(bw);
			
			if (arr[n]->getName()!="")
			{
				Label* l;
				if (labels.contains(arr[n])) {
					l = labels[arr[n]];
				}
				else
				{
					l = new Label();
					l->setText(arr[n]->getName(), dontSendNotification);
					l->setJustificationType(Justification::centred);
					addAndMakeVisible(l);
					labels.set(arr[n], l);
				}
				l->setBounds(rr.removeFromTop(lh));
				rr= rr.withTrimmedLeft(10).withTrimmedRight(10).withTrimmedBottom(10);
			}
			arr[n]->setBounds(rr);
		}
	}

	
	
	void resized() override
	{
//		layoutCompsWithLabel({random, reset}, 2);
	}

	
	void lookAndFeelChanged() override
	{
		auto* laf = &getLookAndFeel();
		reset.setLookAndFeel(laf);
		random.setLookAndFeel(laf);
		display.setLookAndFeel(laf);
	}

	
	virtual ~DebugRig() {
		for (auto l = labels.begin(); l != labels.end(); ++l)
		{
			delete *l;
		}
	}
	
protected:
	Processor& proc;
	Label display;
	TextButton random, reset;
	
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DebugRig);

};
