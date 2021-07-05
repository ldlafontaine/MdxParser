#include "Layer.h"

#include <vector>
#include <sstream>
#include <limits>

using namespace MdxParser;

Layer::Layer() :
	inclusiveSize{ 0 },
	alpha{ 1 },
	coordId{ 0 },
	emissiveGain{ 0 },
	filterMode{ 0 },
	fresnelColor{ 0 },
	fresnelOpacity{ 0 },
	fresnelTeamColor{ 0 },
	shadingFlags{ 0 },
	textureAnimationId{ std::numeric_limits<uint32_t>::max() },
	textureId{ 0 } {
}

Layer::Layer(std::istream& stream, uint32_t version) : Layer() {

	std::streampos streamStartPosition{ stream.tellg() };

	stream.read((char*)&inclusiveSize, sizeof(inclusiveSize));
	stream.read((char*)&filterMode, sizeof(filterMode));
	stream.read((char*)&shadingFlags, sizeof(shadingFlags));
	stream.read((char*)&textureId, sizeof(textureId));
	stream.read((char*)&textureAnimationId, sizeof(textureAnimationId));
	stream.read((char*)&coordId, sizeof(coordId));
	stream.read((char*)&alpha, sizeof(alpha));

	if (version > 800) {
		stream.read((char*)&emissiveGain, sizeof(emissiveGain));
		stream.read((char*)&fresnelColor, sizeof(fresnelColor));
		stream.read((char*)&fresnelOpacity, sizeof(fresnelOpacity));
		stream.read((char*)&fresnelTeamColor, sizeof(fresnelTeamColor));
	}

	std::streampos streamEndPosition{ static_cast<unsigned int>(streamStartPosition) + inclusiveSize };

	while (stream.tellg() < streamEndPosition) {

		// Read Tag
		std::vector<char> tagBuffer(4);
		stream.read(reinterpret_cast<char*>(tagBuffer.data()), 4);
		std::string tag{ tagBuffer.begin(), tagBuffer.end() };

		if (tag == "KMTF") {
			textureIdTracks = TracksChunk<uint32_t>{ tag, stream };
		}
		else if (tag == "KMTA") {
			alphaTracks = TracksChunk<float>{ tag, stream };
		}
		else if (tag == "KMTE") {
			emissiveGainTracks = TracksChunk<float>{ tag, stream };
		}
		else if (tag == "KFC3") {
			fresnelColorTracks = TracksChunk<std::array<float, 3>>{ tag, stream };
		}
		else if (tag == "KFCA") {
			fresnelAlphaTracks = TracksChunk<float>{ tag, stream };
		}
		else if (tag == "KFTC") {
			fresnelTeamColorTracks = TracksChunk<float>{ tag, stream };
		}
	}

	stream.seekg(streamEndPosition);
}

void Layer::write(std::ostream& stream, uint32_t version) {
	std::streampos streamStartPosition{ stream.tellp() };

	stream.write((char*)&inclusiveSize, sizeof(inclusiveSize));
	stream.write((char*)&filterMode, sizeof(filterMode));
	stream.write((char*)&shadingFlags, sizeof(shadingFlags));
	stream.write((char*)&textureId, sizeof(textureId));
	stream.write((char*)&textureAnimationId, sizeof(textureAnimationId));
	stream.write((char*)&coordId, sizeof(coordId));
	stream.write((char*)&alpha, sizeof(alpha));

	if (version > 800) {
		stream.write((char*)&emissiveGain, sizeof(emissiveGain));
		stream.write((char*)&fresnelColor, sizeof(fresnelColor));
		stream.write((char*)&fresnelOpacity, sizeof(fresnelOpacity));
		stream.write((char*)&fresnelTeamColor, sizeof(fresnelTeamColor));
	}

	// Write Optional Track Chunks
	if (textureIdTracks.getTracksCount() > 0) {
		textureIdTracks.write(stream, version);
	}

	if (alphaTracks.getTracksCount() > 0) {
		alphaTracks.write(stream, version);
	}

	if (emissiveGainTracks.getTracksCount() > 0) {
		emissiveGainTracks.write(stream, version);
	}

	if (fresnelColorTracks.getTracksCount() > 0) {
		fresnelColorTracks.write(stream, version);
	}

	if (fresnelAlphaTracks.getTracksCount() > 0) {
		fresnelAlphaTracks.write(stream, version);
	}

	if (fresnelTeamColorTracks.getTracksCount() > 0) {
		fresnelTeamColorTracks.write(stream, version);
	}

	std::streampos streamEndPosition{ stream.tellp() };

	inclusiveSize = static_cast<uint32_t>(streamEndPosition) - static_cast<uint32_t>(streamStartPosition);
	stream.seekp(streamStartPosition);
	stream.write((char*)&inclusiveSize, sizeof(inclusiveSize));
	stream.seekp(streamEndPosition);
}

void Layer::setFilterMode(uint32_t filterMode) {
	this->filterMode = filterMode;
}

void Layer::setShadingFlags(uint32_t shadingFlags) {
	this->shadingFlags = shadingFlags;
}

void Layer::setTextureId(uint32_t textureId) {
	this->textureId = textureId;
}

void Layer::setTextureAnimationId(uint32_t textureAnimationId) {
	this->textureAnimationId = textureAnimationId;
}

void Layer::setCoordId(uint32_t coordId) {
	this->coordId = coordId;
}

uint32_t Layer::getInclusiveSize() const {
	return inclusiveSize;
}

uint32_t Layer::getFilterMode() const {
	return filterMode;
}

uint32_t Layer::getShadingFlags() const {
	return shadingFlags;
}

uint32_t Layer::getTextureId() const {
	return textureId;
}

uint32_t Layer::getTextureAnimationId() const {
	return textureAnimationId;
}

uint32_t Layer::getCoordId() const {
	return coordId;
}
