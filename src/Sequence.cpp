#include "Sequence.h"

#include "Extent.h"

#include <vector>
#include <sstream>

using namespace MdxParser;

Sequence::Sequence() :
	startFrame{ 0 },
	endFrame{ 1 },
	moveSpeed{ 0 },
	flags{ 0 },
	rarity{ 0 },
	syncPoint{ 0 } {
}

Sequence::Sequence(std::istream& stream, uint32_t version) : Sequence() {

	std::streampos streamStartPosition{ stream.tellg() };

	// Read Name
	char nameBuffer[80]{ '\0' };
	stream.read((char*)nameBuffer, 80);
	name = std::string{ nameBuffer };

	stream.read((char*)&startFrame, sizeof(startFrame));
	stream.read((char*)&endFrame, sizeof(endFrame));
	stream.read((char*)&moveSpeed, sizeof(moveSpeed));
	stream.read((char*)&flags, sizeof(flags));
	stream.read((char*)&rarity, sizeof(rarity));
	stream.read((char*)&syncPoint, sizeof(syncPoint));
	extent = Extent{ stream };

	//std::cout << name << " sequence constructed." << std::endl;
}

void Sequence::write(std::ostream& stream, uint32_t version) {
	stream.write(name.c_str(), 80);
	stream.write((char*)&startFrame, sizeof(startFrame));
	stream.write((char*)&endFrame, sizeof(endFrame));
	stream.write((char*)&moveSpeed, sizeof(moveSpeed));
	stream.write((char*)&flags, sizeof(flags));
	stream.write((char*)&rarity, sizeof(rarity));
	stream.write((char*)&syncPoint, sizeof(syncPoint));
	extent.write(stream, version);
}

void Sequence::setName(std::string name) {
	this->name = name;
}

void Sequence::setStartFrame(uint32_t startFrame) {
	this->startFrame = startFrame;
}

void Sequence::setEndFrame(uint32_t endFrame) {
	this->endFrame = endFrame;
}

void Sequence::setMoveSpeed(float moveSpeed) {
	this->moveSpeed = moveSpeed;
}

void Sequence::setFlags(uint32_t flags) {
	this->flags = flags;
}

void Sequence::setRarity(float rarity) {
	this->rarity = rarity;
}

void Sequence::setSyncPoint(uint32_t syncPoint) {
	this->syncPoint = syncPoint;
}

void Sequence::setExtent(Extent extent) {
	this->extent = extent;
}

uint32_t Sequence::getInclusiveSize() const {
	return 132;
}

std::string Sequence::getName() const {
	return name;
}

uint32_t Sequence::getStartFrame() const {
	return startFrame;
}

uint32_t Sequence::getEndFrame() const {
	return endFrame;
}

float Sequence::getMoveSpeed() const {
	return moveSpeed;
}

uint32_t Sequence::getFlags() const {
	return flags;
}

float Sequence::getRarity() const {
	return rarity;
}

uint32_t Sequence::getSyncPoint() const {
	return syncPoint;
}

Extent Sequence::getExtent() const {
	return extent;
}