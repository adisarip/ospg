/*
MTorrent Client Class
*/

#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <string>
using namespace std;

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

class Client
{
  public:
    // Constructors & Destructors
    Client(string ipAddressParm,
           int portNumParm);
    ~Client();

    inline void error(const char* error_message)
    {
        perror(error_message);
        exit(EXIT_FAILURE);
    }

    int connectToServer();
    void disconnectFromServer(int socketFdParm);
 
  // private methods
  protected:
    //void configureClient();

  // private data
  protected:
    //int mSocketFd;
    int mTrackerPortNum;
    string mTrackerIpAddress;
    char mDataBuffer[BUFFER_SIZE];
    struct sockaddr_in mServerAddress;
    struct hostent* mpServer;

};

#endif /* CLIENT_H */
