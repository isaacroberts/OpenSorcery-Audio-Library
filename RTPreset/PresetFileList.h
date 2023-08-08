//
//  PresetFileList.h
//  
//
//  Created by Isaac Roberts on 10/14/22.
//

// Created by Isaac Roberts 

#ifndef PresetFileList_h
#define PresetFileList_h

#include <JuceHeader.h>

class PresetFileList {
public:
	class Listener {
	public:
		Listener() {}
		virtual ~Listener() {
			if (presetFileList)
				presetFileList->removeListener(this);
		}
		virtual void presetNameChanged(int listIndex, String text) {}
		virtual void presetSelectionChanged(int current) {}
		virtual void presetOptionsChanged() {}
	protected:
		PresetFileList* presetFileList=NULL;
		
		friend class PresetFileList;
	};
protected:
	enum Type {
		File, Default, Autosave, Bug
	};
	struct PresetData {
		PresetData() {}
		juce::String name;
		std::unique_ptr<XmlElement> xml=nullptr;
		juce::File file;
		Type type=Bug;
		~PresetData() {
		}
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetData);
	};
public:
	static juce::File getDefaultDirectory();
	PresetFileList(PresetObject* preset, bool useAutosave=false, juce::File directory=juce::File());
//	PresetFileList(PresetObject* preset) : PresetFileList(preset, getDefaultDirectory()) {}
	
	virtual ~PresetFileList();
	
	void setDirectory(juce::File f, bool createIfNotExists=true);
	
	void addDefaultFromBinary(const char* data, size_t size);
	void addDefault(juce::String name, juce::String data);

	void setSelection(int ix);
	
	void remove(juce::String tag);
	void remove(juce::File f);
	void remove(int n);
	
	void save(juce::String name);
	void save(juce::File f);

	void chooseFileToSave();
	void chooseFileToRemove();
	void chooseFileToRename();
	
	int getNumOptions();
	String getOption(int n);
	
	int getSelected() { return selectedPreset; }
	
//	XmlElement* getXML(int n) {
//		return &*presets[n]->xml;
//	}
	
	void addListener(Listener* l);
	void addListener(Listener& l) { addListener(&l); }
	
private:
	void removeListener(Listener* l);
	
	
protected:
	PresetData* getCurrentElement();
	
	void paramValueChanged();
	
	void clearList();
public:
	void scan();
protected:
	void sortPresets();
	
	void saveToBlankElement();
	void load();
	
	void updateSelection();
	
	void updateListenerOptions();
	void updateListenerSelection();
	void updateListenerName(int n, String name);

	void _inthread_updateListenerOptions();
	void _inthread_updateListenerSelection();
	void _inthread_updateListenerName(int n, String name);

protected:
	PresetObject* livePreset;
	juce::OwnedArray<PresetData> presets;
	PresetData* autosave;
	
	juce::File directory;
	
	bool useAutosave;
	
	int selectedPreset;
//	juce::String selectedPresetName;
	bool hasChangedSinceLoad=false;
	std::unique_ptr<FileChooser> fileChooser;
	
	juce::ListenerList<Listener> listeners;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetFileList);
};

#endif /* PresetFileList_h */
