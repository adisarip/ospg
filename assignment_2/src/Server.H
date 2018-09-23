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
};

#endif /* SERVER_H */
