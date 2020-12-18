#include <sys/ioctl.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define DIR(s, d) s->direction = d

#define TICKRATE 1

int
    width = 20,
    height = 12;

enum direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
};

struct snake {
    int *posx;
    int *posy;
    int length;
    int maxLength;
    enum direction direction;
    clock_t lastTick;
};

void delay(int milliseconds);
// char getch(void);