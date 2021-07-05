#include "TextureCoordinateSet.h"

using namespace MdxParser;

TextureCoordinateSet::TextureCoordinateSet() : 
	count{ 0 } {
}

TextureCoordinateSet::TextureCoordinateSet(std::istream& stream) : TextureCoordinateSet() {
	stream.ignore(4); // Skip "UVBS" tag
	stream.read((char*)&count, sizeof(count));

	for (unsigned int i = 0; i < count; i++) {
		std::array<float, 2> textureCoordinate;
		stream.read(reinterpret_cast<char*>(textureCoordinate.data()), sizeof(textureCoordinate));
		textureCoordinates.push_back(textureCoordinate);
	}

	//std::cout << "Texture coordinate set constructed." << std::endl;
}

void TextureCoordinateSet::write(std::ostream& stream, uint32_t version) {
	stream.write("UVBS", 4);
	stream.write((char*)&count, sizeof(count));
	for (std::array<float, 2> textureCoordinate : textureCoordinates) {
		stream.write(reinterpret_cast<char*>(textureCoordinate.data()), sizeof(textureCoordinate));
	}
}

uint32_t TextureCoordinateSet::getInclusiveSize() const {
	return sizeof(char[4]) + sizeof(count) + (sizeof(float) * count * 2);
}

uint32_t TextureCoordinateSet::getCount() const {
	return count;
}

std::vector<std::array<float, 2>> TextureCoordinateSet::getTextureCoordinates() const {
	return textureCoordinates;
}

void TextureCoordinateSet::setTextureCoordinates(std::vector<std::array<float, 2>> textureCoordinates) {
	this->textureCoordinates = textureCoordinates;
	this->count = static_cast<uint32_t>(textureCoordinates.size());
}