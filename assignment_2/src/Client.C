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
using namespace std;

#define SUCCESS  0
#define FAILURE -1
#define BUFFER_SIZE 1024

void error(const char* error_message)
{
    perror(error_message);
    exit(EXIT_FAILURE);
}

int main (int argc, char* argv[])
{
    int socket_fd;
    int port_number;
    int n_bytes;
    char read_buffer[BUFFER_SIZE];
    string buffer;

    struct hostent* p_server;
    struct sockaddr_in server_address;
    int address_len = sizeof(sockaddr_in);
    bzero((char*)&server_address, address_len);

    // parse the input arguments
    if (argc < 3)
    {
        cout << "[ERROR] Enter server ip_address & port_number." << endl;
        exit(EXIT_FAILURE);
    }

    // fetch the port number from the input
    port_number = atoi(argv[2]);

    // fetch the server ip address from the input
    p_server = gethostbyname(argv[1]);
    if (NULL == p_server)
    {
        error("[ERROR] Invalid Server IP ADDRESS (or) NULL");
    }

    // create a socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        error("[ERROR] Opening a socket");
    }

    server_address.sin_family = AF_INET;
    bcopy((char*)p_server->h_addr,
          (char*)&server_address.sin_addr.s_addr,
          p_server->h_length);
    server_address.sin_port = htons(port_number);

    // connect to the server
    if (connect(socket_fd,
                (struct sockaddr*)&server_address,
                address_len) < 0)
    {
        error("[ERROR] Failed connecting to server");
    }

    // sending a file to server
    

    close(socket_fd);
    return(0);
}
