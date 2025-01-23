#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_thread.h>

#define deley 25000

const char *dinosaur[] = {"               __",
                          "              / _)",
                          "     _.----._/ /",
                          "    /         /",
                          " __/  ( |  ( |",
                          "/__.-'|_|--|_|"};

// Dino
void print_dinosaur(int y)
{
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    for (int i = y; i < y + 6; i++)
    {
        mvprintw(i, 6, "%s", dinosaur[i - y]);
    }
    attroff(COLOR_PAIR(1));
    refresh();
}

void clear_dinosaur(int y)
{
    for (int i = y; i < y + 6; i++)
    {
        mvprintw(i, 6, "%s", "                   ");
    }
    refresh();
}

// Game Borders
void print_borders()
{
    for (int i = 0; i <= 19; i++)
        mvprintw(i, 79, "%c", '.');

    for (int i = 0; i <= 79; i++)
        mvprintw(19, i, "%c", '.');
    refresh();
}
// Obstacles

// change (Day/Night) status after some playing

// music effect
int playBeep()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize!");
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize!");
        SDL_Quit();
        return 1;
    }
    Mix_Chunk *beepSound = Mix_LoadWAV("beep.wav");
    if (beepSound == NULL)
    {
        printf("Failed to load sound!\n");
        Mix_Quit();
        SDL_Quit();
        return 1;
    }
    Mix_PlayChannel(-1, beepSound, 0);
    SDL_Delay(1000);
    Mix_FreeChunk(beepSound);

    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
    return 0;
}

int main()
{

    // keep highScore for next runs
    unsigned int Score;
    FILE *f;
    unsigned int HS;
    f = fopen("HighScore.txt", "r");
    if (f == NULL || f == 0)
    {
        printf("HighScore.txt cannot be opened!");
        getch();
        return 1;
    }
    fscanf(f, "%u", &HS);
    fclose(f);
    Score = 0;

    int y = 13;
    initscr();
    printw("HighScore: %d", HS);
    raw();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    while (1)
    {
        Score++;
        mvprintw(1, 0, "                        ");
        mvprintw(1, 0, "%u", Score);
        print_borders();
        print_dinosaur(y);
        char entry = getch();

        if (entry == 27) // ESC ASCII Code = 27
        {
            clear();
            break;
        }
        else if (entry == 32) // Space ASCII Code = 32
        {
            SDL_Thread *thread = SDL_CreateThread(playBeep, "playBeep", NULL);
            while (y > 6)
            {
                usleep(deley);
                clear_dinosaur(y);
                y--;
                print_dinosaur(y);
            }
            while (y < 13)
            {
                usleep(deley);
                clear_dinosaur(y);
                y++;
                print_dinosaur(y);
            }
            SDL_WaitThread(thread, NULL);
        }
        usleep(deley);
    }
    usleep(deley);

    if (Score > HS)
    {
        f = fopen("HighScore.txt", "w");
        if (f == NULL || f == 0)
        {
            printf("\nHighScore.txt cannot be opened!\n");
            getch();
            return 1;
        }

        fprintf(f, "%u", Score);
        fclose(f);
    }

    endwin();
    return 0;
}