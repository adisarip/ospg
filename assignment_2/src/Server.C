// socket programming - a test server

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
using namespace std;

#define SUCCESS  0
#define FAILURE -1
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

void error(const char* error_message)
{
    perror(error_message);
    exit(EXIT_FAILURE);
}

int main (int argc, char* argv[])
{
    int socket_fd;
    int new_socket_fd;
    int port_number;
    int n_bytes;
    char read_buffer[BUFFER_SIZE];
    string buffer;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    int address_len = sizeof(sockaddr_in);

    // Clear the address variables
    bzero((char*)&server_address, address_len);
    bzero((char*)&client_address, address_len);

    // parse the input arguments
    if (argc < 2)
    {
        cout << "[ERROR] Enter port_number" << endl;
        exit(EXIT_FAILURE);
    }
    
    // fetch the port number from inout
    port_number = atoi(argv[1]);

    // Create the Socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        error("[ERROR] Socket creation failed");
    }

    // Binding socket to the port
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);
    if (bind(socket_fd,
             (struct sockaddr*)&server_address,
             address_len) < 0)
    {
        error("Socket binding failed");
    }

    // Listening on port
    if (listen(socket_fd, MAX_CLIENTS) < 0)
    {
        error("Socket listening failed");
    }
    else
    {
        cout << "Listening on port: " << port_number << endl;
    }

    // On Success - Connection established - Ready for data transfer.
    new_socket_fd = accept(socket_fd,
                           (struct sockaddr*)&client_address,
                           (socklen_t*)&address_len);
    if (new_socket_fd < 0)
    {
        error("Socket accept failed");
    }

    // Do Something with client

    close(new_socket_fd);
    close(socket_fd);
    return 0;
}
