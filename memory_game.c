#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// board is 4x4 so 16 cards total, 8 pairs
#define ROWS 4
#define COLS 4
#define TOTAL (ROWS * COLS)
#define SAVEFILE "save_game.txt"

// struct to hold each card's info
typedef struct {
    char sym;       // the letter on the card
    int shown;      // is it flipped up right now
    int done;       // has it been matched already
} Card;

void setup_board(Card board[ROWS][COLS]);
void do_shuffle(char *arr, int n);
void show_board(Card board[ROWS][COLS], int reveal_all);
int get_input(const char *s, int *r, int *c);
int all_matched(Card board[ROWS][COLS]);
void save(Card board[ROWS][COLS], int moves, int score);
int load(Card board[ROWS][COLS], int *moves, int *score);
void cls(void);

int main(void) {
    Card board[ROWS][COLS];
    int moves = 0;
    int score = 0;
    char buf[64];

    srand((unsigned int)time(NULL));

    printf("=== Memory Card Game ===\n");
    printf("type row and col to flip a card (ex: 2 3)\n");
    printf("other commands: save, load, quit\n\n");

    printf("load saved game? (y/n): ");
    fgets(buf, sizeof(buf), stdin);
    if (buf[0] == 'y' || buf[0] == 'Y') {
        if (load(board, &moves, &score)) {
            printf("loaded! moves: %d  score: %d\n\n", moves, score);
        } else {
            printf("no save found, starting fresh\n\n");
            setup_board(board);
        }
    } else {
        setup_board(board);
    }

    while (!all_matched(board)) {
        cls();
        printf("=== Memory Card Game ===   moves: %d   score: %d\n\n", moves, score);
        show_board(board, 0);
        printf("\npick a card (row col): ");

        if (!fgets(buf, sizeof(buf), stdin)) break;
        buf[strcspn(buf, "\n")] = '\0';

        if (strcmp(buf, "quit") == 0) {
            printf("quitting. score was: %d\n", score);
            return 0;
        }
        if (strcmp(buf, "save") == 0) {
            save(board, moves, score);
            printf("saved!\npress enter...");
            fgets(buf, sizeof(buf), stdin);
            continue;
        }
        if (strcmp(buf, "load") == 0) {
            if (load(board, &moves, &score))
                printf("loaded!\n");
            else
                printf("nothing to load\n");
            printf("press enter...");
            fgets(buf, sizeof(buf), stdin);
            continue;
        }

        int r1, c1;
        if (!get_input(buf, &r1, &c1)) {
            printf("bad input, try something like: 1 3\n");
            printf("press enter...");
            fgets(buf, sizeof(buf), stdin);
            continue;
        }

        // skip if already matched or showing
        if (board[r1][c1].done || board[r1][c1].shown) {
            printf("that cards already flipped or matched\n");
            printf("press enter...");
            fgets(buf, sizeof(buf), stdin);
            continue;
        }

        // flip first card
        board[r1][c1].shown = 1;
        cls();
        printf("=== Memory Card Game ===   moves: %d   score: %d\n\n", moves, score);
        show_board(board, 0);
        printf("\ngot [%c] -- now pick second card: ", board[r1][c1].sym);

        if (!fgets(buf, sizeof(buf), stdin)) break;
        buf[strcspn(buf, "\n")] = '\0';

        int r2, c2;
        if (!get_input(buf, &r2, &c2)) {
            printf("bad input\n");
            board[r1][c1].shown = 0;
            printf("press enter...");
            fgets(buf, sizeof(buf), stdin);
            continue;
        }

        // cant pick the same card twice
        if (r1 == r2 && c1 == c2) {
            printf("thats the same card lol\n");
            board[r1][c1].shown = 0;
            printf("press enter...");
            fgets(buf, sizeof(buf), stdin);
            continue;
        }

        if (board[r2][c2].done || board[r2][c2].shown) {
            printf("that one is already used\n");
            board[r1][c1].shown = 0;
            printf("press enter...");
            fgets(buf, sizeof(buf), stdin);
            continue;
        }

        board[r2][c2].shown = 1;
        moves++;

        cls();
        printf("=== Memory Card Game ===   moves: %d   score: %d\n\n", moves, score);
        show_board(board, 0);

        if (board[r1][c1].sym == board[r2][c2].sym) {
            // match!
            board[r1][c1].done = 1;
            board[r2][c2].done = 1;
            score += 10;
            printf("\n  nice, thats a match! +10\n");
            printf("press enter...");
            fgets(buf, sizeof(buf), stdin);
        } else {
            // no match, hide them again
            printf("\n  no match\n");
            printf("press enter to hide...");
            fgets(buf, sizeof(buf), stdin);
            board[r1][c1].shown = 0;
            board[r2][c2].shown = 0;
        }
    }

    cls();
    printf("=== Memory Card Game ===\n\n");
    show_board(board, 1);
    printf("\nyou won!!\n");
    printf("moves:  %d\n", moves);
    printf("score:  %d\n", score);

    // little bonus if they did really well
    // TODO: maybe scale this better later
    if (moves <= (TOTAL / 2) + 2)
        printf("nice memory, bonus +20!\n");

    remove(SAVEFILE);
    return 0;
}

// fisher yates shuffle, learned this in class
void do_shuffle(char *arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

void setup_board(Card board[ROWS][COLS]) {
    // 8 letters, each used twice = 16 cards
    char letters[] = {'A','B','C','D','E','F','G','H'};
    char deck[TOTAL];

    for (int i = 0; i < TOTAL / 2; i++) {
        deck[i] = letters[i];
        deck[i + TOTAL / 2] = letters[i];
    }

    do_shuffle(deck, TOTAL);

    int k = 0;
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            board[r][c].sym = deck[k++];
            board[r][c].shown = 0;
            board[r][c].done = 0;
        }
    }
}

void show_board(Card board[ROWS][COLS], int reveal_all) {
    // print column numbers
    printf("     ");
    for (int c = 0; c < COLS; c++) printf("  %d ", c + 1);
    printf("\n     ");
    for (int c = 0; c < COLS; c++) printf("----");
    printf("\n");

    for (int r = 0; r < ROWS; r++) {
        printf("  %d |", r + 1);
        for (int c = 0; c < COLS; c++) {
            if (reveal_all || board[r][c].done)
                printf(" [%c]", board[r][c].sym);
            else if (board[r][c].shown)
                printf(" (%c)", board[r][c].sym);   // currently flipped
            else
                printf(" [ ]");                      // face down
        }
        printf("\n");
    }
}

// returns 1 if input is valid, 0 if not
int get_input(const char *s, int *r, int *c) {
    int row, col;
    if (sscanf(s, "%d %d", &row, &col) != 2) return 0;
    if (row < 1 || row > ROWS) return 0;
    if (col < 1 || col > COLS) return 0;
    *r = row - 1;  // convert to 0-indexed
    *c = col - 1;
    return 1;
}

int all_matched(Card board[ROWS][COLS]) {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (!board[r][c].done) return 0;
    return 1;
}

void save(Card board[ROWS][COLS], int moves, int score) {
    FILE *f = fopen(SAVEFILE, "w");
    if (!f) {
        printf("couldnt open save file\n");
        return;
    }
    fprintf(f, "%d %d\n", moves, score);
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            fprintf(f, "%c %d %d\n", board[r][c].sym, board[r][c].shown, board[r][c].done);
        }
    }
    fclose(f);
}

int load(Card board[ROWS][COLS], int *moves, int *score) {
    FILE *f = fopen(SAVEFILE, "r");
    if (!f) return 0;

    if (fscanf(f, "%d %d\n", moves, score) != 2) {
        fclose(f);
        return 0;
    }

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            int shown, done;
            if (fscanf(f, " %c %d %d\n", &board[r][c].sym, &shown, &done) != 3) {
                fclose(f);
                return 0;
            }
            board[r][c].done = done;
            // if a card was mid-flip when saved just hide it
            board[r][c].shown = 0;
        }
    }

    fclose(f);
    return 1;
}

void cls(void) {
    // works on linux/mac, untested on windows
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
