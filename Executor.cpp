#include "Executor.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <glob.h>
#include <errno.h>

extern bool debugmode;

process::process()
{
    shellPid=-1;
}

process::process(command thisCom, pid_t thisPid)
{
    shellCom=thisCom;
    shellPid=thisPid;
}

int executor::runCom(command& thisCom)
{
    pid_t myPid;
    int cpos=0;
    if(cpos>=thisCom.tokLen) return(0);
    if(thisCom.toks[0]->cat==8)
    {
        thisCom.getNextCom(cpos);
        inRed.checkRed(thisCom,cpos,thisCom.tokLen);
        outRed.checkRed(thisCom,cpos,thisCom.tokLen);
        inRed.checkRed(thisCom,cpos,thisCom.tokLen);
        outRed.checkRed(thisCom,cpos,thisCom.tokLen);
        thisCom.getAllRecur(cpos);
        pipef.creatPipe(thisCom.pipeLen-1);
    }
    while(cpos<thisCom.tokLen)
    {
        myPid=fork();
        if (myPid==-1)
        {
            cout<<"Can not create new process\n";
            exit(-1);
        }
        else if(myPid==0)
        {
            if(debugmod) cout<<"This is child "<<cpos<<"\n";
            char path[]="/bin:/usr/bin:./";
            setenv("PATH",path,1);
            pipef.setPipe(cpos);
            if(cpos==0)
                if(inRed.setInRed()!=0) exit(-1);
            if(cpos==thisCom.pipeLen-1)
                if(outRed.setOutRed()!=0) exit(-1);
            if(execvp(thisCom.pipeArg[i][0],thisCom.pipeArg[i])==-1)
            {
                if(errno==ENOENT)
                {
                    cout<<thisCom.pipeArg[i][0]<<": command not found\n";
                }
                else
                {
                    cout<<thisCom.pipeArg[i][0]<<": unknown error";
                }
                exit(-1);
            }

        }
    }
    return 0;
}

void executor::excute(command& thisCom, int num)
{
    myPid=fork();
    if(debugmode) if(myPid!=0) cout<<myPid<<"\n";
    if(debugmode) cout<<"AfterFork\n";
    if(myPid==0)
    {
        if(debugmode) cout<<"Child Begin\n";
        char path[]="/bin:/usr/bin:./";
        setenv("PATH",path,1);
        pipef.setPipe(num);
        if(num==0)
            if(inRed.setInRed()!=0) exit(-1);

        if(num==thisCom.pipeLen-1)
            if(outRed.setOutRed()!=0) exit(-1);

        if(num<thisCom.pipeLen-1)
        {
            excute(thisCom,num+1);
        }
        if(execvp(thisCom.pipeArg[i][0],thisCom.pipeArg[i])==-1)
        {
            if(errno==ENOENT)
            {
                cout<<thisCom.pipeArg[i][0]<<": command not found\n";
            }
            else
            {
                cout<<thisCom.pipeArg[i][0]<<": unknown error";
            }
            exit(-1);
        }
    }
    else
    {
        if(debugmode) cout<<"Father Begin\n";
        runPro.shellCom=thisCom;
        runPro.shellPid=myPid;
        wait(NULL);
        if(debugmode) cout<<"Father Back\n";
    }
}
