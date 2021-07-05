#include "Track.h"

using namespace MdxParser;

template <typename T> Track<T>::Track(int32_t frame, T value, T inTan, T outTan) :
	frame{ frame },
	value{ value },
	inTan{ inTan },
	outTan{ outTan } {
}

template <typename T> Track<T>::Track(std::istream& stream, uint32_t interpolationType) {
	read(stream, interpolationType);
}

template <typename T> void Track<T>::read(std::istream& stream, uint32_t interpolationType) {
	stream.read((char*)&frame, sizeof(frame));
	stream.read((char*)&value, sizeof(value));

	if (interpolationType > 1) {
		stream.read((char*)&inTan, sizeof(inTan));
		stream.read((char*)&outTan, sizeof(outTan));
	}
}

template <typename T> void Track<T>::write(std::ostream& stream, uint32_t version, uint32_t interpolationType) {
	stream.write((char*)&frame, sizeof(frame));
	stream.write((char*)&value, sizeof(value));

	if (interpolationType > 1) {
		stream.write((char*)&inTan, sizeof(inTan));
		stream.write((char*)&outTan, sizeof(outTan));
	}
}

template <typename T> int32_t Track<T>::getFrame() const {
	return frame;
}

template <typename T> T Track<T>::getValue() {
	return value;
}

template <typename T> T Track<T>::getInTan() {
	return inTan;
}

template <typename T> T Track<T>::getOutTan() {
	return outTan;
}

template <typename T> void Track<T>::setFrame(int32_t frame) {
	this->frame = frame;
}

template <typename T> void Track<T>::setValue(T value) {
	this->value = value;
}

template <typename T> void Track<T>::setInTan(T inTan) {
	this->inTan = inTan;
}

template <typename T> void Track<T>::setOutTan(T outTan) {
	this->outTan = outTan;
}

template class Track<uint32_t>;
template class Track<float>;
template class Track<std::array<float, 3>>;
template class Track<std::array<float, 4>>;