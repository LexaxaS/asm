#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "tools.hpp"

typedef int cmdel_t;
typedef unsigned long long error_t;

#define prELEM "d"

const size_t CodeArrayMaxLen = 1000;

enum CMD_Commands
    {
    #define DEF_CMD(name, num, ...)\
        CMD_##name = num,
    #include "commands.txt"
    #undef DEF_CMD
    };

enum REG_nums
    {REG_RAX = 1,
     REG_RBX = 2,
     REG_RCX = 3,
     REG_RDX = 4};

enum ArgFormats
    {ARG_FORMAT_IMMED = (1 << 5),
     ARG_FORMAT_REG = (1 << 6),
     ARG_FORMAT_RAM = (1 << 7)};

enum Errors
    {SYNTAX_ERROR = 1};

struct Arg
    {
    cmdel_t immed;
    cmdel_t regnum;
    cmdel_t argFormat;
    };

error_t compile(char* fpname, size_t *arrLen, cmdel_t* codeArray);
error_t writeInFile(FILE* filedest, cmdel_t* codeArray, size_t arrLen);
error_t writeInFileBin(FILE* filedest, cmdel_t* codeArray, size_t arrLen);
error_t SetArg(Text* codeStruct, cmdel_t* codeArray, size_t curLine, cmdel_t code, size_t* position, size_t cmdLen);
error_t parseLine(cmdel_t* codeArray, Text* codeStruct, size_t curLine, size_t* position);

int main()
    {
    FILE* fpsource = fileopener("data.txt");
    FILE* fpdest = fileopenerW("notbin.txt");
    FILE* fpdestbin = fileopenerWB("assemblerfile.bin");
    size_t arrLen = 0;
    cmdel_t* codeArray = (cmdel_t*) calloc(CodeArrayMaxLen, sizeof(*codeArray));
    compile("data.txt", &arrLen, codeArray);
    writeInFile(fpdest, codeArray, arrLen);
    printf("hyum\n", arrLen);
    for(size_t i = 0; i < arrLen; i++)
        printf("%d ", codeArray[i]);
    //writeInFileBin(fpdestbin, codeArray, arrLen);
    fclose(fpsource);
    fclose(fpdest);
    printf("huy\n");
    return 0;
    }

error_t encodeReg(int *regNum, char* reg)
    {
    if (strcmp(reg, "rax") == 0)
        *regNum = REG_RAX;
    else if (strcmp(reg, "rbx") == 0)
        *regNum = REG_RBX;
    else if (strcmp(reg, "rcx") == 0)
        *regNum = REG_RCX;
    else if (strcmp(reg, "rdx") == 0)
        *regNum = REG_RDX;
    return errno;
    }

error_t emitCode(Arg* arg, cmdel_t* codeArr, size_t* pos, cmdel_t code)
    { 
    printf("arg\n");
    codeArr[(*pos)++] = code;
    if (arg->argFormat & ARG_FORMAT_REG)
        codeArr[(*pos)++] = arg->regnum;
    if (arg->argFormat & ARG_FORMAT_IMMED)
        codeArr[(*pos)++] = arg->immed;
    return errno;
    }

// error_t emitCodeArg(cmdel_t* codeArr, size_t* pos, cmdel_t code, cmdel_t value)
//     {
//     codeArr[(*pos)++] = code;
//     codeArr[(*pos)++] = value;
//     return errno;
//     }

error_t emitCodeNoArg(cmdel_t* codeArr, size_t* pos, cmdel_t code)
    {
    printf("noarg\n");
    codeArr[(*pos)++] = code;
    return errno;
    }

error_t compile(char* fpname, size_t *arrLen, cmdel_t* codeArray)
    {
    printf("hyu\n");
    Text codeStruct = setbuf(fpname);

    size_t position = 0;

    for (size_t curLine = 0; curLine < codeStruct.nLines - 1; curLine++)
        {
        parseLine(codeArray, &codeStruct, curLine, &position);    
        }

    *arrLen = position;
    for (size_t i = 0; i < position; i++)
        {
        printf("%d ", codeArray[i]);
        }

    fclose(codeStruct.file);
    return errno;
    }

error_t parseLine(cmdel_t* codeArray, Text* codeStruct, size_t curLine, size_t* position)
    {
    #define DEF_CMD(name, num, isarg, ...)                                                        \
        if (strcasecmp(cmd, #name) == 0)                                                          \
            {                                                                                     \
            if (isarg)                                                                            \
                SetArg(codeStruct, codeArray, curLine, CMD_##name, position, cmdlen);           \
            else                                                                                  \
                emitCodeNoArg(codeArray, position, CMD_##name);                                  \
            }                                                                                     \
        else

    char cmd[5] = "";
    size_t cmdlen = 0;
    if (sscanf(codeStruct->lines[curLine].linePtr, "%s%n", cmd, &cmdlen) != 1)
            return SYNTAX_ERROR;
    printf("<%s>\n", cmd);

    #include "commands.txt"

    /*else*/  printf("unknown\n");

    #undef DEF_CMD
    return errno;
    }



error_t SetArg(Text* codeStruct, cmdel_t* codeArray, size_t curLine, cmdel_t code, size_t* position, size_t cmdLen)
    {
    Arg arg = {};

    const char* strptr = codeStruct->lines[curLine].linePtr + cmdLen;
    const char* stbracketPtr = strchr(strptr, '[');
    const char* clBracketPtr = strchr(strptr, ']');
    printf("\n%s\n", strptr);
    if (stbracketPtr or clBracketPtr)
    {
        if (!stbracketPtr || !clBracketPtr)
        {
            return SYNTAX_ERROR;
        }
        strptr = stbracketPtr + 1;
        arg.argFormat |= ARG_FORMAT_RAM;
    }

    size_t scanlen = 0;
    cmdel_t reg = 0;
    if (sscanf(strptr, " r%cx %n", &reg, &scanlen) == 1)
        {
        arg.argFormat |= ARG_FORMAT_REG;
        int regNum = reg - 'a' + 1;
        // encodeReg(&regNum, reg);
        printf("n = %d\n", scanlen);
        arg.regnum = regNum;
        strptr += scanlen;
        printf("reg = %c\n", reg);
        }

    
    int immed = 0;
    if (*strptr == '+')
        strptr += 1;
    if (sscanf(strptr, " %"prELEM" %n", &immed, &scanlen) == 1)
        {
        arg.argFormat |= ARG_FORMAT_IMMED;
        printf("af = %d\n", arg.argFormat);
        arg.immed = immed;
        strptr += scanlen;
        printf("imm = %d\n", immed);
        }

    printf("af = %d\n", arg.argFormat);
    code |= arg.argFormat;
    emitCode(&arg, codeArray, position, code);
    return errno;
    }

error_t writeInFile(FILE* filedest, cmdel_t* codeArray, size_t arrLen)
    {
    printf("\n\n\n");

    for (size_t i = 0; i < arrLen; i++)
        {
        printf("g = %d\n", codeArray[i]);
            cmdel_t code = codeArray[i]; 
        fprintf(filedest, "%d", 1); 
        }
    return errno;
    }

error_t writeInFileBin(FILE* filedest, cmdel_t* codeArray, size_t arrLen)
    {
    printf("\nbin\n\n");
    fwrite(codeArray, sizeof(*codeArray), arrLen, filedest);
    for (size_t i = 0; i < arrLen; i++)
        printf("<%d> ", codeArray[i]);
    return errno;
    }
