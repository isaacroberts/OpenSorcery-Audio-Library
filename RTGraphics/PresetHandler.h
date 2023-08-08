//
//  PresetHandler.h
//  
//
//  Created by Isaac Roberts on 8/16/20.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

//#include "Presets/Preset.h"

#include "FileLoad.h"
#include "RTGraphics/DebugWindow.h"

class PresetHandlerBase : public ComboBox
{
public:
	virtual void select_folder() = 0;
	virtual void previous() = 0;
	virtual void next() = 0;
	virtual void save_preset() = 0;
	virtual void trash_preset() = 0;
	
	
	virtual void saveState (XmlElement* parent) = 0;
	virtual void loadState (XmlElement* parent) = 0;
	
	virtual void addDefault(String name, const char* data) = 0;
	virtual void updateAfterDefaults() = 0;
	
	
	
	static String nameFromPath(String fullPath) {
		String s = fullPath.substring(fullPath.lastIndexOf(FileLoad::sep));
		s = s.upToFirstOccurrenceOf(".", false, false);
		return s;
	}
	
	struct PresetOption {
		PresetOption(String n) : name(n) {}
		String name;
		virtual bool isFolder() { return false; }
		virtual ~PresetOption() {}
	};
	struct PresetFile : public PresetOption
	{
		PresetFile(String fullPath) : PresetOption(nameFromPath(fullPath)), file (fullPath) {}
		PresetFile(File f) : PresetOption(f.getFileNameWithoutExtension()), file (f) {}
		File file;
		virtual ~PresetFile() {}
	};
	struct PresetDefault : public PresetOption
	{
		PresetDefault(String n) : PresetOption(n), xml(nullptr), success(false) {}
		PresetDefault(String n, XmlElement* xml_) : PresetOption(n), xml(xml_) {
			success = (xml != nullptr);
		}
		PresetDefault(String n, const char* data) : PresetOption(n) {
			xml = juce::parseXML(data);
			success = (xml != nullptr);
		}
		std::unique_ptr<XmlElement> xml;
		bool success;
	};
	struct PresetFolder : public PresetOption
	{
		PresetFolder(String name) : PresetOption(name), presets() {}
		OwnedArray<PresetFile> presets;
		virtual bool isFolder() { return true; }
		virtual ~PresetFolder() {}
	};
	struct PresetFileFolder : public PresetFolder
	{
		PresetFileFolder(File f) : PresetFolder(f.getFileNameWithoutExtension()), file (f) {}
		PresetFileFolder(String fullPath) : PresetFolder(nameFromPath(fullPath)), file (fullPath) {}
		File file;
	};
	

	
};

template<class PresetT>
class PresetHandler :  public PresetHandlerBase
{
	
public:
	PresetHandler(typename PresetT::Preset* loadedPreset)
	: directory(), defaults()
	{
//		onPresetChange = [&](auto){};
//		onKnobsChange = [&](auto) {};
		
		currentPreset = loadedPreset;
		
		setEditableText(true);
		
		File file = File::getSpecialLocation(File::userApplicationDataDirectory);
		
		String companyFolder = ProjectInfo::companyName;
//		companyFolder = companyFolder.removeCharacters ("\"#@,;:<>*^|?\\/.");
		companyFolder = companyFolder.removeCharacters(".");
		companyFolder = juce::File::createLegalFileName(companyFolder);
		
//		if ((SystemStats::getOperatingSystemType() & SystemStats::MacOSX) != 0)
//		{
//			file = file.getChildFile("Audio");
//			if (!file.exists())
//				file.createDirectory();
//			file = file.getChildFile("Presets");
//			if (!file.exists())
//				file.createDirectory();
//		}
		file = file.getChildFile(ProjectInfo::companyName);
		if (!file.exists())
			file.createDirectory();
		file = file.getChildFile(ProjectInfo::projectName);
		if (!file.exists())
			file.createDirectory(); 
		file = file.getChildFile("Presets");
		if (!file.exists())
			file.createDirectory();
		path = file.getFullPathName();
		
		Debug::print("File location", path);
		
		setTextWhenNoChoicesAvailable("No presets found.");

//		std::cout<<"Preset path = "<<path<<std::endl;
		scanDir(false);
		
		if (currentPreset->getName() != "")
		{
			current_name = currentPreset->getName();
			Debug::print("presetName loaded from presets's storage", current_name);
			setText(current_name, dontSendNotification);
			Debug::print("Text = ",getText(), "ID = ", getSelectedId());
		}
		else {
			current_name="";
		}
	}
	
	void addDefault(String name, const char* data) override
	{
		defaults.add(new PresetDefault(name, data));
	}
	void updateAfterDefaults() override
	{
		updateComboBox(true);
		
		if (currentPreset->getName() != "")
		{
			current_name = currentPreset->getName();
			Debug::print("presetName after defaults loaded from presets's storage", current_name);
			setText(current_name, dontSendNotification);
			Debug::print("Text = ",getText(), "ID = ", getSelectedId());
		}
	}
	
//	std::function<void(typename PresetT::Preset*)> onPresetChange;
//	std::function<void(typename PresetT::Preset*)> onKnobsChange;
	
	void setDir(String set)
	{
		path = set;
		directory.clear();
		scanDir(false);
	}
	void scanDir(bool keepId)
	{
		File f(path);
		
		auto childFiles = f.findChildFiles(File::findFiles, false);
		
		for (File entry : childFiles)
		{
//			if (entry.isDirectory())
//			{
//				if (!entry.isHidden())
//				{
//					directory.add(PresetFileFolder(entry));
//				}
//			}
//			else
				if (entry.hasFileExtension(FileLoad::presetSuffix))
					if (! entry.isHidden())
					{
						String name = entry.getFileNameWithoutExtension();
						
						//Ensure sanitized
//						String fixedName = sanitize_filestring(name);
//						if (fixedName != name)
//						{
//							String movedName = fixedName+"."+FileLoad::presetSuffix;
//							File movedFile = entry.getParentDirectory().getChildFile(movedName);
//							name = fixedName;
//							Debug::print("Moved invalid file",name,"to",movedFile.getFullPathName());
//							entry.moveFileTo(File(movedFile));
//						}
						
						directory.add(name);
					}
		}
		
		directory.sortNatural();
		
		updateComboBox(keepId);
	}
private:
	void addUsersSeparator()
	{
		addSeparator();
	}
public:
	void updateComboBox(bool keepId)
	{
//		std::cout<<"update combo box "<<std::endl;
		Debug::print("Update combo box", keepId ? "keepId" : "dont keep id");
		int id = getSelectedId();
		this->clear();
		
		int runningId = 1;
		
		
		if (defaults.size () > 0)
		{
//			addSeparator();
			//Section heading is really ugly and oversized
//			addSectionHeading("Magic Presets");
		}
		
		for (int n=0; n < defaults.size(); ++n)
		{
			this->addItem(defaults[n]->name, runningId++);
		}
		
		if (defaults.size () > 0 && directory.size() > 0)
		{
			addUsersSeparator();
		}
		
		for (int i = 0; i < directory.size(); ++i)
		{
			if (directory[i] != "")
				this->addItem (directory[i], runningId++); // 1-indexed ID for ComboBox
		}
		
		if (id >0)
			setSelectedId(id, dontSendNotification);
	}
	
	void previous() override
	{
		int id = this->getSelectedId();
		if (id==-1)
		{
			//No item selected
			id = this->getNumItems();
		}
		else
		{
			--id;
			if (id <= 0)
				id = this->getNumItems();
		}
		setSelectedId(id);
		
		int ix = getDefaultIndex(getText());
		
		if (ix != -1)
		{
			if (loadDefault(ix))
				current_name = getText();
		}
		else
		{
			if (loadPreset(getText(), true))
			{
				current_name = getText();
			}
		}
	}
	void next() override
	{
		int id = this->getSelectedId();
		if (id==-1)
		{
			//No item selected
//			id = this->getNumItems()-1;
			id = 0;
		}
		else
		{
			++id;
			if (id > this->getNumItems())
				id = 1 ;
		}
		setSelectedId(id);
		
		int ix = getDefaultIndex(getText());
		
		if (ix != -1)
		{
			if (loadDefault(ix))
				current_name = getText();
		}
		else
		{
			if (loadPreset(getText(), true))
			{
				current_name = getText();
			}
		}
	}
	
	virtual bool saveOnSwitch() {
		//Save previous preset to previous name to keep an 'open tabs' feel
		return false;
	}
	int getDefaultIndex(String name)
	{
		for (int n=0; n < defaults.size(); ++n)
		{
			if (defaults[n]->name == name)
			{
				return n;
			}
		}
		return -1;
	}
	
	String sanitize_filestring(String string)
	{
		string = FileLoad::removeInvalidFileCharactersForAllFilesystems(string);
		string = string.trim();
		
		return string;
	}
	void save_preset() override
	{
		String presetName = getText();
		
		presetName = sanitize_filestring(presetName);
		
		if (presetName.isEmpty())
		{
			setErrText("Enter a Name!");
			return;
		}
		
		if (getDefaultIndex(presetName)!=-1)
			return;
			
		String full_path = getFullPath(presetName);
		
		
		if (savePreset(full_path, presetName)){
			if (directory.contains(presetName, true))
			{
			}
			else
			{
				if (directory.size() == 0)
					addUsersSeparator();
				
				directory.add(presetName);
				
				addItem(presetName, defaults.size() + directory.size());
			}
		}
		
//		updateComboBox(true);
	}
	void trash_preset() override
	{
		String presetName = getText();
		
		if (presetName.isEmpty())
			return;
		
		if (getDefaultIndex(presetName) != -1)
			return;
		
		
		String full_path = getFullPath(presetName);
		//remove file
		remove(full_path.toRawUTF8());
		
		remove_preset_from_list();
		
	}
	void remove_preset_from_list()
	{
		int id = getSelectedId();

		if (id <= 0)
			return;
		
		String presetName = getText();

		if (presetName.isEmpty())
			return;
		
		directory.removeString(presetName);
		updateComboBox(false);
			
		if (id==1)
		{
			if (getNumItems() >= 1)
				setSelectedId(1, dontSendNotification);
		}
		else
		{
			setSelectedId(id-1, dontSendNotification);
		}
	}
	
	void folder_selected(const FileChooser& pathChooser)
	{
		if (pathChooser.getResult().getFullPathName().isEmpty())
			return;
		path = pathChooser.getResult().getFullPathName();
		directory.clear();
		scanDir(false);
	}
	void select_folder() override
	{
		FileChooser pathChooser("Select Preset folder.", File(path));

//		bool selected = pathChooser.browseForDirectory();
//		if (!selected)
//			return;
		
		pathChooser.launchAsync(  FileBrowserComponent::canSelectDirectories
								| FileBrowserComponent::openMode
								| FileBrowserComponent::filenameBoxIsReadOnly
								| FileBrowserComponent::useTreeView,
							[&](const FileChooser& c){folder_selected(c); });
	}
	
	void valueChanged (Value & value) override
	{
		String text = getText();
		text = text.trim();
//		std::cout<<"Value = "<<value.toString()<<"; text = "<<text<<std::endl;
		if (text != "")
		{
			if (!text.startsWith("Err:"))
			{
				textSet(text, true);
			}
		}
	}
//	void textChanged()
//	{
//		String text = getText();
//		if (!text.startsWith("Err:"))
//		{
//			textSet(text, false);
//		}
//	}
	void textSet(String curOption, bool fromDropdown)
	{
		curOption = sanitize_filestring(curOption);
		
		if (curOption=="")
			return;
		if (current_name == curOption )
		{
//			preset should already have correct name
//			attemptSwitchSave();
		}
		else
		{
			int defaultIx = getDefaultIndex(curOption);
			
			if (defaultIx!=-1)
			{
				loadDefault(defaultIx);
			}
			else if (directory.contains(curOption))
			{
				if (fromDropdown)
				{
					if (current_name != "")
					{
						attemptSwitchSave();
					}
					
					if (loadPreset(curOption, true))
					{
					}
					else
						curOption = "";
				}
				else
				{
					//setting id already handled by ComboBox
				}
			}
			else
			{
				if (fromDropdown)
				{
					
					if (loadPreset(curOption, true))
					{
	//					directory.addIfNotAlreadyThere(curOption);
					}
					else
						curOption = "";
				}
				else
				{
					int ix = directory.indexOf(current_name);
					currentPreset->setName(curOption);
					changeItemText(ix+1, curOption);
				}
			}
		}
		current_name = curOption;
	}
	
	~PresetHandler()
	{
		directory.clear();
	}
	
	void saveState (XmlElement* parent) override
	{
		Debug::print("PresetHandler::xmlSave");
		XmlElement* xml = new XmlElement (xmlIdentifier());
		parent->addChildElement(xml);
		
		xml->setAttribute("PresetFolder", path);
		
		XmlElement* a_element =new XmlElement("Preset");
		xml->addChildElement(a_element);
		a_element->setAttribute("Name", current_name);
	//	currentPreset->toXML(*a_element);
	}
	void loadState (XmlElement* parent) override
	{
		Debug::print("PresetHandler::xmlLoad");
		XmlElement* xml = parent->getChildByName(xmlIdentifier());
		if (xml != NULL)
		{
			if (xml->hasTagName (xmlIdentifier()))
			{
				path = (String)xml->getStringAttribute("PresetFolder",path);
				setDir(path);
				
				XmlElement* a_element = xml->getChildByName("Preset");
				if (a_element != NULL)
				{
					current_name = a_element->getStringAttribute("Name", "");
					Debug::print("Current name = ",current_name);
	//				currentPreset->fromXML(*a_element);
				}
			}
		}
		
	}
//	void setPresetType(std::shared_ptr<PresetT> set) {
//		presetType = set;
//	}
public:
	void setPreset(typename PresetT::Preset* set) {
		currentPreset = set;
	}
protected:
	bool loadPreset(String name, bool inComboBox) {
//		std::cout<<"loading preset"<<std::endl;
		String full_path = getFullPath(name);
		
		int return_code = 0;
		
		auto xml = FileLoad::loadToXml(full_path, return_code);
		
		if (return_code == FileLoad::LOAD_SUCCESS)
		{
			currentPreset->loadPreset(&*xml);
			currentPreset->setName(name);
			return true;
		}
		else
		{
			switch (return_code)
			{
				case FileLoad::LOAD_BADFILE:
					if (inComboBox)
						trash_preset();
					setErrText("Corrupt file.", "Deleting.");
					break;
				case FileLoad::LOAD_BADXML:
					if (inComboBox)
						trash_preset();
					setErrText("Bad XML.", "Deleting.");
					break;
				case FileLoad::LOAD_FILENOTFOUND:
					if (inComboBox)
						remove_preset_from_list();
					setErrText("File not found.", "Removing from list.");
					break;
				case FileLoad::LOAD_INCOMPLETE:
					setErrText("62. Load Incomplete." "Empty case.");
					break;
				case FileLoad::LOAD_WRONGPLUGIN:
					if (inComboBox)
						remove_preset_from_list();
					setErrText("Wrong plugin.", "Removed from display.");
					break;
				case FileLoad::LOAD_ISDIR:
					if (inComboBox)
						remove_preset_from_list();
					setErrText("Preset is a directory.");
					break;
				case FileLoad::LOAD_NOT_IMPLEMENTED:
					setErrText("72. Not implemented yet");
					break;
			}
			return false;
		}
	}
	
	bool loadDefault(int ix)
	{
		if (!defaults[ix]->success)
			return false;
		else
		{
			currentPreset->loadPreset(&*defaults[ix]->xml);
			currentPreset->setName(defaults[ix]->name);
			return true;
		}
	}
	
	void attemptSwitchSave()
	{
		if (saveOnSwitch())
		{
			savePreset(getFullPath(current_name), current_name);
		}
	}
	
	bool savePreset(String filename, String presetName) {
//		auto* xml = preset->
		XmlElement xml(xmlIdentifier());
		currentPreset->setName(presetName);
		currentPreset->saveToPreset(&xml);
		FileLoad::saveXml(filename, xml);
		return true;
	}
	
//	std::shared_ptr<PresetT> presetType;
	typename PresetT::Preset* currentPreset;
	
	String getFullPath(juce::String text)
	{
		return path + FileLoad::sep + text + FileLoad::presetSuffix;
	}
	void setErrText(String text, String debugExtra="")
	{
		Debug::print(text, debugExtra);
		setText("Err: "+text, dontSendNotification);
	}

	
	inline String xmlIdentifier() {
		return "PresetHandler";
	}
	
	String current_name;
	
	StringArray directory;
	OwnedArray<PresetDefault> defaults;
//	OwnedArray<PresetOption> defaults;
//	OwnedArray<PresetOption> directory;
	String path;
		
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetHandler);
};
