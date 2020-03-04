#include "../base/Hash.h"
#include "../bottle/Lair.h"

namespace BR2 {
Lair::Lair(LairSpec* ps) : _nGlobs(0), _pLairSpec(ps) {
  //DELETE_VECTOR_ELEMENTS(_vecSpawnPoints);
}
Lair::~Lair() {
}

LairSpec::LairSpec(string_t name) : _nGlobs(0)
{
    _iNameHashed = STRHASH(name);
    _name = name;
    //for (size_t i = 0; i < _vecSpawnPoints.size(); ++i) {
    //    DEL_MEM(_vecSpawnPoints[i]);
    //}
    _pClimateParams = new ClimateSpec();
}
LairSpec:: ~LairSpec()  {
    for(size_t i=0; i<_vecLairTiles.size(); ++i){
        LairTile* lt = _vecLairTiles[i];
        DEL_MEM(lt);
    }
    DEL_MEM(_pClimateParams);
}


}//ns Game
