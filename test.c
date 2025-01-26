#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

int deley = 25000;

// Game ground
void print_ground()
{
    for (int i = 0; i <= 19; i++)
        mvprintw(i, 79, "%c", '#');
    for (int i = 0; i <= 79; i++)
        mvprintw(19, i, "%c", '#');
    refresh();
}
// Game BackGround
void changBG(int status)
{

    if (status == 1)
    {
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        bkgd(COLOR_PAIR(1));
    }
    else
    {
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        bkgd(COLOR_PAIR(2));
    }
    refresh();
}

// Dino
int Dino_y = 13;
const int ground = 13;
const int maxHeight = 3;
int jump = 0;
int UPorDown = 1;
const char *dinosaur[] = {"            __",
                          "           | _)",
                          "     _.----| |",
                          "    /        |",
                          " __/  ( |  ( |",
                          "/__.-'|_|--|_|"};

void print_dinosaur(int Dino_y)
{
    for (int i = Dino_y; i < Dino_y + 6; i++)
    {
        mvprintw(i, 6, "%s", dinosaur[i - Dino_y]);
    }
}

// Obstacles
int cactus_x = -1;
const char *cactus[] = {" _  _",
                        "| || | _",
                        "| || || |",
                        "| || || |",
                        " \\_  || |",
                        "   |  _/",
                        "   | | ",
                        "   |_|"};

void print_cactus(int cactus_x)
{
    for (int i = 0; i <= 5; i++)
    {
        if ((cactus_x + i) < 79)
            mvprintw(11, cactus_x + i, "%c", cactus[0][i]);
        else
            break;
    }
    for (int i = 0; i <= 8; i++)
    {
        if ((cactus_x + i) < 79)
            mvprintw(12, cactus_x + i, "%c", cactus[1][i]);
        else
            break;
    }
    for (int i = 0; i <= 9; i++)
    {
        if ((cactus_x + i) < 79)
            mvprintw(13, cactus_x + i, "%c", cactus[2][i]);
        else
            break;
    }
    for (int i = 0; i <= 9; i++)
    {
        if ((cactus_x + i) < 79)
            mvprintw(14, cactus_x + i, "%c", cactus[3][i]);
        else
            break;
    }
    for (int i = 0; i <= 9; i++)
    {
        if ((cactus_x + i) < 79)
            mvprintw(15, cactus_x + i, "%c", cactus[4][i]);
        else
            break;
    }
    for (int i = 0; i <= 8; i++)
    {
        if ((cactus_x + i) < 79)
            mvprintw(16, cactus_x + i, "%c", cactus[5][i]);
        else
            break;
    }
    for (int i = 0; i <= 7; i++)
    {
        if ((cactus_x + i) < 79)
            mvprintw(17, cactus_x + i, "%c", cactus[6][i]);
        else
            break;
    }
    for (int i = 0; i <= 6; i++)
    {
        if ((cactus_x + i) < 79)
            mvprintw(18, cactus_x + i, "%c", cactus[7][i]);
        else
            break;
    }
}

// music effect
Mix_Chunk *beepSound;

int initAudio()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize!\n");
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize!\n");
        SDL_Quit();
        return 1;
    }
    beepSound = Mix_LoadWAV("beep.wav");
    if (beepSound == NULL)
    {
        printf("Failed to load sound!\n");
        Mix_CloseAudio();
        SDL_Quit();
        return 1;
    }
    return 0;
}

int main()
{
    // Change Day/Night status
    int status = 0;
    time_t t1 = time(0);
    time_t t2;

    // keep highScore for next runs
    unsigned int Score;
    FILE *f;
    unsigned int HS;
    f = fopen("HighScore.txt", "r");
    if (f == NULL)
    {
        printf("HighScore.txt cannot be opened!");
        getch();
        return 1;
    }
    fscanf(f, "%u", &HS);
    fclose(f);
    Score = 0;

    // Initialize screen
    initscr();
    start_color();
    use_default_colors();
    raw();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    srand(time(NULL));
    if (initAudio())
    {
        return 1;
    }
    while (1)
    {
        clear();

        // Change Day/Night status
        t2 = time(0);
        changBG(status);
        if ((t2 - t1) >= 24)
        {
            t1 = t2;
            status = !status;
            changBG(status);
        }
        // Change Speed
        if (Score % 10 == 0 && Score != 0)
            deley -= 1;

        printw("HighScore: %u", HS);
        mvprintw(1, 0, "%u", Score);
        print_ground();

        if (jump == 1)
        {
            if (UPorDown == 1)
            {
                if (Dino_y > maxHeight)
                    Dino_y--;
                else
                    UPorDown = 0;
            }
            else
            {
                if (Dino_y < ground)
                    Dino_y++;
                else
                {
                    jump = 0;
                    UPorDown = 1;
                }
            }
        }
        print_dinosaur(Dino_y);

        int repeat = (rand() % 6);
        if (cactus_x < 0)
        {
            if (repeat == 0)
                cactus_x = 79;
        }
        else
        {
            print_cactus(cactus_x);
            cactus_x--;
            if (cactus_x >= 6 && cactus_x <= 12 && Dino_y >= 6)
                break;
        }
        if (cactus_x == 0)
            Score++;

        char entry = getch();
        if (entry == -1)
        {
        }
        else if (entry == 27) // ESC ASCII Code = 27
            break;

        else if (entry == 32 && jump != 1) // Space ASCII Code = 32
        {
            jump = 1;
            Mix_PlayChannel(-1, beepSound, 0);
        }
        usleep(deley);
        refresh();
    }

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

    Mix_FreeChunk(beepSound);
    Mix_CloseAudio();
    SDL_Quit();

    usleep(deley);
    nodelay(stdscr, FALSE);
    mvprintw(5, 37, "GameOver!");
    mvprintw(7, 37, "Press any key to exit");
    getch();

    endwin();
    return 0;
}