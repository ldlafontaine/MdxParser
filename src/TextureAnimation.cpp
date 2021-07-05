#include "TextureAnimation.h"

#include <vector>
#include <sstream>

using namespace MdxParser;

TextureAnimation::TextureAnimation() {
	std::cout << "Empty texture animation constructed." << std::endl;
}

TextureAnimation::TextureAnimation(std::istream& stream, uint32_t version) {

	std::streampos streamStartPosition{ stream.tellg() };

	stream.read((char*)&inclusiveSize, sizeof(inclusiveSize));

	std::streampos streamEndPosition{ static_cast<unsigned int>(streamStartPosition) + inclusiveSize };

	// Read Optional Track Chunks
	while (stream.tellg() < streamEndPosition) {

		// Read Tag
		std::vector<char> tagBuffer(4);
		stream.read(reinterpret_cast<char*>(tagBuffer.data()), 4);
		std::string tag{ tagBuffer.begin(), tagBuffer.end() };

		if (tag == "KTAT") {
			translation = TracksChunk<std::array<float, 3>>{ tag, stream };
		}
		else if (tag == "KTAR") {
			rotation = TracksChunk<std::array<float, 4>>{ tag, stream };
		}
		else if (tag == "KTAS") {
			scaling = TracksChunk<std::array<float, 3>>{ tag, stream };
		}
	}

	stream.seekg(streamEndPosition);
}

void TextureAnimation::write(std::ostream& stream, uint32_t version) {
	stream.write((char*)&inclusiveSize, sizeof(inclusiveSize));

	// Write Optional Track Chunks
	if (translation.getTracksCount() > 0) {
		translation.write(stream, version);
	}

	if (rotation.getTracksCount() > 0) {
		rotation.write(stream, version);
	}

	if (scaling.getTracksCount() > 0) {
		scaling.write(stream, version);
	}
}

uint32_t TextureAnimation::getInclusiveSize() const {
	return inclusiveSize;
}

TracksChunk<std::array<float, 3>>& TextureAnimation::getTranslation() {
	return translation;
}

TracksChunk<std::array<float, 4>>& TextureAnimation::getRotation() {
	return rotation;
}

TracksChunk<std::array<float, 3>>& TextureAnimation::getScaling() {
	return scaling;
}
