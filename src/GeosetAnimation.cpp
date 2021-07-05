#include "GeosetAnimation.h"

#include <vector>
#include <sstream>

using namespace MdxParser;

GeosetAnimation::GeosetAnimation() {
	std::cout << "Emtpy geoset animation constructed." << std::endl;
}

GeosetAnimation::GeosetAnimation(std::istream& stream, uint32_t version) {

	std::streampos streamStartPosition{ stream.tellg() };

	stream.read((char*)&inclusiveSize, sizeof(inclusiveSize));
	stream.read((char*)&alpha, sizeof(alpha));
	stream.read((char*)&flags, sizeof(flags));
	stream.read((char*)&color, sizeof(color));
	stream.read((char*)&geosetId, sizeof(geosetId));

	std::streampos streamEndPosition{ static_cast<unsigned int>(streamStartPosition) + inclusiveSize };

	// Read Optional Track Chunks
	while (stream.tellg() < streamEndPosition) {

		// Read Tag
		std::vector<char> tagBuffer(4);
		stream.read(reinterpret_cast<char*>(tagBuffer.data()), 4);
		std::string tag{ tagBuffer.begin(), tagBuffer.end() };

		if (tag == "KGAO") {
			alphaTracks = TracksChunk<float>{ tag, stream };
		}
		else if (tag == "KGAC") {
			colorTracks = TracksChunk<std::array<float, 3>>{ tag, stream };
		}
	}

	stream.seekg(streamEndPosition);
}

void GeosetAnimation::write(std::ostream& stream, uint32_t version) {
	stream.write((char*)&inclusiveSize, sizeof(inclusiveSize));
	stream.write((char*)&alpha, sizeof(alpha));
	stream.write((char*)&flags, sizeof(flags));
	stream.write((char*)&color, sizeof(color));
	stream.write((char*)&geosetId, sizeof(geosetId));

	// Write Optional Track Chunks
	if (alphaTracks.getTracksCount() > 0) {
		alphaTracks.write(stream, version);
	}

	if (colorTracks.getTracksCount() > 0) {
		colorTracks.write(stream, version);
	}
}

uint32_t GeosetAnimation::getInclusiveSize() const {
	return inclusiveSize;
}

float GeosetAnimation::getAlpha() const {
	return alpha;
}

uint32_t GeosetAnimation::getFlags() const {
	return flags;
}

std::array<float, 3> GeosetAnimation::getColor() const {
	return color;
}

uint32_t GeosetAnimation::getGeosetId() const {
	return geosetId;
}

TracksChunk<float> GeosetAnimation::getAlphaTracks() const {
	return alphaTracks;
}

TracksChunk<std::array<float, 3>> GeosetAnimation::getColorTracks() const {
	return colorTracks;
}
