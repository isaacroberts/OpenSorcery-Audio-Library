//
//  TestRigSplitter.h
//  
//
//  Created by Isaac Roberts on 9/15/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

//using namespace juce;

#include "RT/GuiEditor.h"

#include "RigSlider.h"
#include "RigNumberInput.h"
#include "TestRigBase.h"

//#include "../"

template <class Editor, class Rig>
class TestRigSplitter : public GuiEditor
{
public:
	template <class Processor>
	TestRigSplitter(Processor& p)
	: GuiEditor(p), editor(p), rig(p)
	{
		addAndMakeVisible(editor);
		addAndMakeVisible(rig);
		
		rigLabel.setText("Test Rig", dontSendNotification);
		rigLabel.setJustificationType(Justification::centred);
		addAndMakeVisible(rigLabel);
				
		int w = editor.getWidth();
		int h = editor.getHeight();
		
		defaultEditorDim = rig.isSplitHoriz() ? h : w;
		
		if (rig.isSplitHoriz())
			w += rig.getRigDim();
		else
			h += rig.getRigDim();
		
//		setSize(w, h);
//
//		getConstrainer()->setFixedAspectRatio(float(w)/h);
//		setResizable(true, true);
		
		editor.setResizable(true, false);
		setRatio(float(w)/h, w);
		
		completeSetup();
	}
	
	void connectKnobs() override {
		editor.doConnectKnobs();
		rig.connectKnobs();
	}

	Editor& getEditor()  {
		return editor;
	}
	Rig& getRig() {
		return rig;
	}
	
	void paint (juce::Graphics& g) override
	{
		g.setColour(Colours::black);
		g.drawRect(editor.getBounds());
		g.drawRect(rig.getBounds());
	}
	void resized() override
	{
		bool isHoriz = rig.isSplitHoriz();
		int w = getWidth();
		int h = getHeight();
		
		int currentDim = rig.isSplitHoriz() ? h : w;

		int dim = rig.getRigDim() * (float(currentDim)/defaultEditorDim);
		
		if (isHoriz) {
			editor.setBounds(0, 0, w-dim, h);
			int rlh = 25;
			rigLabel.setBounds(w, 0, dim, rlh);
			rig.setBounds(w-dim, rlh, dim, h-rlh);
		}
		else
		{
			editor.setBounds(0, 0, w, h-dim);
			rig.setBounds(0, h-dim, w, dim);
		}
	}
	
	virtual ~TestRigSplitter() {}
	
	void saveState (XmlElement* xml) override {
		editor.saveState(xml);
		rig.saveState(xml->createNewChildElement("Test_Rig"));
	}
	void loadState (XmlElement* xml) override {
		editor.loadState(xml);
		auto rigState = xml->getChildByName("Test_Rig");
		if (rigState)
			rig.loadState(rigState);
	}

	
public:
//	Processor& proc;
	Editor editor;
	Label rigLabel;
	Rig rig;
	int defaultEditorDim;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestRigSplitter);

};
