#include "../model/VboData.h"

namespace Game {
VboData::VboData(std::shared_ptr<GLContext> ct, size_t iElementSize) : GpuBufferData(ct, GL_ARRAY_BUFFER, iElementSize) {
}
VboData::~VboData() {
}


}//ns game
