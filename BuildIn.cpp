#include "BuildIn.h"
#include "Interpreter.h"
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

BuildIn::BuildIn(){
    numJob=0;
}

string BuildIn::getCurrentDic(){
    char cwd[1000];
    if(!getcwd(cwd,1000)){
        perror("Error: Cannot obtain current working directory");
        exit(0);
    }
    return string(cwd);
}
void BuildIn::handleCD(command thisCommand){
    if(thisCommand.tokLen<3){
        string path=thisCommand.toks[1]->tok;
        string finalPath;
        if(path[0]=='/'){
            finalPath=path;
        }else if(((path.length()>1) && path[0]=='.' && path[1]=='/') || (path[0]=='.' && (path.length()==1))){
            finalPath=getCurrentDic();
            string rest=path.substr(1,path.length());
            finalPath.append(rest);
        }else if(((path.length()>2) && path[0]=='.' && path[1]=='.' && path[2]=='/') || (path[0]=='.' && path[1]=='.' && path.length()==2)){
            finalPath=getCurrentDic();
            int found=finalPath.find_last_of("/");
            finalPath=finalPath.substr(0,found);
            finalPath.append(path.substr(2,path.length()));
        }else{
            finalPath=getCurrentDic();
            finalPath.append("/");
            finalPath.append(path);
        }
        const char *pathTemp=finalPath.c_str();
        int ret=chdir(pathTemp);
        if(ret){
            cout << "[" << path << "]: cannot change directory\n";
        }
    }else{
        cout << "cd: wrong number of arguments\n";
    }
}

void BuildIn::handleExit(command thisCommand){
     if(thisCommand.tokLen<2){
         if(numJob>0){
             cout << "There is at least one suspended job\n";
         }else{
             exit(0);
         }
     }else{
         cout << "exit: wrong number of arguments\n";
     }
}

void BuildIn::handleFG(command thisCommand){
    for(int i=0;i<numJob;i++){
        if(jobs[i]->pids[0]!=0){
            if(kill(jobs[i]->pids[0],SIGCONT)==-1){
                cout << "Kill error\n";
            }
        }
        if(jobs[i]->pids[1]!=0){
            if(kill(jobs[i]->pids[1],SIGCONT)==-1){
                cout << "Kill error\n";
            }
        }
        if(jobs[i]->pids[2]!=0){
            if(kill(jobs[i]->pids[2],SIGCONT)==-1){
                cout << "Kill error\n";
            }
        }
    }
}

void BuildIn::handleJobs(command thisCommand){
    if(numJob>0){
    for(int i=0;i<numJob;i++){
        cout << "[" << i+1 << "] " << jobs[i]->getCommand() << "\n";
    }
    }else{
        cout << "No suspended jobs\n";
    }
}

int BuildIn::storeJob(string command, int pid[]){
    int size;
    size=sizeof pid/sizeof(int);
    if(size>3){return -1;}
    jobs[numJob]=new Jobs();
    jobs[numJob]->command=command;
    jobs[numJob]->pids[0]=pid[0];
    jobs[numJob]->pids[1]=pid[1];
    jobs[numJob++]->pids[2]=pid[2];
    return 0;
}
Jobs::Jobs(){
    for(int i=0;i<3;i++){
        pids[i]=0;
    }
    command="";
}

string Jobs::getCommand(){
    return command;
}
int Jobs::getPid(int index){
    if(index>2){return -1;}
    return pids[index];
}
