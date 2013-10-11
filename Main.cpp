#include <iostream>
#include<unistd.h>
#include "Interpreter.h"
#include "Executor.h"
#include "BuildIn.h"
#include "signal.h"

bool debugmode=false;

int main()
{
        command *coms[1000];
        string newCom,curLoc;
        int comsLen=0;
        BuildIn* shellBuild;
        shellBuild=new BuildIn();
        executor shellExc;
        string cwd=shellBuild->getCurrentDic();
        signal(SIGINT,SIG_IGN);
        signal(SIGTERM,SIG_IGN);
        signal(SIGQUIT,SIG_IGN);
        signal(SIGTSTP,SIG_IGN);
        cout<<"[3150 shell:"<<cwd<<"]$ ";
        while(getline(cin,newCom))
        {
            coms[comsLen]=new command(newCom);
            if (!coms[comsLen]->valid()) cout<<"Error: invalid input command line\n";
            else shellExc.runCom(*coms[comsLen],*shellBuild);
            comsLen++;
            cwd=shellBuild->getCurrentDic();
            cout<<"[3150 shell:"<<cwd<<"]$ ";
        }
        return 0;
}
