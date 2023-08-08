//
//  BasicMeterLAF.h
//  
//
//  Created by Isaac Roberts on 11/21/22.
//

// Created by Isaac Roberts 

#ifndef BasicMeterLAF_h
#define BasicMeterLAF_h

#include <JuceHeader.h>

class BasicMeterLAF : public LookAndFeel_V4
{
public:
	 BasicMeterLAF();
	 BasicMeterLAF(bool isVertical);
	 BasicMeterLAF(Colour col, bool isVertical);

	 void drawMeter (juce::Graphics&, int x, int y, float relLevel,
					 float level, float min, float max,
					 int width, int height,
					 const RTMeter* meter) override;

	virtual ~BasicMeterLAF();
	void setVertical(bool set) { vertical = set; }
	void setColor(Colour set) {col = set;}
	void setBGCol(Colour set) {bg = set;}
	void setOutlineCol(Colour set) {outline = set; }
private:
	Colour col;
	Colour bg;
	Colour outline;
	bool vertical;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicMeterLAF);

};


#endif /* BasicMeterLAF_h */
