/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
#include <libaudioverse/libaudioverse.h>
#include <libaudioverse/libaudioverse_properties.h>
#include <libaudioverse/private_devices.hpp>
#include <libaudioverse/private_objects.hpp>
#include <libaudioverse/private_properties.hpp>
#include <libaudioverse/private_macros.hpp>
#include <libaudioverse/private_hrtf.hpp>
#include <libaudioverse/private_memory.hpp>
#include <limits>
#include <functional>
#include <algorithm>
#include <memory>

class LavHrtfObject: public LavObject {
	public:
	LavHrtfObject(std::shared_ptr<LavDevice> device, LavHrtfData* hrtf);
	virtual void process();
	private:
	float *history = nullptr, *left_response = nullptr, *right_response = nullptr;
	LavHrtfData* hrtf = nullptr;
	bool needs_hrtf_recompute;
};

LavHrtfObject::LavHrtfObject(std::shared_ptr<LavDevice> device, LavHrtfData* hrtf): LavObject(Lav_OBJTYPE_HRTF, device, 1, 2) {
	type = Lav_OBJTYPE_HRTF;
	this->hrtf = hrtf;
	left_response = new float[hrtf->getLength()];
	right_response = new float[hrtf->getLength()];
	history = new float[hrtf->getLength() + device->getBlockSize()](); //odd c++ syntax to create 0-initialized array.
	hrtf->computeCoefficientsStereo(0.0f, 0.0f, left_response, right_response);
	auto markRecompute = [this](){needs_hrtf_recompute = true;};
	getProperty(Lav_HRTF_AZIMUTH).setPostChangedCallback(markRecompute);
	getProperty(Lav_HRTF_ELEVATION).setPostChangedCallback(markRecompute);
}

std::shared_ptr<LavObject>createHrtfObject(std::shared_ptr<LavDevice>device, LavHrtfData* hrtf) {
	auto retval = std::make_shared<LavHrtfObject>(device, hrtf);
	device->associateObject(retval);
	return retval;
}

void LavHrtfObject::process() {
	//calculating the hrir is expensive, do it only if needed.
	if(needs_hrtf_recompute) {
		const float elev = getProperty(Lav_HRTF_ELEVATION).getFloatValue();
		const float az = getProperty(Lav_HRTF_AZIMUTH).getFloatValue();
		hrtf->computeCoefficientsStereo(elev, az, left_response, right_response);
		needs_hrtf_recompute = false;
	}
	float *start = history+hrtf->getLength(), *end = history+hrtf->getLength()+device->getBlockSize();
	//get the block size.
	const unsigned int hrtfLength = hrtf->getLength();
	//roll back the history...
	std::copy(end-hrtf->getLength(), end, history);
	//stick our input on the end...
	std::copy(inputs[0], inputs[0]+block_size, start);
	//finally, do the usual convolution loop.
	for(unsigned int i = 0; i < block_size; i++, start++) {
		float outLeft = 0, outRight = 0;
		for(unsigned int j = 0; j < hrtfLength; j++) {
			outLeft += left_response[j]* *(start-j);
			outRight += right_response[j]* *(start-j);
		}
	outputs[0][i] = outLeft;
	outputs[1][i] = outRight;
	}
}

//begin public api

Lav_PUBLIC_FUNCTION LavError Lav_createHrtfObject(LavDevice* device, const char* hrtfPath, LavObject** destination) {
	PUB_BEGIN
	auto hrtf = new LavHrtfData();
	hrtf->loadFromFile(hrtfPath);
	LOCK(*device);
	auto retval = createHrtfObject(incomingPointer<LavDevice>(device), hrtf);
	*destination = outgoingPointer<LavObject>(retval);
	PUB_END
}
