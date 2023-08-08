//
//  XmlExportButton.h
//  
//
//  Created by Isaac Roberts on 7/4/22.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

#include "../MidiUtil.h"

using namespace juce;

template <class ButtonType>
class XmlExportButton : public ButtonType, public DragAndDropContainer
{
	static_assert(std::is_base_of<juce::Button, ButtonType>::value, "Button Super not derived from Button");

public:
	XmlExportButton()
	: ButtonType(), DragAndDropContainer(),
	saveXml([&](XmlElement*) {}),
	getFilename([&](){ return "data.xml"; }),
	chooser(nullptr),
	defaultFile()
	{
	}
public:
	//Yeah looks backwards but it works
	bool wasDropped=false;
	std::function< void(XmlElement*) > saveXml;
	std::function< String(void)> getFilename;
	std::unique_ptr<FileChooser> chooser;
	File defaultFile;
	
	void mouseDrag(const MouseEvent& mo) override
	{
		if (!isDragAndDropActive())
		{
			wasDropped=false;
			getXmlFile();
		}
	}
	void mouseDown(const MouseEvent& mo) override
	{
		wasDropped=false;
	}
	void mouseUp (const MouseEvent& mo) override
	{
		if (!wasDropped)
		{
			searchForFile();
		}
	}
private:
	
	void getXmlFile()
	{
		String filename = getFilename();

		XmlElement* xml = new XmlElement("Data");
		saveXml(xml);
		
		File outf = File::getSpecialLocation( File::SpecialLocationType::tempDirectory ).getChildFile( filename );
		
		if (outf.exists())
			outf.deleteFile();
				
		if ( std::unique_ptr<FileOutputStream> p_os = outf.createOutputStream() ) {
			xml->writeTo(*p_os);
			
			wasDropped = true;
			DBG( "drag clip file name: " << outf.getFullPathName() );
			
			performExternalDragDropOfFiles( { outf.getFullPathName() }, false, this, [=]() {
				defaultFile=outf;
				DBG( "clip dropped" );
			} );
		}
		delete xml;
	}
	
	//Open FileChooser because user is not dragging
	void searchForFile()
	{
		if (defaultFile.getFullPathName() == "") {
			defaultFile = File::getSpecialLocation(File::userDesktopDirectory);
		}
		if (defaultFile.isDirectory()) {
			defaultFile = defaultFile.getChildFile(getFilename());
		}
		chooser = std::make_unique<FileChooser>(ButtonType::getName(), defaultFile, "*.xml", true);
		int flags = FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::canSelectDirectories;
		chooser->launchAsync(flags, [&](const FileChooser& c){ fileSelected(c); });
	}
	void fileSelected(const FileChooser& c)
	{
		File outf = c.getResult();
#if DEBUG
		std::cout<<"result = "<<outf.getFullPathName();
#endif

		if (outf.isDirectory()) {
			outf = outf.getChildFile(getFilename());
		}
		if (outf.getFileExtension() != "xml") {
			outf = outf.withFileExtension("xml");
		}

		XmlElement* xml = new XmlElement("Data");
		saveXml(xml);
		
		//Not sure if this needs to be done
		if (outf.exists())
			outf.deleteFile();
		
		if ( std::unique_ptr<FileOutputStream> p_os = outf.createOutputStream() ) {
			xml->writeTo(*p_os);
			defaultFile = outf;
		}
		delete xml;
	}

	
public:
	void dragOperationStarted(const DragAndDropTarget::SourceDetails& so) override
	{
		
	}
	void dragOperationEnded(const DragAndDropTarget::SourceDetails& so) override
	{
//		int x = so.localPosition.getX();
//		int y = so.localPosition.getY();
		
//		std::cout<<"x, y = "<<x<<", "<<y<< "/" << this->getWidth() <<", "<< this->getHeight()<<std::endl;
		
	}
	
	bool shouldDropFilesWhenDraggedExternally (const DragAndDropTarget::SourceDetails &sourceDetails, StringArray &files, bool &canMoveFiles) override
	{
		return true;
	}
	bool shouldDropTextWhenDraggedExternally (const DragAndDropTarget::SourceDetails &sourceDetails, String &text) override
	{
		return false;
	}


};
