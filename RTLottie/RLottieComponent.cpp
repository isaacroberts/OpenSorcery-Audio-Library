/** Copyright 2019 Christoph Hart

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	Note: Be aware that the rLottie wrapper files are licensed under a more permissive license than the 
	rest of the HISE codebase. The MIT license only applies where stated in the header.
*/


using namespace juce;

RLottieComponent::RLottieComponent()
: currentAnimation(nullptr)
{
	setOpaque(false);
	bgColor = Colours::transparentBlack;
}

void RLottieComponent::showAndPlay()
{
	setVisible(true);
	play();
}
void RLottieComponent::hideAndStop()
{
	setVisible(false);
	stop();
}
void RLottieComponent::setShownAndPlaying(bool set)
{
	setVisible(set);
	if (set)
		play();
	else stop();
}
void RLottieComponent::setFrameNormalized(float f)
{
	if (currentAnimation != nullptr)
	{
		currentAnimation->setFrameNormalized(f);
		repaint();
	}
}
float RLottieComponent::getCurrentFrameNormalized() const
{
	if (currentAnimation != nullptr)
	{
		return currentAnimation->getCurrentFrameNormalized();
	}
	return 0.f;
}

void RLottieComponent::stop()
{
	stopTimer();
}


void RLottieComponent::play()
{
	if (currentAnimation != nullptr)
	{
		auto frameRate = currentAnimation->getFrameRate();

		if (frameRate > 0.0)
			startTimer(1000.0 / frameRate);
	}
}

void RLottieComponent::loadLottie(const String& jsonCode, bool useOversampling)
{
	currentAnimation = std::make_unique<Lottie>(jsonCode);

	if (useOversampling)
		currentAnimation->setScaleFactor(2.0f);

	resized();
	repaint();
}

void RLottieComponent::resized()
{
	if (currentAnimation != nullptr)
	{
		currentAnimation->setSize(getWidth(), getHeight());
	}
}

void RLottieComponent::paint(Graphics& g)
{
	g.fillAll(bgColor);

	if (currentAnimation != nullptr)
	{
//		currentAnimation->setFrame(currentFrame);
		currentAnimation->render(g, 0, 0);
	}
}

void RLottieComponent::setBackgroundColour(Colour c)
{
	bgColor = c;

	setOpaque(c.getAlpha() == 0xff);
}
void RLottieComponent::setTransparentBackground()
{
	bgColor = Colours::transparentWhite;
	setOpaque(false);
}

void RLottieComponent::timerCallback()
{
	if (currentAnimation != nullptr && currentAnimation->getNumFrames() > 0)
	{
		currentAnimation->advanceFrame();
		repaint();
	}
}

//juce::String RLottieComponent::decompressIfBase64(const String& s)
//{
//#if HI_ZSTD_INCLUDED
//	if (!s.startsWithChar('{'))
//	{
//		MemoryBlock mb;
//
//		if (mb.fromBase64Encoding(s))
//		{
//			String t;
//			zstd::ZDefaultCompressor comp;
//			comp.expand(mb, t);
//			return t;
//		}
//		else
//		{
//			// Hmm, it's not a "valid" JSON but no base64 encoded thingie either...
//			jassertfalse;
//		}
//	}
//
//	return s;
//#else
//	return s;
//#endif
//}


