#ifndef BUILDIN_H
#define	BUILDIN_H
#include "Interpreter.h"
#include "string.h"


using namespace std;

class Jobs{
public:
    int pids[3];
public:
    Jobs();
    int getPid(int index);
    string getCommand();
    string command;
};

class BuildIn {
public:
    Jobs jobs[100];
    BuildIn();
    void handleCD(command thisCommand);
    void handleExit(command thisCommand);
    void handleFG(command thisCommand);
    void handleJobs(command thisCommand); 
};

#endif	

