#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>


extern std::stringstream debugLog;	// Global log for all files

namespace InWrap  
{ 

class Log
{
	std::string outputFilename;

  public:
	std::stringstream log;

	Log(){ outputFilename = "untitled.log"; }
	Log(std::string _outputFilename): outputFilename(_outputFilename){ }
	~Log();

	void setOutputFilename(std::string _outputFilename){ outputFilename = _outputFilename; }
	void clear(){ log.str(""); }
	void writeToDisk();
};


inline Log::~Log()
{
	outputFilename = "";
	log.str("");
}


inline void Log::writeToDisk()
{
	std::ofstream outputFile( outputFilename.c_str(), std::ios::out);
	outputFile << log.str();
	outputFile.close();
}




///////////////////////////////////////////////////////////////////////////////////
///////////// Simple Logging

inline void writeLog(std::string filename, std::string log)
{
	std::ofstream outputFile( (filename+ ".log").c_str(), std::ios::out);
	outputFile << log;
	outputFile.close();
}

inline void writeLogApp(std::string filename, std::string log)
{
	std::ofstream outputFile( (filename+ ".log").c_str(), std::ios::out | std::ios::app);
	outputFile << log;
	outputFile.close();
}

inline void writeLogNew(std::string filename, std::string log)
{
	std::ofstream outputFile( (filename+ ".log").c_str(), std::ios::out);
	outputFile << log;
	outputFile.close();
}

}