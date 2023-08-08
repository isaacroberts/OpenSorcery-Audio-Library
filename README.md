# robertsaudiolibrary

The audio library I used for freelancing in audio plugins. I'm making it public now because I'm switching industries and hope to help other developers get on their feet faster.

Includes the JUCE library, which must be licensed from juce.com.

The JUCE library isn't linked as a submodule because I added minor changes and inserted my libraries into the include tree. 
  The changes are marked with #JUCE UPDATE, so if you are using this repo and need to update to the most recent version of JUCE, you can search the codebase, and either carry over or rollback the changes. 

Using this library requires you to use certain settings in the projucer. You can use the Template project to start with those. 
You will also need to install the boost lockfree module. 

If you are using this library, please let me know and I will add more documentation! At the time of writing I intended to be the only person using this code so it is not documented as well as it should be! 
