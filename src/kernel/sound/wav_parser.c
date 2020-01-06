struct header {
    char group_id[4]; // "RIFF", since this is the riff file format
    uint32_t file_length; // length of file
    char riff_type[4]; // "WAVE", since this a wave file
};

struct fmt_chunk {
    char group_id[4]; // "fmt ", for a format chunk

};

void parse_wav(void *addr) {
    struct header *head = addr;

}