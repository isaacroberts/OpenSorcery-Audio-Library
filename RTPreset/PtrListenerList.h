//
//  PtrListenerList.h
//  
//
//  Created by Isaac Roberts on 8/7/22.
//


template <class Type>
class PtrListenerList
{
protected:
	
	/*
	void addParamListener(void* id, std::function<void(Type)> callback, bool callNow=true) {
		PtrListenerList<Type>::addParamListener(id, callback, callNow);
	}
	void removeParamListener(void* id) {
		PtrListenerList<Type>::removeParamListener(id);
	}
	void clearParamListeners() {
		PtrListenerList<Type>::clearParamListeners();
	}
	void updateParamListeners(void* source) {
		PtrListenerList<Type>::updateParamListeners(source);
	}
	 */
	
	struct Listener {
		Listener(void* p, std::function<void(Type)> c) : id(p), callback(c) {}
		void* id;
		std::function<void(Type)> callback;
	};
	
	juce::OwnedArray<Listener> listeners;
	
	virtual Type getValueForListenerList() = 0;
	
public:
	void addParamListener(void* id, std::function<void(Type)> callback, bool callNow=true) {
		auto l = std::make_unique<Listener>(id, callback);
		if (callNow)
			l->callback(getValueForListenerList());
		listeners.add(std::move(l));
	}
	void removeParamListener(void* id) {
		for (int n=0; n < listeners.size(); ++n)
		{
			if (listeners[n]->id==id)
			{
				listeners.remove(n);
				return;
			}
		}
	}
	void clearParamListeners() {
		listeners.clear();
	}
	
protected:
	
	void updateParamListeners(void* source) {
		if (MessageManager::getInstance()->isThisTheMessageThread())
			_internal_update_frontend(source);
		else
			MessageManager::callAsync([&, source]() { _internal_update_frontend(source); });
	}
	
	void _internal_update_frontend(void* caller) {
		auto value = getValueForListenerList();
		for (int n=0; n < listeners.size(); ++n)
		{
			if (listeners[n]->id != caller) {
				listeners[n]->callback(value);
			}
		}
	}
public:
	PtrListenerList() {}
	virtual ~PtrListenerList() {}
};

