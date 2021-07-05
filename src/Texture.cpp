#include "Texture.h"

#include <vector>
#include <sstream>

using namespace MdxParser;

Texture::Texture() :
	replaceableId{ 0 },
	flags{ 0 } {
}

Texture::Texture(std::istream& stream, uint32_t version) : Texture() {

	stream.read((char*)&replaceableId, sizeof(replaceableId));

	// Read File Name
	char fileNameBuffer[260]{ '\0' };
	stream.read((char*)fileNameBuffer, 260);
	fileName = std::string{ fileNameBuffer };

	stream.read((char*)&flags, sizeof(flags));

	//std::cout << "Texture constructed with a file name of \"" << fileName << "\"." << std::endl;
}

void Texture::write(std::ostream& stream, uint32_t version) {
	stream.write((char*)&replaceableId, sizeof(replaceableId));
	stream.write(fileName.c_str(), 260);
	stream.write((char*)&flags, sizeof(flags));
}

void Texture::setReplaceableId(uint32_t replaceableId) {
	this->replaceableId = replaceableId;
}

void Texture::setFileName(std::string fileName) {
	this->fileName = fileName;
}

void Texture::setFlags(uint32_t flags) {
	this->flags = flags;
}

uint32_t Texture::getInclusiveSize() const {
	return 268;
}

uint32_t Texture::getReplaceableId() const {
	return replaceableId;
}

std::string Texture::getFileName() const {
	return fileName;
}

uint32_t Texture::getFlags() const {
	return flags;
}
