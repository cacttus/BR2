#include "../base/BaseHeader.h"
#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/Gu.h"
#include "../base/ApplicationPackage.h"
#include "../base/GraphicsWindow.h"
#include "../base/InputManager.h"  
#include "../gfx/ShaderBase.h"  
#include "../gfx/ShaderMaker.h"  
#include "../gfx/CameraNode.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/Texture2DSpec.h"
#include "../gfx/TexCache.h"
#include "../bottle/WorldSelect.h"
#include "../bottle/W25GridMesh.h"
#include "../bottle/BottleScript.h"
#include "../model/MeshNode.h"
#include "../model/MeshUtils.h"
#include "../world/Scene.h"

namespace BR2 {
WorldSelect::WorldSelect(std::shared_ptr<BottleScript> cr) {
  _pCongaRoom = cr;
}
WorldSelect::~WorldSelect() {
  _pQuadMeshBackground = nullptr;
}
void WorldSelect::gatherWorlds() {
  if (_pQuadMeshBackground == nullptr) {
    _pQuadMeshBackground = MeshUtils::createScreenQuadMesh(
      _pCongaRoom->getScene()->getActiveCamera()->getViewport()->getWidth(), _pCongaRoom->getScene()->getActiveCamera()->getViewport()->getHeight());
    _pTex = Gu::getTexCache()->getOrLoad(TexFile("Test tex", Gu::getPackage()->makeAssetPath("textures", "test_tex3.png")));
  }

  string_t gsd = _pCongaRoom->getGameSaveDir();
  FileSystem::getAllDirs(gsd, _vecWorldFolders);
  for (size_t iw = 0; iw < _vecWorldFolders.size(); ++iw) {
    string_t wf = _vecWorldFolders[iw];
    wf = FileSystem::getDirectoryFromPath(FileSystem::getPathFromPath(wf));
    _vecWorldFolders[iw] = wf;
  }
}
void WorldSelect::update() {
  if (StringUtil::isNotEmpty(_strSelectedWorld)) {
    if (_bLoadNextFrame == false) {
      _bLoadNextFrame = true;
    }
    else {
      _pCongaRoom->loadOrCreateGame(_strSelectedWorld);
    }
  }
}
string_t WorldSelect::genNewWorldName(string_t baseName) {
  //Get a new world name
  string_t strNewWorld = "NewWorld";
  int iNewWorldIdx = 0;
  bool bFound = true;
  string_t strNewWorldIdx;
  int nIter = 0;
  while (bFound) {
    bFound = false;
    strNewWorldIdx = strNewWorld + TypeConv::intToStr(iNewWorldIdx);
    for (string_t wf : _vecWorldFolders) {
      if (StringUtil::equalsi(wf, strNewWorldIdx)) {
        bFound = true;
        iNewWorldIdx++;
      }
    }
    nIter++;
  }
  return strNewWorldIdx;
}
void WorldSelect::deleteWorld(string_t worldName) {
  string_t gameDir = _pCongaRoom->getGameDirectory(worldName);
  std::vector<string_t> fileExts{ ".dat", ".g" };
  FileSystem::deleteDirectoryRecursive(gameDir, fileExts);
}
void WorldSelect::updateTouch(std::shared_ptr<InputManager> pFingers, float dt) {
  //#ifdef _DEBUG
      //Go Straight to a new world.
  _strSelectedWorld = genNewWorldName("NewWorld");
  //#endif

  if (_eWorldSelectState == WorldSelectState::e::ConfirmDeleteAll) {
    if (pFingers->keyPress(SDL_SCANCODE_Y)) {
      for (string_t wf : _vecWorldFolders) {
        deleteWorld(wf);
      }
      gatherWorlds();
      _eWorldSelectState = WorldSelectState::Select;
    }
    else if (pFingers->keyPress(SDL_SCANCODE_N)) {
      _eWorldSelectState = WorldSelectState::Select;
    }
  }
  else if (_eWorldSelectState == WorldSelectState::e::Select) {
    if (pFingers->keyPress(SDL_SCANCODE_0)) {
      _eWorldSelectState = WorldSelectState::e::ConfirmDeleteAll;
    }
    else if (pFingers->keyPress(SDL_SCANCODE_1)) {
      _strSelectedWorld = genNewWorldName("NewWorld");
    }
    else {
      for (int32_t ikey = SDL_SCANCODE_1; ikey <= SDL_SCANCODE_9; ++ikey) {
        if (pFingers->keyPress((SDL_Scancode)ikey)) {
          size_t ind = ikey - SDL_SCANCODE_1 - 1;
          if (ind < _vecWorldFolders.size()) {
            _strSelectedWorld = _vecWorldFolders[ind];
          }
        }
      }
    }
  }
}
void WorldSelect::draw2d() {
  drawText();
  // _pContext->getTextBoss()->pstrb(TStr("0: New World"), bx, by + cy); cy += 30;
}
void WorldSelect::drawBackgroundImage() {
  std::shared_ptr<CameraNode> bc = _pCongaRoom->getScene()->getActiveCamera();
  Gu::getShaderMaker()->getImageShader_F()->setCameraUf(bc);
  Gu::getShaderMaker()->getImageShader_F()->beginRaster(bc->getViewport()->getWidth(), bc->getViewport()->getHeight());
  {
    //We want depth test so we can see what's in front.
    //glEnable(GL_DEPTH_TEST);
    _pTex->bind(TextureChannel::e::Channel0, Gu::getShaderMaker()->getImageShader_F());

    Gu::getShaderMaker()->getImageShader_F()->draw(_pQuadMeshBackground);
  }
  Gu::getShaderMaker()->getImageShader_F()->endRaster();
}
void WorldSelect::drawText() {
  //int bx = 40;
  //int by = 40;
  //int cx = 0;
  //int cy = 0;
  //if (StringUtil::isNotEmpty(_strSelectedWorld)) {
  //    FontSize iFontHeight = 40;
  //    Box2f quad;

  //    Gu::getTextBoss()->setFontSize(iFontHeight);
  //    Gu::getTextBoss()->setColor(vec4(0.9877, 0.9778, 0.969, 1));

  //    string_t strLoad = TStr("Loading ", _strSelectedWorld, "..");

  //    float dx = Gu::getTextBoss()->getCenteredTextX(strLoad);
  //    float dy = Gu::getTextBoss()->getCenteredTextY(strLoad);

  //    Gu::getTextBoss()->pstrb(TStr(strLoad), dx, dy);
  //}
  //else {
  //    Gu::getTextBoss()->setColor(vec4(0.9877, 0.9778, 0.969, 1));
  //    Gu::getTextBoss()->setFont(std::string("Nunito"));

  //    //Title - testing centering
  //    Gu::getTextBoss()->setFontSize(40);
  //    string_t strTitle = "Animal Motors v0.01";
  //    float titleX = Gu::getTextBoss()->getCenteredTextX(strTitle);
  //    Gu::getTextBoss()->pstrb(TStr(strTitle), titleX, by + cy); cy += 40;

  //    Gu::getTextBoss()->setFontSize(30);
  //    if (_eWorldSelectState == WorldSelectState::Select) {
  //        Gu::getTextBoss()->pstrb(TStr("Select World by typing key"), bx, by + cy); cy += 30;
  //        Gu::getTextBoss()->setFontSize(28);
  //        Gu::getTextBoss()->pstrb(TStr("1: *New World*"), bx, by + cy); cy += 30;

  //        for (size_t n = 0; n < _vecWorldFolders.size(); ++n) {
  //            Gu::getTextBoss()->pstrb(TStr(n + 2, " :", _vecWorldFolders[n], "   ", (int)STRHASH(_vecWorldFolders[n]) % 100, "%%"), bx, by + cy); cy += 30;
  //        }
  //        Gu::getTextBoss()->pstrb(TStr(""), bx, by + cy); cy += 30;
  //        Gu::getTextBoss()->pstrb(TStr("0: **Delete All Worlds"), bx, by + cy); cy += 30;
  //    }
  //    else  if (_eWorldSelectState == WorldSelectState::ConfirmDeleteAll) {
  //        Gu::getTextBoss()->pstrb(TStr("Delete All Worlds ? (Y/N)"), bx, by + cy); cy += 30;
  //        Gu::getTextBoss()->setFontSize(28);
  //    }
  //}
}
}//ns Game
