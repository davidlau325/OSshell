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
#include "Interpreter.h"
#include <string.h>
#include <glob.h>

extern bool debugmode;
extern int debugfd;

string comSym[16]={ "E", "Pipe", "Redirect Input", "Redirect Output", "Redirect Output", "E", "E", "Argument", "Command Name", "Built-in Command", "Input Filename", "Output Filename","Argument" };
command::command()
{
	tokLen=0;
	pipeLen=0;
	for (int i=0; i<20; i++) toks[i]=NULL;
}

command::command(string com)
{
	tokLen=0;
	pipeLen=0;
	string sub;
	int l=com.size(),fp=0,bp=0;
	while (bp<l && fp<l)
	{
		while(com[fp]==' ' && fp<l) fp++;
		bp=fp;
		while(bp+1<l && com[bp+1]!=' ') bp++;
		sub=com.substr(fp,bp-fp+1);
		toks[tokLen]=new token(sub);
		tokLen++;
		fp=bp+1;
	}
}

bool command::comName(int& c)
{
	int p=c;
	if (p>=tokLen) return false;
	if (toks[p]->cat==5 || toks[p]->cat==8)
	{
		toks[p]->cat=8;
		p++;
		if (p>=tokLen) { c=p; return true; }
		while(toks[p]->cat==5 || toks[p]->cat==6 || toks[p]->cat==7)
		{
		    if (toks[p]->cat!=7) toks[p]->cat=12;
			p++;
			if (p>=tokLen) { c=p; return true; }
		}
		c=p;
		return true;
	}
	else return false;
}

bool command::termi(int &c)
{
	int p=c;
	if (p>=tokLen) return true;
	if(toks[p]->cat==3)
	{
		p++;
		if (p>=tokLen) return false;
		if(toks[p]->cat==5 || toks[p]->cat==6 || toks[p]->cat==11)
		{
			toks[p]->cat=11;
			p++;
			c=p;
			return true;
		}
		else return false;
	}
	else if(toks[p]->cat==4)
	{
		p++;
		if (p>=tokLen) return false;
		if(toks[p]->cat==5 || toks[p]->cat==6 || toks[p]->cat==11)
		{
			toks[p]->cat=11;
			p++;
			c=p;
			return true;
		}
		else return false;
	}
	else return false;
}


bool command::recur(int &c,int count)
{
	int p=c;
	if (p>=tokLen) return false;
	if (count>2) return false;
	if(toks[p]->cat==1)
	{
		p++;
		if (p>=tokLen) return false;
		if (comName(p))
		{
			if(termi(p)) { c=p; return true; }
			else if(recur(p,count+1)) { c=p; return true; }
			else return false;
		}
		else return false;
	}
	else return false;
}

int command::valid()
{
	int c=0;
	if(c>=tokLen) return true;
	if(toks[c]->cat==6 || toks[c]->cat==9)
	{
		toks[0]->cat=9;
		c++;
		if(c>=tokLen) return true;
		while(toks[c]->cat==5 || toks[c]->cat==6 || toks[c]->cat==7)
		{
			if(toks[c]->cat!=7) toks[c]->cat=12;
			c++;
			if(c>=tokLen) return true;
		}
		return false;
	}
	else if(comName(c))
	{
		if(c>=tokLen) return true;
		if (toks[c]->cat==2)
		{
			c++;
			if (c>=tokLen) return false;
			if (toks[c]->cat==5 || toks[c]->cat==6 || toks[c]->cat==10)
			{
				toks[c]->cat=10;
				c++;
				if (c>=tokLen) return true;
				if (recur(c,1))
				{
					if (c>=tokLen) return true;
					return false;
				}
				else if(termi(c))
				{
					if(c>=tokLen) return true;
					return false;
				}
				else return false;
			}
			else return false;
		}
		else if(recur(c,1))
		{
			if(c>=tokLen) return true;
			return false;
		}
		else if(termi(c))
		{
			if(c>=tokLen) return true;
			if(toks[c]->cat==2)
			{
				c++;
				if(c>=tokLen) return false;
				if(toks[c]->cat==5 || toks[c]->cat==6 || toks[c]->cat==10)
				{
					toks[c]->cat=10;
					c++;
					if(c>=tokLen) return true;
					return false;
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}
	else return false;
}

void command::printTok()
{
	for(int i=0; i<tokLen; i++)
	{
		cout<<"Token "<<i+1<<": \""<<toks[i]->tok<<"\" ("<<comSym[toks[i]->cat]<<")\n";
	}
}

token::token()
{
	cat=15;
}

token::token(string &t)
{
	tok=t;
	tokc=new char(t.length()+2);
	strcpy(tokc,t.c_str());
	cat=checkCat();
}


int token::checkCat()
{
	int l=tok.size();
	bool star=false;
	char c=0;
	if (l==1)
	{
		c=tok[0];
		switch (c)
		{
		case '|': return 1;
		case '<': return 2;
		case '>': return 3;
		}
	}
	if (l==2)
	{
		if (tok[0]=='>' && tok[1]=='>') return 4;
	}
	if (tok=="cd" || tok=="exit" || tok=="fg" || tok=="jobs") return(6);
	for (int i=0; i<l; i++)
	{
		c=tok[i];
		if (c==9 || c==32 || c==62 || c==60 || c==124 || c==33 || c==96 ||c==39 || c==34) return 0;
		if (c==42) star=true;
	}
	if (star) return 7;
	else return 5;
}


token* command::nextArg(int& cpos)
{
    //if(debugmode) cout<<"In nextArg\n";
    if(cpos>=tokLen) return NULL;
    else
    {
        if (toks[cpos]->cat==7||toks[cpos]->cat==12)
        {
            cpos++;
            return toks[cpos-1];
        }
        else return NULL;
    }
}


void command::getNextCom(int& cpos)
{
    if(cpos>=tokLen) return;

    if(debugmode) cout<<"In getNextCom\n";
    token *next;

    int apos=0;
    pipeArg[pipeLen][apos]=NULL;

    if (toks[cpos]->cat!=8)
    {
        cout<<"No command found at getNextCom\n";
        return;
    }
    if(debugmode)cout<<toks[cpos]->tokc<<"\n";
    pipeCom[pipeLen]=toks[cpos]->tokc;
    pipeArg[pipeLen][apos]=toks[cpos]->tokc;
    cpos++;
    apos++;

    next=nextArg(cpos);
    while(next!=NULL)
    {
        if (next->cat==12)
        {
            pipeArg[pipeLen][apos]=next->tokc;
            apos++;
            pipeArg[pipeLen][apos]=NULL;
            next=nextArg(cpos);
        }
        else
        {
            if(debugmode) cout<<"Wildcard expansion\n";
            glob_t glotBuffer;
            glotBuffer.gl_offs=0;
            if(glob(next->tokc,GLOB_NOCHECK,NULL,&glotBuffer)!=0) cout<<"An Error Happen when handle wildcard";
            for (unsigned int i=0; i<glotBuffer.gl_pathc; i++)
            {
                pipeArg[pipeLen][apos]=glotBuffer.gl_pathv[i];
                apos++;
                pipeArg[pipeLen][apos]=NULL;
            }
            next=nextArg(cpos);
        }
    }
    pipeLen++;
    return;
}

void command::getAllRecur(int& cpos)
{
    if(debugmode) cout<<"In getAllRecur\n";
    if(cpos>=tokLen) return;
    if(toks[cpos]->cat!=1) return;
    cpos++;
    getNextCom(cpos);
    if(cpos>=tokLen) return;
    if(toks[cpos]->cat==1) getAllRecur(cpos);
    return;
}
