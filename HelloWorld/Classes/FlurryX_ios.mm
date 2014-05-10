//
//  FlurryX_ios.mm
//
//  Created by Elviss Strazdins (based on FlurryAnalticsX by Dmitry Matyukhin)
//

#import "FlurryX.h"
#import "Flurry.h"

using namespace std;

inline NSString* CStrToNSString(const char* string)
{
    return [NSString stringWithCString:string encoding:NSUTF8StringEncoding];
}

NSDictionary* CCDictionaryToNSDictionary(const std::map<std::string, std::string>& dictionary)
{
    NSMutableDictionary* result = [NSMutableDictionary dictionary];
	
	auto iter = dictionary.begin();
	
    for (; iter != dictionary.end(); ++iter)
    {
        [result setObject: CStrToNSString(iter->first.c_str())
                   forKey: CStrToNSString(iter->second.c_str())];
    }

    return result;
}

void FlurryX::setAppVersion(const char* version)
{
    [Flurry setAppVersion:CStrToNSString(version)];
}

const char* FlurryX::getFlurryAgentVersion()
{
    return [[Flurry getFlurryAgentVersion] cStringUsingEncoding:NSUTF8StringEncoding];
}

void FlurryX::setShowErrorInLogEnabled(bool value)
{
    [Flurry setShowErrorInLogEnabled:value];
}

void FlurryX::setDebugLogEnabled(bool value)
{
    [Flurry setDebugLogEnabled:value];
}

void FlurryX::setSessionContinueMillis(long millis)
{
	int seconds = (int)millis / 1000;
    [Flurry setSessionContinueSeconds:seconds];
}

void FlurryX::setSecureTransportEnabled(bool value)
{
    [Flurry setSecureTransportEnabled:value];
}

/*
 start session, attempt to send saved sessions to server 
 */
void FlurryX::startSession(const char* apiKey)
{
    [Flurry startSession:CStrToNSString(apiKey) ];
}

/*
 log events or errors after session has started
 */
void FlurryX::logEvent(const char* eventName)
{
    [Flurry logEvent:CStrToNSString(eventName)];
}

void FlurryX::logEvent(const char* eventName, const char* paramName, const char* paramValue)
{
    map<string, string> params;
    params[paramName] = paramValue;
    
    FlurryX::logEvent(eventName, params);
}

void FlurryX::logEvent(const char* eventName, const std::map<std::string, std::string>& parameters)
{
    [Flurry logEvent:CStrToNSString(eventName) withParameters:CCDictionaryToNSDictionary(parameters)];
}

void FlurryX::logError(const char* errorID, const char* message)
{
    [Flurry logError:CStrToNSString(errorID) message:CStrToNSString(message) exception:nil];
}

/* 
 start or end timed events
 */
void FlurryX::logEvent(const char* eventName, bool timed)
{
    [Flurry logEvent:CStrToNSString(eventName) timed:timed];
}

void FlurryX::logEvent(const char* eventName, const std::map<std::string, std::string>& parameters, bool timed)
{
	[Flurry logEvent:CStrToNSString(eventName) withParameters:CCDictionaryToNSDictionary(parameters) timed:timed];
}

void FlurryX::endTimedEvent(const char* eventName)
{
	[Flurry endTimedEvent:CStrToNSString(eventName) withParameters:nil];
}

void FlurryX::endTimedEvent(const char* eventName, const std::map<std::string, std::string>& parameters)
{
    [Flurry endTimedEvent:CStrToNSString(eventName) withParameters:CCDictionaryToNSDictionary(parameters)];
}

/*
 set user info
 */
void FlurryX::setUserID(const char* userID)
{
    [Flurry setUserID:CStrToNSString(userID)];
}

void FlurryX::setAge(int age)
{
    [Flurry setAge:age];
}

void FlurryX::setGender(Gender gender)
{
	NSString* genderString;
	
	switch (gender)
	{
		case FEMALE:
			genderString = @"f";
			break;
		case MALE:
			genderString = @"m";
			break;
		default:
			genderString = @"";
			break;
	}
	
    [Flurry setGender:genderString];
}

/*
 set location information
 */
void FlurryX::setLatitude(double latitude, double longitude, float horizontalAccuracy, float verticalAccuracy)
{
    [Flurry setLatitude:latitude longitude:longitude horizontalAccuracy:horizontalAccuracy verticalAccuracy:verticalAccuracy];
}

void FlurryX::setReportLocation(bool reportLocation)
{
	
}

/*
 optional session settings that can be changed after start session
 */
void FlurryX::setSessionReportsOnCloseEnabled(bool sendSessionReportsOnClose)
{
    [Flurry setSessionReportsOnCloseEnabled:sendSessionReportsOnClose];
}

void FlurryX::setSessionReportsOnPauseEnabled(bool setSessionReportsOnPauseEnabled)
{
    [Flurry setSessionReportsOnPauseEnabled:setSessionReportsOnPauseEnabled];
}

void FlurryX::setEventLoggingEnabled(bool value)
{
    [Flurry setEventLoggingEnabled:value];
}

void FlurryX::reset()
{
	
}
