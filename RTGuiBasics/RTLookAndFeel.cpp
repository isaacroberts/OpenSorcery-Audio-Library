



void FixedTooltipLookAndFeelMethods::drawFixedTooltip (juce::Graphics& g, const juce::String& text, int width, int height)
{
	 g.setColour(juce::Colours::white);
	 g.setFont(juce::Font(10, 0));
	 g.drawFittedText(text, 0, 0, width, height, juce::Justification::centred, 1);
}


void RTLookAndFeel::drawMeter (juce::Graphics& g, int x, int y, float percent,
				float level, float min, float max,
				int width, int height,
				const RTMeter* meter)
{
	//	g.fillAll(Colours::black);
	Colour bg= Colour(10, 10, 70);
	Colour outline= Colours::black;

	Colour col = Colours::green;

	if (height > width)
	{
		int hh = int(height * percent);
		int hh0 = height-hh;
		g.setColour(bg);
		g.fillRect(0, 0, width, hh0);

		g.setColour(col);
		g.fillRect(0, hh0,  width, hh);
	}
	else
	{
		int ww = int(width * percent);

		g.setColour(bg);
		g.fillRect(ww, 0, width-ww, height);
		g.setColour(col);
		g.fillRect(0, 0, ww, height);
	}
	g.setColour(outline);
	g.drawRect(0, 0, width, height);
}
