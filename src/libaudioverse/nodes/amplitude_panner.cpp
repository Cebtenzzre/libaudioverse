/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
#include <libaudioverse/libaudioverse.h>
#include <libaudioverse/libaudioverse_properties.h>
#include <libaudioverse/private/simulation.hpp>
#include <libaudioverse/private/node.hpp>
#include <libaudioverse/private/properties.hpp>
#include <libaudioverse/private/macros.hpp>
#include <libaudioverse/private/memory.hpp>
#include <libaudioverse/implementations/panner.hpp>
#include <libaudioverse/nodes/panner.hpp>
#include <libaudioverse/private/constants.hpp>
#include <libaudioverse/private/kernels.hpp>
#include <limits>
#include <memory>
#include <algorithm>
#include <utility>
#include <vector>

namespace libaudioverse_implementation {

float standard_map_stereo[] = {-90.0f, 90.0f};
float standard_map_51[] = {-22.5f, 22.5f, -110.0f, 110.0f};
float standard_map_71[] = {-22.5f, 22.5f, -150.0f, 150.0f, -110.0f, 110.0f};

//This class needs to be public because of the multipanner, which needs to make a method call against it directly.
//see include/libaudioverse/objects/panner.hpp.

AmplitudePannerNode::AmplitudePannerNode(std::shared_ptr<Simulation> simulation): Node(Lav_OBJTYPE_AMPLITUDE_PANNER_NODE, simulation, 1, 0) {
	appendInputConnection(0, 1);
	appendOutputConnection(0, 0);
}

std::shared_ptr<Node>createAmplitudePannerNode(std::shared_ptr<Simulation> simulation) {
	auto retval = std::shared_ptr<AmplitudePannerNode>(new AmplitudePannerNode(simulation), ObjectDeleter(simulation));
	simulation->associateNode(retval);
	//needed because the inputs/outputs logic needs shared_from_this to be working.
	retval->recomputeChannelMap();
	return retval;
}

void AmplitudePannerNode::recomputeChannelMap() {
	panner.reset();
	bool skipLfe = getProperty(Lav_PANNER_SKIP_LFE).getIntValue() == 1;
	bool skipCenter = getProperty(Lav_PANNER_SKIP_CENTER).getIntValue() == 1;
	Property& channelMap = getProperty(Lav_PANNER_CHANNEL_MAP);
	unsigned int max = channelMap.getFloatArrayLength();
	if(skipLfe && channelMap.getFloatArrayLength() > 2) max++;
	if(skipCenter && channelMap.getFloatArrayLength() > 2) max++;
	resize(1, max);
	getOutputConnection(0)->reconfigure(0, max);
	unsigned int index = 0;
	for(unsigned int i = 0; i < max; i++) {
		if(i == 2 && skipCenter) continue;
		if(i == 3 && skipLfe) continue;
		float angle = channelMap.readFloatArray(index);
		panner.addEntry(angle, i);
		index++;
	}
}

void AmplitudePannerNode::willProcessParents() {
	if(werePropertiesModified(this, Lav_PANNER_CHANNEL_MAP, Lav_PANNER_SKIP_CENTER, Lav_PANNER_SKIP_LFE)) recomputeChannelMap();
}

void AmplitudePannerNode::process() {
	float azimuth = getProperty(Lav_PANNER_AZIMUTH).getFloatValue();
	float passthrough =getProperty(Lav_PANNER_PASSTHROUGH).getFloatValue();
	bool skipCenter=getProperty(Lav_PANNER_SKIP_CENTER).getIntValue() == 1;
	bool skipLfe=getProperty(Lav_PANNER_SKIP_LFE).getIntValue() == 1;
	panner.pan(azimuth, block_size, input_buffers[0], num_output_buffers, &output_buffers[0]);
	if(passthrough != 0.0f) {
		scalarMultiplicationKernel(block_size, passthrough, input_buffers[0], input_buffers[0]);
		for(int i = 0; i < num_output_buffers; i++) {
			if(i ==2 && skipCenter) continue;
			if(i==3 && skipLfe) continue;
			scalarMultiplicationKernel(block_size, 1.0f-passthrough, output_buffers[i], output_buffers[i]);
			additionKernel(block_size, input_buffers[0], output_buffers[i], output_buffers[i]);
		}
	}
}

void AmplitudePannerNode::configureStandardChannelMap(unsigned int channels) {
	switch(channels) {
		case Lav_PANNING_STRATEGY_STEREO:
		getProperty(Lav_PANNER_CHANNEL_MAP).replaceFloatArray(2, standard_map_stereo);
		break;
		case Lav_PANNING_STRATEGY_SURROUND51:
		getProperty(Lav_PANNER_CHANNEL_MAP).replaceFloatArray(4, standard_map_51);
		break;
		case Lav_PANNING_STRATEGY_SURROUND71:
		getProperty(Lav_PANNER_CHANNEL_MAP).replaceFloatArray(6, standard_map_71);
		break;
	};
	getProperty(Lav_PANNER_SKIP_LFE).setIntValue(1);
	getProperty(Lav_PANNER_SKIP_CENTER).setIntValue(1);
}

//begin public api

Lav_PUBLIC_FUNCTION LavError Lav_createAmplitudePannerNode(LavHandle simulationHandle, LavHandle* destination) {
	PUB_BEGIN
	auto simulation = incomingObject<Simulation>(simulationHandle);
	LOCK(*simulation);
	auto retval = createAmplitudePannerNode(simulation);
	*destination = outgoingObject(retval);
	PUB_END
}

Lav_PUBLIC_FUNCTION LavError Lav_amplitudePannerNodeConfigureStandardMap(LavHandle nodeHandle, unsigned int channels) {
	PUB_BEGIN
	if(channels != 2 && channels != 6 && channels != 8) throw LavErrorException(Lav_ERROR_RANGE);
	auto node= incomingObject<Node>(nodeHandle);
	LOCK(*node);
	if(node->getType() != Lav_OBJTYPE_AMPLITUDE_PANNER_NODE) throw LavErrorException(Lav_ERROR_TYPE_MISMATCH);
	std::static_pointer_cast<AmplitudePannerNode>(node)->configureStandardChannelMap(channels);
	PUB_END
}

}