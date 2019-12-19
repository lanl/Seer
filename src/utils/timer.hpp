#pragma once

#include <chrono>
#include <string>
#include <ctime>
#include <unordered_map>
#include <sstream>

namespace InWrap  
{ 

class Timer
{
	std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
	std::chrono::duration<double> elapsed_seconds;
	std::unordered_map< std::string, std::chrono::time_point<std::chrono::system_clock> > timingLog;

  public:

	Timer(){};
	Timer(int x){ start(); };
	~Timer(){};

	void start();
	void stop();
	double stop(int x);

	double getCurrentDuration();			// time in seconds since timer started
	double getDuration();					// time in seconds
	
	static std::string getCurrentTime();	// get the current time
};


inline void Timer::start()
{ 
	startTime = std::chrono::system_clock::now();
}

inline void Timer::stop()
{ 
	endTime = std::chrono::system_clock::now();
	elapsed_seconds = endTime - startTime; 
}

inline double Timer::stop(int x)
{ 
	endTime = std::chrono::system_clock::now();
	elapsed_seconds = endTime - startTime; 

	return elapsed_seconds.count();
}


inline double Timer::getDuration()
{ 
	return elapsed_seconds.count(); 
}

inline double Timer::getCurrentDuration()
{ 
	std::chrono::time_point<std::chrono::system_clock> timeNow;
	timeNow = std::chrono::system_clock::now();
	
	return (timeNow - startTime).count(); 
}


inline std::string Timer::getCurrentTime()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);

	std::stringstream ss;
	ss << "_"<< 1 + ltm->tm_mon << "_" << ltm->tm_mday << "__" << ltm->tm_hour << "_" << ltm->tm_min << "_" << ltm->tm_sec << "_" << std::endl;
	return ss.str();
}

}