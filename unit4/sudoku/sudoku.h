// Compile-time options for the game of Sudoku.

// Game's author
#define AUTHOR "Alexander Kruk"

// Game's title
#define TITLE "Sudoku"

// Banner's colors
#define FG_BANNER COLOR_RED
#define BG_BANNER COLOR_BLACK

// Grid's colors
#define FG_GRID COLOR_RED
#define BG_GRID COLOR_BLACK

// Border's colors
#define FG_BORDER COLOR_WHITE
#define BG_BORDER COLOR_RED

// Logo's colors
#define FG_LOGO COLOR_WHITE
#define BG_LOGO COLOR_BLACK

// Digits's colors
#define FG_DIGITS COLOR_YELLOW
#define BG_DIGITS COLOR_BLACK

// Digits's colors win
#define FG_WIN COLOR_GREEN
#define BG_WIN COLOR_BLACK

// Nicknames for pairs of colors
enum { PAIR_BANNER = 1, PAIR_GRID, PAIR_BORDER, PAIR_LOGO, PAIR_DIGITS, PAIR_WIN};
