//
//  SynchVal.h
//  
//
//  Created by Isaac Roberts on 7/23/21.
//

#pragma once

#include <JuceHeader.h>

using namespace juce;

template <typename Type>
class SynchVal : public AsyncUpdater
{
public:
	class Listener {
	public:
		Listener() = default;
		virtual ~Listener() = default;
		virtual void synchValueChanged(Type& t)=0;
	};
	SynchVal(Type init)
	: AsyncUpdater(),
	val(init), listeners()
	{
		listeners.clear();
	}
	SynchVal()
	: AsyncUpdater(),
	val(), listeners()
	{
		listeners.clear();
	}
	
	void setValue (const Type& newValue) {
		if (newValue != val)
		{
			val = newValue;
			triggerAsyncUpdate();
		}
	}
	Type operator= (const Type& newValue) { setValue(newValue); return val; }
	bool operator==(const Type& other) { return val == other; }
	
	Type operator*() { return val; }
	Type getValue()  { return val; }
	
	void addListener(Listener* listener) {
		if (listener != nullptr)
		{
			listeners.add (listener);
			listener->synchValueChanged(val);
		}
	}
	void removeListener (Listener* listener) {
		listeners.remove (listener);
	}
	
	
	
	
	virtual ~SynchVal() {
		cancelPendingUpdate();
	}
	
protected:
	Type val;
	ListenerList<Listener> listeners;
	
	void callListeners() {
		if (listeners.size() > 0)
		{
			listeners.call ([&] (Listener& l) { l.synchValueChanged (val); });
		}
	}
	
	void handleAsyncUpdate() override {
		callListeners();
	}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynchVal);

};

template<class T>
OutputStream& JUCE_CALLTYPE operator<< (OutputStream& stream, const SynchVal<T>& sval)
{
	return stream << *sval;
}
