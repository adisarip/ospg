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


Server::Server()
{
    // Constructor
}

Server::~Server()
{
    // Destructor
}


void Server::stopListening(int socketFdParm, int socketConnFdParm)
{
    close(socketFdParm);
    close(socketConnFdParm);
}

// This function intializes a server for the Tracker / Client Node.
// It involves : socket() -> bind() -> listen()
// and will be waiting for incoming requests on a particular port.
int Server::startListening(string ipAddressParm, int portNumParm)
{   
    // Create the Socket
    int sSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sSocketFd < 0)
    {
        error("[ERROR] Socket creation failed");
    }

    // set the socket for re-use
    int status = 1;
    if (setsockopt(sSocketFd,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &status,
                   sizeof(int)) < 0)
    {
        error("[ERROR] Socket Re-Use Addr setting failed");
    }

    if (setsockopt(sSocketFd,
                   SOL_SOCKET,
                   SO_REUSEPORT,
                   &status,
                   sizeof(int)) < 0)
    {
        error("[ERROR] Socket Re-Use Port setting failed");
    }

    struct sockaddr_in sServerAddress;
    bzero((char*)&sServerAddress, sizeof(sockaddr_in));
    sServerAddress.sin_family = AF_INET;
    sServerAddress.sin_addr.s_addr = INADDR_ANY;
    sServerAddress.sin_port = htons(portNumParm);

    // Binding socket to the port
    if (bind(sSocketFd,
             (struct sockaddr*)&sServerAddress,
             sizeof(sockaddr_in)) < 0)
    {
        error("[ERROR] Socket binding failed");
    }

    // Start Listening on the port
    if (listen(sSocketFd, MAX_CLIENTS) < 0)
    {
        error("[ERROR] Socket listening failed");
    }
    return sSocketFd;
}

