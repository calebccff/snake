#include "snake.h"

void printAt(char* c, int x, int y)
{
    char str[32];
    strcpy(str, "\033[%d;%dH");
    strcat(str, c);
    printf(str, y, x);
}

// Draws the border and clears the screen
void drawBorder(WINDOW *win)
{
    int i, j;
    for(i = 0; i < width; i++)
    {
        for(j = 0; j < height+5; j++)
        {
            mvwprintw(win, j, i, " ");
        }
    }
    // Top
    for (i = 0; i < width; i++)
    {
        mvwprintw(win, 1, i, "-");
        mvwprintw(win, height, i, "-");
    }
    // Right
    for (i = 0; i < height; i++)
    {
        mvwprintw(win, i, width, "|");
        mvwprintw(win, i, 0, "|");
    }
}

int snake_move(struct snake *s)
{
    int i;
    for(i = s->length; i > 0; i--){
        s->posx[i] = s->posx[i-1];
        s->posy[i] = s->posy[i-1];
    }
    int j;
    for(i = 0; i < s->length; i++){
        for(j = 1; j < s->length; j++){
            if(s->posx[i] == s->posx[j] &&
                s->posy[i] == s->posy[j] && i != j)
                    return 1;
        }
    }

    switch(s->direction){
        case UP:
            s->posy[0] = s->posy[1]-1;
            break;
        case DOWN:
            s->posy[0] = s->posy[1]+1;
            break;
        case LEFT:
            s->posx[0] = s->posx[1]-1;
            break;
        case RIGHT:
            s->posx[0] = s->posx[1]+1;
            break;
        default:
            break;
    }
    return 0;
}

int gameLoop(struct snake *s, WINDOW *win)
{
    printf("\e[1;1H\e[2J"); // Clear screen
    do {
    drawBorder(win);
    mvwprintw(win, s->posy[0], s->posx[0], "@" );
    int i;
    for(i = 1; i < s->length; i++){
        mvwprintw(win, s->posy[i], s->posx[i], "#");
    }
    mvwprintw(win, width, height, "");
    char *debug = (char*)malloc(32*sizeof(char));
    mvwprintw(win, height+4, 2, debug, "x: %d, y: %d, l: %d, d: %d", s->posx[0], s->posy[0], s->length, s->direction);

    if ((clock() - s->lastTick) > 1000*(1/TICKRATE)) {
        s->lastTick = clock();
        int res = snake_move(s);
        //mvwprintw(win, 6, 6, "^");

        if (res){
            //printf("\e[1;1H\e[2J");
            clear();
            char *gameover = "GAME OVER !";
            mvwprintw(win, width/2-strlen(gameover)/2, height/2, gameover);
            return res;
        }
    }

    // All drawing MUST come before the 'fflush'
    ///fflush(stdout);
    char c = wgetch(win);
    switch(c){
        case 'w':
            DIR(s, UP);
            break;
        case 's':
            DIR(s, DOWN);
            break;
        case 'a':
            DIR(s, LEFT);
            break;
        case 'd':
            DIR(s, RIGHT);
            break;
        case 'f': // for debugging
            s->posx[s->length] = s->posx[s->length-1]-1;
            s->length++;
            break;
        default:
            break;
    }
    s->posx[0] = min(max(2, s->posx[0]), width-1);
    s->posy[0] = min(max(2, s->posy[0]), height-1);
    free(debug);
    refresh();
    //delay(100);
    } while(1);
    return 0;
}

int getConsoleInfo(struct winsize *w)
{
    int res;
    res = ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return res;
}

void initSnake(struct snake *s)
{
    s->length = 1;
    s->maxLength = width*height;
    // The +1 is because we cheat and also populate the value after the tail
    // to simplify the movement code by letting us always reference
    // the previous position - even when length = 1
    s->posx = (int*)malloc((s->maxLength+1)*sizeof(int));
    s->posy = (int*)malloc((s->maxLength+1)*sizeof(int));

    s->lastTick = clock();
}

int main (int argc, char **argv)
{
    struct winsize w;
    getConsoleInfo(&w);
    WINDOW *win;

    // width = w.ws_row;
    // height = w.ws_col;

    struct snake *s = (struct snake*)malloc(sizeof(struct snake));
    initSnake(s);
    initscr();
    //noecho();
    //cbreak();
    refresh();
    win = newwin(height+5, width+1, 0, 0);
    //keypad(win, TRUE);
    timeout(0);
    gameLoop(s, win);
    free(s);
    clrtoeol();
    refresh();
    endwin();
    return 0;
}

void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}

// char getch(void)
// {
//     struct timeval tv = { 0L, 0L };
//     fd_set fds;
//     FD_ZERO(&fds);
//     FD_SET(0, &fds);
//     if (!select(1, &fds, NULL, NULL, &tv))
//         return '\0';
//     char buf = 0;
//     struct termios old = {0};
//     if(tcgetattr(0, &old) < 0)
//         perror("tcsetattr()");
//     old.c_lflag &= ~ICANON;
//     old.c_lflag &= ~ECHO;
//     old.c_cc[VMIN] = 1;
//     old.c_cc[VTIME] = 0;
//     if(tcsetattr(0, TCSANOW, &old) < 0)
//         perror("tcsetattr ICANON");
//     if(read(0, &buf, 1) < 0)
//         perror("read()");
//     old.c_lflag |= ICANON;
//     old.c_lflag |= ECHO;
//     if(tcsetattr(0, TCSADRAIN, &old) < 0)
//         perror("tcsetattr ~ICANON");
//     //printf("%c\n", buf);
//     return buf;
//  }