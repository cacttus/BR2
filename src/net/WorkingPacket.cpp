#include "../net/WorkingPacket.h"
#include "../base/GrowBuffer.h"
#include "../net/Packet.h"
#include "../base/Crc32.h"
#include "../net/NetworkUtils.h"
#include "../net/NetworkManager.h"

namespace Game {
///////////////////////////////////////////////////////////////////
WorkingPacket::WorkingPacket(t_string& strPrependTag, Crc32* pCrc32):
 _iLastPacketSize(InvalidPacketSize)
,_strPrependTag(strPrependTag)
,_pCrc32(pCrc32)
{
    _pGrowBuffer = new GrowBuffer<char>(1024);
}
WorkingPacket::~WorkingPacket()
{
    DEL_MEM(_pGrowBuffer);
}
///////////////////////////////////////////////////////////////////
void WorkingPacket::reset()
{
    getGrowBuffer()->clear();
    _iLastPacketSize = InvalidPacketSize;
}
void WorkingPacket::debugPrintPackData(t_string& strPackData)
{

    std::cout<<std::endl;
    for(int i=0; i<strPackData.length(); ++i)
        std::cout<<std::setw(3)<<(unsigned int)(unsigned char)strPackData[i]<<",";
    std::cout<<std::endl<<std::setw(0);
}
RetCode WorkingPacket::tryShitPacket(t_uint64& iPackHeader
                                          , t_uint64& iPackOrdinal
                                          , t_uint64& iPackSize
                                          , t_string& strPackData)
{
    if(_iLastPacketSize == InvalidPacketSize)
        //We haven't parsed the header yet.
        return RetCode::SOCK_NODATA;
    if(getGrowBuffer()->count() != _iLastPacketSize)
        //We haven't parsed the header yet.
        return RetCode::SOCK_NODATA;

    //Base Packet = 
    //[Tag (2)][size(8) ][ord (8)][head (8)][data ...][CRC (4)]
    
    t_uint64 iPackHeadSize = 
        (t_int64)_strPrependTag.length() //Packet::PacketTagSize
        + Packet::PacketLengthSize
        + Packet::PacketOrdinalSizeBytes
        + Packet::PacketHeaderIdSizeBytes 
        ;
    t_uint64 iPackFootSize = 
        Packet::PacketCrcSizeBytes;

    t_uint64 iMinPackSize = iPackHeadSize + iPackFootSize;

    //Copy packet to string.
    strPackData = t_string(getGrowBuffer()->ptr(), getGrowBuffer()->count());
   
    //DEBUG
    t_int64 iDebugSavedPacketSize = _iLastPacketSize;
    t_int64 iDebugActualPacketSize = strPackData.length();
    //DEBUG

    //**NOTE: don't use growbuffer past this line.  It has been reset.
    reset();

    if(strPackData.length() < iMinPackSize)
    {
        BroLogError("Packet was an invalid size.");
        return RetCode::SOCK_READ_INVALID_PACK_SIZE;
    }

    //Check Crc
    t_string strCrc = strPackData.substr(strPackData.length()-Packet::PacketCrcSizeBytes, Packet::PacketCrcSizeBytes);
    t_uint32 uiCrcPacked;
    if(NetworkUtils::unpackUint32(strCrc, uiCrcPacked)==false)
    {
        BroLogError("Packet could not parse CRC value: ", strPackData);
        return RetCode::SOCK_READ_INVALID_CRC_VALUE;
    }

    //Remove CRC footer before computation.
    strPackData = strPackData.substr(0, strPackData.length() - Packet::PacketCrcSizeBytes);

    t_uint32 uiCrcCalc = _pCrc32->compute(strPackData);

    //debugPrintPackData(strPackData);

    //Remove "VT" Tag
    strPackData = strPackData.substr(_strPrependTag.length(), strPackData.length() - _strPrependTag.length());
    
    //Unpack Size (with crc and all headers)
    if(NetworkUtils::unpackUint64(strPackData, iPackSize)==false)
    {
        BroLogError("Packet could not parse Size value: ", strPackData);
        return RetCode::SOCK_READ_INVALID_SIZE_VALUE;
    }

    //Unpack Ordinal
    if(NetworkUtils::unpackUint64(strPackData, iPackOrdinal)==false)
    {
        BroLogError("Packet could not parse Ordinal value: ", strPackData);
        return RetCode::SOCK_READ_INVALID_ORD_VALUE;
    }
    
    //Unpack Header
    if(NetworkUtils::unpackUint64(strPackData, iPackHeader)==false)
    {
        BroLogError("Packet could not parse Header value: ", strPackData);
        return RetCode::SOCK_READ_INVALID_HEADER_VALUE;
    }

    //BroLogInfo("PackSize =", iPackSize, " Ordinal=", iPackOrdinal, " Header=", iPackHeader);
    //**Put CRC last in order to inspect the previous data.
    if (uiCrcPacked != uiCrcCalc)
    {
        BroLogError("Packet CRC32 Failed.");
        //debugPrintPackData(strPackData);
        return RetCode::SOCK_READ_CRC_FAILED;
    }

    return RetCode::SOCK_SUCCESS;
}
void WorkingPacket::tryGetHeader()
{
    t_uint64 lenSize = (t_uint64)_strPrependTag.length() + Packet::PacketLengthSize;
    
    if(_iLastPacketSize != InvalidPacketSize)
        //The packet header [VT][len(8)] has not been parsed.
        return;

    while(_pGrowBuffer->count())
    {
        if(_pGrowBuffer->count()<lenSize)
            return;

        //header VT + len  = 10 chars

        t_uint16 iValidTag = (('V')<<8) | ('T'<<0);

        char v = *((char*)_pGrowBuffer->ptr()+0);
        char t = *((char*)_pGrowBuffer->ptr()+1);
        t_uint16 iSz = (v<<8) | (t<<0);

        //Here: - If the header tag for the message is not correct we shift the data out until either
        // there is no more data, or we find a correct header tag.
        // If the data happens to contain the correct header, but is not an actual message the CRC at the end of the message
        // will filter the message out.
        if(iSz!=iValidTag)
        {
            _pGrowBuffer->shiftOutFirstElement();
        }
        else
        {
            char* pLen = _pGrowBuffer->ptr() + _strPrependTag.length();

            _iLastPacketSize = *((t_int64*)(pLen));
            break;
        }
    }
          
    
}



}//ns Game
