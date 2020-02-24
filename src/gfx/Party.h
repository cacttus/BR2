/**
*  @file Party.h
*  @date December 21, 2016
*  @author MetalMario971
*/
#pragma once
#ifndef __PARTY_14823443362825418313_H__
#define __PARTY_14823443362825418313_H__

#include "../gfx/GfxHeader.h"
#include "../math/MathAll.h"

namespace BR2 {
class Particle : public VirtualMemory {
public:
  vec3 _v; //velocty
  vec3 _p; //position
  bool _bUsed = false;
  t_timeval _created = 0; //in micros
  t_timeval _life = 0; //life in micros
  float _size; // scale
  vec2 _xbl, _xbr, _xtl, _xtr;//tex coords
  vec3 _g;
  vec3 _vRotNormal;
  float _fRotDelta;
  float _fRotCur;

  //float _a;
};
/**
*  @class ParticleManager
*  @brief Manages particle system.
*/
class Party : public VirtualMemory {
public:

  Party(std::shared_ptr<GLContext> pContext);
  virtual ~Party() override;
  void init(std::shared_ptr<Atlas> pAtlas, int iMaxParticles);
  void make(vec3& pos, int count, Hash32 mat,
    float minSpeed, float maxSpeed,
    float minLife, float maxLife,
    float minScl, float maxScl,
    float minGrav, float maxGrav);
  void draw(std::shared_ptr<ShaderBase> pShader);
  void update(float delta);
private:
  //const int _ciMaxParticles = 1024;
  int _iMaxParticles = 1024;

  std::unique_ptr<Particle[]> _pParticles = nullptr;
  std::shared_ptr<Atlas> _pParticleAtlas = nullptr;
  std::shared_ptr<GLContext> _pContext = nullptr;
  std::unique_ptr<QuadBufferMesh> _pQuadBufferMesh = nullptr;
  // PartyShader* _pPartyShader = nullptr;
  string_t _sAtlasName;
  void reset();

};

}//ns Game



#endif
