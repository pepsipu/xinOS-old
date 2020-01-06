#include "raw_frequencies/mario.c"
#include "beep_structure.c"

struct {
    char *song_name;
    char *porter;
    uint32_t song_length;
    struct beep *frequencies;
} songs[] = {
    {
        .frequencies = mario,
        .song_name = "mario",
        .porter = "pepsipu",
        .song_length = sizeof(mario) / sizeof(struct beep),
    }
};
void play_song(int index) {
    for (uint32_t i = 0; i < songs[index].song_length; i++) {
        struct beep current_beep = songs[index].frequencies[i];
        beep(current_beep.freq, current_beep.len, current_beep.delay);
    }
}
