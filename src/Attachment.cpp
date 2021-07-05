#include "Attachment.h"

#include <vector>

using namespace MdxParser;

Attachment::Attachment() {
	std::cout << "Empty attachment constructed." << std::endl;
}

Attachment::Attachment(std::istream& stream, uint32_t version) {

	std::streampos streamStartPosition{ stream.tellg() };

	stream.read((char*)&inclusiveSize, sizeof(inclusiveSize));
	Node::read(stream, version);

	std::vector<char> pathBuffer(260);
	stream.read(reinterpret_cast<char*>(pathBuffer.data()), 260);
	path = std::string{ pathBuffer.begin(), pathBuffer.end() };

	stream.read((char*)&attachmentId, sizeof(attachmentId));

	std::streampos streamEndPosition{ static_cast<unsigned int>(streamStartPosition) + inclusiveSize };

	while (stream.tellg() < streamEndPosition) {

		// Read Tag
		std::vector<char> tagBuffer(4);
		stream.read(reinterpret_cast<char*>(tagBuffer.data()), 4);
		std::string tag{ tagBuffer.begin(), tagBuffer.end() };

		if (tag == "KATV") {
			visibility = TracksChunk<float>{ tag, stream };
		}
	}

	stream.seekg(streamEndPosition);
}

void Attachment::write(std::ostream& stream, uint32_t version) {

	stream.write((char*)&inclusiveSize, sizeof(inclusiveSize));
	Node::write(stream, version);
	stream.write(path.c_str(), 260);
	stream.write((char*)&attachmentId, sizeof(attachmentId));

	// Write Optional Track Chunks
	if (visibility.getTracksCount() > 0) {
		visibility.write(stream, version);
	}
}

uint32_t Attachment::getInclusiveSize() const {
	return inclusiveSize;
}

std::string Attachment::getPath() const {
	return path;
}

uint32_t Attachment::getAttachmentId() const {
	return attachmentId;
}

TracksChunk<float> Attachment::getVisibility() const {
	return visibility;
}
