
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include "filetransfer.h"

extern "C" {
    #include <rpc/rpc.h>
}

#define CHUNK_SIZE 4096

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./file_client <filename>\n";
        return 1;
    }

    const char* filename = argv[1];


    CLIENT *clnt = clnt_create("localhost", FILETRANSFER_PROG, FILETRANSFER_VERS, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror("clnt_create");
        return 1;
    }

    std::ifstream infile(filename, std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Cannot open file: " << filename << "\n";
        clnt_destroy(clnt);
        return 1;
    }

    char buf[CHUNK_SIZE];
    while (!infile.eof()) {
        infile.read(buf, CHUNK_SIZE);
        std::streamsize n = infile.gcount();
        if (n <= 0) break;

        FileChunk chunk;
        chunk.filename = (char*)strdup(filename);

        chunk.data.data_len = n;
        chunk.data.data_val = (char*)malloc(n);
        memcpy(chunk.data.data_val, buf, n);

        chunk.size = n;

        int *ret = send_chunk_1(&chunk, clnt);
        if (ret == NULL) {
            clnt_perror(clnt, "RPC call failed");
            free(chunk.data.data_val);
            free(chunk.filename);
            infile.close();
            clnt_destroy(clnt);
            return 1;
        }
        if (*ret != 0) {
            std::cerr << "Server returned error code: " << *ret << "\n";
            free(chunk.data.data_val);
            free(chunk.filename);
            infile.close();
            clnt_destroy(clnt);
            return 1;
        }

        free(chunk.data.data_val);
        free(chunk.filename);
    }

    infile.close();
    clnt_destroy(clnt);
    std::cout << "File sent successfully via RPC.\n";
    return 0;
}

