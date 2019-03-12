/*******************************************************************************
 * This file is part of "Patrick's Programming Library", Version 7 (PPL7).
 * Web: http://www.pfp.de/ppl/
 *
 * $Author$
 * $Revision$
 * $Date$
 * $Id$
 *
 *******************************************************************************
 * Copyright (c) 2013, Patrick Fedick <patrick@pfp.de>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice, this
 *       list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#include "prolog.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <list>
#include "ppl7.h"

namespace ppl7 {

#define CALC_BUFFERSIZE 64
#undef DEBUG
//#define DEBUG 1

class CalcToken
{
	public:
		enum {
			TYPE_VALUE,
			TYPE_PLUS,
			TYPE_MINUS,
			TYPE_DIVIDE,
			TYPE_MULTIPLY,
			TYPE_POWER,
			TYPE_SHIFT_LEFT,
			TYPE_SHIFT_RIGHT,
		};
		int operator_type;
		double value;

		CalcToken(int operator_type, double value) {
			this->operator_type=operator_type;
			this->value=value;
		}
};

double GetValue(const char **ptr, char *buffer)
{
	int bptr=0;
	const char *start=*ptr;
	int c=*ptr[0];
	if(c=='-' || c=='+') {
		buffer[bptr]=c;
		(*ptr)++;
		bptr++;
		c=*ptr[0];
	}
	if((c<'0' || c>'9') && c!='.') {
		throw SyntaxException("Illegal Value: %s", start);
	}
	int dotts=0;

	while ((c=*ptr[0])!=0) {
		if (c<'0' || c>'9') {
			if (c=='.') {
				dotts++;
				if (dotts>1) throw SyntaxException("Illegal Value: %s", start);
			} else {
				break;
			}
		}
		buffer[bptr]=c;
		(*ptr)++;
		bptr++;
		if (bptr>=CALC_BUFFERSIZE) throw SyntaxException("Value too big: %s", start);
	}
	buffer[bptr]=0;
	return atof(buffer);
}

#ifdef DEBUG
static void PrintTokenList(std::list<CalcToken> &tokenlist)
{
	std::list<CalcToken>::iterator it;
	for (it=tokenlist.begin();it!=tokenlist.end();++it) {
		printf ("type: %d, Value: %f\n",(*it).operator_type, (*it).value);
	}
}
#endif

static double CalcType(double v1, double v2, int type)
{
	if (type==CalcToken::TYPE_PLUS) return v1+v2;
	if (type==CalcToken::TYPE_MINUS) return v1-v2;
	if (type==CalcToken::TYPE_MULTIPLY) return v1*v2;
	if (type==CalcToken::TYPE_DIVIDE) return v1/v2;
	if (type==CalcToken::TYPE_POWER) return pow(v1,v2);
	if (type==CalcToken::TYPE_SHIFT_LEFT) return (ppluint64)v1<<(int)v2;
	if (type==CalcToken::TYPE_SHIFT_RIGHT) return (ppluint64)v1>>(int)v2;
	return 0;
}

static void ResolveType(std::list<CalcToken> &tokenlist, int type)
{
	std::list<CalcToken>::iterator it;
	std::list<CalcToken>::iterator before, after;
	bool found=true;
	while (found) {
		found=false;
		for (it=tokenlist.begin();it!=tokenlist.end();++it) {
			if ((*it).operator_type==type) {
				found=true;
				before=it;
				after=it;
				before--;
				after++;
				if (before!=tokenlist.end() && after !=tokenlist.end())	{
					if ((*before).operator_type!=CalcToken::TYPE_VALUE) throw SyntaxException();
					if ((*after).operator_type!=CalcToken::TYPE_VALUE) throw SyntaxException();
					(*before).value=CalcType((*before).value,(*after).value, type);
					tokenlist.erase(it);
					tokenlist.erase(after);
#ifdef DEBUG
					printf ("After erase\n");
					PrintTokenList(tokenlist);
#endif
				} else throw SyntaxException();
				it=tokenlist.end();
			}
		}
	}
}


static int findClosingBracket(const char *ptr)
{
	int p=0;
	int count=1;
	int c;
	while ((c=ptr[p])!=0) {
		if (c=='(') count++;
		else if (c==')') {
			count--;
			if (count==0) return p;
		}
		p++;
	}
	printf ("Ups\n");
	return p;
	throw SyntaxException("Closing Bracket not found: %s", ptr);
}

double Tokenize(const String &expression, char *buffer)
{
	const char *ptr=expression.c_str();
	std::list<CalcToken> tokenlist;
	int c;
	while ((c=ptr[0])!=0) {
		if (c=='(') {
			int lastbracket=findClosingBracket(ptr+1);
			String s=String(ptr+1,lastbracket);
#ifdef DEBUG
			printf ("DEBUG: %d\n",lastbracket);
			printf ("s=%s\n",(const char*)s);
#endif
			tokenlist.push_back(CalcToken(CalcToken::TYPE_VALUE,Tokenize(s,buffer)));
			ptr+=lastbracket+2;
#ifdef DEBUG
			printf ("New ptr=%s\n",ptr);
#endif
		} else {
			double v=GetValue(&ptr, buffer);
			tokenlist.push_back(CalcToken(CalcToken::TYPE_VALUE,v));
		}
		c=ptr[0];
		if (c=='+') {
			tokenlist.push_back(CalcToken(CalcToken::TYPE_PLUS,0));
		} else if (c=='-') {
			tokenlist.push_back(CalcToken(CalcToken::TYPE_MINUS,0));
		} else if (c=='*') {
			tokenlist.push_back(CalcToken(CalcToken::TYPE_MULTIPLY,0));
		} else if (c=='/') {
			tokenlist.push_back(CalcToken(CalcToken::TYPE_DIVIDE,0));
		} else if (c=='^') {
			tokenlist.push_back(CalcToken(CalcToken::TYPE_POWER,0));
		} else if (c=='<' && ptr[1]=='<') {
			tokenlist.push_back(CalcToken(CalcToken::TYPE_SHIFT_LEFT,0));
			ptr++;
		} else if (c=='>' && ptr[1]=='>') {
			tokenlist.push_back(CalcToken(CalcToken::TYPE_SHIFT_RIGHT,0));
			ptr++;
		} else if (c==0) {
			break;
		} else {
			throw SyntaxException("Illegal Argument: %s", ptr);
		}
		ptr++;
	}
#ifdef DEBUG
	printf ("Expression: %s\n",(const char*)expression);
	PrintTokenList(tokenlist);
#endif
	ResolveType(tokenlist,CalcToken::TYPE_SHIFT_LEFT);
	ResolveType(tokenlist,CalcToken::TYPE_SHIFT_RIGHT);
	ResolveType(tokenlist,CalcToken::TYPE_POWER);
	ResolveType(tokenlist,CalcToken::TYPE_MULTIPLY);
	ResolveType(tokenlist,CalcToken::TYPE_DIVIDE);
	ResolveType(tokenlist,CalcToken::TYPE_PLUS);
	ResolveType(tokenlist,CalcToken::TYPE_MINUS);
#ifdef DEBUG
	printf ("After resolve\n");
	PrintTokenList(tokenlist);
#endif
	return (*tokenlist.begin()).value;
}

double Calc(const String &expression)
{
	char buffer[CALC_BUFFERSIZE];
	String current=expression;
	current.replace(String(" "),String(""));
	current.replace(String("\t"),String(""));
	current.replace(String(","),String("."));
	return Tokenize(current, buffer);
}

}	// EOF namespace ppl6
