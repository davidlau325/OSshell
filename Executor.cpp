#include "Executor.h"
#include "BuildIn.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <glob.h>
#include <errno.h>
#include <signal.h>

extern bool debugmode;
extern int debugfd;

process::process()
{
    shellPid=-1;
}

process::process(command thisCom, pid_t thisPid)
{
    shellCom=thisCom;
    shellPid=thisPid;
}

int executor::runCom(command& thisCom,BuildIn& shellBuild)
{
    int myPid[10];
    int cpos=0,rpos=0;
    if(cpos>=thisCom.tokLen) return(0);
    char path[]="PATH=/bin:/usr/bin:./";
    putenv(path);

    if(thisCom.toks[0]->cat==9){
        if(thisCom.toks[0]->tok.compare("cd")==0){
            shellBuild.handleCD(thisCom);
        }else if(thisCom.toks[0]->tok.compare("exit")==0){
            shellBuild.handleExit(thisCom);
        }else if(thisCom.toks[0]->tok.compare("fg")==0){
            shellBuild.handleFG(thisCom);
        }else if(thisCom.toks[0]->tok.compare("jobs")==0){
            shellBuild.handleJobs(thisCom);
        }else{
            cout << "Error: Invalid Build-In\n";
        }
    }else if(thisCom.toks[0]->cat==8)
    {
        thisCom.getNextCom(cpos);
        inRed.checkRed(thisCom,cpos,thisCom.tokLen);
        outRed.checkRed(thisCom,cpos,thisCom.tokLen);
        inRed.checkRed(thisCom,cpos,thisCom.tokLen);
        outRed.checkRed(thisCom,cpos,thisCom.tokLen);
        thisCom.getAllRecur(cpos);
        outRed.checkRed(thisCom,cpos,thisCom.tokLen);
        pipef.creatPipe(thisCom.pipeLen-1);
        while(rpos<thisCom.pipeLen)
        {
            myPid[rpos]=fork();
            if (myPid[rpos]==-1)
            {
                cout<<"Can not create new process\n";
                exit(-1);
            }
            else if(myPid[rpos]==0)
            {
                signal(SIGINT,SIG_DFL);
                signal(SIGTSTP,SIG_DFL);
                if(debugmode) cout<<"This is child "<<rpos<<"\n";
                pipef.setPipe(rpos);
                if(rpos==0)
                    if(inRed.setInRed()!=0) exit(-1);
                if(rpos==thisCom.pipeLen-1)
                    if(outRed.setOutRed()!=0) exit(-1);
                if(debugmode) cout<<"Child "<<rpos<<" "<<thisCom.pipeArg[rpos][0]<<" jump to new process\n";
                if(execvp(thisCom.pipeArg[rpos][0],thisCom.pipeArg[rpos])==-1)
                {
                    if(errno==ENOENT)
                    {
                        cerr<<thisCom.pipeArg[rpos][0]<<": command not found\n";
                    }
                    else
                    {
                        cerr<<thisCom.pipeArg[rpos][0]<<": unknown error\n";
                    }
                }
                exit(-1);
            }
            else
            {
                if(debugmode) cout<<"This is father after "<<rpos<<" fork\n";
                rpos++;
            }
        }

        if(debugmode) cout<<"Father return after wait\n";




        string comName="";
        for (int i=0; i<thisCom.tokLen; i++)
        {
            if (i!=0) comName+=" ";
            comName+=thisCom.toks[i]->tok;
        }
        for(int i=thisCom.pipeLen; i<10; i++) myPid[i]=-1;
        int fd[2],fd2[2];
        if(thisCom.pipeLen<3)
        {
            fd2[0]=-1; fd2[1]=-1;
        }
        else
        {
            fd2[0]=pipef.pipefd[1][0];
            fd2[1]=pipef.pipefd[1][1];
        }
        if(thisCom.pipeLen<2)
        {
            fd[0]=-1; fd[1]=-1;
        }
        else
        {
            fd[0]=pipef.pipefd[0][0];
            fd[1]=pipef.pipefd[0][1];
        }
        shellBuild.waitingFor(comName,myPid,fd,fd2);
        inRed.reSet();
        outRed.reSet();
        }
        return 0;
}
