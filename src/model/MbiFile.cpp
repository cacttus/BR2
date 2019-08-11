#include "../base/Base.h"
#include "../base/BinaryFile.h"
#include "../base/Img32.h"
#include "../base/AppBase.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderPipe.h"
#include "../gfx/TexCache.h"
#include "../gfx/LightNode.h"
#include "../gfx/ShadowBox.h"
#include "../gfx/Texture2DSpec.h"
#include "../model/Model.h"
#include "../model/ModelCache.h"
#include "../model/MeshSpec.h"
#include "../model/Material.h"
#include "../model/MbiFile.h"
#include "../world/PhysicsWorld.h"


namespace Game {
///////////////////////////////////////////////////////////////////
MbiFile::MbiFile() {
    _pFile = std::make_shared<BinaryFile>();
}
MbiFile::~MbiFile() {
    _pFile = nullptr;
    //DEL_MEM(_pFile);
}
///////////////////////////////////////////////////////////////////
void MbiFile::parseErr(t_string str, bool bDebugBreak, bool bFatal) {
    t_string strhead = Stz "Error: '" + _fileLoc + "': line " + _pFile->pos() + "\r\n  ";
    str = strhead + str;
    //Throw this if you wnt to have an error in your file.
    if (bFatal) {
        BroThrowException(str);
    }
    else {
        BroLogError(str);
        if (bDebugBreak) {
            Gu::debugBreak();
        }
    }
}
void MbiFile::postLoad() {

    //Compute Bone Boxes
    //Debug: we'll do this every time becasue FUCK!s
    BroLogInfo("  Making Bone Boxes..");
    for (std::shared_ptr<ModelSpec> ms : _vecModels) {
        ms->cacheMeshBones();
    }

}
bool MbiFile::loadAndParse(t_string file) {
    _fileLoc = file;
    std::shared_ptr<BinaryFile> fb = std::make_shared<BinaryFile>();
    Gu::getPackage()->getFile(file, fb, false);

    //    return false;

    fb->rewind();

    //header
    int8_t h0, h1, h2, h3;
    fb->readByte(h0);
    fb->readByte(h1);
    fb->readByte(h2);
    fb->readByte(h3);

    if (h0 != 'M' || h1 != 'B' || h2 != 'I' || h3 != 'H') {
        BroLogError("Invalid file header for MBI1 file.");
        Gu::debugBreak();
        return false;
    }

    //version
    float version;
    fb->readFloat(version);
    if (version != c_fVersion) {
        parseErr(Stz "Version mismatch loaded " + version + " expected " + c_fVersion, true, false);
        return false;
    }

    //models
    int32_t nModels;
    fb->readInt32(nModels);
    for (int32_t iModel = 0; iModel < nModels; ++iModel) {
        std::shared_ptr<ModelSpec> ms = std::make_shared<ModelSpec>();
        ms->deserialize(fb);
        _vecModels.push_back(ms);

        Gu::getModelCache()->addSpec(ms);
    }


    //Read textures
    BroLogInfo("  Loading textures..");
    std::map<Hash32, std::shared_ptr<Texture2DSpec>> texs;
    int32_t nTexs;
    fb->readInt32(nTexs);
    Img32 img;
    for (int32_t iTex = 0; iTex < nTexs; ++iTex) {
        Hash32 hTex;
        fb->readUint32(hTex);

        std::shared_ptr<Texture2DSpec> pTex = std::make_shared<Texture2DSpec>(Gu::getGraphicsContext());
        pTex->deserialize(fb);
        if (Gu::getTexCache()->add(pTex->getLocation(), pTex, false) == false) {
            t_string loc = pTex->getLocation();
            //DEL_MEM(pTex);
            pTex = nullptr;
            pTex = Gu::getTexCache()->getOrLoad(loc);
        }
        texs.insert(std::make_pair(hTex, pTex));
    }
    //Resolve textures.
    BroLogInfo("  Resolving textures..");
    for (std::shared_ptr<ModelSpec> ms : _vecModels) {
        for (std::shared_ptr<MeshSpec> mesh : ms->getMeshes()) {
            if (mesh->getMaterial() != nullptr) {
                for (std::pair<TextureChannel::e, std::shared_ptr<TextureSlot>> p : mesh->getMaterial()->getTextureSlots()) {
                    std::map<Hash32, std::shared_ptr<Texture2DSpec>>::iterator it = texs.find(p.second->_iTexFileHashed);
                    if (it != texs.end()) {
                        p.second->_pTex = it->second;
                    }
                    else {
                        BroLogError("Failed to find packed texture map for MBI mesh material.");
                    }


                }
            }
        }
    }

    //Footer
    fb->readByte(h0);
    fb->readByte(h1);
    fb->readByte(h2);
    fb->readByte(h3);
    //if (false)
    //{
    //    for (std::shared_ptr<ModelSpec> ms : _vecModels) {
    //        for (std::shared_ptr<MeshSpec> me : ms->getMeshes()) {
    //            me->debugPrintDataToStdOut();
    //        }
    //    }

    //}

    if (h0 != 'M' || h1 != 'B' || h2 != 'I' || h3 != 'F') {
        parseErr("Invalid file footer for MBI1 file.", true, false);
        return false;
    }

    postLoad();

    return true;
}
void MbiFile::save(t_string file) {
    _fileLoc = file;
    std::shared_ptr<BinaryFile> fb = std::make_shared<BinaryFile>();
    fb->rewind();

    //header
    fb->writeByte('M');
    fb->writeByte('B');
    fb->writeByte('I');
    fb->writeByte('H');

    //version
    fb->writeFloat(std::move(c_fVersion));

    //models
    fb->writeInt32((int32_t)_vecModels.size());
    for (std::shared_ptr<ModelSpec> ms : _vecModels) {
        ms->serialize(fb);
    }

    //Collect textures
    std::map<Hash32, std::shared_ptr<Texture2DSpec>> texs;
    for (std::shared_ptr<ModelSpec> ms : _vecModels) {
        for (std::shared_ptr<MeshSpec> mesh : ms->getMeshes()) {
            if (mesh->getMaterial() != nullptr) {
                for (std::pair<TextureChannel::e, std::shared_ptr<TextureSlot>> p : mesh->getMaterial()->getTextureSlots()) {
                    if (p.second->_iTexFileHashed != 0) {
                        texs.insert(std::make_pair(p.second->_iTexFileHashed, p.second->_pTex));
                    }
                }
            }
        }
    }

    //Write textures
    fb->writeInt32(std::move((int32_t)texs.size()));
    Img32 img;
    for (std::pair<Hash32, std::shared_ptr<Texture2DSpec>> p : texs) {
        fb->writeUint32(std::move(p.first));
        p.second->serialize(fb);
    }

    //Footer
    fb->writeByte('M');
    fb->writeByte('B');
    fb->writeByte('I');
    fb->writeByte('F');

    t_string dir = FileSystem::getPathFromPath(file);
    FileSystem::createDirectoryRecursive(dir);
    fb->writeToDisk(file);

}

}//ns Game
