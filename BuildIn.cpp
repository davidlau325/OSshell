#include "BuildIn.h"
#include "Interpreter.h"

BuildIn::BuildIn(){
}
void BuildIn::handleCD(command thisCommand){
    cout << "handling CD\n";
}

void BuildIn::handleExit(command thisCommand){
    cout << "handling Exit\n";
}

void BuildIn::handleFG(command thisCommand){
    cout << "handling fg\n";
}

void BuildIn::handleJobs(command thisCommand){
    cout << "handling jobs\n";
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
