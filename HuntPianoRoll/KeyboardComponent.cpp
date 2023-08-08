//
//  KeyboardComponent.cpp
//  PianoRollEditor - App
//
//  Created by Samuel Hunt on 04/02/2020.
//

//#include "KeyboardComponent.hpp"


KeyboardComponent::KeyboardComponent (NoteGridStyleSheet::Ptr s)
: style(s)
{
}
void KeyboardComponent::paint (juce::Graphics & g)
{
	if (style->drawKeyboardAsPiano())
		drawAsPiano(g);
	else
		drawAsFlat(g);
}
void KeyboardComponent::drawAsPiano(juce::Graphics& g)
{
	const float btwLengthRatio=.55;
	const float btwHeightRatio = .75f;
	
//	const float noteCompHeight = getHeight() / 128.0;
	float whiteHeight = getHeight() / 75.f;
	const float bW = getWidth() * btwLengthRatio;
	const float bH = whiteHeight * btwHeightRatio;
	const float bYM = whiteHeight * (1-btwHeightRatio)/2.f;
		
	bool drawFonts = whiteHeight > 12;
	
	g.fillAll(style->getSidebarWhiteColor());
		
	//7 * 10 + 5
	for (int ii=74; ii >=0; --ii)
	{
		int n = Scale::intervalToNote(ii);

		float line = (74-ii) * whiteHeight;
		
		g.setColour(juce::Colours::black);
		g.drawHorizontalLine(line, 0, getWidth());
						
		String text = style->getRowLabel(n);
		if (text.isNotEmpty())
		{
			if (drawFonts)
			{
				g.setColour(style->getSidebarWhiteFontColor());
				g.drawText(text, bW+2, line, getWidth()-bW-4, whiteHeight, style->getRowLabelJustification());
			}
		}
	
	}
	
	static const float blackNotePos[] ={ 1 - btwHeightRatio * 0.6f, //C#
										 2 - btwHeightRatio * 0.4f, //D#
										 4 - btwHeightRatio * 0.7f, //F#
										 5 - btwHeightRatio * 0.5f, //G#
										 6 - btwHeightRatio * 0.3f};//A#
	//5 * 10 + 3 = 53
	for (int i = 52; i>=0; --i) {
		
		int n = Scale::darkIntervalToNote(i);
		jassert(n < 128);
		
		int iv = i % 5;
		int o = i/5;
		float line = getHeight() - whiteHeight * (7*o + blackNotePos[iv]-.25+1+1/7.);
		
		g.setColour(style->getSidebarBlackColor());
		g.fillRect(0.f, line+bYM, bW, bH);
		
		if (drawFonts)
		{
			String text = style->getRowLabel(n);
			if (text.isNotEmpty())
			{
				g.setColour(style->getSidebarBlackFontColor());
				g.drawText(text, 2, line+bYM, bW-4, bH, style->getRowLabelJustification());
			}
		}
	}

}
//Drawn with equal size black & white keys
 void KeyboardComponent::drawAsFlat (juce::Graphics & g)
 {

	 const float noteCompHeight = getHeight() / 128.0;
	 bool drawFonts = noteCompHeight > 12;
	 
	 float line = 0;//noteCompHeight;
	 
	 for (int i = 127; i >= 0; i--) {
		 bool white = style->drawAsWhite(i);
		 g.setColour(white ? style->getSidebarWhiteColor() : style->getSidebarBlackColor());
		 g.fillRect(0, (int)line, getWidth(), (int)noteCompHeight);// explicit cast to int otherwise matching function error
		 
		 if (drawFonts)
		 {
			 String text = style->getRowLabel(i);
			 if (text.isNotEmpty())
			 {
				 g.setColour(white ? style->getSidebarWhiteFontColor() : style->getSidebarBlackFontColor());
				 g.drawText(text, 5, line, getWidth()-10, noteCompHeight, style->getRowLabelJustification());
			 }
		 }

		 line += noteCompHeight;
		 
		 g.setColour(juce::Colours::black);
		 g.drawLine(0, line, getWidth(), line);
	 }
 
 }

 
