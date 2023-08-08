
#pragma once

#include <JuceHeader.h>
#include <limits.h>

using namespace juce;

class FileLoad
{
public:
	// ------------------ Utility ------------------------------------------------------
	enum LoadReturnCode : int {
		LOAD_SUCCESS=0, LOAD_FILENOTFOUND, LOAD_WRONGPLUGIN, LOAD_INCOMPLETE, LOAD_BADFILE, LOAD_ISDIR,
		LOAD_NOT_IMPLEMENTED, LOAD_BADXML
	};

	const static juce::String sep;
	
	
	const static juce::String presetSuffix;

	// ------------------ File Paths ----------------------------------------------------
	//Location of presets and other data
	static File getDataLocation()
	{
		File file = File::getSpecialLocation(File::userApplicationDataDirectory);

		file = file.getChildFile(ProjectInfo::companyName);
		if (!file.exists())
			file.createDirectory();
		file = file.getChildFile(ProjectInfo::projectName);
		if (!file.exists())
			file.createDirectory();
		
		return file;
	}
	static File getDataLocation(String filename)
	{
		File file = getDataLocation();
		file = file.getChildFile(filename);
		if (!file.exists())
			file.create();
		return file;
	}
	static File getResourceLocation()
	{
		File file = File::getSpecialLocation(File::commonApplicationDataDirectory);
		
		file = file.getChildFile(ProjectInfo::companyName);
		file = file.getChildFile(ProjectInfo::projectName);
		
		return file;
	}
	static File getResourceLocation(String filename)
	{
		File file = File::getSpecialLocation(File::commonApplicationDataDirectory);
		
		file = file.getChildFile(ProjectInfo::companyName);
		file = file.getChildFile(ProjectInfo::projectName);
		
		File f2 = file.getChildFile(filename);

		if (f2.exists()) {
			return f2;
		}
		return file;
	}

	
	// -------------------- XML ------------------------------------------------------
	static std::unique_ptr<XmlElement> loadToXml(File file, int& errorCode)
	{
		if (! file.exists()) {
			errorCode = LOAD_FILENOTFOUND;
			return NULL;
		}
		if (file.isDirectory()) {
			errorCode = LOAD_ISDIR;
			return NULL;
		}
		std::unique_ptr<XmlElement> xml = XmlDocument::parse(file);
		
		if (xml == nullptr)
		{
			errorCode = LOAD_BADXML;
			return NULL;
		}
		else
		{
			errorCode = LOAD_SUCCESS;
			return xml;
		}
	}
	static std::unique_ptr<XmlElement> loadToXml(String filename, int& errorCode)
	{
		File file = File(filename);
		return loadToXml(file, errorCode);
	}
	static bool saveXml(String filename, std::unique_ptr<XmlElement> xml)
	{
		File f = File(filename);
		return xml->writeTo(f);
	}
	static bool saveXml(String filename, XmlElement* xml)
	{
		File f = File(filename);
		return xml->writeTo(f);
	}
	static bool saveXml(String filename, XmlElement& xml)
	{
		File f = File(filename);
		return xml.writeTo(f);
	}
	static bool saveXml(File f, std::unique_ptr<XmlElement> xml)
	{
		return xml->writeTo(f);
	}
	static bool saveXml(File f, XmlElement* xml)
	{
		return xml->writeTo(f);
	}
	static bool saveXml(File f, XmlElement& xml)
	{
		return xml.writeTo(f);
	}
		
	

	// -------------------- Audio Formats ------------------------------------------------------
public:
	static constexpr const char* const supportedAudioFilesPattern =
								"*.wav"
#if JUCE_USE_MP3AUDIOFORMAT
								";*.mp3"
#endif
#if JUCE_USE_OGGVORBIS
								";*.ogg"
#endif
								";*.aiff;*.aifc;*.aif"
#if JUCE_MAC
								";*.caf"
#endif
#if JUCE_USE_FLAC
								";*.flac"
#endif
#if JUCE_USE_WINDOWS_MEDIA_FORMAT
								";*.wmv"
#endif
										;
	
private:
//	typedef std::unique_ptr<AudioFormatReader> AFRPtr;
	template <typename Format>
	static AudioFormatReader* tryAudioFormat(File file, FileInputStream* stream)
	{
		Format format;
		if (format.canHandleFile(file))
		{
			AudioFormatReader* reader = format.createReaderFor(stream, false);
			return reader;
		}
		return NULL;
	}
	template <typename Format>
	static bool formatSupportsFile(File file)
	{
		Format format;
		return (format.canHandleFile(file));
	}

	static AudioFormatReader* _getAudioFormatReader_ptr(File file)
	{
		auto stream = file.createInputStream().release();
		if (stream)
		{
			if (auto r = tryAudioFormat<WavAudioFormat >(file, stream)) return r;
			#if JUCE_USE_MP3AUDIOFORMAT
			if (auto r = tryAudioFormat<MP3AudioFormat >(file, stream)) return r;
			#endif
			#if JUCE_USE_OGGVORBIS
			if (auto r = tryAudioFormat<OggVorbisAudioFormat >(file, stream)) return r;
			#endif
			if (auto r = tryAudioFormat<AiffAudioFormat >(file, stream)) return r;
			#if JUCE_MAC
			if (auto r = tryAudioFormat<CoreAudioFormat >(file, stream)) return r;
			#endif
			#if JUCE_USE_FLAC
			if (auto r = tryAudioFormat<FlacAudioFormat >(file, stream)) return r;
			#endif
			#if JUCE_USE_LAME_AUDIO_FORMAT
			DBG("LAME not supported yet - requires LAME executable");
//			if (auto r = tryAudioFormat<LAMEEncoderAudioFormat >(file, stream)) return r;
			#endif
			#if JUCE_USE_WINDOWS_MEDIA_FORMAT
			if (auto r = tryAudioFormat<WindowsMediaAudioFormat >(file, stream)) return r;
			#endif
			delete stream;
		}
		return NULL;
	}
public:
	static bool isAudioFormatSupported(File file)
	{
		if (formatSupportsFile<WavAudioFormat >(file)) return true;
		#if JUCE_USE_MP3AUDIOFORMAT
		if (formatSupportsFile<WavAudioFormat >(file)) return true;
		#endif
		#if JUCE_USE_OGGVORBIS
		if (formatSupportsFile<OggVorbisAudioFormat >(file)) return true;
		#endif
		if (formatSupportsFile<AiffAudioFormat >(file)) return true;
		#if JUCE_MAC
		if (formatSupportsFile<CoreAudioFormat >(file)) return true;
		#endif
		#if JUCE_USE_FLAC
		if (formatSupportsFile<FlacAudioFormat >(file)) return true;
		#endif
		#if JUCE_USE_LAME_AUDIO_FORMAT
		DBG("LAME not supported yet - requires LAME executable");
//			if (formatSupportsFile<LAMEEncoderAudioFormat >(file)) return true;
		#endif
		#if JUCE_USE_WINDOWS_MEDIA_FORMAT
		if (formatSupportsFile<WindowsMediaAudioFormat >(file)) return true;
		#endif
		return false;
	}

	static std::unique_ptr<AudioFormatReader> getAudioFormatReader(File file)
	{
		AudioFormatReader* reader = _getAudioFormatReader_ptr(file);
		if (reader)
			return std::unique_ptr<AudioFormatReader>(reader);
		else
			return std::unique_ptr<AudioFormatReader>(nullptr);
	}

	
	static AudioBuffer<float> loadSample(File file, float& sourceRate,  bool isDefinitelyWav=false, float maxSeconds=5)
	{
		AudioFormatReader* reader=NULL;
		if (isDefinitelyWav)
		{
			auto inputStream = file.createInputStream().release();
			WavAudioFormat wavFormat;
			reader = wavFormat.createReaderFor(inputStream, false);
		}
		else {
			reader = _getAudioFormatReader_ptr(file);
		}
		
		if (reader)
		{
			sourceRate = reader->sampleRate;
			int numSamples;
			if (maxSeconds > 0)
			{
				long maxSmp = long(sourceRate * maxSeconds);
				maxSmp = jlimit((long)1, (long)(std::numeric_limits<int>::max()-1), maxSmp);
				numSamples = std::min((int)maxSmp, (int)reader->lengthInSamples);
			}
			else
				numSamples = (int)reader->lengthInSamples;
			AudioBuffer<float> buffer(reader->numChannels, numSamples);
			reader->read(&buffer, 0, numSamples, 0, true, true);
			
			delete reader;
			return buffer;
		}
		
		sourceRate = 0;
		return AudioBuffer<float>();
		
	}
	
	static AudioBuffer<float> loadSample(const char* data, size_t dataSize, float& sourceRate, float maxSeconds = 10)
	{
		//Assumed to be wav
		MemoryInputStream *inputStream = new MemoryInputStream(data, dataSize, false);
		WavAudioFormat wavFormat;
		AudioFormatReader* reader = wavFormat.createReaderFor(inputStream, false);

		if (reader)
		{
			sourceRate = reader->sampleRate;
			int numSamples;
			if (maxSeconds > 0)
			{
				long maxSmp = long(sourceRate * maxSeconds);
				maxSmp = jlimit((long)1, (long)(std::numeric_limits<int>::max()-1), maxSmp);
				numSamples = std::min((int)maxSmp, (int)reader->lengthInSamples);
			}
			else
				numSamples = (int)reader->lengthInSamples;
			AudioBuffer<float> buffer(reader->numChannels, numSamples);
			reader->read(&buffer, 0, numSamples, 0, true, true);
			
			delete reader;
			return buffer;
		}
		sourceRate = 0;
		return AudioBuffer<float>();
	}
	
	// ------------------ Platform Compatibility ------------------------------------------------------
	static bool checkForInvalidWinFilenames(String string)
	{
		#if TARGET_WIN
		//If ladder so we don't have to store it on the stack
		if (string == "CON")
			return true;
		else if (string == "PRN")
			return true;
		else if (string == "AUX")
			return true;
		else if (string == "NUL")
			return true;
		else if (string == "COM1")
			return true;
		else if (string == "COM2")
			return true;
		else if (string == "COM3")
			return true;
		else if (string == "COM4")
			return true;
		else if (string == "COM5")
			return true;
		else if (string == "COM6")
			return true;
		else if (string == "COM7")
			return true;
		else if (string == "COM8")
			return true;
		else if (string == "COM9")
			return true;
		else if (string == "LPT1")
			return true;
		else if (string == "LPT2")
			return true;
		else if (string == "LPT3")
			return true;
		else if (string == "LPT4")
			return true;
		else if (string == "LPT5")
			return true;
		else if (string == "LPT6")
			return true;
		else if (string == "LPT7")
			return true;
		else if (string == "LPT8")
			return true;
		else if (string == "LPT9")
			return true;
		#endif
		return false;
	}
	
	static bool isValidPathForThisFilesystem(String string)
	{
		#if TARGET_OSX
		if (string.contains(":"))
			return false;
		#elif TARGET_WIN
		if (string.containsAnyOf("<>\"|?*"))
			return false;
		if (checkForInvalidWinFilenames(string))
			return false;
		#elif TARGET_LINUX
//		if (string.contains("/"))
//			return false;
		#endif
		return true;
	}
	
	static String removeInvalidFileCharactersForThisFilesystem(String string)
	{
	#if TARGET_OSX
		string = string.removeCharacters(":");
#elif TARGET_WIN
		string = string.removeCharacters("<>:\"/\\|?*");
		
		if (checkForInvalidWinFilenames(string))
		{
			string += "_";
		}
#elif TARGET_LINUX
		string = string.remove("/");
#endif
		return string;
	}
	static String removeInvalidFileCharactersForAllFilesystems(String string)
	{
		string = string.removeCharacters("<>:\"/\\|?*");
		
		if (checkForInvalidWinFilenames(string))
		{
			string += "_";
		}
		
		return string;
	}
};
