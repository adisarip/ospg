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
#include "MTUtils.H"
#include "Server.H"
using namespace std;

//Server::Server(string ipAddressParm, int portNumParm)
Server::Server()
:mSocketFd(0)
,mSocketConnFd(0)
//,mPortNum(portNumParm)
//,mIpAddress(ipAddressParm)
{
    //bzero((char*)&sServerAddress, sizeof(sockaddr_in));
    //bzero((char*)&mClientAddress, sizeof(sockaddr_in));
}

Server::~Server()
{
    stopListening();
}


void Server::stopListening()
{
    close(mSocketFd);
    close(mSocketConnFd);
}

// This function intializes a server for the Tracker / Client Node.
// It involves : socket() -> bind() -> listen()
// and will be waiting for incoming requests on a particular port.
void Server::startListening(string ipAddressParm, int portNumParm)
{
    // Create the Socket
    mSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocketFd < 0)
    {
        error("[ERROR] Socket creation failed");
    }

    // set the socket for re-use
    int status = 1;
    if (setsockopt(mSocketFd,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &status,
                   sizeof(int)) < 0)
    {
        error("[ERROR] Socket Re-Use Addr setting failed");
    }

    if (setsockopt(mSocketFd,
                   SOL_SOCKET,
                   SO_REUSEPORT,
                   &status,
                   sizeof(int)) < 0)
    {
        error("[ERROR] Socket Re-Use Port setting failed");
    }

    struct sockaddr_in sServerAddress;
    bzero((char*)&sServerAddress, sizeof(sockaddr_in));
    sServerAddress.sin_family = AF_INET; // changed from PF_INET earlier
    sServerAddress.sin_addr.s_addr = INADDR_ANY;
    sServerAddress.sin_port = htons(portNumParm);

    // Binding socket to the port
    if (bind(mSocketFd,
             (struct sockaddr*)&sServerAddress,
             sizeof(sockaddr_in)) < 0)
    {
        error("[ERROR] Socket binding failed");
    }

    // Start Listening on the port
    if (listen(mSocketFd, MAX_CLIENTS) < 0)
    {
        error("[ERROR] Socket listening failed");
    }
    else
    {
        cout << "Listening on port: " << portNumParm << endl;
    }
}

