#include "TracksChunk.h"

#include <limits>

using namespace MdxParser;

template <typename T> TracksChunk<T>::TracksChunk() 
	: TracksChunk("") {
}

template <typename T> TracksChunk<T>::TracksChunk(std::string tagValue) :
	tag{ tagValue },
	tracksCount{ 0 },
	interpolationType{ 0 },
	globalSequenceId{ std::numeric_limits<uint32_t>::max() } {
}

template <typename T> TracksChunk<T>::TracksChunk(std::string tagValue, std::istream &stream) 
	: TracksChunk(tagValue) {
	stream.read((char*)&tracksCount, sizeof(tracksCount));
	stream.read((char*)&interpolationType, sizeof(interpolationType));
	stream.read((char*)&globalSequenceId, sizeof(globalSequenceId));

	for (unsigned int i{ 0 }; i < tracksCount; i++) {
		Track<T> track{ stream, interpolationType };
		tracks.push_back(track);
	}

	//std::cout << tag << " tracks chunk constructed." << std::endl;
}

template <typename T> void TracksChunk<T>::write(std::ostream& stream, uint32_t version) {
	stream.write(tag.c_str(), 4);
	stream.write((char*)&tracksCount, sizeof(tracksCount));
	stream.write((char*)&interpolationType, sizeof(interpolationType));
	stream.write((char*)&globalSequenceId, sizeof(globalSequenceId));

	for (Track<T> track : tracks) {
		track.write(stream, version, interpolationType);
	}
}

template <typename T> void TracksChunk<T>::addTrack(Track<T> track) {
	tracks.push_back(track);
	tracksCount = static_cast<uint32_t>(tracks.size());
}

template <typename T> void TracksChunk<T>::addTrack(int32_t frame, T value, T inTan, T outTan) {
	Track<T> track{ frame, value, inTan, outTan };
	tracks.push_back(track);
	tracksCount = static_cast<uint32_t>(tracks.size());
}

template <typename T> std::string TracksChunk<T>::getTag() const {
	return tag;
}

template <typename T> uint32_t TracksChunk<T>::getTracksCount() const {
	return tracksCount;
}

template <typename T> uint32_t TracksChunk<T>::getInterpolationType() const {
	return interpolationType;
}

template <typename T> uint32_t TracksChunk<T>::getGlobalSequenceId() const {
	return globalSequenceId;
}

template <typename T> std::vector<Track<T>>& TracksChunk<T>::getTracks() {
	return tracks;
}

template <typename T> void TracksChunk<T>::setInterpolationType(uint32_t interpolationType) {
	this->interpolationType = interpolationType;
}
template <typename T> void TracksChunk<T>::setGlobalSequenceId(uint32_t globalSequenceId) {
	this->globalSequenceId = globalSequenceId;
}

template class TracksChunk<uint32_t>;
template class TracksChunk<float>;
template class TracksChunk<std::array<float, 3>>;
template class TracksChunk<std::array<float, 4>>;