//#include "../base/Base.h"
//
//#include "../math/Random.h"
//#include "../gfx/ParticleMaker.h"
//#include "../gfx/Atlas.h"
//#include "../gfx/QuadBufferMesh.h"
//#include "../gfx/GpuQuad3.h"
////#include "../gfx/PartyShader.h"
//#include "../gfx/CameraNode.h"
//#include "../gfx/FrustumBase.h"
//#include "../gfx/RenderParams.h"
//#include "../model/VertexFormat.h"
//namespace BR2 {
/////////////////////////////////////////////////////////////////////
//ParticleMaker::ParticleMaker() {
//}
//ParticleMaker::~ParticleMaker() {
//  //  DEL_MEM(_pQuadBufferMesh);
////    DEL_MEM(_pParticleAtlas);
//   // delete [] _pParticles;
//   // DEL_MEM(_pPartyShader);
//}
/////////////////////////////////////////////////////////////////////
//void ParticleMaker::init(std::shared_ptr<Atlas> pAtlas, int iMaxParticles) {
//  _iMaxParticles = iMaxParticles;
//  _pParticles = std::make_unique<Particle[]>(_iMaxParticles);
//
//  _pParticleAtlas = pAtlas;
//
//  _pQuadBufferMesh = std::make_unique<QuadBufferMesh>(Gu::getContext(), _iMaxParticles + 1);
//
//  // _pPartyShader= new PartyShader(_pContext);
//  // _pPartyShader>load("./data/party.vs", "./data/party.ps");
//  reset();
//}
//void ParticleMaker::reset() {
//  t_timeval tv = Gu::getMicroSeconds();
//  for (int i = 0; i < _iMaxParticles; ++i) {
//    _pParticles[i]._bUsed = false;
//  }
//}
//void ParticleMaker::make(vec3& pos, int count, Hash32 mat,
//  float minSpeed, float maxSpeed,
//  float minLife, float maxLife,
//  float minScl, float maxScl,
//  float minGrav, float maxGrav
//) {
//  std::shared_ptr<AtlasSprite> ps = nullptr;
//  vec2 xbl, xbr, xtl, xtr;
//  t_timeval tv = Gu::getMicroSeconds();
//
//  ps = _pParticleAtlas->getSprite((Hash32)mat);
//  if (ps != nullptr) {
//    ps->getAtlas()->getTCoords(&(ps->_viGridPos), &xbl, &xbr, &xtl, &xtr);
//  }
//  int iCount = 0;
//
//  for (int i = 0; i < _iMaxParticles; ++i) {
//    if (_pParticles[i]._bUsed == false) {
//      _pParticles[i]._bUsed = true;
//      t_timeval life = (t_timeval)((minLife + (maxLife - minLife) * Random::frand01()) * 1000000);
//      _pParticles[i]._life = life;
//      _pParticles[i]._created = tv;
//      _pParticles[i]._p = pos;
//      _pParticles[i]._size = minScl + (maxScl - minScl) * Random::frand01();
//      _pParticles[i]._xbl = xbl;
//      _pParticles[i]._xbr = xbr;
//      _pParticles[i]._xtl = xtl;
//      _pParticles[i]._xtr = xtr;
//
//      _pParticles[i]._vRotNormal = Random::rv311().normalize();
//      _pParticles[i]._fRotDelta = Random::frand01() * (float)M_2PI;
//      _pParticles[i]._fRotCur = Random::frand01() * (float)M_2PI;
//
//      _pParticles[i]._g = vec3(0, minGrav + (maxGrav - minGrav) * Random::frand01(), 0);
//      float fspd = minSpeed + (maxSpeed - minSpeed) * Random::frand01();
//      vec3 vel = Random::rv311().normalize() * fspd;
//      _pParticles[i]._v = vel;
//      iCount++;
//    }
//
//    if (iCount == count) {
//      break;
//    }
//  }
//}
//void ParticleMaker::update(float delta) {
//  Quad3f worldQuad;
//  Box2f tex;
//  std::shared_ptr<CameraNode> bc = Gu::getCamera();
//  vec3 normal;// = bc->getViewNormal() *-1;
//  GpuQuad3 pQuad;
//  // std::shared_ptr<FrustumBase> pFrustum = bc->getFrustum();
//  t_timeval tv = Gu::getMicroSeconds();
//
//  //vec3 view = bc->getViewNormal();
//  vec3 right = vec3(1, 0, 0);// bc->getRightNormal();
//  vec3 up = vec3(0, 1, 0);// bc->getUpNormal();
//
//  mat4 mRot;
//
//  if (_pParticles == nullptr) {
//    //Not initialized
//    return;
//  }
//
//  //TODO: optimize by making separate array sof used and not
//  for (int ipt = 0; ipt < _iMaxParticles; ++ipt) {
//    if (_pParticles[ipt]._bUsed == true) {
//      float lifeVal = (float)(tv - _pParticles[ipt]._created) / (float)_pParticles[ipt]._life;
//
//      if (lifeVal >= 1.0f) {
//        _pParticles[ipt]._bUsed = false;
//      }
//      else {
//        //update Velocity, Pos, and Gravity
//        _pParticles[ipt]._p += _pParticles[ipt]._v * delta;
//        _pParticles[ipt]._v += _pParticles[ipt]._g * delta;
//
//        //Calcualate Rotation
//        _pParticles[ipt]._fRotCur = fmodf(_pParticles[ipt]._fRotCur + _pParticles[ipt]._fRotDelta * delta, (float)M_2PI);
//        mRot = mat4::getRotationRad(_pParticles[ipt]._fRotCur, _pParticles[ipt]._vRotNormal);
//
//        normal = mRot * vec4(0, 0, -1, 1);
//
//        //Calculate Size
//        float sz = _pParticles[ipt]._size * (1.0f - lifeVal);
//
//        // Compute Particle Quad
//        _pQuadBufferMesh->getGpuQuad(pQuad);
//
//        //Note: the winding in the quad buffer is backwards 20161222
//        pQuad._vertex[0]->v = -right * sz - up * sz;//bl
//        pQuad._vertex[1]->v = right * sz - up * sz;//br
//        pQuad._vertex[2]->v = -right * sz + up * sz;//tl
//        pQuad._vertex[3]->v = right * sz + up * sz;//tr
//
//        //Rotate vertex, then translate
//        for (int iv = 0; iv < 4; ++iv) {
//          pQuad._vertex[iv]->v = mRot * vec4(pQuad._vertex[iv]->v, 1);
//          pQuad._vertex[iv]->v += _pParticles[ipt]._p;
//          pQuad._vertex[iv]->n = normal; //Normals - Assume here that normal gets normalized in the shader.
//        }
//
//        //Tex Coords
//        pQuad._vertex[0]->x = _pParticles[ipt]._xbl;
//        pQuad._vertex[1]->x = _pParticles[ipt]._xbr;
//        pQuad._vertex[2]->x = _pParticles[ipt]._xtl;
//        pQuad._vertex[3]->x = _pParticles[ipt]._xtr;
//
//        _pQuadBufferMesh->nextQuad();
//      }
//    }
//  }
//}
//void ParticleMaker::draw(std::shared_ptr<ShaderBase> pShader) {
//
//  _pQuadBufferMesh->copyToGpu(_pQuadBufferMesh->getQuadCount());
//  Graphics->pushCullFace();
//  {
//    glDisable(GL_CULL_FACE);
//    _pParticleAtlas->bind(TextureChannel::e::Channel0, pShader);
//    _pQuadBufferMesh->draw(RenderParams(pShader));
//  }
//  Graphics->popCullFace();
//  _pQuadBufferMesh->resetCopy();
//}
//
//
//
//
//
//
//
//}//ns BR2
