//
//  TimelineComponent.hpp
//  PianoRollEditor - App
//
//  Created by Samuel Hunt on 04/02/2020.
//

//#ifndef TimelineComponent_hpp
//#define TimelineComponent_hpp

//#include "PConstants.h"

/*
 Timelime component assumes 4/4
 todo: add support for other time signatures
 TODO: yes, add support for time signatures
 */
class TimelineComponent : public juce::Component {
public:
    
    TimelineComponent ();
    //todo
    //TimelineComponent (const timeSig);
    
    void setup (const int barsToDraw, const int pixelsPerBar);
    void paint (juce::Graphics & g);
    void resized ();
private:
    int barsToDraw;
    int pixelsPerBar;
};

//#endif /* TimelineComponent_hpp */
