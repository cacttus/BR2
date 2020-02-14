/**
*  @file Atlas.h
*  @date December 9, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __ATLAS_14813066743083963786_H__
#define __ATLAS_14813066743083963786_H__

#include "../gfx/GfxHeader.h"
#include "../gfx/Texture2DSpec.h"
#include "../math/MathAll.h"
namespace BR2 {

namespace SpriteFlags { typedef enum { 
    Shift_V = 0x01
    , Shift_H = 0x02 
} e; }

class AtlasSprite {
    std::shared_ptr<Atlas> _pAtlas = nullptr;
public:
    Hash32 _hash;
    AtlasSprite(std::shared_ptr<Atlas> pa) : _pAtlas(pa) { }
    std::shared_ptr<Atlas> getAtlas() { return _pAtlas; }
    string_t _imgLoc;
    ivec2 _viGridPos;
    int _iId;
    std::shared_ptr<Img32> _pGeneratedImage = nullptr;
    //BwMat::e _eMat;
    bool getIsGenerated() { return _pGeneratedImage != nullptr; }

};
/**
*  @class Atlas
*  @brief
*
*/
class Atlas : public Texture2DSpec {
    typedef std::map<Hash32, std::shared_ptr<AtlasSprite>> ImgMap;
    ImgMap _mapImages;
    string_t _strName;
    ivec2 _vSpriteSize;
    ivec2 _vGridSize;
    string_t _strPrecompileFileLocation;//This is only if the atlas is pre-compiled.

    std::shared_ptr<Img32> composeImage(bool bCache);
    string_t getCachedImageFilePath();
    time_t cacheGetGreatestModifyTimeForAllDependencies();
    std::shared_ptr<Img32> tryGetCachedImage();
    void printInfoAndErrors(std::shared_ptr<Img32>);
    void finishCompile(std::shared_ptr<Img32> sp, bool bMipmaps);
    void addImagePrecompiled(int32_t ix, int32_t iy);
public:
    static string_t getGeneratedFileName();
    const string_t& getName() { return _strName; }

    Atlas(std::shared_ptr<GLContext> ct, string_t name, ivec2& viSpriteSize, string_t strImageLoc);
    Atlas(std::shared_ptr<GLContext> ct, string_t name, ivec2& ivGridSize);
    virtual ~Atlas() override;

    static string_t constructPrecompiledSpriteName(int32_t ix, int32_t iy);

    void addImage(Hash32 en, string_t loc);
    void addImage(Hash32 en, string_t loc, std::shared_ptr<Img32> imgData);

    void removeImage(std::shared_ptr<AtlasSprite> ps);
    void compileFiles(bool bMipmaps = true, bool saveAndLoad = true); // Compiles an atlas from a list of sprite files.
    void compilePrecompiled(bool bMipmaps = true);//Compiles an atlas from a precompiled atlas witha ll the sprites already stitched togheter.
    void getTCoords(ivec2* pos, vec2* __out_ bl, vec2* __out_ br, vec2* __out_ tl, vec2* __out_ tr, bool bHalfPixelPadding = false);
    void getTCoords(Hash32 emat, vec2* __out_ bl, vec2* __out_ br, vec2* __out_ tl, vec2* __out_ tr, bool bHalfPixelPadding = false);
    std::shared_ptr<AtlasSprite> getSprite(Hash32 en);

    ivec2 getSpriteSize() { return _vSpriteSize; }
    ivec2 getGridSize() { return _vGridSize; }
    int32_t getLinearTileOffset(Hash32 frameId);
};

}//ns Game



#endif
