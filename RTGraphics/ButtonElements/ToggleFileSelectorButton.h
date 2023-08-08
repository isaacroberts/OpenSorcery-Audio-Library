//
//  ToggleFileSelectorButton.h
//  
//
//  Created by Isaac Roberts on 6/8/22.
//

#ifndef ToggleFileSelectorButton_h
#define ToggleFileSelectorButton_h

#include <JuceHeader.h>

using namespace juce;


//Must be a subclass of some button
	//Using a template allows you to use different types of button

//Component Name = File Selector Title

template <class ButtonSuper>
class ToggleFileSelector : public ButtonSuper, public FileDragAndDropTarget
{
	static_assert(std::is_base_of<juce::Button, ButtonSuper>::value, "ButtonSuper not derived from Button");
public:
	//Usage
	ToggleFileSelector(const char* pattern, File f=File())
		: ButtonSuper(), FileDragAndDropTarget(),
		pattern(pattern),
		file(f),
		chooser(nullptr)
	{
		//Unless you're doing something complex you're going to need '*' for wildcards
		jassert(pattern[0]=='*');
		ButtonSuper::setClickingTogglesState(true);
	}
	ToggleFileSelector(File f=File())
		: ButtonSuper(), FileDragAndDropTarget(),
		pattern("*"),
		file(f),
		chooser(nullptr)
	{
		ButtonSuper::setClickingTogglesState(true);
	}
	
	std::function<void(File)> 	onFileSelected;
	std::function<void()> 		onFileCleared;
	
	
	void setPattern(const char* p) { pattern = p; }

	File getFile() {
		if (ButtonSuper::getToggleState())
			return file;
		else
			return File();
	}
	void clearFile(NotificationType notif) {
		ButtonSuper::setToggleState(false, sendNotificationSync);
		ButtonSuper::setTooltip("");
		if (notif==sendNotificationSync)
			onFileCleared();
		else if (notif==sendNotificationAsync)
			MessageManager::callAsync([&]() { onFileCleared(); });
	}
	void setFile(File set, NotificationType notif) {
		ButtonSuper::setToggleState(true, sendNotificationSync);
		file = set;
		ButtonSuper::setTooltip(set.getFileName());
		
		if (notif==sendNotificationSync)
			onFileSelected(file);
		else if (notif==sendNotificationAsync)
			MessageManager::callAsync([&]() { onFileSelected(file); });
	}
	void syncToBackend(String set) {
		if (set.isNotEmpty())
		{
			File f(set);
			setFile(set, dontSendNotification);
		}
		else
			clearFile(dontSendNotification);
	}
	
	
	
	//Internal
	void internalClickCallback (const ModifierKeys& mods) override
	{
		if (mods.isAltDown())
			clearFile(sendNotificationAsync);
		else if (mods.isShiftDown())
		{
			searchForFile();
		}
		else
		{
			if (ButtonSuper::getToggleState())
			{
				clearFile(sendNotificationAsync);
			}
			else
			{
				searchForFile();
			}
		}
	}

	bool isInterestedInFileDrag (const StringArray &files) override
	{
		String p(pattern);
		for (int n=0; n < files.size(); ++n)
		{
//			if (files[n].matchesWildcard(pattern, true))
			if (matchesPattern(files[n], p))
				return true;
		}
		return false;
	}
	void filesDropped (const StringArray &files, int x, int y) override
	{
		String p(pattern);
		for (int n=0; n < files.size(); ++n)
		{
//			if (files[n].matchesWildcard(pattern, true))
			if (matchesPattern(files[n], p))
			{
				setFile(File(files[n]), sendNotificationAsync);
				return;
			}
		}
	}
	
protected:
	void searchForFile()
	{
		File dflt = file;
		if (dflt.getFullPathName()=="")
			dflt = File::getSpecialLocation(File::userDesktopDirectory);
		chooser = std::make_unique<FileChooser>(ButtonSuper::getName(), dflt, pattern);
		int flags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
		chooser->launchAsync(flags, [&](const FileChooser& c){ fileSelected(c); });
	}
	void fileSelected(const FileChooser& c)
	{
		File result = c.getResult();
#if DEBUG
		std::cout<<"result = "<<result.getFullPathName();
#endif 
		if (result.existsAsFile())
		{
			setFile(result, sendNotificationSync);
		}
		else
		{
			
		}
	}
	bool matchesPattern(String s, String p)
	{
		int c = 0;
		while (c < p.length())
		{
			int aster = p.indexOfChar(c, '*');
			if (aster==-1)
				return false;
			int semi = p.indexOfChar(aster, ';');
			if (semi==-1)
				semi=p.length();
			
			String subs = p.substring(aster+1, semi);
			if (s.endsWithIgnoreCase(subs))
				return true;
			c = semi+1;
		}
		return false;
	}
	
protected:
	const char* pattern;
	File file;
	std::unique_ptr<FileChooser> chooser;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToggleFileSelector);
};







template <class ButtonSuper>
class FileSelector : public ButtonSuper, public FileDragAndDropTarget
{
	static_assert(std::is_base_of<juce::Button, ButtonSuper>::value, "ButtonSuper not derived from Button");
public:
	//Usage
	FileSelector(const char* pattern, File f=File())
		: ButtonSuper(), FileDragAndDropTarget(),
		pattern(pattern),
		file(f),
		chooser(nullptr)
	{
		//Unless you're doing something complex you're going to need '*' for wildcards
		jassert(pattern[0]=='*');
		ButtonSuper::setClickingTogglesState(false);
	}
	FileSelector(File f=File())
		: ButtonSuper(), FileDragAndDropTarget(),
		pattern("*"),
		file(f),
		chooser(nullptr)
	{
		ButtonSuper::setClickingTogglesState(false);
	}
	
	std::function<void(File)> 	onFileSelected;
	
	void setPattern(const char* p) { pattern = p; }

	
	void setFile(File set, NotificationType notif) {
		file = set;
		
		if (notif==sendNotificationSync)
			onFileSelected(file);
		else if (notif==sendNotificationAsync)
			MessageManager::callAsync([&]() { onFileSelected(file); });
	}
	
	
	//Internal
	void internalClickCallback (const ModifierKeys& mods) override
	{
		searchForFile();
	}

//	bool isFromOtherExport(File f)
//	{
//		int64 diff = Time::currentTimeMillis() - f.getCreationTime().toMilliseconds();
//		return (diff < 5000);
//	}
	
	bool isInterestedInFileDrag (const StringArray &files) override
	{
		String p(pattern);
		for (int n=0; n < files.size(); ++n)
		{
			if (matchesPattern(files[n], p))
			{
//				if (!isFromOtherExport(files[n]))
					return true;
//				else
//					std::cout<<"rejected exported item"<<std::endl;
			}
		}
		return false;
	}
	void filesDropped (const StringArray &files, int x, int y) override
	{
		String p(pattern);
		for (int n=0; n < files.size(); ++n)
		{
			if (matchesPattern(files[n], p))
			{
//				if (!isFromOtherExport(files[n]))
				{
					setFile(File(files[n]), sendNotificationAsync);
					return;
				}
//				else
//					std::cout<<"rejected exported item"<<std::endl;
			}
		}
	}
	
protected:
	void searchForFile()
	{
		File dflt = file;
		if (dflt.getFullPathName()=="")
			dflt = File::getSpecialLocation(File::userDesktopDirectory);
		chooser = std::make_unique<FileChooser>(ButtonSuper::getName(), dflt, pattern);
		int flags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
		chooser->launchAsync(flags, [&](const FileChooser& c){ fileSelected(c); });
	}
	void fileSelected(const FileChooser& c)
	{
		File result = c.getResult();
#if DEBUG
		std::cout<<"result = "<<result.getFullPathName();
#endif
		if (result.existsAsFile())
		{
			setFile(result, sendNotificationSync);
		}
		else
		{
			
		}
	}
	bool matchesPattern(String s, String p)
	{
		int c = 0;
		while (c < p.length())
		{
			int aster = p.indexOfChar(c, '*');
			if (aster==-1)
				return false;
			int semi = p.indexOfChar(aster, ';');
			if (semi==-1)
				semi=p.length();
			
			String subs = p.substring(aster+1, semi);
			if (s.endsWithIgnoreCase(subs))
				return true;
			c = semi+1;
		}
		return false;
	}
	
protected:
	const char* pattern;
	File file;
	std::unique_ptr<FileChooser> chooser;
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileSelector);
};



#endif /* ToggleFileSelectorButton_h */
