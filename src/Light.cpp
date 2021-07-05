#include "Light.h"

using namespace MdxParser;

Light::Light() {
	std::cout << "Empty light constructed." << std::endl;
}

Light::Light(std::istream& stream, uint32_t version) {

	std::streampos streamStartPosition{ stream.tellg() };

	stream.read((char*)&inclusiveSize, sizeof(inclusiveSize));
	Node::read(stream, version);
	stream.read((char*)&type, sizeof(type));
	stream.read((char*)&attenuationStart, sizeof(attenuationStart));
	stream.read((char*)&attenuationEnd, sizeof(attenuationEnd));
	stream.read((char*)&color, sizeof(color));
	stream.read((char*)&intensity, sizeof(intensity));
	stream.read((char*)&ambientColor, sizeof(ambientColor));
	stream.read((char*)&ambientIntensity, sizeof(ambientIntensity));

	std::streampos streamEndPosition{ static_cast<unsigned int>(streamStartPosition) + inclusiveSize };

	// Read Optional Track Chunks
	while (stream.tellg() < streamEndPosition) {

		// Read Tag
		std::vector<char> tagBuffer(4);
		stream.read(reinterpret_cast<char*>(tagBuffer.data()), 4);
		std::string tag{ tagBuffer.begin(), tagBuffer.end() };

		if (tag == "KLAS") {
			attenuationStartTracks = TracksChunk<float>{ tag, stream };
		}
		else if (tag == "KLAE") {
			attenuationEndTracks = TracksChunk<float>{ tag, stream };
		}
		else if (tag == "KLAC") {
			colorTracks = TracksChunk<std::array<float, 3>>{ tag, stream };
		}
		else if (tag == "KLAI") {
			intensityTracks = TracksChunk<float>{ tag, stream };
		}
		else if (tag == "KLBI") {
			ambientIntensityTracks = TracksChunk<float>{ tag, stream };
		}
		else if (tag == "KLBC") {
			ambientColorTracks = TracksChunk<std::array<float, 3>>{ tag, stream };
		}
		else if (tag == "KLAV") {
			visibilityTracks = TracksChunk<float>{ tag, stream };
		}
	}

	stream.seekg(streamEndPosition);
}

void Light::write(std::ostream& stream, uint32_t version) {
	stream.write((char*)&inclusiveSize, sizeof(inclusiveSize));
	Node::write(stream, version);
	stream.write((char*)&type, sizeof(type));
	stream.write((char*)&attenuationStart, sizeof(attenuationStart));
	stream.write((char*)&attenuationEnd, sizeof(attenuationEnd));
	stream.write((char*)&color, sizeof(color));
	stream.write((char*)&intensity, sizeof(intensity));
	stream.write((char*)&ambientColor, sizeof(ambientColor));
	stream.write((char*)&ambientIntensity, sizeof(ambientIntensity));

	// Write Optional Track Chunks
	if (attenuationStartTracks.getTracksCount() > 0) {
		attenuationStartTracks.write(stream, version);
	}

	if (attenuationEndTracks.getTracksCount() > 0) {
		attenuationEndTracks.write(stream, version);
	}

	if (colorTracks.getTracksCount() > 0) {
		colorTracks.write(stream, version);
	}

	if (intensityTracks.getTracksCount() > 0) {
		intensityTracks.write(stream, version);
	}

	if (ambientIntensityTracks.getTracksCount() > 0) {
		ambientIntensityTracks.write(stream, version);
	}

	if (ambientColorTracks.getTracksCount() > 0) {
		ambientColorTracks.write(stream, version);
	}

	if (visibilityTracks.getTracksCount() > 0) {
		visibilityTracks.write(stream, version);
	}
}

uint32_t Light::getInclusiveSize() const {
	return inclusiveSize;
}

uint32_t Light::getType() const {
	return type;
}

float Light::getAttenuationStart() const {
	return attenuationStart;
}

float Light::getAttenuationEnd() const {
	return attenuationEnd;
}

std::array<float, 3> Light::getColor() const {
	return color;
}

float Light::getIntensity() const {
	return intensity;
}

std::array<float, 3> Light::getAmbientColor() const {
	return ambientColor;
}

float Light::getAmbientIntensity() const {
	return ambientIntensity;
}

TracksChunk<float> Light::getAttenuationStartTracks() const {
	return attenuationStartTracks;
}

TracksChunk<float> Light::getAttenuationEndTracks() const {
	return attenuationEndTracks;
}

TracksChunk<std::array<float, 3>> Light::getColorTracks() const {
	return colorTracks;
}

TracksChunk<float> Light::getIntensityTracks() const {
	return intensityTracks;
}

TracksChunk<float> Light::getAmbientIntensityTracks() const {
	return ambientIntensityTracks;
}

TracksChunk<std::array<float, 3>> Light::getAmbientColorTracks() const {
	return ambientColorTracks;
}

TracksChunk<float> Light::getVisibilityTracks() const {
	return visibilityTracks;
}
