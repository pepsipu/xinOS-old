
// I created this parser based off of the .wav file format specifications here: https://blogs.msdn.microsoft.com/dawate/2009/06/23/intro-to-audio-programming-part-2-demystifying-the-wav-format/

struct header {
    char group_id[4]; // "RIFF", since this is the riff file format
    uint32_t file_length; // length of file
    char riff_type[4]; // "WAVE", since this a wave file
} __attribute__((packed));

struct fmt_chunk {
    char group_id[4]; // "fmt ", for a format chunk
    uint32_t chunk_len; // length of format chunk, does not include chunk_len or group_id
    uint16_t format_tag; // this is always 1 for a WAV, indicates pulse code modulation is used
    uint16_t channels; // channels are the independent waveform played async
    uint32_t sampling_rate; // how many samples are played per second
    uint32_t audio_frame_size; // average size for a given multichannel audio frame of the WAV
    uint16_t block_size; // bytes needed for one multichannel audio frame
    uint16_t bit_depth; // bits per sample (8 bit, 16 bit, 32 bit)
} __attribute__((packed));

struct data_chunk {
    char group_id[4]; // "data" for data chunks
    uint32_t chunk_len; // length of data
} __attribute__((packed));

void play_wav(void *addr) {
    struct header *head = addr;
    struct fmt_chunk *meta = addr + sizeof(struct header);
    struct data_chunk *first_data = addr + sizeof(struct header) + sizeof(struct fmt_chunk);
    int bytes_per_sample = meta->bit_depth / 8;
    for (int i = 0; i > first_data->chunk_len / bytes_per_sample; i += bytes_per_sample) {

    }
}