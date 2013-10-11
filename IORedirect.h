#ifndef IOREDIRECT_H_INCLUDED
#define IOREDIRECT_H_INCLUDED

#include "Interpreter.h"

/*
return value
0:Redirect output successfully or no redirection is needed
-1:permission denied
-2:no file found
-3:other error occured in file opening
-4:error occured in dup2
-5:error occured in close
*/


class inRedirection
{
    public:
    bool redirectionStatus;
    char *inFile;
    void checkRed(command&, int&,int);
    inRedirection();
    int setInRed();
};

class outRedirection
{
    public:
    bool redirectionStatus;
    bool reWrite;
    char *outFile;
    void checkRed(command&, int&,int);
    int setOutRed();
    outRedirection();
};

class pipeArr
{
    public:
    int pipefd[3][2];
    int pipeNum;
    int pipePos;
    int comPos;
    bool creatPipe(int);
    bool setPipe(int);
};





#endif // IOREDIRECT_H_INCLUDED
