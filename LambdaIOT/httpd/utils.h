/*
  utils.h
  Author: Straight Coder<simpleisrobust@gmail.com>
  Date: April 12, 2020
*/

#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef WIN32
#pragma warning(disable:4996) //_CRT_SECURE_NO_WARNINGS
#endif

typedef struct
{
	char *extension;
	char *content_type;
}TypeHeader;

//long ston(unsigned char* s);
//void ntos(long n, unsigned char* s);

int toLower(int nChar);
int toUpper(int nChar);
int Strnicmp(char *str1, char *str2, int n);

int token_match(const char *s1, const char *s2);
void TrimFloat(char* floatString);
int DecodeB64(unsigned char* data);
int URLDecode(char* url);

char* GetContentType(const char* extension);
unsigned long GetIpAddress(char* addr);

unsigned long TimeElapsed(unsigned long tLast, unsigned long tNow);	//ms

#endif
