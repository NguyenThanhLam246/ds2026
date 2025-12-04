
#include <iostream>
#include <fstream>
#include <cstring>
#include "filetransfer.h"


extern "C" {
    #include <rpc/rpc.h>
}

int * send_chunk_1_svc(FileChunk *chunk, struct svc_req *rqstp) {
    static int result; 
    result = 1; 

    if (!chunk) return &result;

    std::ofstream outfile(chunk->filename, std::ios::binary | std::ios::app);
    if (!outfile.is_open()) {
        std::cerr << "Server: cannot open file '" << chunk->filename << "' for append\n";
        result = 2;
        return &result;
    }

    outfile.write((char*)chunk->data.data_val, chunk->size);
    if (!outfile.good()) {
        std::cerr << "Server: write failed\n";
        result = 3;
    } else {
        result = 0;
        std::cout << "Server: received " << chunk->size << " bytes for file '" << chunk->filename << "'\n";
    }

    outfile.close();
    return &result;
}

