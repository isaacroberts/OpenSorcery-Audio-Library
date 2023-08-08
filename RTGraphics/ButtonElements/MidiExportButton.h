//
//  MidiExportButton.h
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
class MidiExportButton : public ButtonType, public DragAndDropContainer
{
public:
	MidiExportButton()
	: TextButton(), DragAndDropContainer(),
	getMidi([&]() { return std::pair<MidiMessageSequence*, String>(NULL, ""); })
	{
	}
public:
	std::function< std::pair<MidiMessageSequence*, String>()> getMidi;
	
	void mouseDrag(const MouseEvent& mo) override
	{
		if (!isDragAndDropActive())
		{
			//		startDragging("Midi File", this, *WizardLAF::exportDragImage, true, nullptr, &mo.source);
			getMidiFile();
		}
		//	beginDrag();
	}
	
private:
	void getMidiFile()
	{
		const int tps = 96;
		
		MidiFile mf;
		std::pair<MidiMessageSequence*, String> msf = getMidi();
		auto* ms = msf.first;
		
		if (ms)
		{
			mf.setTicksPerQuarterNote( tps);
			mf.addTrack( *ms );
			delete ms;
			
			//Use types
			String filename = msf.second + MidiUtil::getMidiExtension();
			File outf = File::getSpecialLocation( File::SpecialLocationType::tempDirectory ).getChildFile( filename );
			if (outf.exists())
				outf.deleteFile();
			
			if ( std::unique_ptr<FileOutputStream> p_os = outf.createOutputStream() ) {
				mf.writeTo( *p_os, 0 );
				
				DBG( "drag clip file name: " << outf.getFullPathName() );
				
				performExternalDragDropOfFiles( { outf.getFullPathName() }, false, this, [=]() {
					DBG( "clip dropped" );
					//			outf.deleteFile();
				} );
			}
		}
	}
public:
	void dragOperationStarted(const DragAndDropTarget::SourceDetails&) override
	{
		
	}
	void dragOperationEnded(const DragAndDropTarget::SourceDetails&) override
	{
		getMidiFile();
	}
	
	bool shouldDropFilesWhenDraggedExternally (const DragAndDropTarget::SourceDetails &sourceDetails, StringArray &files, bool &canMoveFiles) override
	{
		return true;
		//	for (int n=0; n < files.size(); ++n)
		//	{
		//		if (files[n].endsWith(".mid") || files[n].endsWith(".midi"))
		//			return true;
		//	}
		//	return false;
	}
	bool shouldDropTextWhenDraggedExternally (const DragAndDropTarget::SourceDetails &sourceDetails, String &text) override
	{
		return false;
	}


};
