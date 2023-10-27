#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <assert.h>
#include <string.h>
#include "tools.hpp"


void bufClear(void)
    {
    while (getchar() != '\n') { ; }
    }

bool isZero(double x)
    {
    return fabs(x) < AT;
    }

bool areEqual(double a, double b)
    {
    return fabs(a - b) < AT;
    }

Text setbuf(char filename_i[])
    {
    assert(filename_i);

    Text text = {};

    FILE *fileEO = fileopener(filename_i);
    text.size = fileLen(filename_i);  
    char* buf = (char*) calloc(text.size + 1, sizeof(char));
    buf[text.size] = '\0';
    fread(buf, sizeof(char), text.size, fileEO);
    fclose(fileEO);      
    text.bufPtr = buf;
    text.nLines = countLines(buf);
    text.lines = setPtr(buf, text.nLines, text.size);
    text.file = fileEO;
    return text;
    }  

const String* setPtr(char* buf, size_t nLines, size_t flen)
    {

    size_t buf_i = 0;
    size_t ptr_j = 1;

    String* pointers = (String*) calloc(nLines + 1, sizeof(*pointers));

    size_t lineLen = 0;

    pointers[0].linePtr = &buf[0];

    printf("hyu2\n");
    while (buf_i + 2 < flen)
        {
        if (buf[buf_i] == '\r')
            {
            if (buf[buf_i + 2] != '\r')
                {
                pointers[ptr_j - 1].length = lineLen;
                pointers[ptr_j].linePtr = &(buf[buf_i + 2]);
                ptr_j++;
                }
            buf[buf_i] = '\0';
            buf_i += 2;
            lineLen = 0;
            }
        else if (buf[buf_i] == '\n')
            {
            pointers[ptr_j - 1].length = lineLen;
            pointers[ptr_j].linePtr = &(buf[buf_i + 1]);
            lineLen = 0;
            buf_i++;
            ptr_j++;
            }
        else
            {
            buf_i++;
            lineLen++;
            }
        }
    pointers[ptr_j].linePtr = 0;
    return pointers;
    }

size_t countLines(const char* str)
{
    size_t nlines = 1;
    const char* terPtr = strchr(str, '\n');
    while (terPtr != NULL)
        {
        nlines++;
        terPtr = strchr(terPtr + 1, '\n');
        }
    return nlines;
}

void printBuf(const String* pointers, FILE *SortedEO)
    {
    assert(pointers);
    while (*pointers->linePtr != NULL)
        {
        fprintf(SortedEO, "%s\n", *pointers->linePtr);
        *pointers++;
        }
    }

void bufReturn(char* buf, size_t flen)
    {
    assert(buf);
    assert(flen);

    size_t buf_i = 0;
    while (buf_i + 2 < flen)
        {
        if (buf[buf_i] == '\0')
            {
            buf[buf_i] = '\r';
            buf_i++;
            }
        else
            {
            buf_i++;
            }
        }
    }

FILE *fileopener(char filename[])
    {
    assert(filename);

    FILE *fileEO = fopen(filename, "rb");
    if (fileEO == NULL)
        {
        perror("FILE_ERROR");
        }
    return fileEO;
    }

FILE *fileopenerRB(char filename[])
    {
    assert(filename);

    FILE *fileEO = fopen(filename, "rb");
    if (fileEO == NULL)
        {
        printf("FILE_ERROR\n");
        }
    return fileEO;
    }

FILE *fileopenerW(char filename[])
    {
    assert(filename);

    FILE *fileEO = fopen(filename, "w");
    if (fileEO == NULL)
        {
        printf("FILE_ERROR\n");
        }
    return fileEO;
    }

FILE *fileopenerWB(char filename[])
    {
    assert(filename);

    FILE *fileEO = fopen(filename, "wb");
    if (fileEO == NULL)
        {
        printf("FILE_ERROR\n");
        }
    return fileEO;
    }


size_t fileLen(const char * file)
    {
    assert(file);
    
    struct stat st = {};
    if (stat(file, &st) == 0)
        return (size_t) st.st_size;
    return 0;
    }

