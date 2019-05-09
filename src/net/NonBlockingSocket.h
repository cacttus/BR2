// - 5/2009
#pragma once
#ifndef __NONBLOCKINGSOCKET_H__
#define __NONBLOCKINGSOCKET_H__

#include "../net/NetHeader.h"

namespace Game {; 
/**
*    Base clases for scokeets
*    SERVER SOCKET
    CLIENT SOCKET
        VIRTUAL CLIENT - server side socket that allows us to communicate to client
        
*/
class NonBlockingSocket : public VirtualMemory {
protected:
    NetworkWorker* _pNetworkManager;
    //SocketState        socketState;    // - The state of the socket.
    std::vector<SocketState> stateList;    // - List of state changes. Helps with debug
    fd_set socketFD;            // - The FD for this socket.
    timeval socketPollVal;        // - The value of polling, will be zero.
    SOCKET nbsocket;            // - The socket.
    SOCKADDR_IN socketAddr;            // - The socket address (of the local or remote client/server).
    t_int64 lastSend;            // - The last time data was sent.
    t_int64 lastRecv;            // - The last time data was recv'd.

    mm_allocator<char> _scratchBuffer;

    t_string getIPStr();         // - Returns a string of the ip address of this socket.
    t_string getPortStr();       // - Returns a string of the port number (of this socket).
    
    PacketMakerBase* _pPacketMaker;

    RetCode receiveData(char* dataBuf, t_int64 dataBufLen, t_int64* readSize);// - Make sure to poll for reading before calling this.
    RetCode sendData(const char* dataBuf, t_int64 dataBufLen, t_int64* outBytes = NULL);//DO NOT USE
public:

    NonBlockingSocket(NetworkWorker* pwn);
    virtual ~NonBlockingSocket() override ;

    virtual RetCode setSockOpts()=0; // - Make sure to use inet_addr() when passing in a string address like "10.0.0.1"
    SOCKET& getWsaSocket(){ return nbsocket; }
    RetCode closeSocket(); // - Close the socket an nullify all values
    
    RetCode pollForRead();  // - Return true if this socket can be read from. http://msdn.microsoft.com/en-us/library/ms740141(VS.85).aspx
    RetCode pollForWrite(); // - Return true if this socket can be written to (if you can call sendData). http://msdn.microsoft.com/en-us/library/ms740141(VS.85).aspx
    
    RetCode receiveData(StreamBuffer<char>* pBuf);// - Make sure to poll for reading before calling this.
    RetCode sendData(StreamBuffer<char>* pBuf);//DO NOT USE
    
    //this probably isn't needed.. ?
    // PacketMaker - handles packets.
    // This class sends raw data.
    RetCode sendData(PacketType __in_ ePackType, const t_string& __in_ strData);

    void getNextPacket(Packet* __out_ pack);
    RetCode updateComm();
    void assertState(SocketState state);// - Assert the socket state.
    SocketState getState();// - Return the state of the socket
    void setState( SocketState state );
    VectorEx<SocketState>& getStateList();// - Return the state list.
    void printStateList();

};




}//ns game

#endif
