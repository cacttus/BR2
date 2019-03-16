#include "../base/Hash.h"
#include "../bottle/WalkerSpec.h"
#include "../bottle/Lair.h"


namespace Game {
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
WalkerSpec::WalkerSpec(t_string strName) : _strName(strName) {
    _iNameHashed = STRHASH(strName);
    _mxHopCount.setMin(0);
    _mxHopCount.setMax(0);
}
WalkerSpec::~WalkerSpec() {
    for (size_t i = 0; i<_vecLairTiles.size(); ++i) {
        LairTile* lt = _vecLairTiles[i];
        DEL_MEM(lt);
    }
    /*       for(size_t i = 0; i < _vecFeatures.size(); ++i)
    delete _vecFeatures[i];
    _vecFeatures.resize(0);*/
}
Walker* WalkerSpec::createInstance() {
    Walker* wi = new Walker(this);
    wi->_uiBranches = _mxBranches.next();
    wi->_uiCount = _mxHopCount.next();

    return wi;
}




}//ns Game
