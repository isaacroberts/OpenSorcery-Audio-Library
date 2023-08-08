//
//  RestUtil.h
//  
//
//  Created by Isaac Roberts on 4/24/22.
//

#ifndef RestUtil_h
#define RestUtil_h

#include <JuceHeader.h>

class RestUtil {
public:
	enum ResponseStatus {
		Success, NoInternet, ServerError,
		BadURL, Other, CouldntFindServer
	};
	struct Response {
		ResponseStatus status;
		juce::String response;
		bool success() { return status==Success; }
		bool offline() { return status==NoInternet; }
		juce::var json() {
			jassert (success());
			if (!success()) {
				return juce::var();
			}
			return juce::JSON::parse(response);
		}
	};
	
	RestUtil()
	: stream(nullptr)
	{
		
	}
	
	
	static Response runRequest(juce::String request, int timeoutMs=1000, bool expectResponse=true, bool printOnDebug=true)
	{
		using namespace juce;
		
		StringPairArray responseHeaders;
		int statusCode = 0;
		
		URL url(request);
			
		if (!url.isWellFormed())
		{
			Response resp;
			resp.status=BadURL;
			resp.response = "Malformed URL";
			return resp;
		}
		
		URL::InputStreamOptions streamOpts = URL::InputStreamOptions(URL::ParameterHandling::inAddress).withConnectionTimeoutMs(timeoutMs).withStatusCode(&statusCode);
		
//		std::cout<<"Sending request "<<request<<std::endl;
		std::unique_ptr<InputStream> stream  = url.createInputStream (streamOpts);
		
		Response resp;
		
		if (stream != nullptr)
		{
			if (statusCode != 200)
			{
				if (statusCode==500)
					resp.status=ServerError;
				else
					resp.status=Other;
				resp.response = "Failed to connect, status code = " + String (statusCode);
			}
			else
			{
//				std::cout<<"Response complete: "<<std::endl;
				resp.status=Success;
				resp.response = stream->readEntireStreamAsString();
#if DEBUG
				if (printOnDebug)
				{
					std::cout<<"Response = "<<resp.response<<std::endl;
				}
#endif
				if (expectResponse && resp.response.isEmpty())
				{
					resp.status=ServerError;
					resp.response = "No response from server.";
				}
			}
		}
		else
		{
			if (hasInternetAccess())
			{
				resp.status = CouldntFindServer;
			}
			else
				resp.status = NoInternet;
			resp.response = "Failed to connect";
		}
		return resp;
	}
	static Response runPostRequest(String request, String postData,
								   int timeoutMs=1000, bool expectResponse=true, bool printOnDebug=true)
	{
		URL url(request);
		url = url.withPOSTData(postData);
		
		
		int statusCode;
		
		URL::InputStreamOptions streamOpts = URL::InputStreamOptions(URL::ParameterHandling::inPostData).withConnectionTimeoutMs(timeoutMs).withStatusCode(&statusCode);
		
//		std::cout<<"Sending request "<<request<<std::endl;
		std::unique_ptr<InputStream> stream  = url.createInputStream (streamOpts);
		
		Response resp;
		
		if (stream != nullptr)
		{
			if (statusCode != 200)
			{
				if (statusCode==500)
					resp.status=ServerError;
				else
					resp.status=Other;
				resp.response = "Failed to connect, status code = " + String (statusCode);
			}
			else
			{
//				std::cout<<"Response complete: "<<std::endl;
				resp.status=Success;
				resp.response = stream->readEntireStreamAsString();
#if DEBUG
				if (printOnDebug)
				{
					std::cout<<"Response = "<<resp.response<<std::endl;
				}
#endif
				if (expectResponse && resp.response.isEmpty())
				{
					resp.status=ServerError;
					resp.response = "No response from server.";
				}
			}
		}
		else
		{
			if (hasInternetAccess())
			{
				resp.status = CouldntFindServer;
			}
			else
				resp.status = NoInternet;
			resp.response = "Failed to connect";
		}
		return resp;
	}
	
	
	static bool canConnectToWebsite (const URL& url)
	{
		auto opts = URL::InputStreamOptions(URL::ParameterHandling::inAddress)
				.withConnectionTimeoutMs(2000)
				.withNumRedirectsToFollow(2);
		std::unique_ptr<InputStream> in (url.createInputStream (opts));
		return in != nullptr;
	}

	static bool hasInternetAccess()
	{
			
			
			 //Can't believe bing counted among the most stable websites in the world
		
#if JUCE_MAC
		//Mac will block non-https
		constexpr int numUrls=3;
		const char* urlsToTry[numUrls] = {
									"https://google.com", "https://bing.com", "https://amazon.com"};

#else
		constexpr int numUrls=6;
		const char* urlsToTry[numUrls] = {
									"http://google.com",  "http://bing.com",  "http://amazon.com",
									"https://google.com", "https://bing.com", "https://amazon.com"};
#endif
		
		for (int n=0; n < numUrls; ++n)
		{
			if (canConnectToWebsite (URL (urlsToTry[n])))
				return true;
		}
		return false;

	}
	
	
	~RestUtil()
	{
//		if (stream)
//			stream->cancel();
	}
	void cancel()
	{
//		if (stream)
//			stream->cancel();
	}
	
protected:
	std::unique_ptr<InputStream> stream;
	
};

#endif /* RestUtil_h */
