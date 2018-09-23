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
    //Server(string ipAddressParm,
    //       int portNumParm); // from Tracker
    Server();
    ~Server();

  // private methods
  protected:
    void startListening(string ipAddressParm, int portNumParm);
    void stopListening();

  // protected data - for derived class to use
  protected:
    int mSocketFd;
    int mSocketConnFd;
    //int mPortNum;
    //string mIpAddress;
    //struct sockaddr_in mServerAddress;
    //struct sockaddr_in mClientAddress;
};

#endif /* SERVER_H */
