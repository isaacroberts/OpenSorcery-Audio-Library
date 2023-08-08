/** Copyright 2019 Christoph Hart

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	Note: Be aware that the rLottie wrapper files are licensed under a more permissive license than the
	rest of the HISE codebase. The MIT license only applies where stated in the header.
*/

//#pragma once

using namespace juce;

/** A JUCE component that displays a Lottie animation. */
class RLottieComponent: public Component,
						private Timer
{
public:

	/** Creates a RLottieComponent.  
	
		After you've created an instance, call loadAnimation() and then either play() or setFrameNormalised(). */
	RLottieComponent();

	/** Sets the frame with a normalised value (0...1). */
	void setFrameNormalized(float f);

	/** Returns the current animation position. */
	float getCurrentFrameNormalized() const;
	
	void showAndPlay();
	void hideAndStop();
	void setShownAndPlaying(bool set);
	
	/** Starts playing the animation with the given framerate. */
	void play();

	/** Stops the playback. */
	void stop();

	/** Load an animation from the JSON code. */
	void loadLottie(const String& jsonCode, bool useOversampling=true);

	/** @internal */
	void resized() override;

	/** @internal */
	void paint(Graphics& g) override;

	/** Sets the background colour. If you pass in a opaque colour it might improve the rendering performance. */
	void setBackgroundColour(Colour c);
	void setTransparentBackground();

	/** This will check if the string is a zstd-compressed JSON in base64 format and will decompress and return the uncompressed JSON. 
	
	  Note: in order to use this method, you will need the hi_zstd module, otherwise, this function will not have any effect.
	*/
//	static String decompressIfBase64(const String& s);

protected:
	void timerCallback() override;
private:

	/** @internal */

	Colour bgColor = Colours::black;
	std::unique_ptr<Lottie> currentAnimation;
	
};

