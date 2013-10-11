#include "BuildIn.h"
#include "Interpreter.h"
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

BuildIn::BuildIn(){
    numJob=0;
    jobs=NULL;
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
    if(thisCommand.tokLen==2){
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
     if(thisCommand.tokLen==1){
         if(numJob>0){
             cout << "There is at least one suspended job\n";
         }else{
             exit(0);
         }
     }else{
         cout << "exit: wrong number of arguments\n";
     }
}

void BuildIn::waitingFor(string command, int pid[], int fd[], int fd2[]){
    int status;
    for(int i=0;i<3;i++){
        if(pid[i]!=-1){
            if(waitpid(pid[i],&status,WUNTRACED)!=pid[i]){
                    cout << "Error: waitpid error\n";
            }else{
                if(WIFSTOPPED(status)){
                    if(i==0){
                        storeJob(command,pid,fd,fd2);
                    }
                }else{
                    switch(i){
                        case 0:
                            if(fd[1]!=-1){close(fd[1]);}
                            break;
                        case 1:
                            if(fd[0]!=-1){close(fd[0]);}
                            if(fd2[1]!=-1){close(fd2[1]);}
                            break;
                        case 2:
                            if(fd2[0]!=-1){close(fd2[0]);}
                            break;
                        default:
                            cout << "Super Weird Error!\n";
                            break;
                    }
                }
            }
        }
    }
}

void BuildIn::handleFG(command thisCommand){
    if(thisCommand.tokLen==2){
    int index=atoi(thisCommand.toks[1]->tok.c_str());   
    if(index<numJob || index >0){
        Jobs* head=jobs;
        for(int i=1;i<index;i++){
            head=head->next;
        }
        Jobs* current=new Jobs();
        current->command=head->command;
        current->fd[0]=head->fd[0];
        current->fd[1]=head->fd[1];
        current->fd2[0]=head->fd2[0];
        current->fd2[1]=head->fd2[1];
        for(int j=0;j<3;j++){
            current->pids[j]=head->pids[j];
            if(head->pids[0]!=-1){
            if(kill(head->pids[0],SIGCONT)==-1){
                cout << "Error: Cannot send kill signal\n";
            }
            }
        }
        if(head->next==NULL){
            free(head);
        }else{
            Jobs* temp=head->next;
            head->command=temp->command;
            head->pids[0]=temp->pids[0];
            head->pids[1]=temp->pids[1];
            head->pids[2]=temp->pids[2];
            head->fd[0]=temp->fd[0];
            head->fd[1]=temp->fd[1];
            head->fd2[0]=temp->fd2[0];
            head->fd2[1]=temp->fd2[1];
            head->next=temp->next;
            free(temp);
        }
        cout << "Job wake up: " << head->command << "\n";
        numJob--;
        int status;
        for(int j=0;j<3;j++){
            if(head->pids[j]!=-1){
                if(waitpid(head->pids[j],&status,WUNTRACED)!=head->pids[j]){
                    cout << "Error: waitpid error";
                }else{
                    if(WIFSTOPPED(status)){
                       if(j==0){
                       storeJob(current->command,current->pids,current->fd,current->fd2);
                       }
                    }else{
                        if(head->fd!=NULL){
                            close(head->fd[1]);
                        }
                    }
                }
            }
        }
    }else{
        cout << "fg: no such job\n";
    }
    }else{
        cout << "fg: wrong number of arguments\n";
    }
}
void BuildIn::handleJobs(command thisCommand){
    if(thisCommand.tokLen<2){
    if(numJob>0){
        Jobs* head=jobs;
        int count=1;
        while(head!=NULL){
            cout << "[" << count << "] " << head->command << "\n";
            head=head->next;
        }
    }else{
        cout << "No suspended jobs\n";
    }
    }else{
        cout << "jobs: wrong number of arguments\n";
    }
}

int BuildIn::storeJob(string command, int pid[],int fd[],int fd2[]){
    int size;
    size=sizeof pid/sizeof(int);
    if(size>3){return -1;}
    Jobs* head=jobs;
    
    while(head!=NULL){
        head=head->next;
    }
    head=new Jobs();
    head->command=command;
    head->next=NULL;
    head->pids[0]=pid[0];
    head->pids[1]=pid[1];
    head->pids[2]=pid[2];
    head->fd[0]=fd[0];
    head->fd[1]=fd[1];
    head->fd2[0]=fd2[0];
    head->fd2[1]=fd2[1];
    numJob++;
    return 0;
}
Jobs::Jobs(){
    for(int i=0;i<3;i++){
        pids[i]=0;
    }
    command="";
    fd[0]=-1;
    fd[1]=-1;
    fd2[0]=-1;
    fd2[1]=-1;
}

string Jobs::getCommand(){
    return command;
}
int Jobs::getPid(int index){
    if(index>2){return -1;}
    return pids[index];
}
