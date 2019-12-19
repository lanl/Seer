#include <time.h>
#include <iostream>
#include <cassert>

#include "papiWrapper.h"

int main(int argc, char* argv[])
{
	PAPIWrapper papiEvent;

	assert( papiEvent.initPapi() );

	papiEvent.addPapiEvent(PAPI_L1_TCM);
	//papiEvent.addPapiEvent(PAPI_TOT_CYC);
	//papiEvent.addPapiEvent(PAPI_LST_INS);

	// Do some computation here
	srand ( time(NULL) );
	unsigned stoppoint = 10 + (rand() % 100);
	for (unsigned counter = 0; counter < stoppoint; counter++)
	{
		papiEvent.startPapi();

		unsigned long fact = 1;
		for (unsigned c = 1; c <= counter; c++)
			fact = c * c;

		std::cout << "\nFactorial of " << counter << " is " << fact << std::endl;

		papiEvent.readEvents();
		for (int i=0; i< papiEvent.getNumEvents(); i++)
			std::cout << papiEvent.getPapiEventName(i) << ": " << papiEvent.getHwdValue(i) << std::endl;

		papiEvent.stopReading();
	}

	return 0;
}


// g++ papiDemo.cpp -I../utils/ -lpapi -o papiTest -D PAPI_ENABLED

// Run
// ./papiTest

