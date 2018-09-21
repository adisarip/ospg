/*
Server Class
*/

#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <string>
using namespace std;

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024


class Server
{
  public:
    // Constructor & Destructor
    Server(string ipAddressParm,
           int portNumParm); // from Tracker
    ~Server();

    inline void error(const char* error_message)
    {
        perror(error_message);
        exit(EXIT_FAILURE);
    };

  // private methods
  private:
    void configureServer();

  // protected data - for derived class to use
  protected:
    int mSocketFd;
    int mSocketConnFd;
    int mPortNum;
    string mIpAddress;
    char mReadBuffer[BUFFER_SIZE];
    struct sockaddr_in mServerAddress;
    struct sockaddr_in mClientAddress;
};

#endif /* SERVER_H */
