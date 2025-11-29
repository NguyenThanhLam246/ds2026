#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./client <file_to_send>\n";
        return 1;
    }

    std::string filename = argv[1];

    // Read file
    std::ifstream infile(filename, std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Cannot open file: " << filename << "\n";
        return 1;
    }

    // Get file size
    infile.seekg(0, infile.end);
    long filesize = infile.tellg();
    infile.seekg(0, infile.beg);

    std::cout << "Sending file: " << filename << "\n";
    std::cout << "File size: " << filesize << " bytes\n";

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Connect to server
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    std::cout << "[Connected to server]\n";

    // 1) Send filename length
    int filename_len = filename.size();
    send(sock, &filename_len, sizeof(filename_len), 0);

    // 2) Send filename
    send(sock, filename.c_str(), filename_len, 0);

    // 3) Send file size
    send(sock, &filesize, sizeof(filesize), 0);

    // 4) Send file data
    char buffer[BUFFER_SIZE];
    long totalSent = 0;

    std::cout << "Sending data...\n";

    while (!infile.eof()) {
        infile.read(buffer, BUFFER_SIZE);
        int bytes = infile.gcount();
        send(sock, buffer, bytes, 0);
        totalSent += bytes;

        std::cout << "\rSent: " << totalSent << "/" << filesize << " bytes";
        std::cout.flush();
    }

    std::cout << "\nFile sent successfully!\n";

    infile.close();
    close(sock);

    return 0;
}

