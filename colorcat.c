//#!/usr/bin/c
//shebang to run as c script

#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wchar.h>

typedef const char* color_t;
typedef int color_index_t;

void printHelp(int argc, char** argv);

color_index_t getIndex(char opt);

void printFile(FILE* f, color_index_t ind);

void colorPrint(const wchar_t* wc, color_index_t ind);

static color_t colors[] = {
    "\x1b[91m", // Red
    "\x1b[93m", // Yellow
    "\x1b[92m", // Green
    "\x1b[94m", // Blue
    "\x1b[95m", // Magenta
    "\x1b[96m", // Cyan
    "\x1b[0m",  // Reset
};

int main(int argc, char** argv) {
    setlocale(LC_CTYPE, "");
    wchar_t buf[256] = {};
    color_index_t index = -1; 
    int file_flag = 1, string_flag = 0;
    FILE* read = stdin;

    if (argc > 1) {
        int opt;
        opterr = 0;

        while ((opt = getopt(argc, argv, "scf")) != -1) {
            switch (opt) {
            case 's':
                mbstowcs(buf, argv[optind], sizeof argv[optind]);
                string_flag = 1;
                if (file_flag != 2) {
                    file_flag = 0;
                }
                break;
            case 'f':
                read = fopen(argv[optind], "r");
                file_flag = 2;
                break; 
            case 'c':
                index = getIndex(*argv[optind]);
                break;
            case 'h':
                printHelp(argc, argv);
            default:
                fprintf(stderr, "Unknown option: %c!\n", optopt);
                printHelp(argc, argv);
            }
        }
    } 
    if (file_flag) {
        printFile(read, index);
    }
    if (string_flag) {
        colorPrint(buf, index);
    }

}

void printHelp(int argc, char** argv) {
    puts("Usage");
    printf("\t%s\n", argv[0]);
    puts("\t-h : help");
    puts("\t-f [filename]");
    puts("\t-s [string]");
    puts("\t-c [color]");
    puts("\t\tColors: b - blue");
    puts("\t\t\tr - red");
    puts("\t\t\ty - yellow");
    puts("\t\t\tg - green");
    puts("\t\t\tm - magenta");
    puts("\t\t\tc - cyan");

    exit(1);
}

color_index_t getIndex(char opt) {
    switch(opt) {
    case 'r':
        return 0;
    case 'y':
        return 1;
    case 'g':
        return 2;
    case 'b':
        return 3;
    case 'm': 
        return 4;
    case 'c':
        return 5;
    default:
        return -1;
    }
}

void printFile(FILE* f, color_index_t ind) {
    wchar_t buf[256] = {};
    int i = 0;
    while((buf[i++] = fgetwc(f)) != EOF){
        if (i == 255) {
            i = 0; // Loop char
            colorPrint(buf, ind);
            memset(buf, 0, sizeof buf);
        }
    }
    colorPrint(buf, ind);
}

void colorPrint(const wchar_t* wc, color_index_t ind) {
    if (ind < 0) {
        while (L'\0' != *wc) {
            wprintf(L"%s%lc", colors[++ind], *wc);
            if (ind == 5) ind = -1;
            wc++;
        }
    } else {
        while (L'\0' != *wc) {
            wprintf(L"%s%lc", colors[ind], *wc++);
        }
    }
}
