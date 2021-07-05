#include "Camera.h"

#include <vector>

using namespace MdxParser;

Camera::Camera() {
	std::cout << "Empty camera constructed." << std::endl;
}

Camera::Camera(std::istream& stream, uint32_t version) {

	std::streampos streamStartPosition{ stream.tellg() };

	stream.read((char*)&inclusiveSize, sizeof(inclusiveSize));
	
	// Read Name
	std::vector<char> nameBuffer(80);
	stream.read(reinterpret_cast<char*>(nameBuffer.data()), 80);
	name = std::string{ nameBuffer.begin(), nameBuffer.end() };

	stream.read((char*)&position, sizeof(position));
	stream.read((char*)&fieldOfView, sizeof(fieldOfView));
	stream.read((char*)&farClippingPlane, sizeof(farClippingPlane));
	stream.read((char*)&nearClippingPlane, sizeof(nearClippingPlane));
	stream.read((char*)&targetPosition, sizeof(targetPosition));

	std::streampos streamEndPosition{ static_cast<unsigned int>(streamStartPosition) + inclusiveSize };

	// Read Optional Track Chunks
	while (stream.tellg() < streamEndPosition) {

		// Read Tag
		std::vector<char> tagBuffer(4);
		stream.read(reinterpret_cast<char*>(tagBuffer.data()), 4);
		std::string tag{ tagBuffer.begin(), tagBuffer.end() };

		if (tag == "KCTR") {
			translation = TracksChunk<std::array<float, 3>>{ tag, stream };
		}
		else if (tag == "KTTR") {
			rotation = TracksChunk<float>{ tag, stream };
		}
		else if (tag == "KCRL") {
			targetTranslation = TracksChunk<std::array<float, 3>>{ tag, stream };
		}
	}

	stream.seekg(streamEndPosition);
}

void Camera::write(std::ostream& stream, uint32_t version) {
	stream.write((char*)&inclusiveSize, sizeof(inclusiveSize));
	stream.write(name.c_str(), sizeof(name));
	stream.write((char*)&position, sizeof(position));
	stream.write((char*)&fieldOfView, sizeof(fieldOfView));
	stream.write((char*)&farClippingPlane, sizeof(farClippingPlane));
	stream.write((char*)&nearClippingPlane, sizeof(nearClippingPlane));
	stream.write((char*)&targetPosition, sizeof(targetPosition));

	// Write Optional Track Chunks
	if (translation.getTracksCount() > 0) {
		translation.write(stream, version);
	}

	if (rotation.getTracksCount() > 0) {
		rotation.write(stream, version);
	}

	if (targetTranslation.getTracksCount() > 0) {
		targetTranslation.write(stream, version);
	}
}

uint32_t Camera::getInclusiveSize() const {
	return inclusiveSize;
}

std::string Camera::getName() const {
	return name;
}

std::array<float, 3> Camera::getPosition() const {
	return position;
}

float Camera::getFieldOfView() const {
	return fieldOfView;
}

float Camera::getFarClippingPlane() const {
	return farClippingPlane;
}

float Camera::getNearClippingPlane() const {
	return nearClippingPlane;
}

std::array<float, 3> Camera::getTargetPosition() const {
	return targetPosition;
}

TracksChunk<std::array<float, 3>> Camera::getTranslation() const {
	return translation;
}

TracksChunk<float> Camera::getRotation() const {
	return rotation;
}

TracksChunk<std::array<float, 3>> Camera::getTargetTranslation() const {
	return targetTranslation;
}
