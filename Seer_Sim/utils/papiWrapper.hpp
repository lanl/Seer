#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

namespace Seer  
{ 

#ifdef PAPI_ENABLED

#include <papi.h>

/*
# Papi Errors
 0  PAPI_OK No error
-1  PAPI_EINVAL 	Invalid argument
-2  PAPI_ENOMEM 	Insufficient memory
-3  PAPI_ESYS   	A system or C library call failed, please check errno
-4  PAPI_ESBSTR 	Substrate returned an error, usually the result of an unimplemented feature
-5  PAPI_ECLOST 	Access to the counters was lost or interrupted
-6  PAPI_EBUG   	Internal error, please send mail to the developers
-7  PAPI_ENOEVNT    Hardware event does not exist
-8  PAPI_ECNFLCT    Hardware event exists, but cannot be counted due to counter resource limitations
-9  PAPI_ENOTRUN    No events or event sets are currently not counting
-10 PAPI_EISRUN 	Event Set is currently running
-11 PAPI_ENOEVST    No such event set available
-12 PAPI_ENOTPRESET Event is not a valid preset
-13 PAPI_ENOCNTR    Hardware does not support performance counters
-14 PAPI_EMISC  	‘Unknown error’ code
-15 PAPI_EPERM  	You lack the necessary permissions
-16 PAPI_ENOINIT    PAPI hasn't been initialized yet
-17 PAPI_ENOCMP 	Component Index isn't set
-18 PAPI_ENOSUPP    Not supported
-19 PAPI_ENOIMPL    Not implemented
-20 PAPI_EBUF   	Buffer size exceeded
-21 PAPI_EINVAL_DOM EventSet domain is not supported for the operation
-22 PAPI_EATTR  	Invalid or missing event attributes
-23 PAPI_ECOUNT 	Too many events or attributes
-24 PAPI_ECOMBO 	Bad combination of features
*/
// http://icl.cs.utk.edu/projects/papi/files/documentation/PAPI_USER_GUIDE.htm

class PAPIWrapper
{
	int eventSet;
 	int numEvents;
 	long long *hwdValues;

 	std::vector< std::string > registeredEvent;

  public:
  	PAPIWrapper();
  	~PAPIWrapper();

  	int getNumEvents(){ return registeredEvent.size(); }

  	int initPapi();
  	int createPapiEventSet();
  	int addPapiEvent(int eventCode);
  	int addPapiEvent(std::string eventName);
  	int removePapiEvent(int eventCode);
	int removePapiEvent(std::string eventName);
  	int startPapi();
  	int readEvents();
  	int stopReading();

	//int getNumPapiCounters(){ return PAPI_num_counters(); }
	int getNumPapiCounters(){ return PAPI_num_components(); }

	

  	long long getHwdValue(int pos);
  	std::string getPapiEventName(int pos);

  	int handlePAPIError(std::string msg="")
	{
		std::cout << "Error at :" << msg << std::endl;
    	PAPI_perror(msg.c_str());
    	return 0;
	}
};


inline long long PAPIWrapper::getHwdValue(int pos)
{ 
	if (pos < numEvents)
		return hwdValues[pos]; 

	return -1;
}


inline std::string PAPIWrapper::getPapiEventName(int pos)
{ 
	if (pos < numEvents)
		return registeredEvent[pos]; 

	return "";
}


inline PAPIWrapper::PAPIWrapper()
{
	eventSet = PAPI_NULL;
	hwdValues = NULL;
	numEvents = 0;
}


inline PAPIWrapper::~PAPIWrapper()
{
	registeredEvent.clear();
	eventSet = PAPI_NULL;
	numEvents = 0;

	if (hwdValues != NULL)
		delete []hwdValues;
	hwdValues = NULL;
}


inline int PAPIWrapper::initPapi()
{
	int retval = PAPI_library_init(PAPI_VER_CURRENT);

	if (retval != PAPI_VER_CURRENT)
		return handlePAPIError("PAPI_library_init");

	if (getNumPapiCounters() < 1)
		return handlePAPIError("PAPI_num_counters");

	retval = PAPI_multiplex_init();
	if (retval != PAPI_OK) 
		return handlePAPIError("PAPI_multiplex_init");

	return createPapiEventSet();
}


inline int PAPIWrapper::createPapiEventSet()
{
	// Create an EventSet
	int retval = PAPI_create_eventset(&eventSet);
	if (retval != PAPI_OK)
		return handlePAPIError("PAPI_create_eventset");

	// Assign it to the CPU component
  	retval = PAPI_assign_eventset_component(eventSet, 0);
  	if (retval != PAPI_OK) 
  		return handlePAPIError("PAPI_assign_eventset_component");

  	// Check  current multiplex status
   	retval = PAPI_get_multiplex(eventSet);
   	if (retval == 0) 
   		std::cout << "This event set is not enabled for multiplexing" << std::endl;
   	if (retval < 0) 
   		handlePAPIError( "PAPI_get_multiplex " + std::to_string(retval));

 	// Convert the EventSet to a multiplexed event set
  	if (retval == 1) 
  		if (PAPI_set_multiplex(eventSet) != PAPI_OK) 
  			return handlePAPIError("PAPI_set_multiplex");

	return 1;
}



inline int PAPIWrapper::removePapiEvent(std::string eventName)
{
	int eventCode;
	char *eventNameStr = new char[eventName.length() + 1];
	std::strcpy(eventNameStr, eventName.c_str());

	if (PAPI_event_name_to_code(eventNameStr, &eventCode) != PAPI_OK)
	{
		std::cout << eventName << " was not added!" << std::endl;
		return handlePAPIError("PAPI_event_name_to_code");
	}
	else
		removePapiEvent(eventCode);

	return 1;
}


inline int PAPIWrapper::removePapiEvent(int eventCode)
{
	char eventCodeStr[PAPI_MAX_STR_LEN];
	PAPI_event_code_to_name(eventCode, eventCodeStr);


	if (PAPI_remove_event(eventSet, eventCode) != PAPI_OK)
	{
		std::cout << "event: " << eventCode << ", " << eventCodeStr  << " was not not removed " << PAPI_add_event(eventSet, eventCode) << std::endl;;
		return handlePAPIError("PAPI_remove_event");
	}
	
	// Remove from registered papi counters
	for (int i=0; i<registeredEvent.size(); i++)
		if (registeredEvent[i] == std::string(eventCodeStr))
		{
			std::cout << eventCodeStr  << " found and removing" << std::endl;
			registeredEvent.erase(registeredEvent.begin()+i);
			break;
		}

	std::cout << "event: " << eventCode << ", " << eventCodeStr  << " successfully removed!" << std::endl;
	numEvents++;

	
	return 1;
}


inline int PAPIWrapper::addPapiEvent(std::string eventName)
{
	int eventCode;
	char *eventNameStr = new char[eventName.length() + 1];
	std::strcpy(eventNameStr, eventName.c_str());

	if (PAPI_event_name_to_code(eventNameStr, &eventCode) != PAPI_OK)
	{
		std::cout << eventName << " was not added!" << std::endl;
		return handlePAPIError("PAPI_event_name_to_code");
	}
	else
	{
		addPapiEvent(eventCode);
		std::cout << eventName << " was added!!!" << std::endl;
	}

	return 1;
}


inline int PAPIWrapper::addPapiEvent(int eventCode)
{
	char eventCodeStr[PAPI_MAX_STR_LEN];
	PAPI_event_code_to_name(eventCode, eventCodeStr);


	if (PAPI_add_event(eventSet, eventCode) != PAPI_OK)
	{
		std::cout << "event: " << eventCode << ", " << eventCodeStr  << " was not added " << PAPI_add_event(eventSet, eventCode) << std::endl;;
		return handlePAPIError("PAPI_add_event");
	}
	
	
	registeredEvent.push_back( std::string(eventCodeStr) );

	std::cout << "event: " << eventCode << ", " << eventCodeStr  << " successfully added!" << std::endl;
	numEvents++;

	
	return 1;
}


inline int PAPIWrapper::startPapi()
{
	if (PAPI_start(eventSet) != PAPI_OK)
		return handlePAPIError("PAPI_start");

	hwdValues = new long long[numEvents];

	return 1;
}


inline int PAPIWrapper::readEvents()
{
	if (PAPI_read(eventSet, hwdValues) != PAPI_OK)
		return handlePAPIError("PAPI_read");

	return 1;
}


inline int PAPIWrapper::stopReading()
{
	if (PAPI_stop(eventSet, hwdValues) != PAPI_OK)
		return handlePAPIError("PAPI_stop");

	return 1;
}

#endif

} // Namespace Seer
