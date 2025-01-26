#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_thread.h>

int deley = 25000;

// Game ground
void print_ground()
{
    for (int i = 0; i <= 79; i++)
        mvprintw(19, i, "%c", '#');
    refresh();
}

// Dino
int Dino_y = 13;
int jump = 0;
const char *dinosaur[] = {"               __",
                          "              / _)",
                          "     _.----._/ /",
                          "    /         /",
                          " __/  ( |  ( |",
                          "/__.-'|_|--|_|"};

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

// Obstacles
int cactus_x;
char *cactus[] = {" _  _",
                  "| || | _",
                  "| || || |",
                  "| || || |",
                  " \\_  || |",
                  "   |  _/",
                  "   | | ",
                  "   |_|"};

void print_cactus(int x)
{
    for (int i = 0; i <= 7; i++)
    {
        mvprintw(i + 11, x, "%s", cactus[i]);
    }

    mvprintw(11, x + 5, "%s", "  ");
    mvprintw(12, x + 8, "%s", "  ");
    mvprintw(13, x + 9, "%s", "  ");
    mvprintw(14, x + 9, "%s", "  ");
    mvprintw(15, x + 9, "%s", "  ");
    mvprintw(16, x + 8, "%s", "  ");
    mvprintw(17, x + 7, "%s", "  ");
    mvprintw(18, x + 6, "%s", "  ");

    for (int i = 0; i <= 7; i++)
    {
        mvprintw(i + 11, 79, "          ");
    }

    refresh();
}

void move_cactus(int time)
{
    cactus_x = 80;
    while (time > 0)
    {
        usleep(10000);
        time--;
    }
    while (cactus_x > -10)
    {
        print_cactus(cactus_x);
        cactus_x--;
        usleep(deley);
    }
}

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

    initscr();
    raw();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    srand(time(NULL));
    while (1)
    {
        clear();

        if (Score % 10 == 0 && Score != 0)
            deley -= 10;

        int time = (rand() % 5) + 1;
        move_cactus(time);

        printw("HighScore: %d", HS);
        mvprintw(1, 0, "%u", Score);
        print_ground();
        print_dinosaur(Dino_y);

        char entry = getch();

        if (entry == 27) // ESC ASCII Code = 27
        {
            clear();
            break;
        }
        else if (entry == 32) // Space ASCII Code = 32
        {
            Score++;
            SDL_Thread *thread = SDL_CreateThread(playBeep, "playBeep", NULL);
            if (thread == NULL)
            {
                printf("SDL_CreateThread failed!");
                break;
            }
            while (Dino_y > 4)
            {
                usleep(deley + 100);
                clear_dinosaur(Dino_y);
                Dino_y--;
                print_dinosaur(Dino_y);
            }
            while (Dino_y < 13)
            {
                usleep(deley + 100);
                clear_dinosaur(Dino_y);
                Dino_y++;
                print_dinosaur(Dino_y);
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