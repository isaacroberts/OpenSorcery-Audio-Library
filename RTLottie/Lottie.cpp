//
//  Lottie.cpp
//  
//
//  Created by Isaac Roberts on 6/6/22.
//

// Created by Isaac Roberts


//#include "Lottie.h"



Lottie::Lottie(juce::String jsonData)
: anim(NULL),
origW(0), origH(0), scale(1.0),
frames(0),
lastFrame(-1), curFrame(0),
rate(1.0)
{
	auto hash = String(jsonData.hashCode());
		
	anim = lottie_animation_from_data(jsonData.getCharPointer(), hash.getCharPointer(), "/");
	
	if (anim)
	{
		frames = (int)lottie_animation_get_totalframe(anim);
		rate = lottie_animation_get_framerate(anim);
	}
	else jassertfalse;
}

Lottie::~Lottie()
{
	if (anim)
		lottie_animation_destroy(anim);
}

void Lottie::render(Graphics& g, int x, int y)
{
	if (!anim) return;
	
	Image::BitmapData bd(canvas, Image::BitmapData::ReadWriteMode::writeOnly);

	lottie_animation_render(anim, (size_t)curFrame, reinterpret_cast<uint32*>(bd.data), canvas.getWidth(), canvas.getHeight(), canvas.getWidth() * 4);

	lastFrame = curFrame;

	if (scale == 1.0f)
	{
		g.drawImageAt(canvas, x, y);
	}
	else
	{
		//I mean this should incorporate x and y
		g.drawImageTransformed(canvas, AffineTransform::scale(1.0f / scale));
	}

}

bool Lottie::isValid() const
{
	return anim != nullptr && canvas.isValid();
}
void Lottie::advanceFrame()
{
	if (frames > 0)
		curFrame = (curFrame+1) % frames;
}
bool Lottie::advanceWithoutLooping()
{
	curFrame += 1;
	if (curFrame >= frames)
	{
		curFrame=0;
		return true;
	}
	return false;
}
void Lottie::setFrame(int n)
{
	curFrame = jlimit(0, frames, n);
}
void Lottie::setFrameNormalized(float f)
{
	f = jlimit(0.f, 1.0f, f);
	curFrame = roundToInt(f * frames);
}
float Lottie::getCurrentFrameNormalized() const
{
	if (frames > 0.0)
		return (float)(curFrame) / frames;
	else return 0.0f;

}

int Lottie::getCurrentFrame() const
{
	return curFrame;
}

void Lottie::setScaleFactor(float newScaleFactor)
{
	if (std::fabs(scale-newScaleFactor) > .00001f)
	{
		scale = newScaleFactor;

		setSize(origW, origH);
	}
}

void Lottie::setSize(int width, int height)
{
	origW = width;
	origH = height;

	auto newWidth = roundToInt((float)origW * scale);
	auto newHeight = roundToInt((float)origH * scale);

	if (newWidth != canvas.getWidth() || newHeight != canvas.getHeight())
	{
		canvas = Image(Image::ARGB, newWidth, newHeight, true);
	}
}
