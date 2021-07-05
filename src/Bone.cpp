#include "Bone.h"

#include <sstream>

using namespace MdxParser;

Bone::Bone() : 
	geosetAnimationId{ std::numeric_limits<uint32_t>::max() },
	geosetId{ 0 } {
}

Bone::Bone(std::istream& stream, uint32_t version) : 
	Node{ stream, version },
	geosetAnimationId{ std::numeric_limits<uint32_t>::max() },
	geosetId{ 0 } {
	stream.read((char*)&geosetId, sizeof(uint32_t));
	stream.read((char*)&geosetAnimationId, sizeof(uint32_t));
	//std::cout << name << " bone constructed." << std::endl;
}

void Bone::write(std::ostream& stream, uint32_t version) {
	Node::write(stream, version);
	stream.write((char*)&geosetId, sizeof(uint32_t));
	stream.write((char*)&geosetAnimationId, sizeof(uint32_t));
}

uint32_t Bone::getInclusiveSize() const {
	return nodeInclusiveSize + sizeof(geosetId) + sizeof(geosetAnimationId);
}

uint32_t Bone::getGeosetId() const {
	return geosetId;
}

uint32_t Bone::getGeosetAnimationId() const {
	return geosetAnimationId;
}

void Bone::setGeosetId(uint32_t geosetId) {
	this->geosetId = geosetId;
}
void Bone::setGeosetAnimationId(uint32_t geosetAnimationId) {
	this->geosetAnimationId = geosetAnimationId;
}