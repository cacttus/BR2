#include "../net/NetworkUtils.h"


namespace Game {;

//////////////////////////////////////////////////////////////////////////
t_string NetworkUtils::GetSockErrName( RetCode ret ) {
    t_string rval;
    switch(ret)
    {
    case SOCK_SUCCESS                   :rval.assign("SOCK_SUCCESS"); break;
    case SOCK_BINDFAILED                :rval.assign("SOCK_BINDFAILED"); break;
    case SOCK_ALREADYCLOSED             :rval.assign("SOCK_ALREADYCLOSED"); break;
    case SOCK_INUSE                     :rval.assign("SOCK_INUSE"); break;
    case SOCK_COULDNOTSET               :rval.assign("SOCK_COULDNOTSET"); break;
    case SOCK_LISTENERR                 :rval.assign("SOCK_LISTENERR"); break;
    case SOCK_ACCEPTERR                 :rval.assign("SOCK_ACCEPTERR"); break;
    case SOCK_RECVERR                   :rval.assign("SOCK_RECVERR"); break;
    case SOCK_SENDERR                   :rval.assign("SOCK_SENDERR"); break;
    case SOCK_POLLFAIL                  :rval.assign("SOCK_POLLFAIL"); break;
    case SOCK_STARTUPERR                :rval.assign("SOCK_STARTUPERR"); break;
    case SOCK_NOCONNAVAIL               :rval.assign("SOCK_NOCONNAVAIL"); break;
    case SOCK_CONNCLOSE                 :rval.assign("SOCK_CONNCLOSE"); break;
    case SOCK_ARRAYOUTOFBOUNDS          :rval.assign("SOCK_ARRAYOUTOFBOUNDS"); break;
    case SOCK_NODATA                    :rval.assign("SOCK_NODATA"); break;
    case SOCK_CANNOTBEREAD              :rval.assign("SOCK_CANNOTBEREAD"); break;
    case SOCK_SELERROR                  :rval.assign("SOCK_SELERROR"); break;
    case SOCK_TRUE                      :rval.assign("SOCK_TRUE"); break;
    case SOCK_FALSE                     :rval.assign("SOCK_FALSE"); break;
    case SOCK_DISCONNECT                :rval.assign("SOCK_DISCONNECT"); break;
    case SOCK_CONNERR                   :rval.assign("SOCK_CONNERR"); break;
    case SOCK_CONNPENDING               :rval.assign("SOCK_CONNPENDING"); break;
    case SOCK_TIMEOUT                   :rval.assign("SOCK_TIMEOUT"); break;
    case SOCK_ERR_ADDRINUSE             :rval.assign("SOCK_ERR_ADDRINUSE"); break;
    case SOCK_READ_INVALID_PACK_SIZE    :rval.assign("SOCK_READ_INVALID_PACK_SIZE"); break;
    case SOCK_READ_INVALID_CRC_VALUE    :rval.assign("SOCK_READ_INVALID_CRC_VALUE"); break;
    case SOCK_READ_INVALID_SIZE_VALUE   :rval.assign("SOCK_READ_INVALID_SIZE_VALUE"); break;
    case SOCK_READ_INVALID_ORD_VALUE    :rval.assign("SOCK_READ_INVALID_ORD_VALUE"); break;
    case SOCK_READ_INVALID_HEADER_VALUE :rval.assign("SOCK_READ_INVALID_HEADER_VALUE"); break;
    case SOCK_READ_CRC_FAILED           :rval.assign("SOCK_READ_CRC_FAILED"); break;          



    default                        :rval.assign("Unrecognized Error.  No desc avail."); break;
    }
    return rval;
}
t_string NetworkUtils::GetSockErrStr( RetCode ret ) {
    t_string rval;
    switch(ret)
    {
    case SOCK_SUCCESS                   :rval.assign("Success."); break;
    case SOCK_BINDFAILED                :rval.assign("Binding failed."); break;
    case SOCK_ALREADYCLOSED             :rval.assign("Tried to close the scket when the socket is already closed."); break;
    case SOCK_INUSE                     :rval.assign("The socket is being used."); break;
    case SOCK_COULDNOTSET               :rval.assign("A parameter on the socket could not be set."); break;
    case SOCK_LISTENERR                 :rval.assign("There was an error listening on this socket."); break;
    case SOCK_ACCEPTERR                 :rval.assign("There was an error in accepting a connection."); break;
    case SOCK_RECVERR                   :rval.assign("There was an error in receiving data."); break;
    case SOCK_SENDERR                   :rval.assign("There was an error in sending data."); break;
    case SOCK_POLLFAIL                  :rval.assign("There was an error polling a socket."); break;
    case SOCK_STARTUPERR                :rval.assign("Error in starting up winsock."); break;
    case SOCK_NOCONNAVAIL               :rval.assign("No connections are available when trying to accept connections, this is not necessarily an error, it just means there aren't any pending."); break;
    case SOCK_CONNCLOSE                 :rval.assign("The connection was closed by the client."); break;
    case SOCK_ARRAYOUTOFBOUNDS          :rval.assign("Program Error, an array was accessed out of bounds."); break;
    case SOCK_NODATA                    :rval.assign("There was no data at the socket to be read, this is not necessarily an error."); break;
    case SOCK_CANNOTBEREAD              :rval.assign("The socket could not be read from, it is in a blocking state."); break;
    case SOCK_SELERROR                  :rval.assign("An error in calling select."); break;
    case SOCK_TRUE                      :rval.assign("TRUE"); break;
    case SOCK_FALSE                     :rval.assign("FALSE"); break;
    case SOCK_DISCONNECT                :rval.assign("The socket was disconnected during the call."); break;
    case SOCK_CONNERR                   :rval.assign("There was an error during the connection process"); break;
    case SOCK_CONNPENDING               :rval.assign("Returned when a client socket is currently connecting to another socket."); break;
    case SOCK_TIMEOUT                   :rval.assign("The socket operation timed out."); break;
    case SOCK_ERR_ADDRINUSE             :rval.assign("The socket was already in use.");
    case SOCK_READ_INVALID_PACK_SIZE    :rval.assign("Socket Read Error: Invalid Header Packet Size.");
    case SOCK_READ_INVALID_CRC_VALUE    :rval.assign("Socket Read Error: Invalid Header CRC Value.");
    case SOCK_READ_INVALID_SIZE_VALUE   :rval.assign("Socket Read Error: Invalid Header Size Value.");
    case SOCK_READ_INVALID_ORD_VALUE    :rval.assign("Socket Read Error: Invalid Header Ord Value.");
    case SOCK_READ_INVALID_HEADER_VALUE :rval.assign("Socket Read Error: Invalid Header Header Value.");
    case SOCK_READ_CRC_FAILED           :rval.assign("Socket Read Error: CRC failed.");
    
    
    default                     :rval.assign("Unrecognized Error.  No desc avail."); break;

    }
    return t_string(rval+t_string("(")+GetSockErrName(ret)+t_string(")"));
}
t_string NetworkUtils::GetSockStateName( SocketState ret )
{
    t_string rval;
    switch(ret)
    {
    case SOCK_CLOSED:       rval.assign("SOCK_CLOSED"); break;
    case SOCK_CREATED:      rval.assign("SOCK_CREATED"); break;
    case SOCK_OPEN :        rval.assign("SOCK_OPEN"); break;
    case SOCK_LISTENING:    rval.assign("SOCK_LISTENING"); break;
    case SOCK_CONNECTING:   rval.assign("SOCK_CONNECTING"); break;
    case SOCK_CONNECTED:    rval.assign("SOCK_CONNECTED"); break;
    case SOCK_RECEIVING:    rval.assign("SOCK_RECEIVING"); break;
    default:                rval.assign("Unrecognized State."); break;
    }
    return rval;
}
t_string NetworkUtils::GetSockStateDesc( SocketState ret )
{
    t_string rval;
    switch(ret)
    {
    case SOCK_CLOSED:       rval.assign("The socket has been closed"); break;
    case SOCK_CREATED:      rval.assign("The initial state of a socket when it is instantiated."); break;
    case SOCK_OPEN :        rval.assign("the socket is bound and accepted, but is not receiving data."); break;
    case SOCK_LISTENING:    rval.assign("The socket is listening [server only]."); break;
    case SOCK_CONNECTING:   rval.assign("socket is connecting to the server [client only]/"); break;
    case SOCK_CONNECTED:    rval.assign("The socket is connected to the server [client only]."); break;
    case SOCK_RECEIVING:    rval.assign("The socket is receiving data [server only]."); break;
    default:                rval.assign("Unrecognized State.  No desc avail."); break;
    }
    return t_string(rval+t_string("(")+GetSockStateName(ret)+t_string(")"));
}

/**
*    @fn getIPFromHost()
*    @brief Returns the IP address of the given hostname.
*    @remarks Will NOT work unless Winsock has started with WSAStartup.
*/
t_string NetworkUtils::getIPFromHost( t_string hostName ) {
    hostent* d = gethostbyname(hostName.c_str());

    if( d==NULL ) 
        return t_string("");

    long l=( 
        ntohl( 
            *((unsigned long*)(d)->h_addr)
        )
    );
    unsigned char* c = (unsigned char*)&l;

    t_string ret;
    char cbuf[16];

    memset(cbuf,0,16);
    ret+= itoa( *(c+3), cbuf, 10 );
    
    ret+=".";

    memset(cbuf,0,16);
    ret+= itoa( *(c+2), cbuf, 10 );
    
    ret+=".";
    
    memset(cbuf,0,16);
    ret+= itoa( *(c+1), cbuf, 10 );
    
    ret+=".";
    
    memset(cbuf,0,16);
    ret+= itoa( *(c+0), cbuf, 10 );

    return ret;
}

}//ns game
