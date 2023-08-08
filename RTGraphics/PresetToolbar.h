//
//  PresetToolbar.h
//  Tremelon
//
//  Created by Isaac Roberts on 5/31/20.
//

#ifndef PresetToolbar_h
#define PresetToolbar_h

#include <stdio.h>

#include <JuceHeader.h>

#include "PresetHandler.h"

class PresetToolbar : public Component
{
public:
	
	PresetToolbar(int buttonAmt=5);
//	PresetToolbar();
	
	
	
	void setPresetHandler(PresetHandlerBase* b);
	
//	void updateComboBox(PresetHandlerBase* p);
	
    inline int scale(int x, float s) {
        return int(ceil(x * s));
    }
	void resized();
	
	virtual Button* createButton(int id);

//	void set_a();
//	void set_b();
//	void swap_presets();
	void save();
	void trash();
	
	~PresetToolbar();

	void saveState (XmlElement* xml) {
		handler->saveState(xml);
	}
	void loadState (XmlElement* xml) {
		handler->loadState(xml);
	}
	
	PresetHandlerBase* getPresetHandler() { return handler; }
	
protected:
	PresetHandlerBase* handler;
	//folder, prev, next, save, trash, a, swap, b
	enum ButtonID {
		Folder=0, Prev, Next, Save, Trash
		//, A_, Swap, B_,
	};
	int buttonAmt;
	
	Button** button;

	void refreshPresetBox();
	
	bool presetBoxShowing;
	
	const int lTextOffset=10;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetToolbar);
};


#endif /* PresetToolbar_h */
