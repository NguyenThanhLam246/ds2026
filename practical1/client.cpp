#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));

    string filename = "text.txt";
    send(sock, filename.c_str(), 100, 0);

    ifstream infile(filename, ios::binary);
    infile.seekg(0, infile.end);
    int filesize = infile.tellg();
    infile.seekg(0, infile.beg);

    send(sock, &filesize, sizeof(filesize), 0);

    char buffer[1024];
    infile.read(buffer, filesize);
    send(sock, buffer, filesize, 0);

    infile.close();
    close(sock);
    return 0;
}
