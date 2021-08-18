#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <ctime>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <utility>

#include <mpi.h>

#include "utils/argsParser.hpp"
#include "utils/json.hpp"
#include "utils/log.hpp"
#include "utils/utils.hpp"
#include "utils/timer.hpp"
#include "utils/strConvert.hpp"
#include "utils/mochi.hpp"


#ifdef PAPI_ENABLED	
	#include "utils/papiWrapper.hpp"
#endif

#ifdef CATALYST_ENABLED	
	#include "utils/catalystAdaptor.h"

// VTK Helpers+++
#include "vtkStructures/structuredGrid.h"
#include "vtkStructures/rectilinearGrid.h"
#include "vtkStructures/unstructuredGrid.h"
#include "vtkStructures/polyData.h"
#include "vtkDataStruct.h"

#endif

namespace Seer
{

extern std::stringstream log;
extern std::string logName;

struct eventsUsers
{
	std::map< std::string, std::set<std::string>> eventHashes;

	void insertEventHash(std::string event, std::string hash)
	{
		auto it = eventHashes.find(event);
		if (it != eventHashes.end())
		{
			log << "Found " << event << " inserting: " << hash << ",set size: " << it->second.size() << std::endl; 
			it->second.insert(hash);
		}
		else
		{
			log << "Event Not found " << event << std::endl;
			std::set< std::string> s;
    		s.insert(hash);
			eventHashes.insert( { event, s } );
		} 
	}

	void removeEventHash(std::string event, std::string hash)
	{
		auto it = eventHashes.find(event);
		if (it == eventHashes.end())
		{
			// Nothing to remove here: must be an error
			std::cout << "That key " << event << " does not exist!" << std::endl;
		}
		else
		{
			it->second.erase(hash);
			if (it->second.size() == 0)	// if removing hash makes set empty
				eventHashes.erase(it);
		}
	}

	
	std::vector<std::string> getHashes(std::string event)
	{
		std::vector<std::string> temp;
		temp.resize(0);

		log << "event key: " << event << std::endl;
		
		auto it = eventHashes.find(event);
		if (it != eventHashes.end())
			std::copy(it->second.begin(), it->second.end(), std::back_inserter(temp));
		else
			log << "event key: " << event <<  " not found!" << std::endl;

		return temp;
	}

	void print()
	{
		for (auto mIt=eventHashes.begin(); mIt!=eventHashes.end(); mIt++)
		{
			std::cout << mIt->first << std::endl;
			for (auto sIt=mIt->second.begin(); sIt!=mIt->second.end(); sIt++)
				std::cout << *sIt << " ";
			std::cout << std::endl;
		}
	}
};



class SeerInsituWrap
{
	nlohmann::json jsonInputConfig;	// config file
	nlohmann::json jsonVocab;		// vocab

	bool insitu_on;				// is false if no config file found!

	// Mochi
	bool mochi_on;		
	std::string mochi_database;
	std::string	mochi_address;
	int mochi_multiplex;

	int currentPollingCount;
	int pollIteration;

	MochiInterface mochi;


	// PAPI
	bool papi_on;		// Enable PAPI
	std::vector< std::string > papi_params; 
	std::unordered_map<std::string, long long> perf;	// performance 
  #ifdef PAPI_ENABLED
	PAPIWrapper papiEvent;
  #endif

	bool mpi_profiling_on;		// Enable MPI Profiling

	
	// Catalyst
	bool catalyst_on;	// Enable Catalyst


	// Sensei
	std::string sensei_path;
	bool sensei_on;		// Enable Sensei
	bool veloc_on;		// Enable VeloC



	// Others
	int numRanks;
	int myRank;

	int numTimesteps;
	int currentTimestep;

  #ifdef CATALYST_ENABLED	
	// vtk data
	VTKDataStruct *genericVTK;
  #endif

	// events and users
	eventsUsers eventHash;



	// Private methods
	int parseConfigFile(int argc, char* argv[]);
	int initInSitu(int _myRank, int _numRanks);
	int cleanup();
	
	int recordEvent(std::string name, std::string value);
  	int getEvent(std::string name, std::string &value);
	std::vector<std::string> filterWithPrefix(std::string prefix, std::vector<std::string> allKeys);
	std::vector< std::pair<std::string,std::string> > getMochiKeys(std::string key, std::vector<std::string> keyList);

	void createVTKStruct(std::string strucName);
	void print();

	void readFromMochi();


  public:

	// Sim Events
	std::map<std::string,std::string> simEvents;

  	// Temporary
  #ifdef CATALYST_ENABLED
	CatalystAdaptor cat;
	std::vector<std::string> catalyst_scripts;
	bool catalyst_scripts_changed;
  #endif


  public:
  	SeerInsituWrap();
  	~SeerInsituWrap();

  	int init(int argc, char* argv[], int myRank, int numRanks);
	int init(int _myRank, int _numRanks, MPI_Comm _comm, std::string insituFile);
  	int timestepInit();
  	int timestepExecute(int ts);

  	int isInsituOn(){ return insitu_on; }
	int isMochiOn(){ return mochi_on; }
	int isPapiOn(){ return papi_on; }
  	int isCatalystOn(){ return catalyst_on; }
  	int isSenseiOn(){ return sensei_on; }
  	int isVeloCOn(){ return veloc_on; }
};



inline SeerInsituWrap::SeerInsituWrap()
{
	insitu_on = false;

	// Profiling
	papi_on = false;
	mpi_profiling_on = false;

	// InSitu toolkits
	sensei_on = false;
	catalyst_on = false;
  #ifdef CATALYST_ENABLED
	catalyst_scripts_changed = false;
  #endif
	veloc_on = false;

	// Mochi
	mochi_on = false;
	mochi_database = "";
	mochi_address = "";
	mochi_multiplex = 0;
	currentPollingCount = 0;
	pollIteration = 1; // poll mochi server at every timestep 

	// Others
	myRank = 0;
	numRanks = 0;

	currentTimestep = 0;
	numTimesteps= 0;

	logName = "logs/";
}


inline SeerInsituWrap::~SeerInsituWrap()
{
	if (mochi_on)
		mochi.cleanup();

	perf.clear();
}



inline int SeerInsituWrap::getEvent(std::string name, std::string &value)
{
	if (!insitu_on)
		return 0;


	if (mochi_on)
	{
		int ret = mochi.getValue(name, value);
		return ret;
	}

	return 0;
}

inline int SeerInsituWrap::recordEvent(std::string name, std::string value)
{
	if (!insitu_on)
		return 0;

	if (mochi_on)
		mochi.putKeyValue(name, value);

	return 1;
}


inline std::vector<std::string> SeerInsituWrap::filterWithPrefix(std::string prefix, std::vector<std::string> allKeys)
{
	std::vector<std::string> found;

	for (int i=0; i<allKeys.size(); i++)
	{
		log << "allKeys[i]: " << allKeys[i] << std::endl;
		log << "prefix: " << prefix << std::endl;
		log << "prefix.length(): " << prefix.length() << std::endl;
		if (allKeys[i].compare(0, prefix.length(), prefix) == 0)
			found.push_back(allKeys[i]);
	}

	log << "found.size(): " << found.size() << std::endl;

	return found;
}


inline std::vector< std::pair<std::string,std::string> > SeerInsituWrap::getMochiKeys(std::string key, std::vector<std::string> keyList)
{
	std::vector< std::pair<std::string,std::string> > foundKeyVals;
	foundKeyVals.resize(0);	// making sure that this is empty

	std::vector<std::string> foundKeys = filterWithPrefix(key, keyList);

	for (int i=0; i<foundKeys.size(); i++)
		foundKeyVals.push_back( std::make_pair(foundKeys[i],mochi.getValue(foundKeys[i])) );

	return foundKeyVals;
}



inline int SeerInsituWrap::cleanup()
{
  #ifdef CATALYST_ENABLED
	cat.finalize();
  #endif

	return 1;
}


inline void SeerInsituWrap::createVTKStruct(std::string strucName)
{
  #ifdef CATALYST_ENABLED	
	if (strucName == "structured_grid")
		genericVTK = new StructuredGrid();	
	else if (strucName == "unstructured_grid")
		genericVTK = new UnstructuredGrid();
	else if (strucName == "rectilinear_grid")
		genericVTK = new RectilinearGrid();
  #endif
}


inline void SeerInsituWrap::print()
{
	std::cout << "insitu_on: " << insitu_on << std::endl;
	std::cout << "sensei: " << sensei_on << std::endl;
	std::cout << "mpi profiling: " << mpi_profiling_on << std::endl;
	std::cout << "mochi: " << mochi_on << std::endl;

	std::cout << "Papi: " << papi_on << std::endl;
	for (int i=0; i<papi_params.size(); i++)
		std::cout << papi_params[i] << std::endl;

	for (auto it=perf.begin(); it!=perf.end(); ++it )
		std::cout << it->first << " : " << it->second << std::endl;
}



inline int SeerInsituWrap::initInSitu(int _myRank, int _numRanks)
{
	//
	// Get rank information
	myRank = _myRank;
	numRanks = _numRanks;

	log << "My rank: " << myRank << std::endl;
	log << "Num ranks: " << numRanks << std::endl;

	std::cout << "My rank: " << myRank << std::endl;
	std::cout << "Num ranks: " << numRanks << std::endl;

	//
	// Find log
	if ( jsonInputConfig.contains("log_prefix") )
	{
		
		std::string log_prefix = jsonInputConfig["log_prefix"];
		logName = logName + log_prefix + "_" + std::to_string(myRank);
	}
	else
		logName = logName + "_" + std::to_string(myRank);


	//
	// Find Mochi polling rate
	if ( jsonInputConfig.contains("polling-rate") )
	{
		std::string poll_iteration = jsonInputConfig["polling-rate"];
		pollIteration = Seer::to_int(poll_iteration);
	}
	else
		pollIteration = 1;


	//
	// Get mochi configuration and connect
	if ( jsonInputConfig.contains("mochi-database") )
	{
		std::cout << myRank << "~" << "Mochi-on" << std::endl;
		mochi_on = true;

		// Read in mochi server parameters to connect to it
		if ( jsonInputConfig.find("mochi-database") != jsonInputConfig.end() )
		{
			if ( jsonInputConfig["mochi-database"].find("name") != jsonInputConfig["mochi-database"].end() )
			{
				mochi_database = jsonInputConfig["mochi-database"]["name"];
			}

			if ( jsonInputConfig["mochi-database"].find("address") != jsonInputConfig["mochi-database"].end() )
			{
				mochi_address = jsonInputConfig["mochi-database"]["address"];
				mochi_address += jsonInputConfig["mochi-database"]["port"];
			}

			if ( jsonInputConfig["mochi-database"].find("multiplex") != jsonInputConfig["mochi-database"].end() )
			{
				mochi_multiplex = jsonInputConfig["mochi-database"]["multiplex"];
			}
		}

		std::cout << myRank << "~" << "mochi_database: " << mochi_database << std::endl;
		std::cout << myRank << "~" << "mochi_address: " << mochi_address << std::endl;
		std::cout << myRank << "~" << "mochi_multiplex: " << mochi_multiplex << std::endl;

		// If no mochi info is provided, turn it off!!!
		if ((mochi_database == "" || mochi_address == "") || mochi_multiplex == 0)
		{
			mochi_on = false;
			insitu_on = false;
			std::cout << myRank << "~" << "Could not connect to Mochi, exiting ..." << std::endl;
		}
		else
		{
			log << "mochi on" << std::endl;
			std::cout << myRank << "~" << "Connecting to mochi server ... " << std::endl;
			mochi.init(myRank, mochi_address, mochi_multiplex, mochi_database);
		}

		std::cout << myRank << "~" << "Mochi-on-done" << std::endl;
		
	}

	return (int)mochi_on;
}


inline int SeerInsituWrap::parseConfigFile(int argc, char* argv[])
{
	//
	// Read in  name of insitu config file if it exists
	std::string insituConfigFile;
	bool foundInsituConfig = false;
	for (int i=0; i<argc; i++)
	{
		std::string strArg(argv[i]);
		if (strArg == "--insitu")
		{
			insituConfigFile = std::string(argv[i+1]);
			foundInsituConfig = true;
			break;
		}
	}


	//
	// Quit if there is no config file!
	if (!foundInsituConfig)
	{
		if (myRank == 0)
			std::cerr << "Input insitu configuration file not provided!!!!!!!!!!" << std::endl;

		return 0;
	}


	//
	// Check if input file provided exists
	if ( !fileExisits(insituConfigFile.c_str()) )
	{
		std::cerr << "Could not find input JSON file " << insituConfigFile << "." << std::endl;
		return 0;
	}
	else
	{
		//
		// Check if file is valid
		nlohmann::json jsonInput;
		std::ifstream jsonFile(insituConfigFile);
		insitu_on = true;

		try
		{
			jsonFile >> jsonInput;
		}
		catch (nlohmann::json::parse_error& e)
		{
			insitu_on = false;
			std::cerr << "Input JSON file " << insituConfigFile << " is invalid!\n" 
					  << e.what() << "\n"
					  << "Validate your JSON file using e.g. https://jsonformatter.curiousconcept.com/ " << std::endl;
			return 0;
		}
	}

	//
	// Read in input config JSON file
	std::ifstream jsonFile(insituConfigFile);
	jsonFile >> jsonInputConfig;

	return 1;
}



inline void SeerInsituWrap::readFromMochi()
{
	Timer clock;

	// Key:  NEW_KEY@HASH
	// 		 HASH@Type:Command%count

  clock.start("find_new_keys");
	std::vector<std::string> new_keys_list = mochi.listKeysWithPrefix("NEW_KEY");	// Find the processes who sent requests
  clock.stop("find_new_keys");


	MPI_Barrier(MPI_COMM_WORLD);
	if (myRank == 0)
		std::cout << "readFromMochi ~ new_keys_list.size(): " << new_keys_list.size() << std::endl;


	// For each user, process
	for (int k=0; k<new_keys_list.size(); k++)
	{
	  clock.start("find_keyval");
		std::string key_hash = mochi.getValue(new_keys_list[k]);					// get hash from value and store in key_hash
		std::vector<std::string> userKeys = mochi.listKeysWithPrefix(key_hash);  	// list keys starting with hash

		log << "key: " << new_keys_list[k] << ", value: " << key_hash << std::endl;
		log << "userKeys.size(): " << userKeys.size() << std::endl;

	  clock.stop("find_keyval");


	    std::vector<std::string> processedKeys;		// keys to erase after parsing


		//
		// Add different metrics, scripts, 
		//			... whatever the sim needs ...


	  clock.start("papi-find");
	  #ifdef PAPI_ENABLED
		if (papi_on)
		{
			// Add PAPI counters
			{
				auto foundKeyVals = getMochiKeys(key_hash + "@PAPI:ADD", userKeys);	// Find keys that start with the HASH@PAPI:ADD
				log << "foundKeyVals.size: " << foundKeyVals.size() << std::endl;

				for (int i=0; i<foundKeyVals.size(); i++)
				{
					processedKeys.push_back( foundKeyVals[i].first );
					int status = papiEvent.addPapiEvent(foundKeyVals[i].second);

					log << "foundKeyVals[i].first: " << foundKeyVals[i].first << std::endl;

					if ( !status )
						std::cout << "PAPI:Adding event " << foundKeyVals[i].second << " failed!" << std::endl;
					else
					{
						eventHash.insertEventHash( foundKeyVals[i].second, key_hash );
						log  << " PAPI:Adding event" << foundKeyVals[i].second << std::endl;
					}
						
				}
				
				log << " done adding new papi keys!" << std::endl;
			}

			// Remove PAPI Counters
			{
				auto foundKeyVals = getMochiKeys(key_hash + "@PAPI:DEL", userKeys);

				for (int i=0; i<foundKeyVals.size(); i++)
				{
					processedKeys.push_back( foundKeyVals[i].first );
					int status = papiEvent.removePapiEvent(foundKeyVals[i].second);

					if ( !status )
						std::cout << "PAPI:Remove event: " << foundKeyVals[i].second << " failed!" << std::endl;
					else
					{
						eventHash.insertEventHash( foundKeyVals[i].second, key_hash );
						log << " PAPI:Removed" << foundKeyVals[i].second << std::endl;
					}
				}

				log << " done removing papi keys!" << std::endl;
			}
		}
	  #endif //PAPI_ENABLED
	  clock.stop("papi-find");


	  clock.start("catalyst-find");
	  #ifdef CATALYST_ENABLED
		if (catalyst_on)
		{
			bool catalystChanged = false;

			// Add Catalyst script
			{
				auto foundKeyVals = getMochiKeys(key_hash + "@CATALYST:ADD", userKeys);

				for (int i=0; i<foundKeyVals.size(); i++)
				{
					catalystChanged = true;
					catalyst_scripts_changed = true;

					processedKeys.push_back( foundKeyVals[i].first );

					catalyst_scripts.push_back(foundKeyVals[i].second);	// Add script
					log << " Catalyst: ADDed " << foundKeyVals[i].first << ", " << foundKeyVals[i].second << std::endl;
				}	

				log << " done adding new catalyst scripts" << std::endl;
			}

			// Remove Catalyst script
			{
				auto foundKeyVals = getMochiKeys(key_hash + "@CATALYST:DEL", userKeys);

				for (int i=0; i<foundKeyVals.size(); i++)
				{
					catalystChanged = true;
					catalyst_scripts_changed = true;

					processedKeys.push_back( foundKeyVals[i].first );

					// Remove script
					for (int j=0; j<catalyst_scripts.size(); j++)
						if (catalyst_scripts[j] == std::string(foundKeyVals[i].second))
						{
							catalyst_scripts.erase(catalyst_scripts.begin()+j);
							break;
						}

					log << " Catalyst: Removed " << foundKeyVals[i].first << ", " << foundKeyVals[i].second << std::endl;
				}	

				log << " done removing catalyst scripts" << std::endl;
			}

			// Re-initialize catalyst
			if (catalystChanged)
				cat.init(catalyst_scripts.size(), catalyst_scripts);
		}
	  #endif //CATALYST_ENABLED
	  clock.stop("catalyst-find");


	  clock.start("sim-find");
	  	// Simulation events
		{
			// Add Sim events like timers
			{
				auto foundKeyVals = getMochiKeys(key_hash + "@SIM:ADD", userKeys);

				for (int i=0; i<foundKeyVals.size(); i++)
				{
					std::cout << "SIM:Adding event " << foundKeyVals[i].second << " ... " << std::endl;

					simEvents.insert( std::pair<std::string,std::string>(foundKeyVals[i].second,"") );
					eventHash.insertEventHash( foundKeyVals[i].second, key_hash );
					processedKeys.push_back( foundKeyVals[i].first );

					log  << " SIM:Adding event" << foundKeyVals[i].second << std::endl;
					std::cout << "SIM:Added event " << foundKeyVals[i].second << std::endl;
				}
			}

			// Remove sim events like timers
			{
				auto foundKeyVals = getMochiKeys(key_hash + "@SIM:DEL", userKeys);

				for (int i=0; i<foundKeyVals.size(); i++)
				{
					simEvents.erase( foundKeyVals[i].second );
					eventHash.removeEventHash( foundKeyVals[i].second, key_hash );
					processedKeys.push_back( foundKeyVals[i].first );

					log  << " SIM:Removed event" << foundKeyVals[i].second << std::endl;
				}
			}
		}
	  clock.stop("sim-find");

	  	//
		// End of Reading keys!

		MPI_Barrier(MPI_COMM_WORLD);

		MPI_Barrier(MPI_COMM_WORLD);
		if (myRank == 0)
			std::cout << "End of Reading keys!" << std::endl;

		// Erase the keys that have been processed from the Mochi database
	  clock.start("erase-key");
		if (myRank == 0)
		{
			for (int i=0; i<processedKeys.size(); i++)
				mochi.eraseKey( processedKeys[i] );

			mochi.eraseKey(new_keys_list[k]);
		}
	  clock.stop("erase-key");


	  	MPI_Barrier(MPI_COMM_WORLD);
		if (myRank == 0)
			std::cout << "End of erase keys!" << std::endl;


		log << k << " find_keyval took: " 	<< clock.getDuration("find_keyval") << " s" << std::endl;
		log << k << " papi-find took: " 	<< clock.getDuration("papi-find") << " s" << std::endl;
		log << k << " catalyst-find took: " << clock.getDuration("catalyst-find") << " s" << std::endl;
		log << k << " sim-find took: " 		<< clock.getDuration("papi-find") << " s" << std::endl;
		log << k << " erase-key took: " 	<< clock.getDuration("erase-key") << " s" << std::endl;
	}

	log << "find_new_keys polling took: " 	<< clock.getDuration("find_new_keys") << " s" << std::endl;
}




//
// Core Methods

inline int SeerInsituWrap::init(int argc, char* argv[], int _myRank, int _numRanks)
{
	Timer clock;
	
  clock.start("init");

  clock.start("initialization");

	std::cout << _myRank << "~" << "init..." << std::endl;

	if (!parseConfigFile(argc, argv))
		return 0;

	std::cout << _myRank << "~" << "iiiiiiiiiiiiiiiiiiinit... 1" << std::endl;

	if (!initInSitu( _myRank, _numRanks))
		return 0;

	std::cout << _myRank << "~" << "iiiiiiiiiiiiiiiiinit... 2" << std::endl;
  
  clock.stop("initialization");


	log << "starting SeerInsituWrap::init" << std::endl;

	//
	// Put a value in the keyval storage for testing purposes
	if (myRank == 0)
	{
		std::cout << _myRank << "~" << "iiiiiiiiiiiiiiiiiinit... 4" << std::endl;

		std::string key   = "numRanks";
		std::string value = std::to_string( numRanks );
		//mochi.putKeyValue("00000000@" + key, value);
		mochi.putKeyValue(key, value);

		log << "added numRanks to mochi" << std::endl;
	}

	std::cout << _myRank << "~" << "iiiiiiiiiiiiiiiiiiiiiiinit... 5" << std::endl;

	//
	// Add different metrics, scripts, ... whatever the sim needs
	//

	// PAPI Init
  clock.start("papi");
  
  #ifdef PAPI_ENABLED
	log << "PAPI_ENABLED!" << std::endl;
	if ( jsonInputConfig.contains("papi_counters") )
	{
		papi_on = papiEvent.initPapi();	// Check if papi has been successfully initialized
		if (papi_on)
		{
			for (int i=0; i<jsonInputConfig["papi_counters"].size(); i++)
			{
				std::string papi_counter = jsonInputConfig["papi_counters"][i];
				papiEvent.addPapiEvent( papi_counter );
				eventHash.insertEventHash(papi_counter, "00000000");

				log << "papi event: " << papi_counter << std::endl;
			}
		}
		else
		{
			log << "Could not initialize PAPI" << std::endl;
			std::cout << "Could not initialize PAPI" << std::endl;
		}
	}
  #else
	log << "PAPI_ENABLED not defined!" << std::endl;
  #endif  

  clock.stop("papi");

  std::cout << _myRank << "~" << "iiiiiiiiiiiiiiiiiiiinit... 6" << std::endl;


	// Catalyst
  clock.start("catalyst");

  #ifdef CATALYST_ENABLED
	log << "CATALYST_ENABLED" << std::endl;
	if ( jsonInputConfig.contains("catalyst-scripts") )
	{
		std::cout << "catalyst on" << std::endl;

		for (int i=0; i<jsonInputConfig["catalyst-scripts"].size(); i++)
			if ( isPythonFile( jsonInputConfig["catalyst-scripts"][i] ))
				catalyst_scripts.push_back( jsonInputConfig["catalyst-scripts"][i] );

		cat.init(catalyst_scripts.size(), catalyst_scripts);
		catalyst_on = true;

		std::cout << "catalyst initialized" << std::endl;
		log << "catalyst initialized" << std::endl;
	}
  #else
	log << "CATALYST_ENABLED not defined!" << std::endl;
  #endif

  clock.stop("catalyst");	

	std::cout << _myRank << "~" << "iiiiiiiiiiiiiiiiiiiiiiiiiinit... 7" << std::endl;


	// MPI
	if ( jsonInputConfig.contains("mpi-profiling") )
	{

		MPI_Pcontrol(1);
		log << "mpi profiling on" << std::endl;
	}


	std::cout << _myRank << "~" << "iiiiiiiiiiiiiiiiiiiiiiiiinit... 8" << std::endl;

  clock.stop("init");
	log << "SeerInsituWrap init took: " << clock.getDuration("init") << " s" << std::endl;
	log << "   SeerInsituWrap init initialization took : " << clock.getDuration("initialization") << " s" << std::endl;
	log << "   SeerInsituWrap init papi took : " << clock.getDuration("papi") << " s" << std::endl;
	log << "   SeerInsituWrap init catalyst took : " << clock.getDuration("catalyst") << " s" << std::endl;
	
	//Seer::writeLog(logName, log.str());

	MPI_Barrier(MPI_COMM_WORLD);
	if (myRank == 0) 
		std::cout << "Seer insitu initilized!" << std::endl;

	std::cout << _myRank << "~" << "iiiiiiiiiiiiiiiiiiiiiiiiiiiinit... 9" << std::endl;

	return 1;
}


inline int SeerInsituWrap::init(int _myRank, int _numRanks, MPI_Comm _comm, std::string insituFile)
{
	Timer clock;
	
  clock.start("init");

	std::cout << _myRank << " ~ !!!!!!insitu initiali .... " << insituFile << std::endl; 


  clock.start("initialization");

	//
	// Read in input config JSON file
	std::ifstream jsonFile(insituFile);
	jsonFile >> jsonInputConfig;

	if (!initInSitu( _myRank, _numRanks))
		return 0;

	
  clock.stop("initialization");

	std::cout << _myRank << " ~ !!!!!!insitu initialized" << std::endl; 

	log << "starting SeerInsituWrap::init" << std::endl;

	//
	// Put a value in the keyval storage for testing purposes
	if (myRank == 0)
	{
		std::string key   = "numRanks";
		std::string value = std::to_string( numRanks );
		//mochi.putKeyValue("00000000@" + key, value);
		mochi.putKeyValue(key, value);

		log << "added numRanks to mochi" << std::endl;
	}


	std::cout << _myRank << " ~ !!!!!!insitu initialized" << std::endl; 


	//
	// Add different metrics, scripts, ... whatever the sim needs
	//

	// PAPI Init
  clock.start("papi");
  
  #ifdef PAPI_ENABLED
	log << "PAPI_ENABLED!" << std::endl;
	if ( jsonInputConfig.contains("papi_counters") )
	{
		papi_on = papiEvent.initPapi();	// Check if papi has been successfully initialized
		if (papi_on)
		{
			for (int i=0; i<jsonInputConfig["papi_counters"].size(); i++)
			{
				std::string papi_counter = jsonInputConfig["papi_counters"][i];
				papiEvent.addPapiEvent( papi_counter );
				eventHash.insertEventHash(papi_counter, "00000000");

				log << "papi event: " << papi_counter << std::endl;
			}
		}
		else
		{
			log << "Could not initialize PAPI" << std::endl;
			std::cout << "Could not initialize PAPI" << std::endl;
		}
	}
  #else
	log << "PAPI_ENABLED not defined!" << std::endl;
  #endif  

  clock.stop("papi");


	// Catalyst
  clock.start("catalyst");

  #ifdef CATALYST_ENABLED
	log << "CATALYST_ENABLED" << std::endl;
	if ( jsonInputConfig.contains("catalyst-scripts") )
	{
		std::cout << "catalyst on" << std::endl;

		for (int i=0; i<jsonInputConfig["catalyst-scripts"].size(); i++)
			if ( isPythonFile( jsonInputConfig["catalyst-scripts"][i] ))
				catalyst_scripts.push_back( jsonInputConfig["catalyst-scripts"][i] );

		cat.init(catalyst_scripts.size(), catalyst_scripts);
		catalyst_on = true;

		std::cout << "catalyst initialized" << std::endl;
		log << "catalyst initialized" << std::endl;
	}
  #else
	log << "CATALYST_ENABLED not defined!" << std::endl;
  #endif

  clock.stop("catalyst");	


	// MPI
	if ( jsonInputConfig.contains("mpi-profiling") )
	{
		MPI_Pcontrol(1);
		log << "mpi profiling on" << std::endl;
	}


  clock.stop("init");
	log << "SeerInsituWrap init took: " << clock.getDuration("init") << " s" << std::endl;
	log << "   SeerInsituWrap init initialization took : " << clock.getDuration("initialization") << " s" << std::endl;
	log << "   SeerInsituWrap init papi took : " << clock.getDuration("papi") << " s" << std::endl;
	log << "   SeerInsituWrap init catalyst took : " << clock.getDuration("catalyst") << " s" << std::endl;
	
	//Seer::writeLog(logName, log.str());

	MPI_Barrier(MPI_COMM_WORLD);
	if (myRank == 0) std::cout << "Seer insitu initilized!" << std::endl;

	return 1;
}


inline int SeerInsituWrap::timestepInit()
{
	if (!insitu_on)
		return 0;
	
	Timer clock;
  clock.start("timestepInit");

  clock.start("papi");
  #ifdef PAPI_ENABLED
	if (papi_on)
	{
		perf.clear();
		papiEvent.startPapi();
	}
  #endif
  clock.stop("papi");

	if (mpi_profiling_on)
		MPI_Pcontrol(2);

  clock.stop("timestepInit");

	log << "SeerInsituWrap timestepInit papi took: " << clock.getDuration("papi") << " s" << std::endl;
	log << "SeerInsituWrap timestepInit took: " << clock.getDuration("timestepInit") << " s" << std::endl;

	return 1;
}


inline int SeerInsituWrap::timestepExecute(int ts)
{
	if (!insitu_on)
		return 0;

	Timer clock;
  clock.start("timestepExecute");


	//
	// Send Out events to the Mochi server
	//

  clock.start("mochi_put_data");  

  clock.start("papi");
  #ifdef PAPI_ENABLED
	if (papi_on)
	{	
		papiEvent.readEvents();

		// Get number of events
		for (int e=0; e<papiEvent.getNumEvents(); e++)
		{
			log << "papiEvent.getNumEvents(): " << papiEvent.getNumEvents() << std::endl;
			perf.insert( std::make_pair<std::string, long long>(papiEvent.getPapiEventName(e), papiEvent.getHwdValue(e)) );

			if (mochi_on)
			{
				//std::string key   = papiEvent.getPapiEventName(e) + "_" + std::to_string( myRank );
				std::string key   = papiEvent.getPapiEventName(e) + "#" + std::to_string( myRank ) + "|" + std::to_string(ts);
    			std::string value = std::to_string( papiEvent.getHwdValue(e) );

				log << "key, value from papi: " << key << ":" << value << std::endl;

				// Append user hash before putting in mochi
				std::vector< std::string > userHashes = eventHash.getHashes(papiEvent.getPapiEventName(e));
				for (auto uh=userHashes.begin(); uh!=userHashes.end(); uh++)
				{
					mochi.putKeyValue(*uh + "@" + key, value);
					log << "added to mochi: " << *uh << "@" << key << ":" << value << std::endl;
				}
			}

		}

		papiEvent.stopReading();
	}
  #endif
	clock.stop("papi");

 
	//
	// Sim events
	MPI_Barrier(MPI_COMM_WORLD);
	if (myRank == 0)
		std::cout << "simEvents.size(): " << simEvents.size() << std::endl;
				

	for (auto it=simEvents.begin(); it!=simEvents.end(); ++it)
	{
		MPI_Barrier(MPI_COMM_WORLD);
		if (myRank == 0)
			std::cout << "simEvents it->first: " << it->first << ", " << simEvents[it->first] << std::endl;

		//std::string key = it->first + "_" + std::to_string( myRank );
		std::string key = it->first + "#" + std::to_string( myRank ) + "|" + std::to_string(ts);
		std::string value = simEvents[it->first];

		log << "key, value from sim: " << key << ":" << value << std::endl;

		// Append user hash before putting in mochi
		std::vector< std::string > userHashes = eventHash.getHashes(it->first);
		for (auto uh=userHashes.begin(); uh!=userHashes.end(); uh++)
		{
			mochi.putKeyValue(*uh + "@" + key, value);
			log << "added to mochi: " << *uh << "@" << key << ":" << value << std::endl;
		}
	}



	if (mochi_on && myRank == 0)
	{
		std::string key   = "current_timestep";
    	std::string value = std::to_string( ts );
		//mochi.putKeyValue("00000000@" + key, value);
		mochi.putKeyValue(key, value);
		log << "added to mochi: " << "00000000@" << key << ":" << value << std::endl;
	}

  clock.stop("mochi_put_data");


	if (mpi_profiling_on)
		MPI_Pcontrol(3);

	
	//
	// Read in new values from the mochi server: Mochi listener and parser
	//
  clock.start("mochi_gather");
	if (mochi_on)
	{
		if (currentPollingCount == pollIteration-1)
		{
			currentPollingCount = 0;
			readFromMochi();
		}
		else
			currentPollingCount++;
	}
  clock.stop("mochi_gather");

	currentTimestep++;


	MPI_Barrier(MPI_COMM_WORLD);
		if (myRank == 0)
			std::cout << "End of timestepExecute!" << std::endl;


  clock.stop("timestepExecute");

	log << "SeerInsituWrap timestepExecute took: " << clock.getDuration("timestepExecute") << " s" << std::endl;
	log << " SeerInsituWrap timestepExecute papi took: " << clock.getDuration("papi") << " s" << std::endl;
	log << " SeerInsituWrap timestepExecute: mochi put data took: " << clock.getDuration("mochi_put_data") << " s" << std::endl;
	log << " SeerInsituWrap timestepExecute: mochi gather data took: " << clock.getDuration("mochi_gather") << " s" << std::endl;
  
	return 1;
}


} //Namespace Seer