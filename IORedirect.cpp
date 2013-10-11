#include "IORedirect.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>


extern bool debugmode;

void inRedirection::checkRed(command &thisCom, int &cpos,int tokLen)
{
    if (debugmode) cout<<"checkRed start\n";
    if (cpos>=tokLen) return;
    if (thisCom.toks[cpos]->cat==2)
    {
        cpos++;
        redirectionStatus=true;
        inFile=thisCom.toks[cpos]->tokc;
        cpos++;
    }
    if (debugmode) cout<<"checkRed end\n";
}

inRedirection::inRedirection()
{
    inFile=NULL;
    redirectionStatus=false;
}


int inRedirection::setInRed()
{
    if (debugmode) cout<<"setInRed start\n";
    int infd;
    if (!redirectionStatus) return(0);
    if(debugmode) cout<<"Setting input redirection\n";
    infd=open(inFile,O_RDONLY,0400);
    if (infd==-1)
    {
        if(errno==EACCES)
        {
            cout<<inFile<<": permission denied\n";
            return(-1);
        }
        else if(errno==ENOENT)
        {
            cout<<inFile<<": no such file or directory\n";
            return(-2);
        }
        else return(-3);
    }
    close(STDIN_FILENO);
    if(dup2(infd,STDIN_FILENO)==-1) return -4;
    close(infd);
    return(0);
}

void inRedirection::reSet()
{
    redirectionStatus=false;
    inFile=NULL;
}

outRedirection::outRedirection()
{
    redirectionStatus=false;
    outFile=NULL;
}

void outRedirection::checkRed(command& thisCom,int& cpos,int tokLen)
{
    if (debugmode) cout<<"checkRed start\n";
    if (cpos>=tokLen) return;
    if (thisCom.toks[cpos]->cat==3)
    {
        cpos++;
        redirectionStatus=true;
        reWrite=true;
        outFile=thisCom.toks[cpos]->tokc;
        cpos++;
    }
    else if(thisCom.toks[cpos]->cat==4)
    {
        cpos++;
        redirectionStatus=true;
        reWrite=false;
        outFile=thisCom.toks[cpos]->tokc;
        cpos++;
    }
    if (debugmode) cout<<"checkRed end\n";
}

int outRedirection::setOutRed()
{

    if (debugmode) cout<<"setOutRed start\n";
    int outfd;
    if (!redirectionStatus) return 0;
    cout<<"Setting output redirection\n";
    if(reWrite) outfd=open(outFile,O_WRONLY|O_CREAT|O_TRUNC,00700);
    else outfd=open(outFile,O_WRONLY|O_APPEND|O_CREAT,00700);
    if (outfd==-1)
    {
        if(errno==EACCES)
        {
            cout<<outFile<<": permission denied\n";
            return -1;
        }
        else
        {
            cout<<"Wrong when open the file\n";
            return(-3);
        }
    }
    close(STDOUT_FILENO);
    if(dup2(outfd,STDOUT_FILENO)==-1)
    {
        cout<<"Wrong when dup2 out stream\n";
        return(-4);
    }
    close(outfd);
    return 0;
}

void outRedirection::reSet()
{
    redirectionStatus=false;
    outFile=NULL;
}

bool pipeArr::creatPipe(int num)
{
    pipeNum=num;
    pipePos=0;
    comPos=0;
    for(int i=0; i<num; i++)
    {
        if(pipe(pipefd[i])!=0)
        {
            cout<<"Pipe failed\n";
            return false;
        }
    }
    return true;
}

bool pipeArr::setPipe(int comPos)
{
    cout<<"Set pipe "<<comPos<<"\n";
    if(comPos>=pipeNum) return true;
    if(comPos==0)
    {
        close(pipefd[0][0]);
        close(STDOUT_FILENO);
        if(dup2(pipefd[0][1],STDOUT_FILENO)==-1)
        {
            cout<<"setPipe fail\n";
            return false;
        }
        close(pipefd[0][1]);
    }
    if(comPos==pipeNum)
    {
        close(pipefd[comPos-1][1]);
        close(STDIN_FILENO);
        if (dup2(pipefd[comPos-1][0],STDIN_FILENO)==-1)
        {
            cout<<"setPipe fail\n";
            return false;
        }
        close(pipefd[comPos-1][0]);
    }
    else
    {
        close(STDIN_FILENO);
        if (dup2(pipefd[comPos-1][0],STDIN_FILENO)==-1)
        {
            cout<<"setPipe fail\n";
            return false;
        }
        close(pipefd[comPos-1][0]);
        if(dup2(pipefd[comPos][1],STDOUT_FILENO)==-1)
        {
            cout<<"setPipe fail\n";
            return false;
        }
        close(pipefd[comPos][1]);
    }
    return true;
}
