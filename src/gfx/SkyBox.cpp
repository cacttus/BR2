#include "../base/Gu.h"
#include "../base/GLContext.h"
#include "../gfx/SkyBox.h"
#include "../gfx/TexCache.h"
#include "../gfx/Atlas.h"
#include "../gfx/RenderUtils.h"
#include "../model/Material.h"
#include "../model/VaoDataGeneric.h"
#include "../model/MeshNode.h"
#include "../model/MeshData.h"
#include "../model/VertexFormat.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/RenderParams.h"

namespace Game {
SkyBox::SkyBox() {
}
SkyBox::~SkyBox() {
}
void SkyBox::init(std::shared_ptr<Atlas> pAtlas, float fBoxDiagonalSize2, bool bDeferred) {
  _pAtlas = pAtlas;
  _fSize = fBoxDiagonalSize2;
  // 2-->3
  // 0-->1
  // x-->  ^ y/z

  std::vector<v_v3n3x2> verts;
  std::vector<v_index32> inds;
  int iOff = 0;
  float siz = fBoxDiagonalSize2;
  float siz2 = siz * 0.5f;
  vec3 du, dv, origin;

  //Top, Sid1, Sid2, Bot
  int eTop = 0;
  int eSid1 = 1;
  int eSid2 = 2;
  int eBot = 3;

  //L
  du = vec3(0, 0, 1);
  dv = vec3(0, 1, 0);
  origin = vec3(-siz2, -siz2, -siz2); //Bottom left of quad as its' facing you
  side(&verts, &inds, iOff, du, dv, siz, origin, eSid1);

  //R
  du = vec3(0, 0, -1);
  dv = vec3(0, 1, 0);
  origin = vec3(siz2, -siz2, siz2);
  side(&verts, &inds, iOff, du, dv, siz, origin, eSid1);

  //B
  du = vec3(1, 0, 0);
  dv = vec3(0, 0, 1);
  origin = vec3(-siz2, -siz2, -siz2);
  side(&verts, &inds, iOff, du, dv, siz, origin, eBot);

  //T
  du = vec3(1, 0, 0);
  dv = vec3(0, 0, -1);
  origin = vec3(-siz2, siz2, siz2);
  side(&verts, &inds, iOff, du, dv, siz, origin, eTop);

  //A
  du = vec3(-1, 0, 0);
  dv = vec3(0, 1, 0);
  origin = vec3(siz2, -siz2, -siz2);
  side(&verts, &inds, iOff, du, dv, siz, origin, eSid2);

  //F
  du = vec3(1, 0, 0);
  dv = vec3(0, 1, 0);
  origin = vec3(-siz2, -siz2, siz2);
  side(&verts, &inds, iOff, du, dv, siz, origin, eSid2);


  if (bDeferred == true) {
    //Make the d_lighting flat shade this.
    for (size_t i = 0; i < verts.size(); ++i) {
      verts[i].n.construct(0, 0, 0);
    }
  }

  std::shared_ptr<MeshData> ms = std::make_shared<MeshData>(
    (void*)verts.data(), verts.size(),
    (void*)inds.data(), inds.size(), v_v3n3x2::getVertexFormat(), nullptr);
  std::shared_ptr<Material> mt = std::make_shared<Material>();
  mt->addTextureBinding(_pAtlas, TextureChannel::Channel0, TextureType::e::Color, 1.0f);
  ms->setMaterial(mt);

  _pMesh = MeshNode::create(ms);


}
void SkyBox::side(std::vector<v_v3n3x2>* verts, std::vector<v_index32>* inds,
  int32_t& iOff, vec3& du, vec3& dv, float siz, vec3& origin, int eMat) {
  v_v3n3x2 v;
  vec3 n;
  vec2 tl, tr, bl, br;

  _pAtlas->getTCoords(eMat, &bl, &br, &tl, &tr);

  n = dv.cross(du).normalize();

  v.v = origin;
  v.n = n;
  v.x = bl;
  verts->push_back(v);
  v.v = origin + du * siz;
  v.n = n;
  v.x = br;
  verts->push_back(v);
  v.v = origin + dv * siz;
  v.n = n;
  v.x = tl;
  verts->push_back(v);
  v.v = origin + du * siz + dv * siz;
  v.n = n;
  v.x = tr;
  verts->push_back(v);


  inds->push_back(iOff + 0);
  inds->push_back(iOff + 1);
  inds->push_back(iOff + 3);
  inds->push_back(iOff + 0);
  inds->push_back(iOff + 3);
  inds->push_back(iOff + 2);
  iOff += 4;

}
void SkyBox::draw(RenderParams& rp) {
  Graphics->pushCullFace();
  Graphics->pushBlend();
  //We can't disable depth testing here unless we put an additional forward stage before thed eferred stage.
 // Gu::pushDepthTest();
  {
    Graphics->enableCullFace(false);
    Graphics->enableBlend(false);
    //glDisable(GL_CULL_FACE);
    //glDisable(GL_BLEND);

    _pMesh->drawDeferred(rp);
    // int wc = 0;
    // int pickid = 0;
    // vec4 x(0,0,0,0);
    // rp.getShader()->setUf("_ufWeightCount", (void*)&wc);
    // rp.getShader()->setUf("_ufPickId", (void*)&pickid);//also, 0
    // _pAtlas->bind(TextureChannel::e::Channel0, rp.getShader());

    // //HACK:hacky here.. should use material
    // rp.getShader()->setUf("_ufSpecColor", (void*)&x);
    // rp.getShader()->setUf("_ufSpecIntensity", (void*)&x);
    // rp.getShader()->setUf("_ufSpecHardness", (void*)&x);
    // rp.getShader()->setUf("_ufMirrorPower", (void*)&x);


    // //rp.addTexture(_pAtlas);
    //// rp.addTexture(Gu::getTexCache()->getOrLoad("./data-dc/spr/barrier-1.png"));
    // rp.setMesh(_pMesh);
    // rp.draw();
  }
  Graphics->popCullFace();
  Graphics->popBlend();
  //  Gu::popDepthTest();

}








}//ns Game
