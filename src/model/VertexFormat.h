/**
*
*    @file VertexFormat.h
*    @date May 8, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __VERTEXTYPES_14942223503900412386_H__
#define __VERTEXTYPES_14942223503900412386_H__

#include "../model/ModelHeader.h"

namespace Game {

class VertexComponent : public VirtualMemory {
    friend class VertexFormat;
    int32_t _iSizeBytes;
    int32_t _iComponentCount;
    GLenum _eDataType;
    VertexUserType::e _eUserType = VertexUserType::e::None;
    int32_t _iLocation;
    int32_t _iByteOffset;
public:
    int32_t getSizeBytes() { return _iSizeBytes; }
    int32_t getComponentCount() { return _iComponentCount; }
    GLenum getDataType() { return _eDataType; }
    GLenum getAttributeType();
    VertexUserType::e getUserType() { return _eUserType; }
    int32_t getByteOffset() { return _iByteOffset; }
    int32_t getLocation() { return _iLocation; }
    t_string getUserTypeName();
};
class VertexFormat : public VirtualMemory {
    std::shared_ptr<GLContext> _pContext = nullptr;
    std::map<int, std::shared_ptr<VertexComponent>> _vecComponents; //mapped to VertexUserType
    int32_t _iVertexSizeBytes;
    t_string _strName;
public:
    VertexFormat(std::shared_ptr<GLContext> pContext, t_string strName);
    virtual ~VertexFormat();
    t_string getName() { return _strName; }
    int32_t getSizeBytes() const { return _iVertexSizeBytes; }
    // void enableAndBindAllArraysForVaoBuffer(std::shared_ptr<VboData> pVboData);
    void addComponent(VertexUserType::e eUserType);
    void addComponent(GLenum type, int componentCount, int size, VertexUserType::e eUserType = VertexUserType::e::None); //ex GL_FLOAT, 3, sizeof(vec4) for an aligned vec3
    std::shared_ptr<VertexComponent> getComponentForUserType(VertexUserType::e eUserType);
    const std::map<int, std::shared_ptr<VertexComponent>>& getComponents() { return _vecComponents; }
    static GLenum computeAttributeType(GLenum type, GLuint count);
    static t_string getUserTypeName(VertexUserType::e t);
    int matchTypeForShaderType(std::shared_ptr<VertexFormat> shaderType);
};

}//NS Game

#endif
