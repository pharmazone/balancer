/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@login.dknet.dk> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 *
 * $Id: file2c.c,v 1.1 2007-12-14 07:15:53 pg Exp $
 *
 */

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    int i,j,k;
    //char s[10];

    if (argv[1] && !strcmp(argv[1], "--help")) {
        fprintf(stderr,
            "Usage: file2c [<begin> [<end>]]\n"
            "\n"
            "Read characters from stdin and prints their comma-separated\n"
            "codes. String length will not exceed 72 symbols.\n"
            "If set, <begin> will be printed before character codes and\n"
            "<end> after them.\n");
        return 1;
    }

    if (argc > 1)
        printf("%s\n",argv[1]);
    k = 0;
    j = 0;
    while((i = getchar()) != EOF) {
        if(k++) {
            putchar(',');
            j++;
        }
        if (j > 70) {
            putchar('\n');
            j = 0;
        }
        printf("%d",i);
        if (i > 99)
            j += 3;
        else if (i > 9)
            j += 2;
        else
            j++;
    }
    putchar('\n');
    if (argc > 2)
        printf("%s\n",argv[2]);
    return 0;
}
