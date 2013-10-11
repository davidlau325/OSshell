#ifndef _EXCUTOR
#define _EXCUTOR
#include "Interpreter.h"
#include "IORedirect.h"
#include <sys/types.h>
#include "BuildIn.h"

class process
{
    public:
    pid_t shellPid;
    command shellCom;
    process();
    process(command, pid_t);
};

class executor
{
    public:
    process runPro;
    process waitPro[100];
    inRedirection inRed;
    outRedirection outRed;
    pipeArr pipef;
    public:
    int runCom(command&,BuildIn&);
    void excute(command&,int);
};
#endif
