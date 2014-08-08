/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/

/**Demonstrates asynchronous audio output.*/
#include <libaudioverse/libaudioverse.h>
#include <libaudioverse/libaudioverse_properties.h>
#include <thread>
#include <chrono>
#include <stdio.h>

#define ERRCHECK(x) do {\
if((x) != Lav_ERROR_NONE) {\
	printf(#x " errored: %i", (x));\
	return;\
}\
} while(0)\

void main() {
	ERRCHECK(Lav_initializeLibrary());
	unsigned int max_outputs = 0;
	ERRCHECK(Lav_getPhysicalOutputCount(&max_outputs));
	printf("%u outputs detected.\n", max_outputs);
	for(unsigned int i = 0; i < max_outputs; i++) {
		printf("\n\n");
		char* name;
		ERRCHECK(Lav_getPhysicalOutputName(i, &name));
		printf("%s:\n", name);
		Lav_free(name);
		float latency = 0.0f;
		ERRCHECK(Lav_getPhysicalOutputLatency(i, &latency));
		printf("Latency: %f\n", latency);
		unsigned int channels = 0;
		ERRCHECK(Lav_getPhysicalOutputChannels(i, &channels));
		printf("channels: %u\n", channels);
	}
}