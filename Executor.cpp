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
    int cpos=0,rpos=0;
    if(cpos>=thisCom.tokLen) return(0);
    if(thisCom.toks[0]->cat==8)
    {
        thisCom.getNextCom(cpos);
        inRed.checkRed(thisCom,cpos,thisCom.tokLen);
        outRed.checkRed(thisCom,cpos,thisCom.tokLen);
        inRed.checkRed(thisCom,cpos,thisCom.tokLen);
        outRed.checkRed(thisCom,cpos,thisCom.tokLen);
        thisCom.getAllRecur(cpos);
        //pipef.creatPipe(thisCom.pipeLen-1);
        //cout<<rpos<<" "<<thisCom.tokLen<<"\n";
        while(rpos<thisCom.pipeLen)
        {
            myPid=fork();
            if (myPid==-1)
            {
                cout<<"Can not create new process\n";
                exit(-1);
            }
            else if(myPid==0)
            {
                if(debugmode) cout<<"This is child "<<rpos<<"\n";
                char path[]="/bin:/usr/bin:./";
                setenv("PATH",path,1);
                //pipef.setPipe(rpos);
                if(rpos==0)
                    if(inRed.setInRed()!=0) exit(-1);
                if(rpos==thisCom.pipeLen-1)
                    if(outRed.setOutRed()!=0) exit(-1);
                //if(debugmode) cout<<"Child "<<rpos<<" jump to new process\n";
                //if(debugmode) cout<<thisCom.pipeArg[rpos][0]<<"\n";
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
                //
                wait(NULL);
                //
                rpos++;
                inRed.reSet();
                outRed.reSet();
            }
        }
    }
    return 0;
}
