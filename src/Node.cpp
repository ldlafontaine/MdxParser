#include "Node.h"

#include <vector>
#include <sstream>
#include <limits>

using namespace MdxParser;

Node::Node() :
	nodeInclusiveSize{ 0 },
	flags{ 0 },
	objectId{ 0 },
	parentId{ std::numeric_limits<uint32_t>::max() },
	translation{ "KGTR" },
	rotation{ "KGRT" },
	scale{ "KGSC" } {
}

Node::Node(std::istream& stream, uint32_t version) : Node() {
	read(stream, version);
}

void Node::read(std::istream& stream, uint32_t version) {

	std::streampos streamStartPosition{ stream.tellg() };

	stream.read((char*)&nodeInclusiveSize, sizeof(nodeInclusiveSize));

	// Read Name
	char nameBuffer[80]{ '\0' };
	stream.read((char*)nameBuffer, 80);
	name = std::string{ nameBuffer };

	stream.read((char*)&objectId, sizeof(objectId));
	stream.read((char*)&parentId, sizeof(parentId));
	stream.read((char*)&flags, sizeof(flags));

	// Read Optional Track Chunks
	while (static_cast<unsigned int>(stream.tellg()) < static_cast<unsigned int>(streamStartPosition) + nodeInclusiveSize) {

		// Read Tag
		std::vector<char> tagBuffer(4);
		stream.read(reinterpret_cast<char*>(tagBuffer.data()), 4);
		std::string tag{ tagBuffer.begin(), tagBuffer.end() };

		if (tag == "KGTR") {
			translation = TracksChunk<std::array<float, 3>>{ tag, stream };
		}
		else if (tag == "KGRT") {
			rotation = TracksChunk<std::array<float, 4>>{ tag, stream };
		}
		else if (tag == "KGSC") {
			scale = TracksChunk<std::array<float, 3>>{ tag, stream };
		}
	}

	stream.seekg(static_cast<int>(streamStartPosition) + nodeInclusiveSize);

	//std::cout << name << " node constructed." << std::endl;
}

void Node::write(std::ostream& stream, uint32_t version) {
	std::streampos streamStartPosition{ stream.tellp() };

	stream.write((char*)&nodeInclusiveSize, sizeof(nodeInclusiveSize));
	stream.write(name.c_str(), 80);
	stream.write((char*)&objectId, sizeof(objectId));
	stream.write((char*)&parentId, sizeof(parentId));
	stream.write((char*)&flags, sizeof(flags));

	// Write Optional Track Chunks
	if (translation.getTracksCount() > 0) {
		translation.write(stream, version);
	}

	if (rotation.getTracksCount() > 0) {
		rotation.write(stream, version);
	}

	if (scale.getTracksCount() > 0) {
		scale.write(stream, version);
	}

	std::streampos streamEndPosition{ stream.tellp() };

	nodeInclusiveSize = static_cast<uint32_t>(streamEndPosition) - static_cast<uint32_t>(streamStartPosition);
	stream.seekp(streamStartPosition);
	stream.write((char*)&nodeInclusiveSize, sizeof(nodeInclusiveSize));
	stream.seekp(streamEndPosition);
}

uint32_t Node::getNodeInclusiveSize() const {
	return nodeInclusiveSize;
}

std::string Node::getName() const {
	return name;
}

uint32_t Node::getObjectId() const {
	return objectId;
}

uint32_t Node::getParentId() const {
	return parentId;
}

bool Node::getFlag(Flag flag) const {
	return (flags & static_cast<uint32_t>(flag)) == static_cast<uint32_t>(flag);
}

uint32_t Node::getFlags() const {
	return flags;
}

TracksChunk<translation>& Node::getTranslation() {
	return translation;
}

TracksChunk<quaternion>& Node::getRotation() {
	return rotation;
}

TracksChunk<scale>& Node::getScale() {
	return scale;
}

void Node::setName(std::string name) {
	this->name = name;
}

void Node::setObjectId(uint32_t objectId) {
	this->objectId = objectId;
}

void Node::setParentId(uint32_t parentId) {
	this->parentId = parentId;
}

void Node::setFlags(uint32_t flags) {
	this->flags = flags;
}

bool Node::hasParent() {
	return parentId != std::numeric_limits<uint32_t>::max();
}