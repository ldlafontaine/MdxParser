#include "Material.h"

#include <sstream>

using namespace MdxParser;

Material::Material() :
	inclusiveSize{ 0 },
	flags{ 0 },
	layersCount{ 0 },
	priorityPlane{ 0 } {
}

Material::Material(std::istream& stream, uint32_t version) : Material() {

	std::streampos streamStartPosition{ stream.tellg() };

	stream.read((char*)&inclusiveSize, sizeof(inclusiveSize));
	stream.read((char*)&priorityPlane, sizeof(priorityPlane));
	stream.read((char*)&flags, sizeof(flags));

	if (version > 800) {
		std::vector<char> shaderBuffer(80);
		stream.read(reinterpret_cast<char*>(shaderBuffer.data()), 80);
		shader = std::string{ shaderBuffer.begin(), shaderBuffer.end() };
	}

	stream.ignore(4); // Skip "LAYS" tag
	stream.read((char*)&layersCount, sizeof(layersCount));

	// Read Layers
	for (unsigned int i{ 0 }; i < layersCount; i++) {
		Layer layer{ stream, version };
		layers.push_back(layer);
	}

	stream.seekg(static_cast<int>(streamStartPosition) + inclusiveSize);

	//std::cout << "Material constructed with a size of " << inclusiveSize << " bytes." << std::endl;
}

void Material::write(std::ostream& stream, uint32_t version) {
	std::streampos streamStartPosition{ stream.tellp() };

	stream.write((char*)&inclusiveSize, sizeof(inclusiveSize));
	stream.write((char*)&priorityPlane, sizeof(priorityPlane));
	stream.write((char*)&flags, sizeof(flags));

	if (version > 800) {
		stream.write(shader.c_str(), 80);
	}

	stream.write("LAYS", 4); // Skip "LAYS" tag
	stream.write((char*)&layersCount, sizeof(layersCount));

	// Read Layers
	for (Layer layer : layers) {
		layer.write(stream, version);
	}

	std::streampos streamEndPosition{ stream.tellp() };

	inclusiveSize = static_cast<uint32_t>(streamEndPosition) - static_cast<uint32_t>(streamStartPosition);
	stream.seekp(streamStartPosition);
	stream.write((char*)&inclusiveSize, sizeof(inclusiveSize));
	stream.seekp(streamEndPosition);
}

void Material::addLayer(Layer layer) {
	layers.push_back(layer);
	layersCount = static_cast<uint32_t>(layers.size());
}

uint32_t Material::getInclusiveSize() const {
	return inclusiveSize;
}

uint32_t Material::getPriorityPlane() const {
	return priorityPlane;
}

uint32_t Material::getFlags() const {
	return flags;
}

std::string Material::getShader() const {
	return shader;
}

uint32_t Material::getLayersCount() const {
	return layersCount;
}

std::vector<Layer> Material::getLayers() const {
	return layers;
}