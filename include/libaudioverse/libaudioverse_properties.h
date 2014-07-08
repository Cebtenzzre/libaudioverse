/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
#pragma once
#include "libaudioverse.h"
#ifdef __cplusplus
extern "C" {
#endif

/**This file contains the property enums for Libaudioverse.

It is worth keeping separate because it will grow rapidly and contain documentation comments and etc.

The standard for naming is:
Lav_OBJTYPE_PROPNAME

Furthermore, all libaudioverse properties are negative, save on those objects for which documentation says otherwise.  Examples of objects with positive properties include highly configurable mixers and attenuators, among others.
*/

enum lav_STANDARD_PROPERTIES {
	Lav_OBJECT_SUSPENDED = -100,
};

enum Lav_SINE_PROPERTIES {
	Lav_SINE_FREQUENCY = -1,
};

enum Lav_FILE_PROPERTIES {
	Lav_FILE_POSITION = -1,
	Lav_FILE_PITCH_BEND = -2,
};

enum Lav_HRTF_PROPERTIES {
	Lav_HRTF_AZIMUTH = -1,
	Lav_HRTF_ELEVATION = -2,
};

enum Lav_ATTENUATOR_PROPERTIES {
	Lav_ATTENUATOR_MULTIPLIER = -1,
};

enum Lav_MIXER_PROPERTIES {
	Lav_MIXER_MAX_PARENTS = -1,
	Lav_MIXER_INPUTS_PER_PARENT = -2,
};

enum Lav_DELAY_PROPERTIES {
	Lav_DELAY_DELAY = -1,
	Lav_DELAY_DELAY_MAX = -2,
	Lav_DELAY_FEEDBACK = -3,
};

#ifdef __cplusplus
}
#endif