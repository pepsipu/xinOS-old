#include <games/XinAdventures/xa_main.c>

struct game {
    char *name;
    char *author;
    int (*init)();
};

struct game games[] = {
        {.name = "XinAdventures", .author = "pepsipu", .init = xa_main},
};