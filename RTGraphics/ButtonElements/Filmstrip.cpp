//
//  Filmstrip.cpp
//  
//
//  Created by Isaac Roberts on 8/9/20.
//

// Created by Isaac Roberts


#include "Filmstrip.h"

#include "UniqueHashCoder.h"

Filmstrip::Filmstrip(int64 imgHash, int numSprites)
	: num_sprites_on_filmstrip(numSprites),
	hashCode(imgHash)
{
	xOffset=0;
	yOffset=0;
}
Filmstrip::Filmstrip(const char* imgData, int imgSize, int numSprites)
: num_sprites_on_filmstrip(numSprites)
{
	hashCode = UniqueHashCoder::getUniqueHashCode();
	
		img = ImageFileFormat::loadFrom(imgData, (size_t)imgSize);
	ImageCache::addImageToCache(img, hashCode);
	
	xOffset=0;
	yOffset=0;
}


Filmstrip::~Filmstrip()
{
	
}

void Filmstrip::drawFrame(Graphics& g, int frameNo, int x, int y)
{
	if (imagesVertical())
	{
//		float h = float(img.getHeight())/float(num_sprites_on_filmstrip);
		// dim = height
		int takeY = int(frameNo * dim);
		g.drawImage(img,
					x, y, img.getWidth(), (int)dim,
					0, takeY, img.getWidth(), (int)dim);
	}
	else
	{
//		float w = float(img.getWidth())/float(num_sprites_on_filmstrip);
		//dim = width
		int takeX = int(frameNo * dim);
		
		g.drawImage(img,
					x, y, (int)dim, img.getHeight(),
					takeX, 0, (int)dim, img.getHeight());
	}
}
void Filmstrip::resize()
{
	img = ImageCache::getFromHashCode(hashCode);
	if (imagesVertical())
	{
		dim = float(img.getHeight()) / float(num_sprites_on_filmstrip);
	}
	else
	{
		dim = float(img.getWidth()) / float(num_sprites_on_filmstrip);
	}

}
void Filmstrip::resize(float scale)
{
	if ((scale-1) <= .001)
	{
		resize();
	}
	else
	{
		//Re-get from cache to prevent low resolution from multiple rescales
		img = ImageCache::getFromHashCode(hashCode);
		//Rescale to size * scale
		if (imagesVertical())
		{
			dim = float(img.getHeight()) / float(num_sprites_on_filmstrip);
			
			int height = int(scale * dim);
			img = img.rescaled(img.getWidth() * scale, height * num_sprites_on_filmstrip);
		}
		else
		{
			dim = float(img.getWidth()) / float(num_sprites_on_filmstrip);
			
			int width = int(scale * dim);
			
			img = img.rescaled(width * num_sprites_on_filmstrip, img.getHeight() * scale);

		}
	}
}
