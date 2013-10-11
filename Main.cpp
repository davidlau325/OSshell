#include <iostream>
#include<unistd.h>
#include "Interpreter.h"
#include "Executor.h"
#include "BuildIn.h"

bool debugmode=false;

int main()
{
        command *coms[1000];
        string newCom,curLoc;
        char curDir[1000];
        int comsLen=0;
        BuildIn* shellBuild;
        executor shellExc;
        getcwd(curDir,1000);
        curLoc=curDir;
        cout<<"[3150 shell:"<<curLoc<<"]$ ";
        
        shellBuild=new BuildIn();
        while(getline(cin,newCom))
        {
            coms[comsLen]=new command(newCom);
            if (!coms[comsLen]->valid()) cout<<"Error: invalid input command line\n";
            else shellExc.runCom(*coms[comsLen],*shellBuild);
            comsLen++;
            cout<<"[3150 shell:"<<curLoc<<"]$ ";
        }
        return 0;
}
