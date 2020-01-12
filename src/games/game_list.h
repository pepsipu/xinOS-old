#include <games/XinSnake/xa_main.c>

struct game {
    char *name;
    char *author;
    int (*init)();
};

struct game games[] = {
        {.name = "xinSnake", .author = "pepsipu", .init = xa_main},
        {.name = "xinMusic", .author = "pepsipu", .init = 0}
};