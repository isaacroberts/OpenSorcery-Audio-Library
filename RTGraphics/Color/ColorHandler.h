//
//  ColorHandler.h
//  
//
//  Created by Isaac Roberts on 12/17/21.
//

// Created by Isaac Roberts 

#pragma once

#include <JuceHeader.h>

using namespace juce;

class ColorHandler {
public:
//	ColorHandler();
	//	virtual ~ColorHandler();
	
	static void main();
	
	static void rgb2lab( int R, int G, int B, float & l_s, float &a_s, float &b_s );
	static void lab2rgb( float l_s, float a_s, float b_s, float& R, float& G, float& B );
	static void lab2rgb( float l_s, float a_s, float b_s, int& R, int& G, int& B );

	static juce::ColourGradient labGradient(int r1, int g1, int b1, int r2, int g2, int b2, float x1, float y1, float x2, float y2);
//	static juce::ColourGradient addLabToGradient(juce::ColourGradient, int r1, int g1, int b1, float x2, float y2);

	
	static juce::Colour lab2col(float l, float a, float b);
	
protected:
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColorHandler);

};
