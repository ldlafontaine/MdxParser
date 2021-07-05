#include "Geoset.h"

#include <sstream>

using namespace MdxParser;

Geoset::Geoset() :
	inclusiveSize{ 0 },
	vertexCount{ 0 },
	normalCount{ 0 },
	faceTypeGroupsCount{ 0 },
	faceGroupsCount{ 0 },
	facesCount{ 0 },
	vertexGroupsCount{ 0 },
	matrixGroupsCount{ 0 },
	matrixIndicesCount{ 0 },
	materialId{ 0 },
	selectionGroup{ 0 },
	selectionFlags{ 0 },
	lod{ 0 },
	extentsCount{ 0 },
	tangentsCount{ 0 },
	skinCount{ 0 },
	textureCoordinateSetsCount{ 1 } {
}

Geoset::Geoset(std::istream& stream, uint32_t version) : Geoset() {

	std::streampos streamStartPosition{ stream.tellg() };

	stream.read((char*)&inclusiveSize, sizeof(inclusiveSize));
	stream.ignore(4); // Skip "VRTX" tag
	stream.read((char*)&vertexCount, sizeof(vertexCount));

	// Read Vertex Positions
	for (unsigned int i = 0; i < vertexCount; i++) {
		std::array<float, 3> vertexPosition;
		stream.read(reinterpret_cast<char*>(vertexPosition.data()), sizeof(float) * 3);
		vertexPositions.push_back(vertexPosition);
	}

	stream.ignore(4); // Skip "NRMS" tag
	stream.read((char*)&normalCount, sizeof(normalCount));

	// Read Vertex Normals
	for (unsigned int i = 0; i < normalCount; i++) {
		std::array<float, 3> vertexNormal;
		stream.read(reinterpret_cast<char*>(vertexNormal.data()), sizeof(float) * 3);
		vertexNormals.push_back(vertexNormal);
	}

	stream.ignore(4); // Skip "PTYP" tag
	stream.read((char*)&faceTypeGroupsCount, sizeof(faceTypeGroupsCount));

	// Read Face Type Groups
	for (unsigned int i = 0; i < faceTypeGroupsCount; i++) {
		uint32_t faceTypeGroup;
		stream.read((char*)&faceTypeGroup, sizeof(faceTypeGroup));
		faceTypeGroups.push_back(faceTypeGroup);
	}

	stream.ignore(4); // Skip "PCNT" tag
	stream.read((char*)&faceGroupsCount, sizeof(uint32_t));

	// Read Face Groups
	for (unsigned int i = 0; i < faceGroupsCount; i++) {
		uint32_t faceGroup;
		stream.read((char*)&faceGroup, sizeof(faceGroup));
		faceGroups.push_back(faceGroup);
	}

	stream.ignore(4); // Skip "PVTX" tag
	stream.read((char*)&facesCount, sizeof(uint32_t));

	// Read Faces
	for (unsigned int i = 0; i < facesCount; i++) {
		uint16_t face;
		stream.read((char*)&face, sizeof(face));
		faces.push_back(face);
	}

	stream.ignore(4); // Skip "GNDX" tag
	stream.read((char*)&vertexGroupsCount, sizeof(vertexGroupsCount));

	// Read Vertex Groups
	for (unsigned int i = 0; i < vertexGroupsCount; i++) {
		uint8_t vertexGroup;
		stream.read((char*)&vertexGroup, sizeof(vertexGroup));
		vertexGroups.push_back(vertexGroup);
	}

	stream.ignore(4); // Skip "MTGC" tag
	stream.read((char*)&matrixGroupsCount, sizeof(matrixGroupsCount));

	// Read Matrix Groups
	for (unsigned int i = 0; i < matrixGroupsCount; i++) {
		uint32_t matrixGroup;
		stream.read((char*)&matrixGroup, sizeof(matrixGroup));
		matrixGroups.push_back(matrixGroup);
	}

	stream.ignore(4); // Skip "MATS" tag
	stream.read((char*)&matrixIndicesCount, sizeof(matrixIndicesCount));

	// Read Matrix Indices
	for (unsigned int i = 0; i < matrixIndicesCount; i++) {
		uint32_t matrixIndex;
		stream.read((char*)&matrixIndex, sizeof(matrixIndex));
		matrixIndices.push_back(matrixIndex);
	}

	stream.read((char*)&materialId, sizeof(materialId));
	stream.read((char*)&selectionGroup, sizeof(selectionGroup));
	stream.read((char*)&selectionFlags, sizeof(selectionFlags));

	if (version > 800) {
		stream.read((char*)&lod, sizeof(lod));

		char lodNameBuffer[80]{ '\0' };
		stream.read((char*)&lodNameBuffer, 80);
		lodName = std::string{ lodNameBuffer };
	}

	extent = Extent{ stream };
	stream.read((char*)&extentsCount, sizeof(extentsCount));

	for (unsigned i = 0; i < extentsCount; i++) {
		Extent sequenceExtent{ stream };
		sequenceExtents.push_back(sequenceExtent);
	}

	while (static_cast<unsigned int>(stream.tellg()) < static_cast<unsigned int>(streamStartPosition) + inclusiveSize) {

		std::vector<char> tagBuffer(4);
		stream.read(reinterpret_cast<char*>(tagBuffer.data()), 4);
		std::string tag{ tagBuffer.begin(), tagBuffer.end() };

		if (tag == "TANG") {
			uint32_t tangentsCount;
			stream.read((char*)&tangentsCount, sizeof(tangentsCount));
			stream.ignore(sizeof(float) * (tangentsCount * 4));
		}
		else if (tag == "SKIN") {
			uint32_t skinCount;
			stream.read((char*)&skinCount, sizeof(skinCount));
			stream.ignore(sizeof(uint8_t) * skinCount);
		}
		else if (tag == "UVAS") {
			stream.read((char*)&textureCoordinateSetsCount, sizeof(textureCoordinateSetsCount));

			for (unsigned i = 0; i < textureCoordinateSetsCount; i++) {
				TextureCoordinateSet textureCoordinateSet{ stream };
				textureCoordinateSets.push_back(textureCoordinateSet);
			}
		}
	}

	stream.seekg(static_cast<int>(streamStartPosition) + inclusiveSize);

	//std::cout << "Geoset constructed with a size of " << inclusiveSize << " bytes." << std::endl;
}

void Geoset::write(std::ostream& stream, uint32_t version) {
	std::streampos streamStartPosition{ stream.tellp() };

	stream.write((char*)&inclusiveSize, sizeof(uint32_t));
	stream.write("VRTX", 4);
	stream.write((char*)&vertexCount, sizeof(vertexCount));

	// Write Vertex Positions
	for (unsigned int i = 0; i < vertexCount; i++) {
		stream.write((char*)&vertexPositions.at(i).at(0), sizeof(float));
		stream.write((char*)&vertexPositions.at(i).at(1), sizeof(float));
		stream.write((char*)&vertexPositions.at(i).at(2), sizeof(float));
	}

	stream.write("NRMS", 4);
	stream.write((char*)&normalCount, sizeof(normalCount));

	// Write Vertex Normals
	for (unsigned int i = 0; i < normalCount; i++) {
		stream.write((char*)&vertexNormals.at(i).at(0), sizeof(float));
		stream.write((char*)&vertexNormals.at(i).at(1), sizeof(float));
		stream.write((char*)&vertexNormals.at(i).at(2), sizeof(float));
	}

	stream.write("PTYP", 4);
	stream.write((char*)&faceTypeGroupsCount, sizeof(faceTypeGroupsCount));

	// Write Face Type Groups
	for (unsigned int i = 0; i < faceTypeGroupsCount; i++) {
		stream.write((char*)&faceTypeGroups.at(i), sizeof(uint32_t));
	}

	stream.write("PCNT", 4);
	stream.write((char*)&faceGroupsCount, sizeof(uint32_t));

	// Write Face Groups
	for (unsigned int i = 0; i < faceGroupsCount; i++) {
		stream.write((char*)&faceGroups.at(i), sizeof(uint32_t));
	}

	stream.write("PVTX", 4);
	stream.write((char*)&facesCount, sizeof(uint32_t));

	// Write Faces
	for (unsigned int i = 0; i < facesCount; i++) {
		stream.write((char*)&faces.at(i), sizeof(uint16_t));
	}

	stream.write("GNDX", 4);
	stream.write((char*)&vertexGroupsCount, sizeof(vertexGroupsCount));

	// Write Vertex Groups
	for (unsigned int i = 0; i < vertexGroupsCount; i++) {
		stream.write((char*)&vertexGroups.at(i), sizeof(uint8_t));
	}

	stream.write("MTGC", 4);
	stream.write((char*)&matrixGroupsCount, sizeof(matrixGroupsCount));

	// Write Matrix Groups
	for (unsigned int i = 0; i < matrixGroupsCount; i++) {
		stream.write((char*)&matrixGroups.at(i), sizeof(uint32_t));
	}

	stream.write("MATS", 4);
	stream.write((char*)&matrixIndicesCount, sizeof(matrixIndicesCount));

	// Write Matrix Indices
	for (unsigned int i = 0; i < matrixIndicesCount; i++) {
		stream.write((char*)&matrixIndices.at(i), sizeof(uint32_t));
	}

	stream.write((char*)&materialId, sizeof(materialId));
	stream.write((char*)&selectionGroup, sizeof(selectionGroup));
	stream.write((char*)&selectionFlags, sizeof(selectionFlags));

	if (version > 800) {
		// Write LOD
		stream.write((char*)&lod, sizeof(lod));

		// Write LOD Name
		stream.write(lodName.c_str(), 80);
	}

	extent.write(stream, version);
	stream.write((char*)&extentsCount, sizeof(extentsCount));

	// Write Sequence Extents
	for (Extent sequenceExtent : sequenceExtents) {
		sequenceExtent.write(stream, version);
	}

	if (version > 800) {
		if (tangentsCount > 0) {
			stream.write("TANG", 4);
			stream.write((char*)&tangentsCount, sizeof(tangentsCount));
			for (std::array<float, 4> tangent : tangents) {
				stream.write(reinterpret_cast<char*>(tangent.data()), sizeof(tangent));
			}
		}

		if (skinCount > 0) {
			stream.write("SKIN", 4);
			stream.write((char*)&skinCount, sizeof(skinCount));
			stream.write(reinterpret_cast<char*>(skin.data()), (sizeof(uint8_t) * skinCount));
		}
	}

	stream.write("UVAS", 4);
	stream.write((char*)&textureCoordinateSetsCount, sizeof(textureCoordinateSetsCount));

	// Write Texture Coordinate Sets
	for (TextureCoordinateSet textureCoordinateSet : textureCoordinateSets) {
		textureCoordinateSet.write(stream, version);
	}

	std::streampos streamEndPosition{ stream.tellp() };

	inclusiveSize = static_cast<uint32_t>(streamEndPosition) - static_cast<uint32_t>(streamStartPosition);
	stream.seekp(streamStartPosition);
	stream.write((char*)&inclusiveSize, sizeof(inclusiveSize));
	stream.seekp(streamEndPosition);
}

uint32_t Geoset::getInclusiveSize() const {
	return inclusiveSize;
}

uint32_t Geoset::getVertexCount() const {
	return vertexCount;
}

std::vector<std::array<float, 3>> Geoset::getVertexPositions() const {
	return vertexPositions;
}

uint32_t Geoset::getNormalCount() const {
	return normalCount;
}

std::vector<std::array<float, 3>> Geoset::getVertexNormals() const {
	return vertexNormals;
}

uint32_t Geoset::getFaceTypeGroupsCount() const {
	return faceTypeGroupsCount;
}

std::vector<uint32_t> Geoset::getFaceTypeGroups() const {
	return faceTypeGroups;
}

uint32_t Geoset::getFaceGroupsCount() const {
	return faceGroupsCount;
}

std::vector<uint32_t> Geoset::getFaceGroups() const {
	return faceGroups;
}

uint32_t Geoset::getFacesCount() const {
	return facesCount;
}

std::vector<uint16_t> Geoset::getFaces() const {
	return faces;
}

uint32_t Geoset::getVertexGroupsCount() const {
	return vertexGroupsCount;
}

std::vector<uint8_t> Geoset::getVertexGroups() const {
	return vertexGroups;
}

uint32_t Geoset::getMatrixGroupsCount() const {
	return matrixGroupsCount;
}

std::vector<uint32_t> Geoset::getMatrixGroups() const {
	return matrixGroups;
}

uint32_t Geoset::getMatrixIndicesCount() const {
	return matrixIndicesCount;
}

std::vector<uint32_t> Geoset::getMatrixIndices() const {
	return matrixIndices;
}

uint32_t Geoset::getMaterialId() const {
	return materialId;
}

uint32_t Geoset::getSelectionGroup() const {
	return selectionGroup;
}

uint32_t Geoset::getSelectionFlags() const {
	return selectionFlags;
}

uint32_t Geoset::getLod() const {
	return lod;
}

std::string Geoset::getLodName() const {
	return lodName;
}

Extent Geoset::getExtent() const {
	return extent;
}

uint32_t Geoset::getExtentsCount() const {
	return extentsCount;
}

std::vector<Extent> Geoset::getSequenceExtents() const {
	return sequenceExtents;
}

uint32_t Geoset::getTextureCoordinateSetsCount() const {
	return textureCoordinateSetsCount;
}

std::vector<TextureCoordinateSet> Geoset::getTextureCoordinateSets() const {
	return textureCoordinateSets;
}

void Geoset::setInclusiveSize(uint32_t inclusiveSize) {
	this->inclusiveSize = inclusiveSize;
}

void Geoset::setVertexCount(uint32_t vertexCount) {
	this->vertexCount = vertexCount;
}

void Geoset::setVertexPositions(std::vector<std::array<float, 3>> vertexPositions) {
	this->vertexPositions = vertexPositions;
	this->vertexCount = static_cast<uint32_t>(vertexPositions.size());
}

void Geoset::setNormalCount(uint32_t normalCount) {
	this->normalCount = normalCount;
}

void Geoset::setVertexNormals(std::vector<std::array<float, 3>> vertexNormals) {
	this->vertexNormals = vertexNormals;
	this->normalCount = static_cast<uint32_t>(vertexNormals.size());
}

void Geoset::setFaceTypeGroupsCount(uint32_t faceTypeGroupsCount) {
	this->faceTypeGroupsCount = faceTypeGroupsCount;
}

void Geoset::setFaceTypeGroups(std::vector<uint32_t> faceTypeGroups) {
	this->faceTypeGroups = faceTypeGroups;
	this->faceTypeGroupsCount = static_cast<uint32_t>(faceTypeGroups.size());
}

void Geoset::setFaceGroupsCount(uint32_t faceGroupsCount) {
	this->faceGroupsCount = faceGroupsCount;
}

void Geoset::setFaceGroups(std::vector<uint32_t> faceGroups) {
	this->faceGroups = faceGroups;
	this->faceGroupsCount = static_cast<uint32_t>(faceGroups.size());
}

void Geoset::setFacesCount(uint32_t facesCount) {
	this->facesCount = facesCount;
}

void Geoset::setFaces(std::vector<uint16_t> faces) {
	this->faces = faces;
	this->facesCount = static_cast<uint32_t>(faces.size());
}

void Geoset::setVertexGroupsCount(uint32_t vertexGroupsCount) {
	this->vertexGroupsCount = vertexGroupsCount;
}

void Geoset::setVertexGroups(std::vector<uint8_t> vertexGroups) {
	this->vertexGroups = vertexGroups;
	this->vertexGroupsCount = static_cast<uint32_t>(vertexGroups.size());
}

void Geoset::setMatrixGroupsCount(uint32_t matrixGroupsCount) {
	this->matrixGroupsCount = matrixGroupsCount;
}

void Geoset::setMatrixGroups(std::vector<uint32_t> matrixGroups) {
	this->matrixGroups = matrixGroups;
	this->matrixGroupsCount = static_cast<uint32_t>(matrixGroups.size());
}

void Geoset::setMatrixIndicesCount(uint32_t matrixIndicesCount) {
	matrixIndicesCount = matrixIndicesCount;
}

void Geoset::setMatrixIndices(std::vector<uint32_t> matrixIndices) {
	this->matrixIndices = matrixIndices;
	this->matrixIndicesCount = static_cast<uint32_t>(matrixIndices.size());
}

void Geoset::setMaterialId(uint32_t materialId) {
	this->materialId = materialId;
}

void Geoset::setSelectionGroup(uint32_t selectionGroup) {
	this->selectionGroup = selectionGroup;
}

void Geoset::setSelectionFlags(uint32_t selectionFlags) {
	this->selectionFlags = selectionFlags;
}

void Geoset::setLod(uint32_t lod) {
	this->lod = lod;
}

void Geoset::setLodName(std::string lodName) {
	this->lodName = lodName;
}

void Geoset::setExtent(Extent extent) {
	this->extent = extent;
}

void Geoset::setExtentsCount(uint32_t exentsCount) {
	this->extentsCount = exentsCount;
}

void Geoset::setSequenceExtents(std::vector<Extent> sequenceExtents) {
	this->sequenceExtents = sequenceExtents;
}

void Geoset::setTextureCoordinateSetsCount(uint32_t textureCoordinateSetsCount) {
	this->textureCoordinateSetsCount = textureCoordinateSetsCount;
}

void Geoset::setTextureCoordinateSets(std::vector<TextureCoordinateSet> textureCoordinateSets) {
	this->textureCoordinateSets = textureCoordinateSets;
}