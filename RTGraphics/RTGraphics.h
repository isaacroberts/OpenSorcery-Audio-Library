/** BEGIN_JUCE_MODULE_DECLARATION

    ID:            RTGraphics
    vendor:       Roberts.Tech
    version:        0.7.0
    name:          RT Graphics
    description:
    website:       isaacroberts.tech
    license:        Proprietary

    dependencies: 

    END_JUCE_MODULE_DECLARATION
*/


#ifndef RT_GRAPHICS_MODULE
#define RT_GRAPHICS_MODULE
#endif

//#include <juce_audio_formats/juce_audio_formats.h>


/*

class DebugWindow;
class FileLoad;
class FixedTooltip;
template<class PresetT> class PresetHandler;
class RTMeter;
class TwoValueSlider;


#include "DebugWindow.h"
#include "FileLoad.h"
#include "FixedTooltip.h"
#include "PresetHandler.h"
#include "RTMeter.h"
*/


/*
 
 
 JUCE UPDATE:
 
 
 Slider Popup Name Capabilities:
	 Go to juce::Slider::PopupDisplayComponent
		juce_gui_basics > widgets > juce_Slider.cpp
		line ~1294
	 
	Replace the functions with:
 
		 bool twoLine() {
			 return ! owner.getName().isEmpty();
		 }
		 void paintContent (Graphics& g, int w, int h) override
		 {
			 g.setFont (font);
			 g.setColour (owner.findColour (TooltipWindow::textColourId, true));
			 String drText = twoLine() ? owner.getName()+"\n"+text : text;
			 g.drawFittedText (drText, Rectangle<int> (w, h), Justification::centred, 2);
		 }
		 void getContentSize (int& w, int& h) override
		 {
			 w = std::max(font.getStringWidth (text), font.getStringWidth(owner.getName())) + 18;
			 h = (int) (font.getHeight() * (twoLine() ? 2.6f : 1.6f) );
		 }
 
 
 
 Color utility functions:
	Goto juce::Colour
	juce_graphics > colour > Colour.cpp
	line ~237
 
		 Colour Colour::hex(int col)
		 {
			 return Colour(static_cast<uint8> ((col >> 16) & 0xff),
						   static_cast<uint8> ((col >> 8) & 0xff),
						   static_cast<uint8> (col & 0xff));
		 }
		 Colour Colour::hex(int col, float a)
		 {
			 return Colour(static_cast<uint8> ((col >> 16) & 0xff),
						   static_cast<uint8> ((col >> 8) & 0xff),
						   static_cast<uint8> (col & 0xff),
						   static_cast<uint8> (jlimit(0.f, 1.f, a)*255));
		 }
		 Colour Colour::grey(float gg)
		 {
			 return Colour(gg, gg, gg);
		 }
		 Colour Colour::grey(float f, float a)
		 {
			 auto gg = ColourHelpers::floatToUInt8(f);
			 auto ag = ColourHelpers::floatToUInt8(a);
			 return Colour(gg, gg, gg, ag);
		 }
		Colour Colour::greyx(uint8 f)
		{
			auto gg = ColourHelpers::floatToUInt8(f);
			return Colour(gg, gg, gg);
		}
		Colour Colour::greyx(uint8 gg, float a)
		{
			auto ag = ColourHelpers::floatToUInt8(a);
			return Colour(gg, gg, gg, ag);
		}



 
	and Colour.h:
	juce_graphics > colour > Colour.h
	line ~64
		//Creates from hex integer. Color(0xffaa5b);
		 static Colour hex(int rgbHex);
		 static Colour hex(int rgbHex, float a);

		 //Creates gray color from single float 0-1
		 static Colour grey(float f);
		 //Creates gray color with alpha
		 static Colour grey(float f, float a);

		//Creates gray color from single int 0-255
		static Colour greyx(uint8 g);
		//Creates gray color with alpha
		static Colour greyx(uint8 g, float a);


 

 
	Component positioning capabilities:


	Component.h
	juce_gui_basics > components > Component.h

		 private:
		//Allows developer to size components directly from their positions in a PSD.
			 static float docW, docH;
		 public:
			static float getDocW() { return docW; }
			static float getDocH() { return docH; }
			//Called first to set the size of the image in PSD. Ex: 3650 x 2420
			 static void setDocSize(float w, float h);
			 //Called on main component to set each item's position in the PSD
			 void setBoundsInDoc(float x, float y, float w, float h);
			 //Called to set subcomponents. Must include a Rectangle with the bounds of the current element.
			 //Ex: SpeakerFace::resized() {
			 //		Rectangle<float> speakerFaceBounds(900, 50, 1200, 800);
			 //		speakerKnob.setBoundsInDoc(950, 75, 100, 100, speakerFaceBounds);
			 void setBoundsInDoc(float x, float y, float w, float h, Rectangle<float> parentDoc);


	Component.cpp
	juce_gui_basics > components > Component.cpp
	line ~1229
 
	 float Component::docW = 500;
	 float Component::docH = 500;
		 
	 void Component::setDocSize(float w, float h) {
		 docW = w;
		 docH = h;
	 }
	 void Component::setBoundsInDoc(float x, float y, float w, float h)
	 {
		 float rx = x/docW;
		 float ry = y/docH;
		 float rw = w/docW;
		 float rh = h/docH;
		 
		 setBoundsRelative(rx, ry, rw, rh);
	 }
	 void Component::setBoundsInDoc(float x, float y, float w, float h, Rectangle<float> pd)
	 {
		 float rx = (x - pd.getX())/pd.getWidth ();
		 float ry = (y - pd.getY())/pd.getHeight();
		 float rw = w/pd.getWidth ();
		 float rh = h/pd.getHeight();
		 
		 setBoundsRelative(rx, ry, rw, rh);
	 }

 
 */
