//
//  BlockStorage.cpp
//  
//
//  Created by Isaac Roberts on 8/29/20.
//

// Created by Isaac Roberts

#if __has_include(<boost/lockfree/stack.hpp>)

//#include "BlockStorage.h"

BlockPtr::BlockPtr(BlockStorage& bs)
 : storage(&bs)
{
	BlockStorage::BlockObject* bo  = storage->getBlockObject();
	block = &bo->block;
	id = bo->id;
	jassert(block->getNumSamples() == storage->getCurSamples());
}
BlockPtr::BlockPtr(BlockStorage* bs)
 : storage(bs)
{
	BlockStorage::BlockObject* bo  = storage->getBlockObject();
	block = &bo->block;
	id = bo->id;
	jassert(block->getNumSamples() == storage->getCurSamples());
}
BlockPtr::BlockPtr(ContextR& c)
 : storage(&c.getStorage())
{
	BlockStorage::BlockObject* bo  = storage->getBlockObject();
	block = &bo->block;
	id = bo->id;
	jassert(block->getNumSamples() == storage->getCurSamples());
}
BlockPtr::BlockPtr(ContextNR& c)
 : storage(&c.getStorage())
{
	BlockStorage::BlockObject* bo  = storage->getBlockObject();
	block = &bo->block;
	id = bo->id;
	jassert(block->getNumSamples() == storage->getCurSamples());
}
//Move constructor
BlockPtr::BlockPtr(BlockPtr&& other)
{
	block = other.block;
	id = other.id;
	storage = other.storage;
	
	other.block = NULL;
	other.id = -1;
	
	jassert(block->getNumSamples() == storage->getCurSamples());
}

BlockPtr::~BlockPtr()
{
	if (id != -1)
	{
		storage->returnBlock(id);
	}
}


PersistentBlockPtr::PersistentBlockPtr()
 : block(NULL), storage(NULL), id(-1), allocated(false)
{}
PersistentBlockPtr::PersistentBlockPtr(BlockStorage& bs)
 : block(NULL), storage(&bs), id(-1), allocated(false)
{}

void PersistentBlockPtr::alloc(BlockStorage& bs)
{
	if (!storage)
		storage = &bs;
	if (!allocated)
	{
		allocated=true;
		BlockStorage::BlockObject* bo  = storage->getBlockObject();
		block = &bo->block;
		id = bo->id;
	}
	jassert(block->getNumSamples() == storage->getCurSamples());
	
}
void PersistentBlockPtr::alloc()
{
	if (!allocated)
	{
		jassert(storage);
		allocated=true;
		BlockStorage::BlockObject* bo  = storage->getBlockObject();
		block = &bo->block;
		id = bo->id;
	}
	jassert(block->getNumSamples() == storage->getCurSamples());
}
void PersistentBlockPtr::dealloc()
{
	if (allocated)
	{
		storage->returnBlock(id);
		block=NULL;
		id=-1;
		allocated=false;
	}
}


PersistentBlockPtr::~PersistentBlockPtr()
{
	if (allocated) {
		allocated=false;
		storage->returnBlock(id);
		block=NULL;
	}
}



BlockStorage::BlockStorage()
: numChannels(0),
maxSamples(0),
curSamples(0),
blocks(),
freeBlocks(7)
{
	
}

BlockStorage::BlockStorage(int maxCapacity) :
numChannels(0),
maxSamples(0),
curSamples(0),
blocks(),
freeBlocks(maxCapacity)
{
	allocateN(maxCapacity);
}



void BlockStorage::allocateN_(int N) {
//	if (!blocks.empty())
//		blocks.reserve(N);
//	else
//		blocks.reserve(blocks.size() + N);
	
	freeBlocks.reserve(N);
	
	for (int m=0; m < N; ++m)
	{
		auto* b = getBO();
		blocks.push_back(b);
		freeBlocks.push(blocks.back());
	}
}

BlockStorage::~BlockStorage()
{
	while (blocks.size() > 0)
	{
		auto iter = blocks.begin();
		delete *iter;
		blocks.pop_front();
	}
//	freeBlocks.clear();
//	freeBlocks.reset();
	
}

void voidFunctor(BlockStorage::BlockObject* b)
{
}


void BlockStorage::free_()
{
	
	while (blocks.size() > 0)
	{
		auto iter = blocks.begin();
		delete *iter;
		blocks.pop_front();
	}
	
	freeBlocks.consume_all(voidFunctor);
	
}
void BlockStorage::refresh(int newNumC, int newSamples)
{
	if (numChannels != newNumC || maxSamples != newSamples)
	{
		numChannels = newNumC;
		maxSamples = newSamples;
		
		int neededBlocks = (int)blocks.size();
		
		free();
		prepare(numChannels, maxSamples);
		allocateN(neededBlocks);
	}
}

void BlockStorage::prepare_(int c, int numSamples)
{
	numChannels = c;
	maxSamples = numSamples;
	
//	for (int n=0; n < blocks.size(); ++n) {
	for (auto iter = blocks.begin(); iter != blocks.end(); ++iter)
		(*iter)->prepare(numChannels, maxSamples);
	
}
void BlockStorage::setCurrentSampleAmt(int set)
{
	if (curSamples == set)
		return;
	jassert(set <= maxSamples);
		
	curSamples = set;
	
	for (auto iter = blocks.begin(); iter != blocks.end(); ++iter)
		(*iter)->setCurSampleAmt(curSamples);
}


void BlockStorage::reprepare_(int c, int numS)
{
	//Unused
	
//	for (int n=0; n < blocks.size(); ++n)
	for (auto iter = blocks.begin(); iter != blocks.end(); ++iter)
		(*iter)->reprepare(c, numS, numChannels, maxSamples);

	numChannels = c;
	maxSamples = numS;
}


BlockStorage::BlockObject* BlockStorage::getBO()
{
	if (numChannels ==0 || maxSamples == 0)
		return new BlockObject((int)blocks.size());
	else
		return new BlockObject((int)blocks.size(), numChannels, maxSamples);
}


BlockStorage::BlockObject::BlockObject(int id_)
:
data(NULL),
id(id_),
allocated(false),
ch(0)
{
	allocated = false;
}
BlockStorage::BlockObject::BlockObject(int id_, int c, int s)
:
//heap(),
data(NULL),
id(id_),
ch(c)
{
	if (ch!=0 && s !=0)
	{
		//		heap.free();
		  
		data = new float*[ch];
		for (int cc = 0; cc < ch; ++cc)
		{
			data[cc] =new float[s];
		}
		block = juce::dsp::AudioBlock<float>(data, c, s);
		allocated = true;
	}
	else
		allocated = false;
}
void BlockStorage::BlockObject::prepare(int c, int s)
{
	ch = c;

	if (!allocated)
	{
//		heap.free();
		
		data = new float*[ch];
		for (int cc = 0; cc < ch; ++cc)
		{
			data[cc] =new float[s];
		}
		block = juce::dsp::AudioBlock<float>(data, c, s);
		allocated = true;
	}
	else
	{
	}
}
void BlockStorage::BlockObject::setCurSampleAmt(int s)
{
	block = juce::dsp::AudioBlock<float>(data, ch, s);
}
void BlockStorage::BlockObject::reprepare(int c, int s, int orig_c, int orig_s)
{
	//UNUSED
	jassert(false);
	//this may be called twice due to multiple instances
	if (!allocated)
	{
//		block = juce::dsp::AudioBlock<float>(heap, c, s);
		allocated = true;
	}
	else if (c > orig_c || s > orig_s)
	{
		allocated = false;
//		heap.free();
//		block = juce::dsp::AudioBlock<float>(heap, c, s);
		allocated = true;
	}
}
BlockStorage::BlockObject::~BlockObject() {
	if (allocated)
	{
		allocated=false;
		for (int c=0; c < ch; ++c) {
			delete [] data[c];
		}
		delete[] data;
//		heap.free();
	}
}

BlockStorage::BlockObject* BlockStorage::getBlockObject_()
{
	BlockObject* blockobject;
	
	if (freeBlocks.pop(blockobject))
	{
		jassert(blockobject->getNumSamples() == getCurSamples());
		return blockobject;
	}
	else
	{
		BlockObject* object = getBO();
		jassert(object->getNumSamples() == getCurSamples());

#if DEBUG
		std::cout<<"Allocating block "<<object->id<<std::endl;
#endif
		blocks.push_back(object);
		
		return object;
	}
}
void BlockStorage::returnBlock_(int ix)
{
	for (auto iter = blocks.begin(); iter!=blocks.end(); ++iter)
	{
//		if (ix==(*iter)->id)
		if (ix==0)
		{
			bool sxs = freeBlocks.push(*iter);
			if (sxs)
			{
				return;
			}
			else
			{
				// ???
				return;
			}
		}
		ix--;
	}
	
}

#else
#include "BlockStorage.h"
//juce::dsp::AudioBlock<float> BlockPtr::dummy;

#endif
