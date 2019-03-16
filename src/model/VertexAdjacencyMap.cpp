#include "../model/VertexAdjacencyMap.h"


namespace Game {;
//////////////////////////////////////////////////////////////////////////
VertexAdjacencyMap::VertexAdjacencyMap() {
    _vmap = new AdjacencyMap();
}
VertexAdjacencyMap::~VertexAdjacencyMap()
{
    AdjacencyMap::iterator ite1 = _vmap->begin();
    for (; ite1 != _vmap->end(); ite1++)
    {
        FaceList* fl = ite1->second;
        fl->clear();
        delete fl;
    }
    _vmap->clear();
    delete _vmap;//HEAP:
}
//////////////////////////////////////////////////////////////////////////
void VertexAdjacencyMap::addMappingIgnoreDupes(v_index32 index, t_uint32 face_index)
{
    AdjacencyMap::iterator ite1 = _vmap->find(index);
    FaceList* fl;

    if(ite1 == _vmap->end())
    {
        fl = new FaceList();//HEAP:
        _vmap->insert(std::make_pair(index, fl));
    }
    else
        fl = ite1->second;

    // - Ignore duplicate face index if encountered.
    if( fl->find(face_index) != fl->end() )
        return;

    fl->insert(face_index);

}
/**
*    @fn getVertexIndexMapping
*    @brief 
*    @return NULL if the given face list wasn't found. OR a list of faces
*/
VertexAdjacencyMap::FaceList* VertexAdjacencyMap::getVertexIndexMapping(v_index32 index)
{
    FaceList* ret = nullptr ;
    AdjacencyMap::iterator ite = _vmap->find(index);

    if(ite!=_vmap->end()) {
        ret = ite->second;
    }

    return ret;
}


}//ns game
