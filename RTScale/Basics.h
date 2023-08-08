//
//  Basics.h
//  
//
//  Created by Isaac Roberts on 5/15/22.
//

// Created by Isaac Roberts 

//#pragma once

namespace Scale
{
//	enum ModeNames {
//		Ionian_=0, Dorian_, Phrygian_, Lydian_, Mixolydian_, Aeolian_, Locrian_
//	};
	
	
	inline int mod(int v, int m) {
		return v >= 0 ? v % m : ((v % m) + m)%m;
	}
	inline int mod(int v) //assume 12
	{
		return v >= 0 ? v % 12 : ((v % 12) + 12)%12;
	}
	inline int mod12(int v) {
		return v >= 0 ? v % 12 : ((v % 12) + 12)%12;
	}
	inline int mod7(int v)  {
		return v >= 0 ? v % 7 : ((v % 7) + 7)%7;
	}
	inline int mod5(int v)  {
		return v >= 0 ? v % 5 : ((v % 5) + 5)%5;
	}

	inline int oct12(int v) 		{ return std::floor(v/12.); }
	inline int oct7(int v) 			{ return std::floor(v/7.); }
	inline int oct5(int v) 			{ return std::floor(v/5.); }
	inline int oct(int v, int k) 	{ return std::floor(v/float(k)); }
	inline int oct_(int v, int k) 	{ return std::floor(v/float(k)); }

	
	constexpr const char* const note_names[12] {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};


	
	constexpr const int interval[7] {0, 2, 4, 5, 7, 9, 11};
	constexpr const int modeNum[12] { 0, -1,  1, -1,  2,  3, -1,  4, -1,  5, -1,  6};
	constexpr const int darkInterval[5] = {1, 3, 6, 8, 10};
	constexpr const int darkModeNum[12] = {-1,  0, -1,  1, -1, -1,  2, -1,  3, -1,  4, -1};

	
	extern std::string getNoteName(int n);
	extern std::string getSpacedNoteName(int n);

	extern int getNoteNumber(juce::String name, bool allowPureNumber=true);

	extern bool isPitchWhite(int n);
	inline bool isPitchBlack(int n) { return !isPitchWhite(n); }
	
	extern int getWhiteNote(int iv);
	extern int getBlackNote(int iv);
	extern int intervalToNote(int iv);
	extern int darkIntervalToNote(int iv);

	//Returns Iv + Oct*7
	extern int noteToIntervalOct(int note);
	//Returns Iv, Oct
	extern int noteToInterval(int note, int& oct);
	//Returns Iv
	extern int noteToInterval(int note);
	
	//Returns whether previous note on keyboard was white, from interval
	extern bool previousKeyWasWhite(int iv);
	extern bool nextKeyIsWhite(int iv);
	
	
	
	
	
}
