/*
    To connect to server enter the command as shown below
    $ ./client <server_ip_address> <port_number>
    Transferring files between Client & Server.
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include "MTUtils.H"
#include "Client.H"
using namespace std;

/*
Client::Client(string ipAddressParm,
               int portNumParm)
:mTrackerPortNum(portNumParm)
,mTrackerIpAddress(ipAddressParm)
,pServer(NULL)
{
    bzero((char*)&mServerAddress, sizeof(sockaddr_in));
    //bzero(mDataBuffer, BUFFER_SIZE);
}*/

Client::Client()
//:pServer(NULL)
{
    //bzero((char*)&mServerAddress, sizeof(sockaddr_in));
    //bzero(mDataBuffer, BUFFER_SIZE);
}



Client::~Client()
{
    // Good Bye !!!
}


// connect to the server
/*int Client::connectToServer()
{
    int sSocketFd = connectToServer(mTrackerIpAddress, mTrackerPortNum);
    return sSocketFd;
}*/


int Client::connectToServer(string ipAddressParm, int portNumParm)
{
    // fetch the server ip address from the input
    struct hostent* pServer;
    pServer = gethostbyname(ipAddressParm.c_str());
    if (NULL == pServer)
    {
        error("[ERROR] Invalid Server IP ADDRESS (or) NULL");
    }

    // create a socket
    int sSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sSocketFd < 0)
    {
        error("[ERROR] Opening a socket");
    }

    struct sockaddr_in sServerAddress;
    bzero((char*)&sServerAddress, sizeof(sockaddr_in));


    sServerAddress.sin_family = AF_INET;
    bcopy((char*)pServer->h_addr,
          (char*)&sServerAddress.sin_addr.s_addr,
          pServer->h_length);
    sServerAddress.sin_port = htons(portNumParm);

    // connect to the Tracker server
    int sServerAddrLen = sizeof(sockaddr_in);
    if (connect(sSocketFd,
                (struct sockaddr*)&sServerAddress,
                sServerAddrLen) < 0)
    {
        error("[ERROR] Node: Failed connecting to server");
    }

    return sSocketFd;
}


// disconnect to the server
void Client::disconnectFromServer(int sockectFdParm)
{
    // disconnect from the Server using the socket file descriptor
    close(sockectFdParm);
}
