#include "Executor.h"
#include "BuildIn.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <glob.h>
#include <errno.h>

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
    pid_t myPid[10];
    int cpos=0,rpos=0;
    if(cpos>=thisCom.tokLen) return(0);
    char path[]="/bin:/usr/bin:./";
    setenv("PATH",path,1);

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
        pipef.creatPipe(thisCom.pipeLen-1);
        //cout<<rpos<<" "<<thisCom.tokLen<<"\n";
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
                if(debugmode) cout<<"This is child "<<rpos<<"\n";
                //cout<<"1 in "<<rpos<<"\n";
                pipef.setPipe(rpos);
                //cout<<"2 in "<<rpos<<"\n";
                if(rpos==0)
                    if(inRed.setInRed()!=0) exit(-1);
                //cout<<"3 in "<<rpos<<"\n";
                if(rpos==thisCom.pipeLen-1)
                    if(outRed.setOutRed()!=0) exit(-1);
                //cout<<"4 in "<<rpos<<"\n";
                if(debugmode) cout<<"Child "<<rpos<<" "<<thisCom.pipeArg[rpos][0]<<" jump to new process\n";
                if(execvp(thisCom.pipeArg[rpos][0],thisCom.pipeArg[rpos])==-1)
                {
                    if(errno==ENOENT)
                    {
                        cout<<thisCom.pipeArg[rpos][0]<<": command not found\n";
                    }
                    else
                    {
                        cout<<thisCom.pipeArg[rpos][0]<<": unknown error\n";
                    }
                }
                exit(-1);
            }
            else
            {
                if(debugmode) cout<<"This is father after "<<rpos<<" fork\n";
                if(debugmode) cout<<"Return from wait "<<rpos<<"\n";
                rpos++;
                inRed.reSet();
                outRed.reSet();
            }
        }
    }
    cout<<myPid[0]<<" "<<myPid[1]<<"\n";
    if(debugmode) cout<<"Father return after wait\n";

    waitpid(myPid[0],NULL,WUNTRACED);
    close(pipef.pipefd[0][1]);
    waitpid(myPid[1],NULL,WUNTRACED);
    close(pipef.pipefd[0][0]);
    return 0;
}
