#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);

    int addrlen = sizeof(address);
    int client_socket = accept(server_fd, (sockaddr*)&address, (socklen_t*)&addrlen);

    char filename[100];
    recv(client_socket, filename, 100, 0);

    int filesize;
    recv(client_socket, &filesize, sizeof(filesize), 0);

    char buffer[1024];
    int received = recv(client_socket, buffer, filesize, 0);

    ofstream outfile(filename, ios::binary);
    outfile.write(buffer, received);
    outfile.close();

    close(client_socket);
    close(server_fd);
    return 0;
}
