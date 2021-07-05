#include "Helper.h"

using namespace MdxParser;

Helper::Helper() {
	//std::cout << "Empty helper constructed." << std::endl;
}

Helper::Helper(std::istream& stream, uint32_t version)
	: Node{ stream, version } {
	//std::cout << name << " helper constructed." << std::endl;
}

void Helper::write(std::ostream& stream, uint32_t version) {
	Node::write(stream, version);
}

uint32_t Helper::getInclusiveSize() const {
	return nodeInclusiveSize;
}