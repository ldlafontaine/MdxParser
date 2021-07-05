#include "Model.h"

#include <string>
#include <stdint.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace MdxParser;

Model::Model() : 
	version{ 1000 },
	blendTime{ 0 } {
}

Model::Model(const char* filePath) : Model() {
	std::ifstream stream;
	stream.open(filePath, std::ios::in | std::ios::binary);

	if (!stream.is_open()) {
		std::cerr << "File could not be opened." << std::endl;
	}

	std::cout << "File opened successfully." << std::endl;

	std::vector<char> fileHeader(4);
	stream.read(reinterpret_cast<char*>(fileHeader.data()), 4);

	if (std::string{ fileHeader.begin(), fileHeader.end() } == "MDLX") {
		while (!stream.eof()) {

			// Read header to determine chunk tag and size.
			std::vector<char> headerBuffer(4);

			stream.read(reinterpret_cast<char*>(headerBuffer.data()), 4);
			std::string tag{ headerBuffer.begin(), headerBuffer.end() };

			uint32_t size;
			stream.read((char*)&size, 4);

			std::cout << tag << " chunk detected with a size of " << size << " bytes." << std::endl;

			std::streampos chunkStartPosition{ stream.tellg() };
			std::streampos chunkEndPosition{ static_cast<unsigned int>(chunkStartPosition) + size };

			if (tag == "VERS") {
				stream.read((char*)&version, sizeof(version));
				std::cout << "Model version is " << version << "." << std::endl;
			}
			else if (tag == "MODL") {
				// Read Name
				char nameBuffer[80]{ '\0' };
				stream.read((char*)nameBuffer, 80);
				name = std::string{ nameBuffer };

				// Read Animation File Name
				char animationFileNameBuffer[260]{ '\0' };
				stream.read((char*)animationFileNameBuffer, 260);
				animationFileName = std::string{ animationFileNameBuffer };

				// Read Extent and Blend Time
				extent = Extent{ stream };
				stream.read((char*)&blendTime, sizeof(blendTime));

				std::cout << "Model name is " << name << "." << std::endl;
			}
			else if (tag == "SEQS") {
				readVector(sequences, stream, size);
			}
			else if (tag == "GLBS") {
				while (stream.tellg() < chunkEndPosition) {
					uint32_t globalSequence;
					stream.read((char*)&globalSequence, sizeof(globalSequence));
					globalSequences.push_back(globalSequence);
				}
			}
			else if (tag == "TEXS") {
				readVector(textures, stream, size);
			}
			else if (tag == "SNDS") {
				// Handle chunk.
			}
			else if (tag == "MTLS") {
				readVector(materials, stream, size);
			}
			else if (tag == "TXAN") {
				readVector(textureAnimations, stream, size);
			}
			else if (tag == "GEOS") {
				readVector(geosets, stream, size);
			}
			else if (tag == "GEOA") {
				readVector(geosetAnimations, stream, size);
			}
			else if (tag == "BONE") {
				readVector(bones, stream, size);
			}
			else if (tag == "LITE") {
				readVector(lights, stream, size);
			}
			else if (tag == "HELP") {
				readVector(helpers, stream, size);
			}
			else if (tag == "ATCH") {
				readVector(attachments, stream, size);
			}
			else if (tag == "PIVT") {
				while (stream.tellg() < chunkEndPosition) {
					float pivotBuffer[3];
					stream.read((char*)&pivotBuffer, sizeof(pivotBuffer));
					std::array<float, 3> pivot{ pivotBuffer[0], pivotBuffer[1], pivotBuffer[2] };
					pivots.push_back(pivot);
				}
			}
			else if (tag == "PREM") {
				// Handle chunk.
			}
			else if (tag == "PRE2") {
				// Handle chunk.
			}
			else if (tag == "RIBB") {
				// Handle chunk.
			}
			else if (tag == "EVTS") {
				// Handle chunk.
			}
			else if (tag == "CAMS") {
				readVector(cameras, stream, size);
			}
			else if (tag == "CLID") {
				readVector(collisionShapes, stream, size);
			}
			else if (tag == "BPOS") {
				// Handle chunk.
			}
			else if (tag == "FAFX") {
				// Handle chunk.
			}
			else if (tag == "CORN") {
				// Handle chunk.
			}
			else {
				break;
			}

			stream.seekg(chunkEndPosition);
		};
	}

	stream.close();
}

template <class T> void Model::readVector(std::vector<T>& vector, std::istream& stream, uint32_t size) {
	std::streampos streamStartPosition{ stream.tellg() };
	while (static_cast<unsigned int>(stream.tellg()) < static_cast<unsigned int>(streamStartPosition) + size) {
		T member{ stream, version };
		vector.push_back(member);
	}
}

void Model::write(const char* filePath) {
	std::ofstream stream;
	stream.open(filePath, std::ios::out | std::ios::binary);

	if (!stream.is_open()) {
		std::cerr << "Could not write to file." << std::endl;
	}

	// Write file header
	stream.write("MDLX", 4);

	// Write VERS chunk
	stream.write("VERS", 4);
	uint32_t versionChunkSize{ sizeof(version) };
	stream.write((char*)&versionChunkSize, sizeof(versionChunkSize));
	stream.write((char*)&version, sizeof(version));

	// Write MODL chunk
	stream.write("MODL", 4);
	uint32_t modelChunkSize{ 80 + 260 + extent.getInclusiveSize() + sizeof(blendTime) };
	stream.write((char*)&modelChunkSize, sizeof(modelChunkSize));
	stream.write(name.c_str(), 80);
	stream.write(animationFileName.c_str(), 260);
	extent.write(stream, version);
	stream.write((char*)&blendTime, sizeof(blendTime));

	// Write SEQS chunk
	if (!sequences.empty()) {
		stream.write("SEQS", 4);
		writeVector(sequences, stream);
	}

	// Write GLBS chunk
	if (!globalSequences.empty()) {
		stream.write("GLBS", 4);
		uint32_t globalSequencesChunkSize{ sizeof(uint32_t) * static_cast<uint32_t>(globalSequences.size()) };
		stream.write((char*)&globalSequencesChunkSize, sizeof(globalSequencesChunkSize));
		for (uint32_t globalSequence : globalSequences) {
			stream.write((char*)&globalSequence, sizeof(globalSequence));
		}
	}

	// Write TEXS chunk
	if (!textures.empty()) {
		stream.write("TEXS", 4);
		writeVector(textures, stream);
	}

	// Write SNDS Chunk
	// Not yet implemented.

	// Write MTLS chunk
	if (!materials.empty()) {
		stream.write("MTLS", 4);
		writeVector(materials, stream);
	}

	// Write TXAN chunk
	if (!textureAnimations.empty()) {
		stream.write("TXAN", 4);
		writeVector(textureAnimations, stream);
	}

	// Write GEOS chunk
	if (!geosets.empty()) {
		stream.write("GEOS", 4);
		writeVector(geosets, stream);
	}

	// Write GEOA chunk
	if (!geosetAnimations.empty()) {
		stream.write("GEOA", 4);
		writeVector(geosetAnimations, stream);
	}

	// Write BONE chunk
	if (!bones.empty()) {
		stream.write("BONE", 4);
		writeVector(bones, stream);
	}

	// Write LITE chunk
	if (!lights.empty()) {
		stream.write("LITE", 4);
		writeVector(lights, stream);
	}

	// Write HELP chunk
	if (!helpers.empty()) {
		stream.write("HELP", 4);
		writeVector(helpers, stream);
	}

	// Write ATCH chunk
	if (!attachments.empty()) {
		stream.write("ATCH", 4);
		writeVector(attachments, stream);
	}

	// Write PIVT chunk
	if (!pivots.empty()) {
		stream.write("PIVT", 4);
		uint32_t pivotsChunkSize{ sizeof(float) * 3 * static_cast<uint32_t>(pivots.size()) };
		stream.write((char*)&pivotsChunkSize, sizeof(pivotsChunkSize));
		for (auto pivot : pivots) {
			for (float position : pivot) {
				stream.write((char*)&position, sizeof(position));
			}
		}
	}

	// Write PREM Chunk
	// Not yet implemented.

	// Write PRE2 Chunk
	// Not yet implemented.

	// Write RIBB Chunk
	// Not yet implemented.

	// Write EVTS Chunk
	// Not yet implemented.

	// Write CAMS Chunk
	if (!cameras.empty()) {
		stream.write("CAMS", 4);
		writeVector(cameras, stream);
	}

	// Write CLID Chunk
	if (!collisionShapes.empty()) {
		stream.write("CLID", 4);
		writeVector(collisionShapes, stream);
	}

	if (version > 800) {
		// Write BPOS Chunk
		// Not yet implemented.

		// Write FAFX Chunk
		// Not yet implemented.

		// Write CORN Chunk
		// Not yet implemented.
	}

	stream.close();
}

template <class T> void Model::writeVector(std::vector<T>& vector, std::ostream& stream) {
	std::streampos streamStartPosition{ stream.tellp() };

	// Write dummy chunk size
	uint32_t chunkSize{ 0 };
	stream.write((char*)&chunkSize, sizeof(chunkSize));

	std::streampos chunkStartPosition{ stream.tellp() };

	// Write members to stream
	for (T member : vector) {
		member.write(stream, version);
	}
	
	std::streampos streamEndPosition{ stream.tellp() };

	chunkSize = static_cast<uint32_t>(streamEndPosition) - static_cast<uint32_t>(chunkStartPosition);
	stream.seekp(streamStartPosition);
	stream.write((char*)&chunkSize, sizeof(chunkSize));
	stream.seekp(streamEndPosition);

	std::cout << "Wrote chunk with a size of " << chunkSize << "." << std::endl;
}

void Model::addSequence(Sequence sequence) {
	sequences.push_back(sequence);
}

void Model::addTexture(Texture texture) {
	textures.push_back(texture);
}

void Model::addMaterial(Material material) {
	materials.push_back(material);
}

void Model::addGeoset(Geoset geoset) {
	geosets.push_back(geoset);
}

void Model::addBone(Bone bone) {
	bones.push_back(bone);
}

void Model::addHelper(Helper helper) {
	helpers.push_back(helper);
}

void Model::addPivot(pivot pivot) {
	pivots.push_back(pivot);
}

Bone* Model::findBone(int id) {
	Bone* matchingBone{ nullptr };
	for (Bone& bone : bones) {
		if (bone.getObjectId() == id) {
			matchingBone = &bone;
			break;
		}
	}
	return matchingBone;
}

Bone* Model::findBone(std::string nodeName) {
	Bone* matchingBone{ nullptr };
	for (Bone& bone : bones) {
		if (bone.getName() == nodeName) {
			matchingBone = &bone;
			break;
		}
	}
	return matchingBone;
}

Helper* Model::findHelper(int id) {
	Helper* matchingHelper{ nullptr };
	for (Helper& helper : helpers) {
		if (helper.getObjectId() == id) {
			matchingHelper = &helper;
			break;
		}
	}
	return matchingHelper;
}

Helper* Model::findHelper(std::string nodeName) {
	Helper* matchingHelper{ nullptr };
	for (Helper& helper : helpers) {
		if (helper.getName() == nodeName) {
			matchingHelper = &helper;
			break;
		}
	}
	return matchingHelper;
}

Node* Model::findNode(int id) {
	Node* matchingNode{ nullptr };
	for (Node* node : getNodes()) {
		if (node->getObjectId() == id) {
			matchingNode = node;
			break;
		}
	}
	return matchingNode;
}

Node* Model::findNode(std::string nodeName) {
	Node* matchingNode{ nullptr };
	for (Node* node : getNodes()) {
		if (node->getName() == nodeName) {
			matchingNode = node;
			break;
		}
	}
	return matchingNode;
}

void Model::setVersion(uint32_t version) {
	this->version = version;
}

void Model::setName(std::string name) {
	this->name = name;
}

void Model::setAnimationFileName(std::string animationFileName) {
	this->animationFileName = animationFileName;
}

void Model::setExtent(Extent extent) {
	this->extent = extent;
}

void Model::setBlendTime(uint32_t blendTime) {
	this->blendTime = blendTime;
}

uint32_t Model::getVersion() const {
	return version;
}

std::string Model::getName() const {
	return name;
}

std::string Model::getAnimationFileName() const {
	return animationFileName;
}

Extent Model::getExtent() const {
	return extent;
}

uint32_t Model::getBlendTime() const {
	return blendTime;
}

std::vector<Sequence>& Model::getSequences() {
	return sequences;
}

std::vector<Texture>& Model::getTextures() {
	return textures;
}

std::vector<Material>& Model::getMaterials() {
	return materials;
}

std::vector<TextureAnimation>& Model::getTextureAnimations() {
	return textureAnimations;
}

std::vector<Geoset>& Model::getGeosets() {
	return geosets;
}

std::vector<GeosetAnimation>& Model::getGeosetAnimations() {
	return geosetAnimations;
}

std::vector<Bone>& Model::getBones() {
	return bones;
}

std::vector<Helper>& Model::getHelpers() {
	return helpers;
}

std::vector<pivot>& Model::getPivots() {
	return pivots;
}

std::vector<Camera>& Model::getCameras() {
	return cameras;
}

std::vector<CollisionShape>& Model::getCollisionShapes() {
	return collisionShapes;
}

std::vector<Node*> Model::getNodes() {
	std::vector<Node*> nodes;
	for (Bone& bone : bones) nodes.push_back(&bone);
	for (Helper& helper : helpers) nodes.push_back(&helper);
	return nodes;
}

int Model::getSequencesCount() const {
	return static_cast<int>(sequences.size());
}

int Model::getTexturesCount() const {
	return static_cast<int>(sequences.size());
}

int Model::getMaterialsCount() const {
	return static_cast<int>(materials.size());
}

int Model::getTextureAnimationsCount() const {
	return static_cast<int>(textureAnimations.size());
}

int Model::getGeosetsCount() const {
	return static_cast<int>(geosets.size());
}

int Model::getGeosetAnimationsCount() const {
	return static_cast<int>(geosetAnimations.size());
}

int Model::getBonesCount() const {
	return static_cast<int>(bones.size());
}

int Model::getHelpersCount() const {
	return static_cast<int>(helpers.size());
}

int Model::getPivotsCount() const {
	return static_cast<int>(pivots.size());
}

int Model::getCamerasCount() const {
	return static_cast<int>(cameras.size());
}

int Model::getCollisionShapesCount() const {
	return static_cast<int>(collisionShapes.size());
}

int Model::getNodesCount() const {
	int count{ static_cast<int>(bones.size()) };
	count += static_cast<int>(helpers.size());
	return count;
}
