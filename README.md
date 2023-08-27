# robertsaudiolibrary

The audio library I used for freelancing in audio plugins. I'm making it public now because I'm switching industries and hoping to help other developers get on their feet!

Includes the JUCE library, which must be licensed from https://juce.com.

The JUCE library isn't linked as a submodule because I added minor changes and inserted my libraries into the include tree. 
  The changes are marked with //JUCE UPDATE, so if you are using this repo and need to update to the most recent version of JUCE, you can search the codebase, and either carry over or rollback the changes. 

Using this library requires you to use certain settings in the projucer. You can use the Template project to start with those. 
You will also need to install the boost lockfree module. 

An example project is included because I was rarely able to use this library without copying & pasting my own prior code :) 

If you are using this library, please let me know and I will add more documentation! When writing the code I intended to be the only person using it so it a bit messy at times!

# About the Code:

In the code, the library is referred to as "RT" (RTCompressor, RTGain, etc), because my company name changed several times. The RT stands for "Roberts.Tech" 

## FXProcessor

The FXProcessor class is a combined process chain / preset manager / AudioProcessor object. This makes it very fast to set up projects, and the centralization makes it easy to make large changes late into the project. 

The FXProcessor replaces the AudioProcessor, meaning it's the main object that gets top-level calls from JUCE and creates the main gui window.

## Presets

I used my own presets module, although I probably didn't need to. It made it a lot easier to change around presets.  

The FXProcessor expects a subclass of PresetObject, which will contain your list of parameters and any nested PresetOjbects. 

## Process Chain

Processor chains are laid out as chains & trees of FX, just like in JUCE.

I used an adaptation of the juce_ProcessorChain, renamed to "Chain" in RT/FX. 

The Chain class uses templating to improve speed. Each processor elements is an FX object.

The Templating allows you to customize your process chain from a single file.

```
typedef Chain<Piano, MoogFilter, RTCompressor, RTReverb, RTLowPassButterworth> JazzyPianoSynthChain;
```

Because of the templating, FX objects don't have to derive from the FX base class as long as they have the same function signature, which is laid out in RT > FX > Templates. 

```
typedef FXProcessor<JazzyPianoSynthChain, JazzyPianoPreset, HasInstr=True> JazzyPianoProcessor;
```

## Processing Modes

  My FX modules extend from the concepts in the JUCE modules. 
```
  //Used to set up processing, allocate data, etc
	void prepare2(Spec& spec) override;

  //Main processing, replacing the input audio block
	void process (ContextR&);
  //Main processing, reading from an input block and writing to an output block
	void process (ContextNR&);

  //Bypass processes. Can be used to tail audio for reverbs/delays, or decay filters.
  // inputIsSilent allows fx to assume that input=0, simplifying calculations. 
	void bypassProcess(ContextR&  c, bool allowTail, bool inputIsSilent) 
	void bypassProcess(ContextNR& c, bool allowTail, bool inputIsSilent) 

  // Only relevant for compressors. Most modules redirect calls to process 
 	void sidechain(InputBlock& level, ContextR& c) { process(c); }
 	void sidechain(InputBlock& level, ContextNR& c) { process(c); }

  // I think this is unused, but resets state variables and things like that 
	void reset2() override;
```


My library uses bypassProcess and silenceProcess to signify to processors that they don't need to do full processing.

For example, Filters can use this to assume input=0 and simplify calculations 
These functions are added to some of the JUCE Fx modules where needed. 

Each FX object must have process replacing and process non-replacing (process(ContextR&) and process(ContextNR&)). 

