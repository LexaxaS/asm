#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>


struct String
{
    const char* linePtr;
    size_t length;
};

struct Text
{
    char* bufPtr;
    String* lines;
    size_t size;
    size_t nLines;
    FILE* file;
};

const double AT = 1e-6;

size_t countLines(const char* str);
size_t fileLen(const char * file);
bool isZero(double x);
bool areEqual(double a, double b);

Text setbuf(char filename_i[]);
String* setPtr(char* buf, size_t nLines, size_t flen);
void printBuf(const String* pointers, FILE *SortedEO);
void bufReturn(char* buf, size_t flen);
void bufClear(void);

FILE *fileopener(char filename[]);
FILE *fileopenerRB(char filename[]);
FILE *fileopenerW(char filename[]);
FILE *fileopenerWB(char filename[]);
size_t fileLen(const char * file);


#endif
