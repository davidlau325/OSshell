/*
cat of class token
0: wrong input;
15: not determined
1 : '|'
2 : '<'
3 : '>'
4 : '>>'
5 : filename or arg or command
6 : built-in or arg or filename
7 : arg with star
8 : command
9 : built-in
10: input filename
11: output filename
12: arg without star

filename : 5 || 6
arg      : 5 || 6 || 7
built-in : 6
command  : 5
*/

#ifndef _Interpreter
#define _Interpreter

#include <iostream>
#include <string>

using namespace std;

class token
{
public:
	string tok;
	char tokc[255];
	int cat;
	token();
	token(string&);
	int checkCat();
	~token();
};


class command
{
public:
	token* toks[20];
	int tokLen;
	int pipeLen;
    char* pipeCom[20];
    char* pipeArg[20][130];
	int valid();
	bool comName(int&);
	bool recur(int&,int);
	bool termi(int&);
	void printTok();
	command(string);
	command();
	token* nextArg(int&);
	void getNextCom(int&);
	void getAllRecur(int&);
};

#endif

