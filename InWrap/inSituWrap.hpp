#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <ctime>
#include <sstream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <mpi.h>
#include <utility>


#include "utils/argsParser.hpp"
#include "utils/json.hpp"
#include "utils/log.hpp"
#include "utils/utils.hpp"
#include "utils/timer.hpp"


#ifdef INSITU_ON

#ifdef MOCHI_ENABLED
	#include "utils/mochi.hpp"
#endif

#ifdef PAPI_ENABLED	
	#include "utils/papiWrapper.hpp"
#endif

#ifdef CATALYST_ENABLED	
	#include "utils/catalystAdaptor.h"
#endif

// VTK Helpers+++
#include "vtkStructures/structuredGrid.h"
#include "vtkStructures/rectilinearGrid.h"
#include "vtkStructures/unstructuredGrid.h"
#include "vtkStructures/polyData.h"
#include "vtkDataStruct.h"


namespace InWrap
{


class InsituWrap
{
	bool insitu_on;
	std::stringstream log;		// logging
	nlohmann::json jsonInput;	// json

	int numRanks;
	int myRank;
	int numTimesteps;

	int currentTimestep;


	// Mochi
	bool mochi_on;		
	std::string mochi_database;
	std::string	mochi_address;
	int mochi_multiplex;
  #ifdef MOCHI_ENABLED
	MochiInterface mochi;
  #endif
		

	// PAPI
	bool papi_on;		// Enable PAPI
	std::vector< std::string > papi_params; 
	std::unordered_map<std::string, long long> perf;	// performance 
  #ifdef PAPI_ENABLED
	PAPIWrapper papiEvent;
  #endif

	// Catalyst
	bool catalyst_on;	// Enable Catalyst
	std::vector<std::string> catalyst_scripts;


	bool tau_on;				// Enable TAU
	bool mpi_profiling_on;		// Enable MPI Profiling

	std::string sensei_path;
	bool sensei_on;		// Enable Sensei
	bool veloc_on;		// Enable VeloC


	// vtk
	VTKDataStruct *genericVTK;

	// Events to record in sim
	std::vector<std::string> events;	// non-papi event 

  public:
  	#ifdef CATALYST_ENABLED
		CatalystAdaptor cat;
  	#endif

  public:
  	InsituWrap();
  	~InsituWrap();

  	int init(int argc, char* argv[], int myRank, int numRanks);
  	int cleanup();
  	void print();

  	int timestepInit();
  	int timestepExecute(int ts);

  	int recordEvent(std::string name, std::string value);
  	int getEvent(std::string name, std::string &value);

  	int isInsituOn(){ return insitu_on; }
  	int isCatalystOn(){ return catalyst_on; }
  	int isSenseiOn(){ return sensei_on; }
  	int isVeloCOn(){ return veloc_on; }
  	int isMochiOn(){ return mochi_on; }

  	void createVTKStruct(std::string strucName);
};



inline InsituWrap::InsituWrap()
{
	insitu_on = false;

	tau_on = false;
	papi_on = false;

	mpi_profiling_on = false;

	sensei_on = false;
	catalyst_on = false;
	veloc_on = false;

	mochi_on = false;
	mochi_database = "";
	mochi_address = "";
	mochi_multiplex = 0;

	myRank = 0;
	numRanks = 0;
	numTimesteps= 0;

	currentTimestep = 0;
}


inline InsituWrap::~InsituWrap()
{
  #ifdef MOCHI_ENABLED
	if (mochi_on)
		mochi.cleanup();
  #endif

	perf.clear();
}


inline void InsituWrap::createVTKStruct(std::string strucName)
{
	if (strucName == "structured_grid")
		genericVTK = new StructuredGrid();	
	else if (strucName == "unstructured_grid")
		genericVTK = new UnstructuredGrid();
	else if (strucName == "rectilinear_grid")
		genericVTK = new RectilinearGrid();
}


inline void InsituWrap::print()
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



inline int InsituWrap::init(int argc, char* argv[], int _myRank, int _numRanks)
{
	//
	// initialize insitu
	myRank = _myRank;
	numRanks = _numRanks;

	log << "My rank: " << myRank << std::endl;
	log << "Num ranks: " << numRanks << std::endl;

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
	// Read in input
	std::ifstream jsonFile(insituConfigFile);
	jsonFile >> jsonInput;


	//
	// Process JSON input file
	if ( jsonInput.find("sensei") != jsonInput.end() )
	{
		sensei_on = jsonInput["sensei"];
		log << "sensei on" << std::endl;
	}


	if ( jsonInput.find("events-to-record") != jsonInput.end() )
		for (int i=0; i<jsonInput["events-to-record"].size(); i++)
		{
			events.push_back( jsonInput["events-to-record"][i] );
			log << "event: " << jsonInput["events-to-record"][i] << std::endl;
		}



	// Papi
  #ifdef PAPI_ENABLED
	if ( jsonInput.find("papi") != jsonInput.end() )
	{
		papi_on = jsonInput["papi"];

		if (papi_on)
		{
			papi_on = papiEvent.initPapi();

			for (int i=0; i<jsonInput["papi_counters"].size(); i++)
			{
				std::string papi_counter = jsonInput["papi_counters"][i];
				papiEvent.addPapiEvent( papi_counter );

				log << "papi event: " << papi_counter << std::endl;
			}
		}
	} 
  #endif  


	// Mochi
  #if MOCHI_ENABLED
	if ( jsonInput.find("mochi") != jsonInput.end() )
	{
		mochi_on = jsonInput["mochi"];

		// Read in mochi server parameters to connect to it
		if (mochi_on)
		{
			if ( jsonInput.find("mochi-database") != jsonInput.end() )
			{
				if ( jsonInput["mochi-database"].find("name") != jsonInput["mochi-database"].end() )
					mochi_database = jsonInput["mochi-database"]["name"];

				if ( jsonInput["mochi-database"].find("address") != jsonInput["mochi-database"].end() )
				{
					mochi_address = jsonInput["mochi-database"]["address"];
					mochi_address += jsonInput["mochi-database"]["port"];
				}

				if ( jsonInput["mochi-database"].find("multiplex") != jsonInput["mochi-database"].end() )
					mochi_multiplex = jsonInput["mochi-database"]["multiplex"];
			}

			// If no mochi info is provided, turn it off!!!
			if ((mochi_database == "" || mochi_address == "") || mochi_multiplex == 0)
				mochi_on = false;
		}


		// push some values to the mochi server
		if (mochi_on)
		{
			log << "mochi on" << std::endl;
			mochi.init(mochi_address, mochi_multiplex, mochi_database);

			if (myRank == 0)
			{
				std::string key   = "numRanks";
				std::string value = std::to_string( numRanks );
				mochi.putKeyValue(key, value);

			   #ifdef PAPI_ENABLED
				if (papi_on)
				{
					key   = "num_papi_counters";
					value = std::to_string( papiEvent.getNumEvents() );
					mochi.putKeyValue(key, value);

					for (int e=0; e<papiEvent.getNumEvents(); e++)
					{ 
						key   = "papi_counter_" + std::to_string( e );
						value = papiEvent.getPapiEventName(e);
						mochi.putKeyValue(key, value);
					}
				}
			  #endif

				int keyExists = mochi.existsKey("numRanks");
				//std::cout << "!!!!!!!!!!!!!!!!!!mochi.existsKey(numRanks) "<< keyExists << std::endl;
			}
		}
	}
  #endif


	// Catalyst
  #ifdef CATALYST_ENABLED
	if ( jsonInput.find("catalyst") != jsonInput.end() )
	{
		catalyst_on = jsonInput["catalyst"];
		std::cout << "catalyst on" << std::endl;

		for (int i=0; i<jsonInput["catalyst-scripts"].size(); i++)
		{
			if ( isPythonFile( jsonInput["catalyst-scripts"][i] ))
			{
				catalyst_scripts.push_back( jsonInput["catalyst-scripts"][i] );
			}
		}

		if (catalyst_on)
		{
			cat.init(catalyst_scripts.size(), catalyst_scripts);
			std::cout << "catalyst initiated" << std::endl;
		}
		else
		{
			std::cout << "catalyst NOT ON!!!" << std::endl;
		}
	}

  #endif


	// MPI
	if ( jsonInput.find("mpi-profiling") != jsonInput.end() )
	{
		mpi_profiling_on  = jsonInput["mpi-profiling"];
		if (mpi_profiling_on)
		{
			MPI_Pcontrol(1);
			log << "mpi profiling on" << std::endl;
		}
	}

	return 1;
}




inline int InsituWrap::timestepInit()
{
	if (!insitu_on)
		return 0;


  #ifdef PAPI_ENABLED
	if (papi_on)
	{
		perf.clear();
		papiEvent.startPapi();
	}
  #endif

	if (mpi_profiling_on)
		MPI_Pcontrol(2);

	return 1;
}



inline int InsituWrap::timestepExecute(int ts)
{
	if (!insitu_on)
		return 0;

  #ifdef PAPI_ENABLED
	if (papi_on)
	{
		//std::cout << "papiEvent.getNumEvents():" << papiEvent.getNumEvents() << std::endl;
		papiEvent.readEvents();

		for (int e=0; e<papiEvent.getNumEvents(); e++)
		{
			perf.insert( std::make_pair<std::string, long long>(papiEvent.getPapiEventName(e), papiEvent.getHwdValue(e)) );
			//std::cout << "papiEvent.getPapiEventName(e): " << papiEvent.getPapiEventName(e) << std::endl;

		  #ifdef MOCHI_ENABLED
			if (mochi_on)
			{
				std::string key   = papiEvent.getPapiEventName(e) + "_" + std::to_string( myRank );
    			std::string value = std::to_string( papiEvent.getHwdValue(e) );
   				mochi.putKeyValue(key, value);

				std::cout << myRank << " ~ " << ts << " : mochi on, papi: " << key << ", " << value << std::endl;
			}
		  #endif
		}

		papiEvent.stopReading();
	}
  #endif


  #ifdef MOCHI_ENABLED
	if (mochi_on && myRank == 0)
	{
		std::string key   = "current_timestep";
    	std::string value = std::to_string( ts );
		mochi.putKeyValue(key, value);

		std::cout << myRank << " ~ " << ts << " output current ts " << std::endl;
	}
  #endif


 //  #ifdef CATALYST_ENABLED
	// if (catalyst_on)
	// {
	// 	std::cout << myRank << " ~ " << ts << " copreocessing catalyst.... " << std::endl;
	// 	cat.coProcess(genericVTK->getGrid(), ts / 1.0, ts, ts == (numTimesteps - 1));
	// 	std::cout << myRank << " ~ " << ts << " ... copreocessing catalyst!" << std::endl;
	// }
 //  #endif

	if (mpi_profiling_on)
		MPI_Pcontrol(3);

	

	// Read in new values
  #ifdef MOCHI_ENABLED

	if ( mochi.existsKey("NEW_KEY") )
	{
		std::cout << myRank << " ~ " << ts << " NEW_KEY  detected " << std::endl;
		std::vector<std::string> keysInDB;
  	  #ifdef PAPI_ENABLED

		// Add Papi counters
		{
			std::vector<std::string> foundKeys = mochi.listKeysWithPrefix("ADD_PAPI");

			if (foundKeys.size() > 0)
			{
				std::cout << myRank << " ~ " << ts << "ADD_PAPI  found " << foundKeys.size() << std::endl;

				std::vector<std::string> foundVals;
				for (int i=0; i<foundKeys.size(); i++)
				{
					std::cout << myRank << " ~ " << ts << " ADD foundKeys[i]" << foundKeys[i] << std::endl;
					foundVals.push_back( mochi.getValue(foundKeys[i]) );
					keysInDB.push_back(foundKeys[i]);
				}


				for (int i=0; i<foundVals.size(); i++)
				{
					if ( !papiEvent.addPapiEvent(foundVals[i]) )
					{
	 					std::cout << "Adding event " << foundVals[i] << " failed!" << std::endl;
					}
					else
					{
						std::cout << myRank << " ~ " << ts << " ADDed" << foundVals[i] << std::endl;
						mochi.putKeyValue("num_papi_counters", std::to_string(papiEvent.getNumEvents()));
					}

					// Remove that key
					//mochi.eraseKey( foundKeys[i] );
				}
			}

			std::cout << myRank << " ~ " << ts << " done adding new papi keys " << std::endl;
		}


		// Remove Papi counters
		{
			std::vector<std::string> foundKeys = mochi.listKeysWithPrefix("REMOVE_PAPI");

			if (foundKeys.size() > 0)
			{
				std::cout << myRank << " ~ " << ts << " REMOVE_PAPI  found " << std::endl;

				std::vector<std::string> foundVals;
				for (int i=0; i<foundKeys.size(); i++)
				{
					std::cout << myRank << " ~ " << ts << " REMOVE foundKeys[i]" << foundKeys[i] << std::endl;
					foundVals.push_back( mochi.getValue(foundKeys[i]) );
					keysInDB.push_back(foundKeys[i]);
				}


				for (int i=0; i<foundVals.size(); i++)
				{
					if ( !papiEvent.removePapiEvent(foundVals[i]) )
					{
	 					std::cout << "Remove event " << foundVals[i] << " failed!" << std::endl;
					}
					else
					{
						std::cout << myRank << " ~ " << ts << " Removed" << foundVals[i] << std::endl;
						mochi.putKeyValue("num_papi_counters", std::to_string(papiEvent.getNumEvents()));
					}

					// Remove that key
					//mochi.eraseKey( foundKeys[i] );
				}
			}

			std::cout << myRank << " ~ " << ts << " done removing papi keys " << std::endl;
		}

		// 
		for (int e=0; e<papiEvent.getNumEvents(); e++)
		{ 
			std::string key   = "papi_counter_" + std::to_string( e );
			std::string value = papiEvent.getPapiEventName(e);
			mochi.putKeyValue(key, value);
		}
  	  #endif //PAPI_ENABLED


	  #ifdef CATALYST_ENABLED
		//catalyst_scripts
		if (catalyst_on)
		{
			// Add Catalyst script
			{
				std::vector<std::string> foundKeys = mochi.listKeysWithPrefix("ADD_CATALYST_SCRIPT");

				if (foundKeys.size() > 0)
				{
					std::cout << myRank << " ~ " << ts << "ADD_CATALYST_SCRIPT  found " << foundKeys.size() << std::endl;

					std::vector<std::string> foundVals;
					for (int i=0; i<foundKeys.size(); i++)
					{
						std::cout << myRank << " ~ " << ts << " ADD foundKeys[i]" << foundKeys[i] << std::endl;
						foundVals.push_back( mochi.getValue(foundKeys[i]) );
						keysInDB.push_back(foundKeys[i]);
					}


					for (int i=0; i<foundVals.size(); i++)
					{
						std::cout << myRank << " ~ " << ts << " ADDed foundVals[i]" << foundVals[i] << std::endl;
						catalyst_scripts.push_back(foundVals[i]);	// Add script
						//mochi.eraseKey( foundKeys[i] );				// Remove that key
					}
				}

				std::cout << myRank << " ~ " << ts << " done adding new catalyst " << std::endl;
			}


			// Remove Catalyst script
			{
				std::vector<std::string> foundKeys = mochi.listKeysWithPrefix("REMOVE_CATALYST_SCRIPT");

				if (foundKeys.size() > 0)
				{
					std::vector<std::string> foundVals;
					for (int i=0; i<foundKeys.size(); i++)
					{
						foundVals.push_back( mochi.getValue(foundKeys[i]) );
						keysInDB.push_back(foundKeys[i]);
					}




					for (int i=0; i<foundVals.size(); i++)
					{
						// Remove script
						for (int j=0; j<catalyst_scripts.size(); j++)
							if (catalyst_scripts[j] == std::string(foundVals[i]))
							{
								 catalyst_scripts.erase(catalyst_scripts.begin()+j);
								 break;
							}

						//mochi.eraseKey( foundKeys[i] );				// Remove that key
					}
				}
			}

			// Reinitialize catalyst
			//std::cout << "cat.finalize()" << std::endl;
			//cat.finalize();
			std::cout << "cat.init()" << std::endl;
			cat.init(catalyst_scripts.size(), catalyst_scripts);
			std::cout << "cat.init() done!!!" << std::endl;
		}
      #endif //CATALYST_ENABLED

		MPI_Barrier(MPI_COMM_WORLD);
		if (myRank == 0)
		{
			for (int i=0; i<keysInDB.size(); i++)
				mochi.eraseKey( keysInDB[i] );

			mochi.eraseKey("NEW_KEY");
		}
	}

  #endif //MOCHI_ENABLED
  

	currentTimestep++;
  
	return 1;
}





inline int InsituWrap::cleanup()
{
  #ifdef CATALYST_ENABLED
	cat.finalize();
  #endif
	return 1;
}



inline int InsituWrap::getEvent(std::string name, std::string &value)
{
	if (!insitu_on)
		return 0;

  #ifdef MOCHI_ENABLED
	if (mochi_on)
	{
		int ret = mochi.getValue(name, value);
		return ret;
	}
  #endif

	return 0;
}

inline int InsituWrap::recordEvent(std::string name, std::string value)
{
	if (!insitu_on)
		return 0;

  #ifdef MOCHI_ENABLED
	if (mochi_on)
		mochi.putKeyValue(name, value);
  #endif

	return 1;
}


} //Inwrap


#endif