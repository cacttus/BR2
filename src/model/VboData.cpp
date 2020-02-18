
#include "../model/VboData.h"

namespace BR2 {
VboData::VboData(std::shared_ptr<GLContext> ct, size_t iElementSize) : GpuBufferData(ct, GL_ARRAY_BUFFER, iElementSize) {
}

VboData::~VboData() {
}
}//ns BR2
