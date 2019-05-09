#include "../net/Packet.h"
#include "../net/NetworkManager.h"

namespace Game {
///////////////////////////////////////////////////////////////////
Packet::Packet()
{
    reset();
    //Do not Pre-Init the data. If we're empty then keep it that way.                 
    //This saves Memory
    //_pData = new DynamicBuffer<char>(1024);
}
Packet::~Packet()
{
    DEL_MEM(_pData);
}
///////////////////////////////////////////////////////////////////
DynamicBuffer<char>* Packet::getData() { return _pData; }//Must return reference**
t_uint64 Packet::getOrdinal() { return _iOrdinal; }
t_uint64 Packet::getTotalSizeBytes() { return _iTotalSizeBytes; }
t_bool Packet::getHasData() { return (_pData != NULL) && (_pData->getIsEmpty()==false); }
t_uint64 Packet::getPacketTypeId() { return _iHeaderId; }
void Packet::copyTo(Packet* pOther)
{
    pOther->_iHeaderId = _iHeaderId;
    pOther->_iOrdinal = _iOrdinal;
    if (pOther->_pData)
    {
        if (_pData == NULL)
            _pData = new DynamicBuffer<char>(PacketDefaultChunkSize);
        pOther->_pData->copyFrom(_pData);
    }
}
//NetPackType Packet::getPacketType()
//{ 
//    //This can be further optimized by just storing the type on the packet.
//    return _pNetworkManager->getPackType(getPacketTypeId());
//}
void Packet::setData(t_string& strData)
{
    if(_pData!=NULL)
        DEL_MEM(_pData);

    _pData = new DynamicBuffer<char>(Packet::PacketDefaultChunkSize);
    _pData->write(strData.c_str(), strData.length());
}
void Packet::reset()
{
   _iHeaderId = 0;   // Packet Header Id parsed from 8 values. [xxxxxxxx]
   _iOrdinal = 0;   // Packet Id Order as appears on the client who sent hte packet. - Increment this this is a standin for time - packet time not totally reliable.
   _iTotalSizeBytes = 0;
   _pData = NULL;//May be Null
   _pClient = NULL; // May be NULL

}
}//ns Game
