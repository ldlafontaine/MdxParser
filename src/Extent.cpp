#include "Extent.h"

#include <stdint.h>
#include <iostream>

using namespace MdxParser;

Extent::Extent() :
	boundsRadius{ 0 },
	maximum{ 0 },
	minimum{ 0 } {
}

Extent::Extent(std::istream& stream) : Extent() {
	stream.read((char*)&boundsRadius, sizeof(boundsRadius));
	stream.read((char*)&minimum, sizeof(minimum));
	stream.read((char*)&maximum, sizeof(maximum));
}

void Extent::write(std::ostream& stream, uint32_t version) {
	stream.write((char*)&boundsRadius, sizeof(boundsRadius));
	stream.write((char*)&minimum, sizeof(minimum));
	stream.write((char*)&maximum, sizeof(maximum));
}

void Extent::setBoundsRadius(float boundsRadius) {
	this->boundsRadius = boundsRadius;
}

void Extent::setMinimum(std::array<float, 3> minimum) {
	this->minimum = minimum;
}

void Extent::setMaximum(std::array<float, 3> maximum) {
	this->maximum = maximum;
}

uint32_t Extent::getInclusiveSize() const {
	return sizeof(boundsRadius) + sizeof(minimum) + sizeof(maximum);
}

float Extent::getBoundsRadius() const {
	return boundsRadius;
}

std::array<float, 3> Extent::getMinimum() const {
	return minimum;
}

std::array<float, 3> Extent::getMaximum() const {
	return maximum;
}