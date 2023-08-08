//
//  PresetCombo.h
//  
//
//  Created by Isaac Roberts on 10/14/22.
//

#ifndef PresetCombo_h
#define PresetCombo_h

#include <JuceHeader.h>
#include "PresetFileList.h"

class PresetCombo : public ComboBox, PresetFileList::Listener
{
public:
	PresetCombo()
	: ComboBox(),
	PresetFileList::Listener()
	{
		setup();
	}
	PresetCombo(PresetFileList* presetList)
	: ComboBox(),
	PresetFileList::Listener()
	{
		presetList->addListener(this);
		
		setup();
	}
	PresetCombo(PresetFileList& presetList) : PresetCombo(&presetList) {}
	
	void setup()
	{
//		setEditableText(true);
		
		onChange = [&] () { onComboChange(); };
	}
	
	void onComboChange()
	{
		presetFileList->setSelection(getSelectedId()-1);
	}
	
	//jassert (presetFileList != NULL);
	
	
	
	void presetNameChanged(int index, String text) override
	{
		changeItemText(index+1, text);
	}
	void presetSelectionChanged(int current) override {
		setSelectedId(current+1);
	}
	void presetOptionsChanged() override {
		clear();
		for (int n=0; n < presetFileList->getNumOptions(); ++n)
		{
			addItem(presetFileList->getOption(n), n+1);
		}
		setSelectedId(presetFileList->getSelected()+1);
	}
	
};


class PresetComboFixedSections : public ComboBox, PresetFileList::Listener
{
public:
	PresetComboFixedSections()
	: ComboBox(),
	PresetFileList::Listener()
	{
		setup();
	}
	PresetComboFixedSections(PresetFileList* presetList)
	: ComboBox(),
	PresetFileList::Listener()
	{
		presetList->addListener(this);
		
		setup();
	}
	PresetComboFixedSections(PresetFileList& presetList) : PresetComboFixedSections(&presetList) {}
	
	void setup()
	{
//		setEditableText(true);
		
		onChange = [&] () { onComboChange(); };
	}
	
	void onComboChange()
	{
		presetFileList->setSelection(getSelectedId()-1);
	}
	
	//jassert (presetFileList != NULL);
	
	
	
	void presetNameChanged(int index, String text) override
	{
		changeItemText(index+1, text);
	}
	void presetSelectionChanged(int current) override {
		setSelectedId(current+1);
	}
	void presetOptionsChanged() override {
		clear();
		int curSecIx=0;
		for (int n=0; n < presetFileList->getNumOptions(); ++n)
		{
			if (curSecIx < sections.size() && n==sections[curSecIx]->pos)
			{
				addSectionHeading(sections[curSecIx]->title);
				curSecIx ++;
			}
			addItem(presetFileList->getOption(n), n+1);
		}
		setSelectedId(presetFileList->getSelected()+1);
	}
	
	//Remember to call presetOptionsChanged() after calling this
	void addSection(int pos, String title) {
		sections.add(new SectionPosition(pos, title));
	}
	
protected:
	struct SectionPosition {
		SectionPosition(int n, String t) : pos(n), title(t) {}
		int pos;
		String title;
	};
	juce::OwnedArray<SectionPosition> sections;
};


#endif /* PresetCombo_h */
