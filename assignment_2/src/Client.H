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
    Client();
    ~Client();

  protected:
    int connectToServer(string ipAddressParm, int portNumParm);
    void disconnectFromServer(int socketFdParm);
};

#endif /* CLIENT_H */
