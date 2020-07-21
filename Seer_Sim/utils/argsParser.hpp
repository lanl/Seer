#pragma once

#include <vector>
#include <string>

namespace Seer  
{ 

std::vector <std::string> arguments;

inline int parseArgs(int numArgs, char *argv[])
{

	arguments.clear();
	for (int i=0; i<numArgs; i++)
		arguments.push_back( std::string( argv[i] ) );

	return numArgs;
}


// -1 if not found; pos if found
inline int findArg(std::string argument, int startPos=0)
{
	for (int i=startPos; i<arguments.size(); i++)
		if (arguments[i] == argument)
			return i;	

	return 0;
}


inline int getNumMatchingArgs(std::string argument)
{
	int argCount = 0;
	for (int i=0; i<arguments.size(); i++)
	{
		//std::cout << arguments[i] << std::endl;
		if (arguments[i] == argument)
			argCount++;
	}

	return argCount;
}


inline int getArgument(int argPos, std::string & argValue)
{
	if (argPos > arguments.size())
		return 0;
	else
		argValue = arguments[argPos];

	return 1;
}


}
