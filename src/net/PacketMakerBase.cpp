#include "../net/PacketMakerBase.h"
#include "../net/NonBlockingSocket.h"
#include "../net/WorkingPacket.h"
#include "../base/GrowBuffer.h"
#include "../net/Packet.h"
#include "../base/GlobalUtils.h"
#include "../scene/EngineSystemParams.h"
#include "../net/NetworkWorker.h"
#include "../net/NetworkUtils.h"
#include "../base/DynamicBuffer.h"
#include "../base/Crc32.h"
#include "../mem/StreamBuffer.h"
#include "../base/Gu.h"

namespace Game {
///////////////////////////////////////////////////////////////////
PacketMakerBase::PacketMakerBase(NonBlockingSocket* pSocket) :
    _pSocket(pSocket)
    , _iCurrentOrdinal(0)
{
    t_int32 iMaxOutgoingBufferSizeBytes = Gu::getEngineConfig()->getMaxSoftwareOutgoingBufferSizeBytes();
    t_int32 iMaxIncomingBufferSizeBytes = Gu::getEngineConfig()->getMaxSoftwareOutgoingBufferSizeBytes();
    _pCrc32 = new Crc32();

    _pIncomingPacket = new WorkingPacket(NetworkWorker::getPacketPrependTag(), _pCrc32);
    _pIncomingDataBuffer = new StreamBuffer<char>(iMaxOutgoingBufferSizeBytes);
    _pOutgoingDataBuffer = new StreamBuffer<char>(iMaxIncomingBufferSizeBytes);

}
PacketMakerBase::~PacketMakerBase()
{
    for (t_vecsize i = 0; i < _vecIncomingPackets.count(); i++)
        DEL_MEM(_vecIncomingPackets[i]);
    _vecIncomingPackets.resize(0);

    for (t_vecsize i = 0; i < _vecOutgoingPackets.count(); i++)
        DEL_MEM(_vecOutgoingPackets[i]);
    _vecOutgoingPackets.resize(0);

    DEL_MEM(_pIncomingPacket);
    DEL_MEM(_pIncomingDataBuffer);
    DEL_MEM(_pOutgoingDataBuffer);
    DEL_MEM(_pCrc32);
}
///////////////////////////////////////////////////////////////////
//RetCode PacketMakerBase::getNextPacket(Packet* __out_ out_pack)
//{
//    RetCode ret = updateSocketComm();
//
//    out_pack = getNextPacket();
//
//    return SOCK_SUCCESS;
//}
t_uint32 PacketMakerBase::computeCrc32(const t_string& str)
{
    return _pCrc32->compute(str);
}
RetCode PacketMakerBase::updateSocketComm()
{
    RetCode ret;
    ret = updateReadSocket();
    if (ret != SOCK_SUCCESS && ret != SOCK_NODATA)
        return ret;

    ret = updateWriteSocket();
    if (ret != SOCK_SUCCESS && ret != SOCK_NODATA)
        return ret;

    return SOCK_SUCCESS;
}
void PacketMakerBase::getNextPacket(Packet* __out_ out_pack)
{
    Packet* pop = popPacket();
    if (pop != NULL)
    {
        pop->copyTo(out_pack);
        DEL_MEM(pop); //HEAP
    }
}
Packet* PacketMakerBase::popPacket()
{
    Packet* ret = NULL;
    if (_vecIncomingPackets.count() == 0)
        return ret;

    ret = _vecIncomingPackets[0];
    _vecIncomingPackets.erase(_vecIncomingPackets.begin());

    return ret;
}
void PacketMakerBase::pushPacket(Packet* p)
{
    _vecIncomingPackets.add(p);
}
RetCode PacketMakerBase::updateReadSocket()
{
    RetCode ret;

    AssertOrThrow2(_pSocket != NULL);
    
    do
    {
        ret = _pSocket->receiveData(_pIncomingDataBuffer);

        if (ret == SOCK_NODATA && _pIncomingDataBuffer->getUsedCount() == 0)
            return SOCK_SUCCESS;//no more data
        if (ret == SOCK_BUFFER_FULL)
            return SOCK_SUCCESS;
        if (ret != SOCK_SUCCESS)
            return ret; //error

        addReceivedDataToPacket();

    } while (ret == SOCK_SUCCESS && _pIncomingDataBuffer->getUsedCount() > 0);

    return SOCK_SUCCESS;
}
void PacketMakerBase::addReceivedDataToPacket()
{
    char c;
    t_bool bEndOfBuffer;

    for (int iByte = 0; iByte < _pIncomingDataBuffer->getUsedCount(); ++iByte)
    {
        bEndOfBuffer = _pIncomingDataBuffer->readStream(c);

        if (bEndOfBuffer == false)
            _pIncomingPacket->getGrowBuffer()->add(c);

        checkAddPacket();

        if (bEndOfBuffer == true)
        {
            //NOTE: the databuffer here SHOULD be equal to iUsedCount
            if (iByte != _pIncomingDataBuffer->getUsedCount() - 1)
                BroLogWarn("Problem reading network data - Databuffer wasn't maxed out but somehow we hit the end of the buffer.");
            break;
            //Data buffer maxed out.
            //Not an error.
        }
    }
}
t_bool PacketMakerBase::checkAddPacket()
{
    //TCP stream: Adds a packet after receiving all data.
    t_uint64 iHeaderId;
    t_uint64 iOrdinal;
    t_string strRawData;
    t_uint64 iSize;
    RetCode ret;

    _pIncomingPacket->tryGetHeader();

    ret = _pIncomingPacket->tryShitPacket(iHeaderId, iOrdinal, iSize, strRawData);

    if (ret == RetCode::SOCK_NODATA)
    {
        return false;
    }
    else if (ret != SOCK_SUCCESS)
    {
        //An error occurred we logged it already, ignore, but debug here
        int n = 0;
        n++;
    }
    else if (ret == RetCode::SOCK_SUCCESS)
    {
        //Create a new packet with the parsed data
        Packet* pack = new Packet();
        pack->_iHeaderId = iHeaderId;
        pack->_iOrdinal = iOrdinal;
        if (strRawData.length() > 0)
        {
            pack->_pData = new DynamicBuffer<char>(Packet::PacketDefaultChunkSize);
            pack->_pData->write(strRawData.c_str(), strRawData.length());
        }
        pack->_iTotalSizeBytes = iSize;
        //_pClient is set downstraem
        pushPacket(pack);
    }
    else
    {
        BroLogError("Failed to understand socket return code from tryShitPacket:", (int)ret);
    }
    return true;
}
/**
*    @fn
*    @brief Fill out the _strPacketData and _headerId ONLY ** we fill in the rest of this information.
*/
//RetCode PacketMakerBase::sendData(NetPackType __in_ ePackType, const t_string& __in_ strData)
//{
//    t_uint64 iPackid = _pNetworkManager->getPackId(ePackType);
//    return sendData(iPackid, strData);
//}
RetCode PacketMakerBase::sendData(t_uint64 __in_ iPackId, const t_string& __in_ strData)
{
    Packet* newPack = makeOutgoingPacket(iPackId, strData);
    _vecOutgoingPackets.add(newPack);

    return RetCode::SOCK_SUCCESS;
}
Packet* PacketMakerBase::makeOutgoingPacket(t_uint64 __in_ iniPackId, const t_string& __in_ strPackDataNoHeader)
{
    t_string strPrependTag = NetworkWorker::getPacketPrependTag();
    t_uint64 iPackId = iniPackId;
    t_string strPackHead = NetworkUtils::packUint64(iPackId);
    t_string strPackData = strPackHead + strPackDataNoHeader;
    t_string strOrd = NetworkUtils::packUint64(++_iCurrentOrdinal);

    t_int64 iPackSize_Size = 8; //size of the pack size value
    t_int64 iPackSize_Crc32 = 4;
    t_string strPackSize =
        NetworkUtils::packUint64(
            strPrependTag.length()
            + iPackSize_Size
            + strOrd.length()
            + strPackData.length()
            + iPackSize_Crc32
        );

    t_string strPacketWithoutCrc = strPrependTag + strPackSize + strOrd + strPackData;
    t_uint32 uiCrc = computeCrc32(strPacketWithoutCrc);
    t_string strCrc = NetworkUtils::packUint32(uiCrc);

    t_string strFinalPacket = strPacketWithoutCrc + strCrc;

    Packet* pRet = new Packet();//HEAP
    pRet->setData(strFinalPacket);

    return pRet;
}
void PacketMakerBase::addDataToOutgoingBuffer()
{
    //Fill outgoing network buffer with packet data.
    t_string data;
    DynamicBuffer<char>* packData = NULL;
    Packet* pack = NULL;

    while (_vecOutgoingPackets.count() > 0
           && _pOutgoingDataBuffer->getFreeCount() > 0)
    {
        //Delete packet if we're empty.
        if (data.length() == 0)
        {
            if (pack != NULL)
            {
                _vecOutgoingPackets.erase(_vecOutgoingPackets.begin());
                DEL_MEM(pack); //HEAP
            }

            if (_pOutgoingDataBuffer->getFreeCount() > 0)
            {
                pack = _vecOutgoingPackets[0];
                packData = _vecOutgoingPackets[0]->getData();
                if (packData != NULL)
                    data = packData->toString();
                else
                    return;
            }
        }
        else
        {
            _pOutgoingDataBuffer->writeStream(data);
        }
    }

}
RetCode PacketMakerBase::updateWriteSocket()
{
    RetCode ret = SOCK_SUCCESS;

    AssertOrThrow2(_pSocket != NULL);

    while(ret==SOCK_SUCCESS
          &&  !_pOutgoingDataBuffer->getIsWriteFull()
          )
    {
        addDataToOutgoingBuffer();

        ret = _pSocket->sendData(_pOutgoingDataBuffer);

        if (ret != SOCK_SUCCESS)
            break; //error
    }

    return ret;
}


}//ns Game

