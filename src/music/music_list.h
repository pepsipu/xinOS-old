#ifndef MUSIC
#define MUSIC

#include "raw_frequencies/mario.c"
#include "raw_frequencies/axelf.c"
#include "raw_frequencies/tetris.c"
#include "raw_frequencies/russian_anthem.c"
#include "raw_frequencies/merry_christmas.c"
#include "raw_frequencies/lose.c"
#include "beep_structure.c"
#include <kernel/sound/pc_speaker.c>
#include <kernel/utils/strings.c>

struct song_entry {
    char *song_name;
    char *porter;
    uint32_t song_length;
    struct beep *frequencies;
} songs[] = {
        /*
    {
        .frequencies = mario,
        .song_name = "mario",
        .porter = "pepsipu",
        .song_length = sizeof(mario) / sizeof(struct beep)
    },
         */
    {
        .frequencies = russia,
        .song_name = "russian anthem",
        .porter = "ararouge",
        .song_length = sizeof(russia) / sizeof(struct beep)
    },
    /*{
        .frequencies = axel,
        .song_name = "axel f",
        .porter = "ararouge",
        .song_length = sizeof(axel) / sizeof(struct beep)
    },
    {
        .frequencies = tetris,
        .song_name = "tetris",
        .porter = "ararouge",
        .song_length = sizeof(tetris) / sizeof(struct beep)
    },
    {
        .frequencies = merry_christmas,
        .song_name = "christmas",
        .porter = "ararouge",
        .song_length = sizeof(merry_christmas) / sizeof(struct beep)
    },*/
    {
        .frequencies = lose,
        .song_name = "snake_lose",
        .porter = "pepsipu",
        .song_length = sizeof(lose) / sizeof(struct beep)
    }
};
void play_song_by_index(int index) {
    for (uint32_t i = 0; i < songs[index].song_length; i++) {
        struct beep current_beep = songs[index].frequencies[i];
        beep(current_beep.freq, current_beep.len, current_beep.delay);
    }
}
void play_song(char *name) {
    for (uint32_t i = 0; i < sizeof(songs) / sizeof(struct song_entry); i++) {
        if (string_cmp(name, songs[i].song_name)) {
            play_song_by_index(i);
            return;
        }
    }
}

#endif