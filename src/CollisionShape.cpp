#include "CollisionShape.h"

using namespace MdxParser;

CollisionShape::CollisionShape() {
	//std::cout << "Empty collision shape constructed." << std::endl;
}

CollisionShape::CollisionShape(std::istream& stream, uint32_t version) {

	std::streampos streamStartPosition{ stream.tellg() };

	Node::read(stream, version);
	stream.read((char*)&type, sizeof(type));

	//std::cout << name << " collision shape constructed with a type of " << type << "." << std::endl;

	int verticesCount;
	switch (type) {
		case 0:
			verticesCount = 2;
			break;
		case 1:
			verticesCount = 2;
			break;
		case 2:
			verticesCount = 1;
			break;
		case 3:
			verticesCount = 2;
			break;
	}

	for (int i{ 0 }; i < verticesCount; i++) {
		std::array<float, 3> vertex;
		stream.read((char*)&vertex, sizeof(vertex));
		vertices.push_back(vertex);
		//std::cout << "Vertex " << i << " is positioned at {" << vertex.at(0) << ", " << vertex.at(1) << ", " << vertex.at(2) << "}." << std::endl;
	}

	if (type == 2 || type == 3) {
		stream.read((char*)&radius, sizeof(radius));
		//std::cout << "Radius is " << radius << "." << std::endl;
	}

	std::streampos streamEndPosition{ stream.tellg() };
	inclusiveSize = static_cast<int>(streamEndPosition) - static_cast<int>(streamStartPosition);
	//std::cout << "Inclusive size is " << inclusiveSize << "." << std::endl;

	//std::cout << name << " collision shape constructed." << std::endl;
}

void CollisionShape::write(std::ostream& stream, uint32_t version) {

	Node::write(stream, version);
	stream.write((char*)&type, sizeof(type));

	for (std::array<float, 3> vertex : vertices) {
		stream.write((char*)&vertex, sizeof(vertex));
	}

	if (type == 2 || type == 3) {
		stream.write((char*)&radius, sizeof(radius));
	}
}

uint32_t CollisionShape::getInclusiveSize() const {
	return inclusiveSize;
}

uint32_t CollisionShape::getType() const {
	return type;
}

std::vector<std::array<float, 3>> CollisionShape::getVertices() const {
	return vertices;
}

float CollisionShape::getRadius() const {
	return radius;
}
