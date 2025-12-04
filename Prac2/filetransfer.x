const MAXFILEDATA = 4096;

struct FileChunk {
    string filename<256>;
    opaque data<MAXFILEDATA>;
    int size; /* number of bytes sent in this chunk */
};

program FILETRANSFER_PROG {
    version FILETRANSFER_VERS {
        int send_chunk(FileChunk) = 1;
    } = 1;
} = 0x31234567;

