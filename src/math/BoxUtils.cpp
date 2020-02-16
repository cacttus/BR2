#include "../math/MathHeader.h"
#include "../math/BoxUtils.h"
#include "../math/Vec3x.h"
#include "../gfx/GfxHeader.h"

namespace BR2 {
///////////////////////////////////////////////////////////////////


vec3 BoxUtils::normalForSide(BoxSide::e side){
    vec3 vret;
    switch(side){
    case BoxSide::e::Right:  vret.construct( 1,  0,  0); break;
    case BoxSide::e::Left:   vret.construct(-1,  0,  0); break;
    case BoxSide::e::Bottom: vret.construct(0 , -1,  0); break;
    case BoxSide::e::Top:    vret.construct(0 ,  1,  0); break;
    case BoxSide::e::Back:   vret.construct(0 ,  0, -1); break;
    case BoxSide::e::Front:  vret.construct(0 ,  0,  1); break;
        default: Br2ThrowNotImplementedException();           break;
    };

    return vret;
}

vec3 BoxUtils::normalUpForSide(BoxSide::e side){
    vec3 vret;
    switch(side){
    case BoxSide::e::Right:  vret.construct(0, 1, 0); break;
    case BoxSide::e::Left:   vret.construct(0, 1, 0); break;
    case BoxSide::e::Bottom: vret.construct(0, 0, 1); break;
    case BoxSide::e::Top:    vret.construct(0, 0, 1); break;
    case BoxSide::e::Back:   vret.construct(0, 1, 0); break;
    case BoxSide::e::Front:  vret.construct(0, 1, 0); break;
        default: Br2ThrowNotImplementedException();           break;
    };

    return vret;
}
int32_t BoxUtils::cubeSideToGlCubeMapEnum(BoxSide::e side){
    int32_t ret;
    switch(side){

    case BoxSide::e::Left: ret = GL_TEXTURE_CUBE_MAP_NEGATIVE_X; break;
    case BoxSide::e::Right: ret = GL_TEXTURE_CUBE_MAP_POSITIVE_X; break;
    case BoxSide::e::Bottom: ret = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y; break;
    case BoxSide::e::Top: ret = GL_TEXTURE_CUBE_MAP_POSITIVE_Y; break;
    case BoxSide::e::Back: ret = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; break;
    case BoxSide::e::Front: ret = GL_TEXTURE_CUBE_MAP_POSITIVE_Z; break;

        default: Br2ThrowNotImplementedException();           break;
    };

    return ret;
}





}//ns Game
