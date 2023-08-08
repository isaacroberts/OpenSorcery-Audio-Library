//
//  FX.cpp
//  JuceLibrary - Shared Code
//
//  Created by Isaac Roberts on 7/3/20.
//

// Created by Isaac Roberts

FX::FX()
:
bypass(false),
rate(0)
{
}

//void FX::attachMixer()
//{
//	mixer = new Mixer();
//}

//Wrap functions
void FX::prepare(const ProcessSpec& spec)
{
	rate = spec.sampleRate;
	
//	if (mixer)
//		mixer->prepare(spec);
	
	prepare2(spec);
}
void FX::prepare(float r,int c,  int b)
{
	rate = r;
	ProcessSpec spec;
	spec.numChannels = c;
	spec.sampleRate = r;
	spec.maximumBlockSize= b;
	
//	if (mixer)
//		mixer->prepare(spec);
	prepare2(spec);
		
}
//void FX::process(ContextR& p)
//{
//
////	#if DEBUG
////	jassert (hasBeenPrepared());
////	#endif
//
//	if (mixer)
//		mixer->frame();//Updates asyncMix
//
//	if (isBypassed(p)) {
//		return;
//	}
////	std::cout<<"start proc"<<std::endl;
//	if (mixer)
//		mixer->replacingPreMix(p);
//	processR(p);
//	if (mixer)
//		mixer->replacingPostMix(p);
////	std::cout<<"end proc"<<std::endl;
//}
//void FX::process(ContextNR& p)
//{
//	if (isBypassed(p)) {
//		return;
//	}
//	if (mixer)
//		mixer->frame();
//
//	processNR(p);
//	if (mixer)
//		mixer->nrMix(p);
//}
//void FX::sidechain(InputBlock& l, Context& p)
//{
////	#if DEBUG
////	jassert (hasBeenPrepared());
////	#endif
//
//	if (mixer)
//		mixer->frame();//Updates asyncMix
//
//	if (isBypassed(p)) {
//		return;
//	}
////	std::cout<<"start proc"<<std::endl;
//	if (mixer)
//		mixer->replacingPreMix(p);
//
//	sidechainR(l, p);
//
//	if (mixer)
//		mixer->replacingPostMix(p);
////	std::cout<<"end proc"<<std::endl;
//}
//void FX::sidechain(InputBlock& l, ContextNR& p)
//{
//	if (isBypassed(p)) {
//		return;
//	}
//	if (mixer)
//		mixer->frame();
//
//	sidechainNR(l, p);
//	if (mixer)
//		mixer->nrMix(p);
//}
//void FX::process(InputBlock& from, OutputBlock& to, BlockStorage& bs)
//{
//	auto pnr = ContextNR(bs, from, to);
//	process(pnr);
//}
//void FX::process(OutputBlock& b, BlockStorage& bs)
//{
//	auto pcr = ContextR(bs, b);
//	process(pcr);
//}
void FX::reset()
{
	reset2();
}

//bool FX::isBypassed(ContextNR &context)
//{
//	if (context.isBypassed() || bypass )
//	{
//		context.bypassProcess();
//		return true;
//	}
//	else
//		return false;
//}
//bool FX::isBypassed(ContextR& context)
//{
//	if ( bypass || context.isBypassed()) {
//		return true;
//	}
//	else return false;
//}
/*
bool FX::isBypassed(ContextNR &context, bool condition)
{
	if (condition)
	{
//		Util::copy(context.getOutputBlock(), context.getInputBlock());
		context.bypassProcess();
		bypassProcess(context,false, true);
		return true;
	}
	else if (context.isBypassed() || bypass )
	{
//		Util::copy(context.getOutputBlock(), context.getInputBlock());
		context.bypassProcess();
		bypassProcess(context, true, false);
		return true;
	}
	else
		return false;
}
bool FX::isBypassed(ContextR& context, bool condition)
{
	if (condition) {
		bypassProcess(context, false, false);
		return true;
	}
	if (bypass || context.isBypassed()) {
		bypassProcess(context, true, false);
		return true;
	}
	else return false;
}
*/

FX::~FX()
{
}


/*
void FX::save(XmlElement* parent)
{
	XmlElement* element =new XmlElement(getNameForPreset());
	parent->addChildElement(element);
	extraSave(element);
	preset->save(element);
}
void FX::load(XmlElement* parent)
{
	XmlElement* element = parent->getChildByName(getNameForPreset());
	extraLoad(element);
	if (element != NULL)
	{
		preset->load(element);
	}
}
 */
