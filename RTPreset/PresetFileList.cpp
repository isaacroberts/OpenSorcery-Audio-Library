//
//  PresetFileList.cpp
//  
//
//  Created by Isaac Roberts on 10/14/22.
//

// Created by Isaac Roberts

#include "PresetFileList.h"

#include "RTGraphics/FileLoad.h"

juce::File PresetFileList::getDefaultDirectory()
{
	juce::File directory = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
	
	juce::String companyFolder = ProjectInfo::companyName;
	companyFolder = companyFolder.removeCharacters(".");
	companyFolder = juce::File::createLegalFileName(companyFolder);
	
	directory = directory.getChildFile(ProjectInfo::companyName);
	if (!directory.exists())
		directory.createDirectory();
	directory = directory.getChildFile(ProjectInfo::projectName);
	if (!directory.exists())
		directory.createDirectory();
	directory = directory.getChildFile("Presets");
		
	return directory;
}

PresetFileList::PresetFileList(PresetObject* p, bool usesAutosave, juce::File dir)
: 	livePreset(p),
presets(),
directory(dir),
useAutosave(usesAutosave),
selectedPreset(-1),
//selectedPreset(""),
fileChooser(nullptr),
listeners()
{
	if (directory.getFullPathName().isEmpty())
		directory = getDefaultDirectory();
	
	if (useAutosave)
	{
		autosave = new PresetData();
		autosave->name = "[auto]";
		autosave->type = Autosave;
		autosave->xml = std::make_unique<XmlElement>("Preset");
		presets.add(autosave);
	}
		
	if (useAutosave)
	{
		//TODO: Add self as listener to all parameters
		jassertfalse;
	}
}

//Destructor
PresetFileList::~PresetFileList()
{
	fileChooser.reset();
	presets.clear();
}

/*
 struct PresetData {
	 juce::String name;
	 XmlElement* xml=NULL;
	 juce::File file;
	 bool isLocked=false;
 };

 */

void PresetFileList::paramValueChanged()
{
	hasChangedSinceLoad=true;
}
int PresetFileList::getNumOptions()
{
	return presets.size();
}
String PresetFileList::getOption(int n)
{
	if (n < presets.size())
		return presets[n]->name;
	else return "";
}

PresetFileList::PresetData* PresetFileList::getCurrentElement()
{
	return presets[selectedPreset];
}


void PresetFileList::setDirectory(juce::File dir, bool createIfNotExists)
{
	directory = dir;
	
	clearList();
	
	if (!directory.exists())
	{
		if (createIfNotExists)
			directory.createDirectory();
	}
	else
		scan();
}
void PresetFileList::addDefaultFromBinary(const char* data, size_t size)
{
	
}
void PresetFileList::addDefault(juce::String name, juce::String str_xml)
{
//	juce::String str_xml(char_xml);
	
	PresetData* def = new PresetData();
	def->type = Default;
	def->xml = juce::XmlDocument::parse(str_xml);
	if (!def->xml) {
		jassertfalse;
		delete def;
		return;
	}
	def->name = name;
	
	presets.add(def);
}

void PresetFileList::clearList()
{
	for (int n=presets.size()-1; n >= 0; n--)
	{
		if (presets[n]->type == File)
			presets.remove(n);
	}
	
}
void PresetFileList::scan()
{
	if (!directory.exists())
	{
		directory.createDirectory();
		return;
	}

	auto childFiles = directory.findChildFiles(juce::File::findFiles, false);
	
	for (juce::File entry : childFiles)
	{
		if (entry.hasFileExtension(FileLoad::presetSuffix))
			if (! entry.isHidden())
			{
				String name = entry.getFileNameWithoutExtension();
				
				PresetData * data = new PresetData();
				data->name = name;
				data->file = entry;
				data->type = File;
				
				//TODO: Read XML to determine if it has a name element
				
				presets.add(data);
			}
	}
	
	sortPresets();
	
	updateListenerOptions();
}

void PresetFileList::sortPresets()
{
	
}
void PresetFileList::setSelection(int ix)
{
	if (ix<0 || ix >= presets.size())
		ix = -1;
	if (selectedPreset != ix)
	{
		selectedPreset = ix;
		if (selectedPreset != -1)
			load();
		updateListenerSelection();
	}
}
void PresetFileList::remove(juce::String tag)
{
	for (int n=0; n < presets.size(); ++n)
	{
		if (presets[n]->name == tag)
		{
			remove(n);
			return;
		}
	}
}

void PresetFileList::remove(juce::File f)
{
	for (int n=0; n < presets.size(); ++n)
	{
		if (presets[n]->file == f)
		{
			remove(n);
			return;
		}
	}
}

void PresetFileList::remove(int n)
{
	if (n<selectedPreset)
	{
		selectedPreset--;
	}
	
	if (presets[n]->type==File)
	{
		presets[n]->file.deleteFile();
	}
	
	presets.remove(n);
	
	if (selectedPreset)
	{
		if (presets.size()==0)
			selectedPreset=-1;
		else if (selectedPreset>=  presets.size())
			selectedPreset--;
	}
	
	updateListenerOptions();
}
void PresetFileList::save(juce::String name)
{
	PresetData* newp=NULL;
	int ix = -1;
	for (int n=0; n < presets.size(); ++n)
	{
		if (presets[n]->name == name)
		{
			newp = presets[n];
			ix = n;
			break;
		}
	}

	if (newp==NULL)
	{
		newp = new PresetData();
		newp->name = name;
	}
	newp->xml.reset(new XmlElement("Preset"));

	livePreset->save(&*newp->xml, true);
	
	newp->xml->setAttribute("preset_name", name);
	
	juce::File f = directory.getChildFile( name + "." + FileLoad::presetSuffix);
	newp->file = f;
	
	if (f.exists())
		f.deleteFile();
	
	if ( std::unique_ptr<FileOutputStream> p_os = f.createOutputStream() ) {
		newp->xml->writeTo(*p_os);
		presets.add(newp);
		
		selectedPreset = ix;
		
		if (ix < presets.size()-1)
			updateListenerSelection();
		else
			updateListenerOptions();
	}
	else
		delete newp;
	
}
void PresetFileList::save(juce::File f)
{
	PresetData* newp=NULL;
	int ix = -1;
	for (int n=0; n < presets.size(); ++n)
	{
		if (presets[n]->file == f)
		{
			newp = presets[n];
			ix = n;
			break;
		}
	}
	
	if (newp==NULL)
	{
		newp = new PresetData();
		newp->name = f.getFileNameWithoutExtension();
		newp->file = f;
		ix = presets.size();
	}
	newp->xml.reset(new XmlElement("Preset"));

	livePreset->save(&*newp->xml, true);
	
	if (f.exists())
		f.deleteFile();
	
	if ( std::unique_ptr<FileOutputStream> p_os = f.createOutputStream() ) {
		newp->xml->writeTo(*p_os);
		presets.add(newp);
		
		selectedPreset = ix;
		
		if (ix < presets.size()-1)
			updateListenerSelection();
		else
			updateListenerOptions();
	}
	else delete newp;
}
void PresetFileList::saveToBlankElement()
{
	if (!useAutosave) return;
	if (autosave == NULL) return;
	
	livePreset->save(&*autosave->xml, true);
}
void PresetFileList::load()
{	
	if (selectedPreset<0 || selectedPreset >= presets.size())
		return;

	if (hasChangedSinceLoad)
		saveToBlankElement();
	
	PresetData* data = getCurrentElement();
	
	if (!data->xml)
	{
		if (data->type==File)
		{
			data->xml = XmlDocument::parse(data->file);
			if (!data->xml)
			{
				remove(selectedPreset);
				return;
			}
		}
		else if (data->type==Default)
			jassertfalse;
		else if (data->type==Autosave)
		{
			jassertfalse;
			data->xml = std::make_unique<XmlElement>("Preset");
		}
	}
	//If load failed
	if (!data->xml)
		return;
	
	livePreset->load(&*data->xml, true);
	
	//TODO: Loading from XML will trigger updates
	
	hasChangedSinceLoad = false;
}

void PresetFileList::chooseFileToSave()
{
	fileChooser.reset(new FileChooser("Save Preset", directory, "*.preset"));
	
	int flags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;
	fileChooser->launchAsync(flags, [&](const FileChooser& c) {
		juce::File f = c.getResult();
		if (f.getFullPathName().isNotEmpty())
		{
			save(f);
		}
		fileChooser.reset();
	});
}
void PresetFileList::chooseFileToRemove()
{
	
}
void PresetFileList::chooseFileToRename()
{
	
}

void PresetFileList::updateSelection()
{
	
}


void PresetFileList::addListener(Listener* l)
{
	l->presetFileList = this;
	listeners.add(l);
	l->presetOptionsChanged();
	l->presetSelectionChanged(selectedPreset);
}
void PresetFileList::removeListener(Listener* l)
{
	l->presetFileList = NULL;
	listeners.remove(l);
}

void PresetFileList::updateListenerOptions() {
	if (juce::MessageManager::getInstance()->isThisTheMessageThread()) {
		_inthread_updateListenerOptions();
	}
	else {
		juce::MessageManager::callAsync([&](){ _inthread_updateListenerOptions(); });
	}
}
void PresetFileList::updateListenerSelection() {
	if (juce::MessageManager::getInstance()->isThisTheMessageThread()) {
		_inthread_updateListenerSelection();
	}
	else {
		juce::MessageManager::callAsync([&](){ _inthread_updateListenerSelection(); });
	}
}
void PresetFileList::updateListenerName(int n, String name) {
	if (juce::MessageManager::getInstance()->isThisTheMessageThread()) {
		_inthread_updateListenerName(n, name);
	}
	else {
		juce::MessageManager::callAsync([&](){ _inthread_updateListenerName(n, name); });
	}
}
/*
 virtual void presetNameChanged(int listIndex, String text) {}
 virtual void presetSelectionChanged(int current) {}
 virtual void presetOptionsChanged() {}
 */
void PresetFileList::_inthread_updateListenerOptions()
{
	listeners.call([&](Listener& l) { l.presetOptionsChanged(); });
}
void PresetFileList::_inthread_updateListenerSelection()
{
	listeners.call([&](Listener& l) { l.presetSelectionChanged(selectedPreset); });
}
void PresetFileList::_inthread_updateListenerName(int n, String name)
{
	listeners.call([&](Listener& l) { l.presetNameChanged(n, name); });
}
