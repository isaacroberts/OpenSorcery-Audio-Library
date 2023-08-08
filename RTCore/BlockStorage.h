//
//  BlockStorage.h
//  
//
//  Created by Isaac Roberts on 8/29/20.
//

// Created by Isaac Roberts 

//#pragma once

//#include <JuceHeader.h>

#if __has_include(<boost/lockfree/stack.hpp>)

#ifdef __GNUG__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcomma"
#include <boost/lockfree/stack.hpp>
#pragma GCC diagnostic pop
#else 
#include <boost/lockfree/stack.hpp>
#endif

#include <stack>

class BlockStorage;
class ContextR;
class ContextNR;

class BlockPtr {
public:
	BlockPtr(BlockStorage&);
	BlockPtr(BlockStorage*);
	
	BlockPtr(ContextR&);
	BlockPtr(ContextNR&);
	
	//Move constructor
	BlockPtr(BlockPtr&&);

	~BlockPtr();
	
	juce::dsp::AudioBlock<float>& operator*() {
		return *block;
	}
	juce::dsp::AudioBlock<float>* operator->() {
		return block;
	}
	juce::dsp::AudioBlock<float>* getBlock() {
		return block;
	}
	float* operator[](int c) {
		return block->getChannelPointer(c);
	}
	operator juce::dsp::AudioBlock<float>() {
		return *block;
	}
	operator juce::dsp::AudioBlock<float>&() {
		return *block;
	}

private:
//	BlockPtr(const BlockPtr& other)=delete;
	
	juce::dsp::AudioBlock<float>* block;
	BlockStorage* storage;
	int id;
public:
	int getID() { return id;}
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlockPtr);
};

class PersistentBlockPtr {
	/* The PersistentBlockPtr avoids allocating and deallocating BlockPtr objects to allow BlockPtrs to persist
	 */
public:
	PersistentBlockPtr(BlockStorage&);
	PersistentBlockPtr();
//
//	PersistentBlockPtr(ContextR&);
//	PersistentBlockPtr(ContextNR&);
	
	~PersistentBlockPtr();
	
	void setBlockStorage(BlockStorage& bs) {storage = &bs;}
	void setBlockStorage(BlockStorage* bs) {storage =  bs;}
	
	void alloc(BlockStorage& bs);
	void alloc();
	void dealloc();
	
	inline void free() {dealloc();}
	
	inline bool isAllocated() { return allocated;}
	
	juce::dsp::AudioBlock<float>& operator*() {
		jassert(allocated);
		return *block;
	}
	juce::dsp::AudioBlock<float>* operator->() {
		jassert(allocated);
		return block;
	}
	
	float* operator[](int c) {
		jassert(allocated);
		return block->getChannelPointer(c);
	}
private:
	PersistentBlockPtr(const BlockPtr& other)=delete;
	PersistentBlockPtr(BlockPtr&&)=delete;
	
	juce::dsp::AudioBlock<float>* block;
	BlockStorage* storage;
	int id;
	bool allocated=false;
public:
	int getID() { return id;}
};

class BlockStorage {
	friend class BlockPtr;
	friend class PersistentBlockPtr;
	
	
public:
//	BlockPtr
	BlockStorage();//maxCapacity defaults to 7
	BlockStorage(int maxCapacity);
	~BlockStorage();
	
	
	void prepare(int channels, int samples) { prepare_(channels, samples); }
	void reprepare(int channels, int samples) { reprepare_(channels, samples); }
	void allocateN(int n) { allocateN_(n); }
	void free() {free_();}
	
	void refresh(int channels, int samples);
	
	void setCurrentSampleAmt(int set);
	inline void setCurSamples(int set) { setCurrentSampleAmt(set); }
	inline int getCurSamples() { return curSamples; }
	class BlockObject {
	public:
		BlockObject(int id);
		BlockObject(int id, int c, int s);
		~BlockObject();

		void prepare(int c, int s);
		void setCurSampleAmt(int s);
		void reprepare(int c, int s, int orig_c, int orig_s);

		int getNumSamples() { return (int)block.getNumSamples(); }
		
//		juce::HeapBlock<char> heap;
		float** data;
		juce::dsp::AudioBlock<float> block;
		int id;
		bool allocated;
		int ch;
	};
private:
	
	
	BlockObject* getBlockObject() {return getBlockObject_();}
	void returnBlock(int ix) { returnBlock_(ix); }
	
	
	
	void prepare_(int c, int s);
	void reprepare_(int c, int s);
	void allocateN_(int n);
	void free_();
	BlockObject* getBlockObject_();
	void returnBlock_(int n);
	
//	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlockStorage);

	BlockObject* getBO();
	
	int numChannels;
	int maxSamples;
	int curSamples;
	
	std::list<BlockObject*> blocks;
//	std::stack<BlockObject*> freeBlocks;
//	juce::AbstractFifo freeBlocks;
	boost::lockfree::stack<BlockObject*> freeBlocks;
	
//	std::mutex freeBlockLock;
	
};

typedef boost::lockfree::stack<BlockStorage::BlockObject*> BoStack;

#elif NOBLOCKSTORE

//pass 

#else


#error "Include boost lockfree"

class BlockStorage;
class ContextR;
class ContextNR;

class BlockPtr {
public:
	/*
		Must include boost/lockfree/stack to use this class
	 
		
	 */
	BlockPtr(BlockStorage&) {}
	BlockPtr(BlockStorage*) {}
	
	BlockPtr(ContextR&) {}
	BlockPtr(ContextNR&) {}
	
	//Move constructor
	BlockPtr(BlockPtr&&) {}

	~BlockPtr() {}
	
	static juce::dsp::AudioBlock<float> dummy;
	
	juce::dsp::AudioBlock<float>& operator*() {jassertfalse;return dummy;}
	juce::dsp::AudioBlock<float>* operator->() {jassertfalse;return NULL;}
	
	float* operator[](int c) {jassertfalse;return NULL;}
	
public:
	int getID() { jassertfalse;return 0;}
};

class PersistentBlockPtr {
	/*
		Must include boost/lockfree/stack to use this class
	 */
public:
	PersistentBlockPtr(BlockStorage&) {}
	PersistentBlockPtr() {}
	
	~PersistentBlockPtr() {}
	
	void setBlockStorage(BlockStorage& bs) {jassertfalse;}
	void setBlockStorage(BlockStorage* bs) {jassertfalse;}
	
	void alloc(BlockStorage& bs) {jassertfalse;}
	void alloc() {jassertfalse;}
	void dealloc() {jassertfalse;}
	
	inline void free() {jassertfalse;}
	
	inline bool isAllocated() { return false;}
	
	juce::dsp::AudioBlock<float>& operator*() {jassertfalse;return BlockPtr::dummy;}
	juce::dsp::AudioBlock<float>* operator->() {jassertfalse;return NULL;}
	
	float* operator[](int c) {jassertfalse;return NULL;}
private:
public:
	int getID() { jassertfalse;return 0;}
};

class BlockStorage {
	friend class BlockPtr;
	friend class PersistentBlockPtr;
	
	
public:
//	BlockPtr
	BlockStorage() {}
	BlockStorage(int maxCapacity) {}
	~BlockStorage() {}
	
	
	void prepare(int channels, int samples) { jassertfalse; }
	void reprepare(int channels, int samples) {jassertfalse;}
	void allocateN(int n) { jassertfalse; }
	void free() {jassertfalse;}
	
	void refresh(int channels, int samples) {jassertfalse;}
	
};

#endif

//typedef std::unique_ptr<BlockStorage::BlockPtr_> BlockPtr;


