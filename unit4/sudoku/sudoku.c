// Implements the game of Sudoku

#include <ctype.h>
#include <ncurses.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sudoku.h"
#include <time.h>

// Macro for processing control characters
#define CTRL(x) ((x) & ~0140)

// Size of each int (in bytes) in *.bin files
#define INTSIZE 4

// Wrapper for our game's globals
struct
{
    // The current level
    char *level;

    // The game's board
    int board[9][9];

    // Original board from file
    int orig_board[9][9];

    // The board's number
    int number;

    // The board's top-left coordinates
    int top, left;

    // The cursor's current location between (0,0) and (8,8)
    int y, x;

    // Marker for wining
    bool win_row, win_col, win_box, win;

    // Previous step for undo
    int pr_y, pr_x, pr_value;

    // Is previus Exist
    bool pr_exist;

} g;

// Prototypes
void draw_grid(void);
void draw_borders(void);
void draw_logo(void);
void draw_numbers(void);
void hide_banner(void);
bool load_board(void);
void handle_signal(int signum);
void log_move(int ch);
void redraw_all(void);
bool restart_game(void);
void show_banner(char *b);
void show_cursor(void);
void shutdown(void);
bool startup(void);

// Newly-added prototypes
void checkrows(void);
void checkcols(void);
void checkboxes(void);
void checkwin(void);
void reposition(int ch);
void update_numbers(int ch);
void undo_move(void);
void copy_board(void);


int main(int argc, char *argv[])
{
    // Ensure that number of arguments is as expected
    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "Usage: sudoku n00b|l33t [#]\n");
        return 1;
    }

    // Ensure that level is valid
    if (strcmp(argv[1], "debug") == 0)
    {
        g.level = "debug";
    }
    else if (strcmp(argv[1], "n00b") == 0)
    {
        g.level = "n00b";
    }
    else if (strcmp(argv[1], "l33t") == 0)
    {
        g.level = "l33t";
    }
    else
    {
        fprintf(stderr, "Usage: sudoku n00b|l33t [#]\n");
        return 2;
    }

    // n00b and l33t levels have 1024 boards; debug level has 9
    int max = (strcmp(g.level, "debug") == 0) ? 9 : 1024;

    // Ensure that #, if provided, is in [1, max]
    if (argc == 3)
    {
        // Ensure n is integral
        char c;
        if (sscanf(argv[2], " %d %c", &g.number, &c) != 1)
        {
            fprintf(stderr, "Usage: sudoku n00b|l33t [#]\n");
            return 3;
        }

        // Ensure n is in [1, max]
        if (g.number < 1 || g.number > max)
        {
            fprintf(stderr, "That board # does not exist!\n");
            return 4;
        }

        // Seed PRNG with # so that we get same sequence of boards
        srand(g.number);
    }
    else
    {
        // Seed PRNG with current time so that we get any sequence of boards
        srand(time(NULL));

        // Choose a random n in [1, max]
        g.number = rand() % max + 1;
    }

    // Start up ncurses
    if (!startup())
    {
        fprintf(stderr, "Error starting up ncurses!\n");
        return 5;
    }

    // Register handler for SIGWINCH (SIGnal WINdow CHanged)
    signal(SIGWINCH, (void (*)(int)) handle_signal);

    // Start the first game
    if (!restart_game())
    {
        shutdown();
        fprintf(stderr, "Could not load board from disk!\n");
        return 6;
    }
    copy_board();
    redraw_all();

    // Let the user play
    int ch;
    do
    {
        // Refresh the screen
        refresh();

        // Get user's input
        ch = getch();

        // Capitalize input to simplify cases
        ch = toupper(ch);

        // Process user's input
        switch (ch)
        {
            // Start a new game
            case 'N':
                g.number = rand() % max + 1;
                if (!restart_game())
                {
                    shutdown();
                    fprintf(stderr, "Could not load board from disk!\n");
                    return 6;
                }
                break;

            // Restart current game
            case 'R':
                if (!restart_game())
                {
                    shutdown();
                    fprintf(stderr, "Could not load board from disk!\n");
                    return 6;
                }
                break;

            // Let user manually redraw screen with ctrl-L
            case CTRL('l'):
                redraw_all();
                break;

            // Move cursor on board
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
                reposition(ch);
                break;

            // Update numbers
            case '0' ... '9':
                update_numbers(ch);
                checkrows();
                checkcols();
                checkboxes();
                checkwin();
                break;
            case CTRL('Z'):
            case 'U':
                undo_move();

        }

        // Log input (and board's state) if any was received this iteration
        if (ch != ERR)
        {
            log_move(ch);
        }
    }
    while (ch != 'Q');

    // Shut down ncurses
    shutdown();

    // Tidy up the screen (using ANSI escape sequences)
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);

    // That's all folks
    printf("\nkthxbai!\n\n");
    return 0;
}


// Draw's the game's board
void draw_grid(void)
{
    // Get window's dimensions
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);

    // Determine where top-left corner of board belongs
    g.top = maxy / 2 - 7;
    g.left = maxx / 2 - 30;

    // Enable color if possible
    if (has_colors())
    {
        attron(COLOR_PAIR(PAIR_GRID));
    }
    // Print grid
    for (int i = 0 ; i < 3 ; ++i)
    {
        mvaddstr(g.top + 0 + 4 * i, g.left, "+-------+-------+-------+");
        mvaddstr(g.top + 1 + 4 * i, g.left, "|       |       |       |");
        mvaddstr(g.top + 2 + 4 * i, g.left, "|       |       |       |");
        mvaddstr(g.top + 3 + 4 * i, g.left, "|       |       |       |");
    }
    mvaddstr(g.top + 4 * 3, g.left, "+-------+-------+-------+");

    // Remind user of level and #
    char reminder[maxx + 1];
    sprintf(reminder, "   playing %s #%d", g.level, g.number);
    mvaddstr(g.top + 14, g.left + 25 - strlen(reminder), reminder);

    // Disable color if possible
    if (has_colors())
    {
        attroff(COLOR_PAIR(PAIR_GRID));
    }
}


// Draws game's borders.
void draw_borders(void)
{
    // Get window's dimensions
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);

    // Enable color if possible (else b&w highlighting)
    if (has_colors())
    {
        attron(A_PROTECT);
        attron(COLOR_PAIR(PAIR_BORDER));
    }
    else
    {
        attron(A_REVERSE);
    }

    // Draw borders
    for (int i = 0; i < maxx; i++)
    {
        mvaddch(0, i, ' ');
        mvaddch(maxy - 1, i, ' ');
    }

    // Draw header
    char header[maxx + 1];
    sprintf(header, "%s by %s", TITLE, AUTHOR);
    mvaddstr(0, (maxx - strlen(header)) / 2, header);

    // Draw footer
    mvaddstr(maxy - 1, 1, "[N]ew Game   [R]estart Game");
    mvaddstr(maxy - 1, maxx - 13, "[Q]uit Game");

    // Disable color if possible (else b&w highlighting)
    if (has_colors())
    {
        attroff(COLOR_PAIR(PAIR_BORDER));
    }
    else
    {
        attroff(A_REVERSE);
    }
}


// Draws game's logo (Must be called after draw_grid has been called at least once)
void draw_logo(void)
{
    // Determine top-left coordinates of logo
    int top = g.top + 2;
    int left = g.left + 30;

    // Enable color if possible
    if (has_colors())
    {
        attron(COLOR_PAIR(PAIR_LOGO));
    }

    // Draw logo
    mvaddstr(top + 0, left, "               _       _          ");
    mvaddstr(top + 1, left, "              | |     | |         ");
    mvaddstr(top + 2, left, " ___ _   _  __| | ___ | | ___   _ ");
    mvaddstr(top + 3, left, "/ __| | | |/ _` |/ _ \\| |/ / | | |");
    mvaddstr(top + 4, left, "\\__ \\ |_| | (_| | (_) |   <| |_| |");
    mvaddstr(top + 5, left, "|___/\\__,_|\\__,_|\\___/|_|\\_\\\\__,_|");

    // Sign logo
    char signature[3 + strlen(AUTHOR) + 1];
    sprintf(signature, "by %s", AUTHOR);
    mvaddstr(top + 7, left + 35 - strlen(signature) - 1, signature);

    // Disable color if possible
    if (has_colors())
    {
        attroff(COLOR_PAIR(PAIR_LOGO));
    }
}


//Draw's game's numbers (Must be called after draw_grid has been called at least once)

void draw_numbers()
{
    // Enable color if possible
    if (has_colors())
    {
        if (g.win)
        {
            attron(COLOR_PAIR(PAIR_WIN));
        }
        else
        {
            attron(COLOR_PAIR(PAIR_DIGITS));
        }
    }

    // Iterate over board's numbers
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            // Determine char
            char c = (g.board[i][j] == 0) ? '.' : g.board[i][j] + '0';
            mvaddch(g.top + i + 1 + i / 3, g.left + 2 + 2 * (j + j / 3), c);
            refresh();
        }
    }


    // Disable color if possible
    if (has_colors())
    {
        attroff(COLOR_PAIR(PAIR_BANNER));
    }
}


// Designed to handles signals (e.g., SIGWINCH)

void handle_signal(int signum)
{
    // Handle a change in the window (i.e., a resizing)
    if (signum == SIGWINCH)
    {
        redraw_all();
    }

    // Re-register myself so this signal gets handled in future too
    signal(signum, (void (*)(int)) handle_signal);
}


// Hides banner
void hide_banner(void)
{
    // Get window's dimensions
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);

    // Overwrite banner with spaces
    for (int i = 0; i < maxx; i++)
    {
        mvaddch(g.top + 16, i, ' ');
    }
}


// Loads current board from disk, returning true iff successful
bool load_board(void)
{
    // Open file with boards of specified level
    char filename[strlen(g.level) + 5];
    sprintf(filename, "%s.bin", g.level);
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        return false;
    }

    // Determine file's size
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);

    // Ensure file is of expected size
    if (size % (81 * INTSIZE) != 0)
    {
        fclose(fp);
        return false;
    }

    // Compute offset of specified board
    int offset = ((g.number - 1) * 81 * INTSIZE);

    // Seek to specified board
    fseek(fp, offset, SEEK_SET);

    // Read board into memory
    if (fread(g.board, 81 * INTSIZE, 1, fp) != 1)
    {
        fclose(fp);
        return false;
    }


    // w00t
    fclose(fp);
    return true;
}


// Logs input and board's state to log.txt to facilitate automated tests
void log_move(int ch)
{
    // Open log
    FILE *fp = fopen("log.txt", "a");
    if (fp == NULL)
    {
        return;
    }

    // Log input
    fprintf(fp, "%d\n", ch);

    // Log board
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            fprintf(fp, "%d", g.board[i][j]);
        }
        fprintf(fp, "\n");
    }

    // That's it
    fclose(fp);
}


// (Re)draws everything on the screen
void redraw_all(void)
{
    // Reset ncurses
    endwin();
    refresh();

    // Clear screen
    clear();

    // Re-draw everything
    draw_borders();
    draw_grid();
    draw_logo();
    draw_numbers();

    // Show cursor
    show_cursor();
}


// (Re)starts current game, returning true iff succesful
bool restart_game(void)
{
    g.win = false;
    // Reload current game
    if (!load_board())
    {
        return false;
    }

    // Redraw board
    copy_board();
    draw_grid();
    draw_numbers();

    // Get window's dimensions
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);

    // Move cursor to board's center
    g.y = g.x = 4;
    show_cursor();

    // Remove log, if any
    remove("log.txt");

    // w00t
    return true;
}


// Shows cursor at (g.y, g.x)
void show_cursor(void)
{
    // Restore cursor's location
    move(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 * (g.x + g.x / 3));
}


// Shows a banner (Must be called after show_grid has been called at least once)

void show_banner(char *b)
{
    // Enable color if possible
    if (has_colors())
    {
        attron(COLOR_PAIR(PAIR_BANNER));
    }

    // Determine where top-left corner of board belongs
    mvaddstr(g.top + 16, g.left + 64 - strlen(b), b);

    // Disable color if possible
    if (has_colors())
    {
        attroff(COLOR_PAIR(PAIR_BANNER));
    }
}


// Shuts down ncurses
void shutdown(void)
{
    endwin();
}


// Starts up ncurses and returns true iff successful
bool startup(void)
{
    // Initialize ncurses
    if (initscr() == NULL)
    {
        return false;
    }

    // Prepare for color if possible
    if (has_colors())
    {
        // Enable color
        if (start_color() == ERR || attron(A_PROTECT) == ERR)
        {
            endwin();
            return false;
        }

        // Initialize pairs of colors
        if (init_pair(PAIR_BANNER, FG_BANNER, BG_BANNER) == ERR ||
            init_pair(PAIR_GRID, FG_GRID, BG_GRID) == ERR ||
            init_pair(PAIR_BORDER, FG_BORDER, BG_BORDER) == ERR ||
            init_pair(PAIR_LOGO, FG_LOGO, BG_LOGO) == ERR ||
            init_pair(PAIR_DIGITS, FG_DIGITS, BG_DIGITS) == ERR ||
            init_pair(PAIR_WIN, FG_WIN, BG_WIN))
        {
            endwin();
            return false;
        }
    }

    // Don't echo keyboard input
    if (noecho() == ERR)
    {
        endwin();
        return false;
    }

    // Disable line buffering and certain signals
    if (raw() == ERR)
    {
        endwin();
        return false;
    }

    // Enable arrow keys
    if (keypad(stdscr, true) == ERR)
    {
        endwin();
        return false;
    }

    // Wait 1000 ms at a time for input
    timeout(1000);

    // w00t
    return true;
}

// move cursor on board
void reposition(int ch)
{
    switch (ch)
    {
        // move up
        case KEY_UP:
            // check end of board and if end move on another side
            g.y = (g.y != 0) ? g.y - 1 : 8;
            move(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 * (g.x + g.x / 3));
            break;

        // move down
        case KEY_DOWN:
            // check end of board and if end move on another side
            g.y = (g.y != 8) ? g.y + 1 : 0;
            move(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 * (g.x + g.x / 3));
            break;

        // move left
        case KEY_LEFT:
            // check end of board and if end move on another side
            g.x = (g.x != 0) ? g.x - 1 : 8;
            move(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 * (g.x + g.x / 3));
            break;

        // move right
        case KEY_RIGHT:
            // check end of board and if end move on another side
            g.x = (g.x != 8) ? g.x + 1 : 0;
            move(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 * (g.x + g.x / 3));
            break;
    }
}

// update numbers on board
void update_numbers(int ch)
{
    if (g.orig_board[g.y][g.x] == 0 && !g.win)
    {
        // save previous data for undo
        g.pr_y = g.y;
        g.pr_x = g.x;
        g.pr_value = g.board[g.y][g.x];

        // is previus
        g.pr_exist = true;

        // write number to board
        g.board[g.y][g.x] = ch - '0';

        // drow number on display
        ch = (ch == '0') ? '.' : ch;
        mvaddch(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 q* (g.x + g.x / 3), ch);
        refresh();
    }
    move(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 * (g.x + g.x / 3));
}

// copy number of board for comparison current data with original data
void copy_board(void)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            // copy number beetween boards
            g.orig_board[i][j] = g.board[i][j];
        }
    }
}

void checkrows(void)
{
    g.win_row = true;

    for (int i = 0; i < 9; i++)
    {
        if (i != g.x && (g.board[g.y][i] == g.board[g.y][g.x]))
        {
            g.win_row = false;
        }
    }

    // if in row not dublicate set color
    if (!g.win_row)
    {
        // Enable color if possible
        if (has_colors())
        {
            attron(COLOR_PAIR(PAIR_BORDER));
        }
    }
    // if dublicate set color
    else
    {
        // Enable color if possible
        if (has_colors())
        {
            attron(COLOR_PAIR(PAIR_DIGITS));
        }
    }

    // drow in certain color
    for (int i = 0; i < 9; i++)
    {
        // Determine char
        char c = (g.board[g.y][i] == 0) ? '.' : g.board[g.y][i] + '0';
        mvaddch(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 * (i + i / 3), c);
        refresh();
    }
    move(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 * (g.x + g.x / 3));
}

void checkcols(void)
{
    g.win_col = true;

    for (int i = 0; i < 9; i++)
    {
        if (i != g.x && (g.board[g.y][i] == g.board[g.y][g.x]))
        {
            g.win_col = false;
        }
    }

    // if dublicate in col set color
    if (!g.win_col)
    {
        // Enable color if possible
        if (has_colors())
        {
            attron(COLOR_PAIR(PAIR_BORDER));
        }
    }
    // if not dublicate in col ser color
    else
    {
        // Enable color if possible
        if (has_colors())
        {
            attron(COLOR_PAIR(PAIR_DIGITS));
        }
    }

    // drow in certain color
    for (int i = 0; i < 9; i++)
    {
        // Determine char
        char c = (g.board[i][g.x] == 0) ? '.' : g.board[i][g.x] + '0';
        mvaddch(g.top + i + 1 + i / 3, g.left + 2 + 2 * (g.x + g.x / 3), c);
        refresh();
    }
    move(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 * (g.x + g.x / 3));
}

void checkboxes(void)
{
    //start point for look dublicate in box
    int start_gy = (g.y / 3) * 3, start_gx = (g.x / 3) * 3;
    // count of dublicates in box
    int count_number = 0;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (g.board[i + start_gy][j + start_gx] == g.board[g.y][g.x])
            {
                // count of dublicates
                count_number++;
            }
        }
    }

    // if more then one number in box
    if (count_number > 1)
    {
        g.win_box = false;
        // Enable color if possible
        if (has_colors())
        {
            attron(COLOR_PAIR(PAIR_BORDER));
        }
    }
    else if (count_number == 1)
    {
        g.win_box = true;
        // Enable color if possible
        if (has_colors())
        {
            attron(COLOR_PAIR(PAIR_DIGITS));
        }
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            // Determine char
            char c = (g.board[i + start_gy][j + start_gx] == 0) ? '.' : g.board[i + start_gy][j + start_gx] + '0';
            mvaddch(g.top + (i + start_gy)  + 1 + (i + start_gy) / 3, g.left + 2 + 2 * ((j + start_gx) + (1 + start_gx) / 3), c);
            refresh();
        }
        move(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 * (g.x + g.x / 3));
    }

}

void checkwin(void)
{
//    bool board_full = false;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            // check if board is full and or any of wins is false
            if (!(g.win_box || g.win_col || g.win_row) || g.board[i][j] == 0)
            {
                g.win = false;
                return;
            }
        }
    }
    g.win = true;
    draw_numbers();

    // draw winner banner
    char reminder[36];
    sprintf(reminder, "   YOU WON SUDOKU!   ");
    mvaddstr(g.top + 14, g.left + 25 - strlen(reminder), reminder);
    move(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 * (g.x + g.x / 3));

}

void undo_move(void)
{
    // check if previus exist
    if (g.pr_exist)
    {
        // restore prev coordinates cursor
        g.x = g.pr_x;
        g.y = g.pr_y;

        // move cursor
        move(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 * (g.x + g.x / 3));

        // restore value
        g.board[g.y][g.x] = g.pr_value;

        // draw value on the board
        char c = (g.board[g.y][g.x] == 0) ? '.' : g.board[g.y][g.x] + '0';
        mvaddch(g.top + g.y + 1 + g.y / 3, g.left + 2 + 2 * (g.x + g.x / 3), c);
        refresh();

        // change colors of board
        checkrows();
        checkcols();
        checkboxes();
        checkwin();
    }
}



