// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>
#include <atlstr.h>


struct periods
{
	short start,end;
	periods *next;
};

struct StructWord
{
	char c;
	StructWord *next;
	StructWord *prev;
};

class DictWord
{
public:
	DictWord(short n, CString DictChar);
	~DictWord();
	int Add();
	CString GetString(bool Heading);
private:
	StructWord SWord;
	StructWord *Current;
	StructWord *Head;
	StructWord *Tale;

	CString CharBase;
	int CharBaseLength;
	bool PureAdd;
};

class DictCreator 
{
public:
	DictCreator(periods* range);
	bool Open(const char* DictName);
	void Close();
	void Work(int n, int nn);
private:
	FILE* DictFile;
	char BaseDict[255];
	CString Dict;
};



// TODO: reference additional headers your program requires here
