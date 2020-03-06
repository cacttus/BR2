/**
*  @file WorldSelect.h
*  @date 20200305
*  @author MetalMario971
*/
#pragma once
#ifndef __WORLDSELECT_15834678604150061997_H__
#define __WORLDSELECT_15834678604150061997_H__

#include "../bottle/BottleHeader.h"

namespace BR2 {
/**
*  @class WorldSelect
*  @brief Essentially, this is just a UIScreen.  TODO: turn this into a UiScript.
*/
namespace WorldSelectState { typedef enum { Select, ConfirmDeleteAll } e; }

class WorldSelect : public VirtualMemory {
public:
  WorldSelect(std::shared_ptr<BottleScript> cr);
  virtual ~WorldSelect() override;

  void gatherWorlds();
  void drawBackgroundImage();
  void draw2d();
  void update();
  void updateTouch(std::shared_ptr<InputManager>, float dt);

private:
  std::shared_ptr<MeshNode> _pQuadMeshBackground = nullptr;
  std::shared_ptr<BottleScript> _pCongaRoom = nullptr;
  std::vector<string_t> _vecWorldFolders;
  WorldSelectState::e _eWorldSelectState = WorldSelectState::e::Select;
  std::shared_ptr<Texture2DSpec> _pTex = nullptr;
  string_t _strSelectedWorld = "";
  bool _bLoadNextFrame = false;
  string_t genNewWorldName(string_t baseName);
  void deleteWorld(string_t worldName);
  void drawText();
};

}//ns Game



#endif
