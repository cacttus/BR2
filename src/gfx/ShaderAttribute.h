/**
*  @file ShaderAttribute.h
*  @date May 5, 2017
*  @author MetalMario971
*/
#pragma once
#ifndef __SHADERATTRIBUTE_1494032876996691758_H__
#define __SHADERATTRIBUTE_1494032876996691758_H__

#include "../gfx/GfxHeader.h"
#include "../model/ModelHeader.h"

namespace BR2 {
/**
*    @class ShaderAttribute
*    @brief
*
*/
class ShaderAttribute : public VirtualMemory {
    friend class GlslShaderBase;
public:
    enum { NoAttribLocationFound = -1 };
private:
    string_t _strName;
    Hash32 _iNameHashed = 0;
    int32_t _iGLLocation = -1;    //shader location
    GLenum _eGLAttribType;
    GLsizei _iGLAttribSize;//*Note this isn't the "components" But would return "1" for FLOAT_VEC3 etc.
    string_t _strError;
    //We need to still store the datatype 
    VertexUserType::e _eUserType = VertexUserType::e::None;
    VertexUserType::e parseUserType(string_t& name);
    void parseAttribute(std::shared_ptr<ShaderBase> pShaderBase, int32_t attribIndex);
    bool _bIsBuiltInAttrib = false;
    static bool isOpenGLBuiltInAttrib(string_t strName);
public:
    ShaderAttribute(std::shared_ptr<ShaderBase> pShaderBase, int32_t attribIndex);
    bool isGLSLBuiltInAttrib(){return _bIsBuiltInAttrib;}

    // VertexComponentType getComponentType() { return _eVertexComponentType; }
    const string_t& getName() { return _strName; }
    string_t getError() { return _strError; }
    Hash32 getNameHashed() { return _iNameHashed; }
    int32_t getGlLocation() { return _iGLLocation; } // layout=0 ,.. etc
    GLenum getGlAttributeType() { return _eGLAttribType; }//returns GL_FLOAT_VEC3 for _V301, etc.
    GLsizei getGlAttributeSize() { return _iGLAttribSize; } //*Note this isn't the "components"
    VertexUserType::e getUserType() { return _eUserType; } //Our custom system type which gets matched in the VAO.

};
}//ns Game



#endif
