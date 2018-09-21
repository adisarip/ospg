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
#include "Client.H"
using namespace std;


Client::Client(string ipAddressParm,
               int portNumParm)
:mTrackerIpAddress(ipAddressParm)
,mTrackerPortNum(portNumParm)
,mpServer(NULL)
{
    bzero((char*)&mServerAddress, sizeof(sockaddr_in));
    bzero(mDataBuffer, BUFFER_SIZE);
}


Client::~Client()
{
    // Good Bye !!!
}


// connect to the server
int Client::connectToServer()
{
    // fetch the server ip address from the input
    mpServer = gethostbyname(mTrackerIpAddress.c_str());
    if (NULL == mpServer)
    {
        error("[ERROR] Invalid Server IP ADDRESS (or) NULL");
    }

    // create a socket
    int sSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sSocketFd < 0)
    {
        error("[ERROR] Opening a socket");
    }

    mServerAddress.sin_family = AF_INET;
    bcopy((char*)mpServer->h_addr,
          (char*)&mServerAddress.sin_addr.s_addr,
          mpServer->h_length);
    mServerAddress.sin_port = htons(mTrackerPortNum);

    // connect to the Tracker server
    int sServerAddrLen = sizeof(sockaddr_in);
    if (connect(sSocketFd,
                (struct sockaddr*)&mServerAddress,
                sServerAddrLen) < 0)
    {
        error("[ERROR] Node: Failed connecting to server");
    }

    return sSocketFd;
}

void Client::disconnectFromServer(int sockectFdParm)
{
    // disconnect from the Server using the socket file descriptor
    close(sockectFdParm);
}
