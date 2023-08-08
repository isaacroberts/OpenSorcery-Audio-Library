//
//  FileLoad.cpp
//  
//
//  Created by Isaac Roberts on 2/3/22.
//

#include "FileLoad.h"

#ifdef _WIN32
const juce::String FileLoad::sep = "\\";
#else
const juce::String FileLoad::sep =  "/";
#endif

const juce::String FileLoad::presetSuffix = ".preset";
