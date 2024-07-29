#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

inline void writeLog(std::string filename, std::string log)
{
	std::ofstream outputFile( (filename + ".log").c_str(), std::ios::out);
	outputFile << log;
	outputFile.close();
}

inline void appendLog(std::string filename, std::string log)
{
	std::ofstream outputFile( (filename + ".log").c_str(), std::ios::app);
	outputFile << log;
	outputFile.close();
}