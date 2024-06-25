#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <assert.h>
#include <stdint.h>
#include <limits.h>
#include <signal.h>
#include <errno.h>

#ifndef _WIN32
#include <uinstd.h>
#else
#include <getopt.h>
#include <Windows.h>
#endif

#if !defined(__BIG_ENDIAN__) || !defined(__LITTLE_ENDIAN__)
#    if ('\x01\x02\x03\x04' == 0x01020304) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#        define __BIG_ENDIAN__
#    endif
#endif

#define COLORCODE L"\x1b[38;2;%d;%d;%dm"
#define COLOR_FORMAT(r, g, b) COLORCODE, r, g, b

typedef unsigned char uchar;

typedef union crgba
{
    struct
    {
#ifdef __BIG_ENDIAN__
        uchar r;
        uchar g;
        uchar b;
        uchar a;
#else 
        uchar a;
        uchar b;
        uchar g;
        uchar r;
#endif
    };
    uint32_t rgb;
} crgba;


#define RGBA(r, g, b, a) (((uint32_t)(r) << 24) | ((uint32_t)(g) << 16) | (uint32_t)(b) << 8 | (uint32_t)a)

#define RGB(r, g, b) RGBA(r, g, b, 0xFF)

#define RED(rgb_)   (((uint32_t)(rgb_) & 0xFF000000) >> 24)

#define GREEN(rgb_) (((uint32_t)(rgb_) & 0x00FF0000) >> 16)

#define BLUE(rgb_)  (((uint32_t)(rgb_) & 0x0000FF00) >> 8)

#define RGB_COLOR_FORMAT(rgb_) COLORCODE, RED(rgb_.r), GREEN(rgb_.r), BLUE(rgb_.r)

static const crgba RGB_COLOR_TABLE[] = {
    { .rgb = RGB(255, 0  , 0  ) },
    { .rgb = RGB(255, 165, 0  ) },
    { .rgb = RGB(255, 255, 0  ) },
    { .rgb = RGB(0  , 255, 0  ) },
    { .rgb = RGB(0  , 0  , 255) },
    { .rgb = RGB(75 , 0  , 130) },
    { .rgb = RGB(127, 0  , 255) },
};

static const int TABLE_SIZE = sizeof(RGB_COLOR_TABLE) / sizeof(crgba);

struct Settings
{
    int number_lines;
};
static struct Settings GlobalSettings = { 0 };

static const struct option long_options[] = {
    { "show-all", no_argument, NULL, 'A' },
    { "help",      no_argument, NULL, 'h' },
    { 0, 0, 0, 0 }
};

void help();

void sig(int u);

void reset();

void cat(FILE *const in, FILE *const out);

void char_out(wchar_t c, FILE* const out);

int main(int argc, char** argv)
{
    setlocale(LC_CTYPE, "");
    signal(SIGINT, sig);

    FILE *in = stdin, *const out = stdout;
    while (1)
    {
        int idx = 0;
        int c = getopt_long(argc, argv, "Ah", long_options, &idx);
        if (c == -1)
            break;

        switch (c)
        {
        case 'h':
            help();
            break;
        default:
            exit(1);
        }
    }
    if (optind == argc)
    {
        cat(in, out);
    }
    
    for (int f = optind; f < argc; ++f)
    {
        const char* fname = argv[f];
        int err = 0;
        if (strcmp(fname, "-") == 0)
        {
            in = stdin;
        }
#ifdef _MSC_VER
        else if (GetFileAttributesA(fname) & FILE_ATTRIBUTE_DIRECTORY)
        {
            printf("%s: Is a directory\n", fname);
            continue;
        }
#endif
        else
        {
            err = fopen_s(&in, fname, "r+");
        }
        switch (err)
        {
        case 0:
            cat(in, out);
            break;
        case ENOENT:
            printf("%s: No such file or directory\n", fname);
            exit(1);
        case EACCES:
            printf("%s: Permission denied\n", fname);
            exit(1);
        case EISDIR:
            printf("%s: Is a directory\n", fname);
            //exit(1);
            break;
        case ENXIO:
            printf("%s: Is not a regular file\n", fname);
            exit(1);
        case EPIPE:
            exit(1);
        }
    }
    return 0;
}

void reset()
{
    puts("\x1b[0m");
}

void cat(FILE *const in, FILE *const out)
{
    wchar_t c;
    size_t line_number = 1;

    while ((c = fgetwc(in)) != WEOF)
    {
        char_out(c, out);

    }
    if (ferror(in))
    {
        fprintf(stderr, "ERROR READING FROM %d", _fileno(in));
    }
    reset();
}

void char_out(wchar_t c, FILE* const out)
{
    static float t = 0.0f;
    static crgba r, v;
    static int table_index = 0, r_, g_, b_, up = 1;

    if (c != L' ' && c != L'\0')
    {
        r = RGB_COLOR_TABLE[table_index];
        v = RGB_COLOR_TABLE[table_index + 1];
        r_ = (int)((1 - t) * r.r + t * v.r);
        g_ = (int)((1 - t) * r.g + t * v.g);
        b_ = (int)((1 - t) * r.b + t * v.b);
        if (t >= 1.0f)
        {
            t = 0.0f;
            table_index = (table_index == TABLE_SIZE - 2) ? 0 : table_index + 1;
        }
        else
        {
            t += 0.05f;
        }
        fwprintf(out, COLOR_FORMAT(r_, g_, b_));
    }
    _putw(c, out);
}

void help()
{
    const wchar_t* helpstr[] = {
        L"Usage: rcat [OPTIONS]... FILE...",
        L"",
        L"Concatenate files and print to standard output with a rainbow color.",
        L"",
        L"OPTIONS",
        L"-H,--help    This output.",
        L"",
        L"Examples:",
        L"  rcat f - g      Output f's contents, then stdin, then g's contents",
        L"  rcat            Copy standard input to standard output",
        L"  fortune | rcat  Display a rainbow cookie",
        NULL
    };

    int i = 0;
    while (helpstr[i])
    {
        int j = 0;
        while (helpstr[i][j] != L'\0')
        {
            char_out(helpstr[i][j], stdout);
            ++j;
        }
        puts("");
        ++i;
    }
    reset();
    exit(1);
}

void sig(int u)
{
    (void)u;
    reset();
}

