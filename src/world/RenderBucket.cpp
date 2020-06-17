#include "../base/Gu.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/CameraNode.h"
#include "../gfx/ShaderMaker.h"
#include "../base/GLContext.h"
#include "../model/MeshNode.h"
#include "../model/MeshSpec.h"
#include "../model/Material.h"
#include "../world/PhysicsGrid.h"
#include "../world/RenderBucket.h"
#include "../gfx/LightNode.h"
#include "../gfx/RenderSettings.h"

namespace BR2 {
RenderBucket::RenderBucket() {
}
RenderBucket::~RenderBucket() {
}
bool RenderBucket::hasItems() {
  return
    (_mapObjs.size() > 0) ||
    (_mapGrids.size() > 0) ||
    (_mapDirLights.size() > 0) ||
    (_mapLights.size() > 0) ||
    (_mapPointLights.size() > 0) ||
    (_mapMeshes.size() > 0)
    ;
}
void RenderBucket::start(float maxdist2, std::shared_ptr<CameraNode> n) {
  _mapObjs.clear();
  _mapGrids.clear();

  _mapDirLights.clear();
  _mapLights.clear();
  _mapPointLights.clear();
  _mapMeshes.clear();

  _fMaxDist2 = maxdist2;
  _cam = n;
}
float RenderBucket::distToCam(Box3f* bn) {
  float dist = _cam->getPos().distance2(bn->center());
  return dist;
}
void RenderBucket::addObj(std::shared_ptr<SceneNode> bn) {
  //float fDist2 = (parms->_pFrustum->getNearPlaneCenterPoint() - pGrid->getNodeCenterR3()).length2();
  float dist = distToCam(bn->getBoundBoxObject());
  _mapObjs.insert(std::make_pair(dist, bn));

  //collect the meshes, and sort them
  //TODO remove this in favor of adding meshes (etc) to the modelnode.
  bn->collect(getThis<RenderBucket>());
}
void RenderBucket::addGrid(std::shared_ptr<PhysicsGrid> bn) {
  float dist = distToCam(bn->getBoundBox());
  _mapGrids.insert(std::make_pair(dist, bn));
}
void RenderBucket::collect(std::shared_ptr<SceneNode> bn) {
  //collects renderable items into specific buffers
  if (bn->isHidden() == false) {
    float fDist = distToCam(bn->getBoundBoxObject());
    if (std::dynamic_pointer_cast<LightNodeDir>(bn) != nullptr) {
      _mapDirLights.insert(std::make_pair(fDist, std::dynamic_pointer_cast<LightNodeDir>(bn)));
      _mapLights.insert(std::make_pair(fDist, std::dynamic_pointer_cast<LightNodeBase>(bn)));
    }
    else if (std::dynamic_pointer_cast<LightNodePoint>(bn) != nullptr) {
      _mapPointLights.insert(std::make_pair(fDist, std::dynamic_pointer_cast<LightNodePoint>(bn)));
      _mapLights.insert(std::make_pair(fDist, std::dynamic_pointer_cast<LightNodeBase>(bn)));
    }
    else if (std::dynamic_pointer_cast<MeshNode>(bn) != nullptr) {
      std::shared_ptr<MeshNode> mn = std::dynamic_pointer_cast<MeshNode>(bn);
      if (mn->getMeshSpec()->getMaterial() != nullptr && mn->getMeshSpec()->getMaterial()->getEnableTransparency() && Gu::getRenderSettings()->enableTransparency()) {
        _mapMeshesTransparent.insert(std::make_pair(fDist, std::dynamic_pointer_cast<MeshNode>(bn)));
      }
      else {
        _mapMeshes.insert(std::make_pair(fDist, std::dynamic_pointer_cast<MeshNode>(bn)));
      }
    }
  }
}
void RenderBucket::sortAndDrawMeshes(
  std::function<std::shared_ptr<ShaderBase>(std::shared_ptr<VertexFormat>)> shaderSearch,
  std::function<void(std::shared_ptr<ShaderBase>)> shaderBind,
  std::function<void(std::shared_ptr<ShaderBase>, std::shared_ptr<MeshNode>)> shaderDraw) {
  //Sort drawable objects by shader so that we don't have to call setUf and bind() multiple times.
  _renderMap.clear();

  for (std::pair<float, std::shared_ptr<MeshNode>> p : getMeshes()) {
    if (p.second->getMeshSpec() != nullptr) {
      std::shared_ptr<VertexFormat> fmt = p.second->getMeshSpec()->getVertexFormat();

      std::shared_ptr<ShaderBase> sb = shaderSearch(fmt);
      if (sb != nullptr) {
        RenderMap::iterator it;
        it = _renderMap.find(sb);
        if (it == _renderMap.end()) {
          _renderMap.insert(std::make_pair(sb, std::vector<std::shared_ptr<MeshNode>>()));
          it = _renderMap.find(sb);
        }
        it->second.push_back(p.second);
      }
    }
  }

  for (RenderMap::iterator it = _renderMap.begin(); it != _renderMap.end(); it++) {
    std::shared_ptr<ShaderBase> sb = it->first;
    shaderBind(sb);
    for (std::shared_ptr<MeshNode> n : it->second) {
      shaderDraw(sb, n);
    }
  }
}






}//ns Game
