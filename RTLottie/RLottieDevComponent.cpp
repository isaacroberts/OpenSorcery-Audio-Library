/*  ===========================================================================
*
*   This file is part of HISE.
*   Copyright 2016 Christoph Hart
*
*   HISE is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   HISE is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with HISE.  If not, see <http://www.gnu.org/licenses/>.
*
*   Commercial licenses for using HISE in an closed source project are
*   available on request. Please visit the project's website to get more
*   information about commercial licensing:
*
*   http://www.hise.audio/
*
*   HISE is based on the JUCE library,
*   which must be separately licensed for closed source applications:
*
*   http://www.juce.com
*
*   ===========================================================================
*/


using namespace juce;


RLottieDevComponent::RLottieDevComponent() :
	animationComponent(),
	editor(doc, &tok),
	loadButton("Load"),
	compileButton("Apply changes"),
	frameSlider("Timeline"),
	autoPlayButton("Play"),
	exportButton("Compress")
{
	setLookAndFeel(&alaf);

	autoPlayButton.setClickingTogglesState(true);

	addAndMakeVisible(animationComponent);
	addAndMakeVisible(editor);
	addAndMakeVisible(loadButton);
	addAndMakeVisible(compileButton);
	addAndMakeVisible(autoPlayButton);
	addAndMakeVisible(frameSlider);
	addAndMakeVisible(exportButton);

	frameSlider.setColour(Slider::ColourIds::thumbColourId, Colour(0xFF888888));

	editor.setColour(CodeEditorComponent::ColourIds::backgroundColourId, Colour(0xFF333333));

	loadButton.onClick = [this]()
	{
		auto s = SystemClipboard::getTextFromClipboard();

		s = RLottieComponent::decompressIfBase64(s);

		auto obj = JSON::parse(s);

		doc.replaceAllContent(JSON::toString(obj));

		animationComponent.loadAnimation(s, true);
	};

	exportButton.onClick = [this]()
	{
		auto text = doc.getAllContent();

		auto obj = JSON::parse(text);
		auto data = JSON::toString(obj, true);

		DBG("Before: " + String(data.length()));

		MemoryBlock mb;

		zstd::ZDefaultCompressor comp;
		
		comp.compress(data, mb);

		auto compressed = mb.toBase64Encoding();

		DBG("After: " + String(compressed.length()));

		doc.replaceAllContent(compressed);
	};

	autoPlayButton.onClick = [this]()
	{
		auto shouldPlay = autoPlayButton.getToggleState();

		frameSlider.setEnabled(!shouldPlay);

		if (shouldPlay)
		{
			startTimer(50);
			animationComponent.play();
			autoPlayButton.setButtonText("Stop");
		}

		else
		{
			stopTimer();
			animationComponent.stop();
			autoPlayButton.setButtonText("Play");
		}

	};
	frameSlider.setSliderStyle(Slider::LinearBar);

	frameSlider.onValueChange = [this]()
	{
		autoPlayButton.setToggleState(false, sendNotificationSync);
		animationComponent.setFrameNormalised(frameSlider.getValue());
	};

	frameSlider.setRange(0.0, 1.0, 0.001);
	frameSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);

	setSize(800, 600);
}

RLottieDevComponent::~RLottieDevComponent()
{
	
}

void RLottieDevComponent::timerCallback()
{
	if (autoPlayButton.getToggleState())
	{
		frameSlider.setValue(animationComponent.getCurrentFrameNormalised(), dontSendNotification);
	}
}

//==============================================================================
void RLottieDevComponent::paint(Graphics& g)
{
	g.fillAll(Colour(0xFF555555));
}

void RLottieDevComponent::resized()
{
	auto b = getLocalBounds();

	editor.setBounds(b.removeFromBottom(getHeight() / 3));

	auto row = b.removeFromBottom(28);

	loadButton.setBounds(row.removeFromLeft(100));
	b.removeFromLeft(5);
	compileButton.setBounds(row.removeFromLeft(100));
	exportButton.setBounds(row.removeFromRight(80));
	autoPlayButton.setBounds(row.removeFromRight(80));
	
	frameSlider.setBounds(row.reduced(5, 2));

	animationComponent.setBounds(b);
}



