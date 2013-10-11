#ifndef BUILDIN_H
#define	BUILDIN_H
#include "Interpreter.h"
#include "string.h"


using namespace std;

class Jobs{
public:
    Jobs* next;
    int pids[3];
    int fd[2];
    int fd2[2];
    string command;
public:
    Jobs();
    int getPid(int index);
    string getCommand();
};

class BuildIn {
public:
    Jobs* jobs;
    int numJob;
    BuildIn();
    void handleCD(command thisCommand);
    void handleExit(command thisCommand);
    void handleFG(command thisCommand);
    void handleJobs(command thisCommand); 
    void waitingFor(string command,int pid[],int fd[],int fd2[]);
    int storeJob(string command,int pid[],int fd[],int fd2[]);
    string getCurrentDic();
};

#endif	

