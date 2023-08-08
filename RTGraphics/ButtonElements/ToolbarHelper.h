//
//  ToolbarHelper.h
//  Lo-Fi Sampler
//
//  Created by Isaac Roberts on 7/14/20.
//  Copyright © 2020 New Nation LLC. All rights reserved.
//

#ifndef ToolbarHelper_h
#define ToolbarHelper_h

#include "HyperLinkToolbarButton.h"

#include <JuceHeader.h>

namespace ToolbarHelper
{
	juce::ToolbarButton* createButton(int id, bool isHyperlink, const char* imgData, const int size, const char* downData, const int downSize)
	{
		assert (imgData != NULL);
		
		Image i = juce::ImageFileFormat::loadFrom(imgData, (size_t) size);
		juce::DrawableImage *d = new juce::DrawableImage();
		d->setImage(i);
		
		
		//	std::cout<<filename<<std::endl;
		juce::DrawableImage *d2=NULL;
		if (downData != NULL)
		{
			i = juce::ImageFileFormat::loadFrom(downData, (size_t)downSize);
			d2 = new juce::DrawableImage();
			d2->setImage(i);
		}
		if (isHyperlink)
		{
			return new HyperLinkToolbarButton(id, "", std::unique_ptr<Drawable>(d), std::unique_ptr<Drawable>(d2));
		}
		else
		{
			return new juce::ToolbarButton(id, "", std::unique_ptr<Drawable>(d), std::unique_ptr<Drawable>(d2));
		}
	}
	juce::ToolbarButton* imagelessButton(int id, bool isHyperlink)
	{
		Image i;
		juce::DrawableImage *d = new juce::DrawableImage();
		d->setImage(i);

		if (isHyperlink)
		{
			return new HyperLinkToolbarButton(id, "", std::unique_ptr<Drawable>(d), std::unique_ptr<Drawable>((Drawable*)NULL));
		}
		else
		{
			return new juce::ToolbarButton(id, "", std::unique_ptr<Drawable>(d), std::unique_ptr<Drawable>((Drawable*)NULL));
		}

	}
}



#endif /* ToolbarHelper_h */
