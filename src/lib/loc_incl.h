#include <stdarg.h>

#define putchar(d) U0putch(d)
#define PAD_RIGHT 1
#define PAD_ZERO 2

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

void U0putch(int);
int U0getch();
static void printchar(char **, int);
static int prints(char **, const char *, int, int);
static int printi(char **, int, int, int, int, int, int);
static int print(char **, const char *, va_list);
