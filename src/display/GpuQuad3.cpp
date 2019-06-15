#include "../display/GpuQuad3.h"
#include "../math/MathAll.h"
#include "../base/TStr.h"
#include "../model/VertexFormat.h"
namespace Game {
GpuQuad3::GpuQuad3()
{
    for (int i = 0; i < 4; ++i) {
        _vertex[i] = NULL;
    }
}
GpuQuad3::~GpuQuad3() {
}
void GpuQuad3::debugValidate()
{
#ifdef _DEBUG
    for (int i = 0; i < 4; ++i) {
        AssertOrThrow2(_vertex[i] != NULL);
    }
#endif
}
void GpuQuad3::copyQuadPos(Quad3f* pQuad)
{
    debugValidate();

    _vertex[0]->v = pQuad->p0(); 
    _vertex[1]->v = pQuad->p1();
    _vertex[2]->v = pQuad->p2();
    _vertex[3]->v = pQuad->p3();
}
void GpuQuad3::copyQuadNormal(vec3* n)
{
    debugValidate();

    _vertex[0]->n = *n;
    _vertex[1]->n = *n;
    _vertex[2]->n = *n;
    _vertex[3]->n = *n;
}

void GpuQuad3::copyQuadTcoords(Box2f* pQuad)
{
    debugValidate();
    _vertex[3]->x.x = pQuad->tl().x;
    _vertex[3]->x.y = pQuad->tl().y;
    _vertex[2]->x.x = pQuad->br().x;
    _vertex[2]->x.y = pQuad->tl().y;
    _vertex[1]->x.x = pQuad->tl().x;
    _vertex[1]->x.y = pQuad->br().y;
    _vertex[0]->x.x = pQuad->br().x;
    _vertex[0]->x.y = pQuad->br().y;

}
void GpuQuad3::copyQuadColor(Color4f* color)
{
    debugValidate();
    //Color is for font color

    //_vertex[0]->c = *(static_cast<Vec4f*>(color));
    //_vertex[1]->c = *(static_cast<Vec4f*>(color));
    //_vertex[2]->c = *(static_cast<Vec4f*>(color));
    //_vertex[3]->c = *(static_cast<Vec4f*>(color));
}

}//ns game
