#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        return 1;
    }

    std::cout << "===== FILE TRANSFER SERVER RUNNING =====\n";
    std::cout << "Waiting for client on port " << PORT << "...\n";

    // Accept connection
    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("Accept failed");
        return 1;
    }

    std::cout << "[Client connected]\n";

    // Receive filename length
    int filename_len = 0;
    recv(new_socket, &filename_len, sizeof(filename_len), 0);

    // Receive filename
    char filename[256];
    recv(new_socket, filename, filename_len, 0);
    filename[filename_len] = '\0';

    std::cout << "Receiving file: " << filename << "\n";

    // Receive file size
    long filesize;
    recv(new_socket, &filesize, sizeof(filesize), 0);
    std::cout << "File size: " << filesize << " bytes\n";

    // Save file
    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "Error opening file for writing!\n";
        return 1;
    }

    char buffer[BUFFER_SIZE];
    long totalReceived = 0;

    std::cout << "Receiving data...\n";

    while (totalReceived < filesize) {
        int bytes = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (bytes <= 0) break;

        outfile.write(buffer, bytes);
        totalReceived += bytes;

        std::cout << "\rReceived: " << totalReceived << "/" << filesize << " bytes";
        std::cout.flush();
    }

    std::cout << "\nFile received successfully!\n";

    outfile.close();
    close(new_socket);
    close(server_fd);

    return 0;
}

