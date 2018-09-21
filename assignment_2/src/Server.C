// socket programming - a test server

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include "Server.H"
using namespace std;

Server::Server(string ipAddressParm,
               int portNumParm)
:mSocketFd(0)
,mSocketConnFd(0)
,mIpAddress(ipAddressParm)
,mPortNum(portNumParm)
{
    bzero((char*)&mServerAddress, sizeof(sockaddr_in));
    bzero((char*)&mClientAddress, sizeof(sockaddr_in));
    bzero(mReadBuffer, BUFFER_SIZE);
    configureServer();
}

Server::~Server()
{
    close(mSocketFd);
    close(mSocketConnFd);
}

// This function intializes a server for the Tracker / Client Node.
// It involves : socket() -> bind() -> listen()
// and will be waiting for incoming requests on a particular port.
void Server::configureServer()
{ 
    int n_bytes;
    struct sockaddr_in client_address;

    // Create the Socket
    mSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocketFd < 0)
    {
        error("[ERROR] Tracker: Socket creation failed");
    }

    // set the socket for re-use
    int status = 1;
    if (setsockopt(mSocketFd,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &status,
                   sizeof(int)) < 0)
    {
        error("[ERROR] 1Tracker: Socket Re-Use setting failed");
    }

    if (setsockopt(mSocketFd,
                   SOL_SOCKET,
                   SO_REUSEPORT,
                   &status,
                   sizeof(int)) < 0)
    {
        error("[ERROR] 2Tracker: Socket Re-Use setting failed");
    }


    // Binding socket to the port
    mServerAddress.sin_family = PF_INET;
    mServerAddress.sin_addr.s_addr = INADDR_ANY;
    mServerAddress.sin_port = htons(mPortNum);
    if (bind(mSocketFd,
             (struct sockaddr*)&mServerAddress,
             sizeof(sockaddr_in)) < 0)
    {
        error("[ERROR] Tracker: Socket binding failed");
    }

    // Start Listening on the port
    if (listen(mSocketFd, MAX_CLIENTS) < 0)
    {
        error("[ERROR] Tracker: Socket listening failed");
    }
    else
    {
        cout << "Listening on port: " << mPortNum << endl;
    }
}

