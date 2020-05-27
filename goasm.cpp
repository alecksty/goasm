// goasm.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <string.h>

//==========================================================================================
// ʹ��˵����
//
// LABLES:[���]
//              COMMAND[ָ��]
//                           PARAMENTS[����]
//                                       COMMENT[ע��]
// ��ͬ�Ŀ��
// +-----W1-----+----W2-----+-----W3-----+-----W4------+
//
// Usage:goasm -i <i.asm> -o <o.asm> [-upcase/lowcase] [-w1 20] [-w2 10] [-w3 15] [-cchar ;]
//
//==========================================================================================

#ifdef _DEBUG
	#define PRESS_ANY_KEY_TO_CONTINUE()		{printf("Press Any Key To Continue!\r\n");getchar();}
#else
	#define PRESS_ANY_KEY_TO_CONTINUE()		{}
#endif

//==========================================================================================
//
//==========================================================================================
//	��ִ�
#define MAX_STRING			256

//	�ע��
#define MAX_COMMENT			1024

//	�л���
#define MAX_LINE_BUF		1024

//	��ת��
#define CASE_NULL			0

//	ת��Сд
#define CASE_LOWER			1

//	ת����д
#define CASE_UPPER			2

//==========================================================================================
//
//==========================================================================================

//	�ֶο��
int w1 = 20;
int w2 = 20;
int w3 = 20;

//	ע�ͷ���
char cComment = ';';
//	���
char arrLable[MAX_STRING];
//	����
char arrCommand[MAX_STRING];
//	����
char arrParament[MAX_STRING];
//	ע��
char arrComment[MAX_COMMENT];

_TCHAR *pInp = _T("i.asm");
_TCHAR *pOut = _T("o.asm");
FILE *fpInp;
FILE *fpOut;

//	ģʽ,0-������,1-Сд��2-��д
int iCaseMode = CASE_NULL;

//==========================================================================================
//	�ո�
//==========================================================================================
int isSpace(char c)
{
	switch(c)
	{
	case ' ':
	case '\t':
	case '\r':
	case '\n':
	case '\a':
		return 1;
	}
	return 0;
}

//==========================================================================================
//	��ȡ����
//==========================================================================================
int sReadWord(char *p,char *d)
{
	char *pOld = p;

	if(!p)
		return 0;
	if(!d)
		return 0;

	//	���˿ո�
	while(isSpace(*p)){
		p++;
	}

	while(*p && !isSpace(*p)){
		*d = *p;
		d++; 
		p++;
	}

	*d = 0;

	return p - pOld;
}

//==========================================================================================
//	�����ַ�
//==========================================================================================
int inStrSub(char c,char *s)
{
	int i;
	for(i = 0; *s; i++){
		if (*s == c) {
			return i + 1;
		}
		s++;
	}

	return 0;
}

//==========================================================================================
//	ȥ����β�ո�
//==========================================================================================
int strLimit(char *s)
{
	char *p = s;
	int l = strlen(s);
	
	//	β���ҿո�
	while(l > 1 && isSpace(s[l])){
		l--;
	}
	s[l] = 0;

	//	ǰ���ҿո�
	while(isSpace(*p))
		p++;

	while(*p){
		*s = *p;
		s++;
		p++;
	}
	return 1;
}

//==========================================================================================
//	Сд
//==========================================================================================
int strLower(char *s)
{
	while(*s){
		if((*s >= 'A') && (*s <= 'Z'))
			*s = *s - 'A' + 'a';
		s++;
	}
	return 1;
}

//==========================================================================================
//	��д
//==========================================================================================
int strUpper(char *s)
{
	while(*s){
		if((*s >= 'a') && (*s <= 'z'))
			*s = *s - 'a' + 'A';
		s++;
	}
	return 1;
}

//==========================================================================================
//	�ֽ����и��ֶ�
//==========================================================================================
int sCheckLine(const char buf[],char sLable[],char sCommand[],char sParament[],char sComment[])
{
	int i = 0;
	char *p = (char *)buf;

	sLable[0]		= 0;
	sCommand[0]		= 0;
	sParament[0]	= 0;
	sComment[0]		= 0;

	//	ȥ��ĩβ�ո�
	int l = strlen(p);

	if((p[l-1] == '\r')||( p[l-1] == '\n'))
		p[l-1] = 0;
	
	if((p[l-2] == '\r')|| (p[l-2] == '\n'))
		p[l-2] = 0;

	//	����ǰ�ո�
	while(isSpace(*p))
		p++;
	
	//	����ע�� 
	i = 0;
	while((p[i] != ';') && p[i])
		i++;

	//	�ҵ�ע��
	if(p[i] == ';'){
		strcpy(sComment,&p[i]);
		p[i] = 0;
	}

	//	���ұ��
	i = 0;
	while((p[i] != ':') && p[i])
		i++;
	
	//	�ҵ����
	if(p[i] == ':'){
		i++;
		strncpy(sLable,p,i);
		p += i;
	}

	//	�ҵ�����
	i = sReadWord(p,sCommand);

	//	ʣ�µ��ǲ���
	p += i;

	//	���˿ո�
	while(isSpace(*p))
		p++;

	strcpy(sParament,p);

	strLimit(sLable);
	strLimit(sCommand);
	strLimit(sParament);
	strLimit(sComment);

	return 1;
}

//==========================================================================================
//	�̹ſ����
//==========================================================================================
int _tmain(int argc, _TCHAR* argv[])
{
	char buf[MAX_LINE_BUF];
	int i;
	int l;

	if(argc < 3)
	{
		_tprintf(_T("====================================================\r\n"));
		_tprintf(_T("Convert Assember File For Best Format Tools 1.00 (R)\r\n"));
		_tprintf(_T("====================================================\r\n"));
		_tprintf(_T("LABLES:[���]\n"));
		_tprintf(_T("             COMMAND:[ָ��]\n"));
		_tprintf(_T("                         PARAMENTS:[����]\n"));
		_tprintf(_T("                                      COMMENT:[ע��]\n"));
		_tprintf(_T("(4 diffrent widths:��ͬ�Ŀ��)\r\n"));
		_tprintf(_T("+-----W1-----+----W2-----+-----W3-----+-----W4------+"));
		_tprintf(_T("Usage:\r\n"));
		_tprintf(_T("  goasm -i <i.asm> -o <o.asm> [-upcase/lowcase] [-w1 n] [-w2 n] [-w3 n]\r\n"));
		PRESS_ANY_KEY_TO_CONTINUE();
		return 0;
	}

	for(i = 1; i < argc; i++)
	{
		if(!_tcscmp(argv[i],_T("-i")))
		{
			pInp = argv[++i];
		}
		else if(!_tcscmp(argv[i],_T("-o")))
		{
			pOut = argv[++i];
		}
		else if(!_tcscmp(argv[i],_T("-w1")))
		{
			w1 = _tstoi(argv[++i]);
		}
		else if(!_tcscmp(argv[i],_T("-w2")))
		{
			w2 = _tstoi(argv[++i]);
		}
		else if(!_tcscmp(argv[i],_T("-w3")))
		{
			w3 = _tstoi(argv[++i]);
		}
		else if(!_tcscmp(argv[i],_T("-u"))||!_tcscmp(argv[i],_T("-upcase")))
		{
			iCaseMode = CASE_UPPER;
		}
		else if(!_tcscmp(argv[i],_T("-l"))||!_tcscmp(argv[i],_T("-lowcase")))
		{
			iCaseMode = CASE_LOWER;
		}
		else
		{
			_tprintf(_T("ERROR:unknow parament:%s\r\n"),argv[i]);
		}
	}

	_tprintf(_T("Open Input File %s\r\n"),pInp);

	//	ԭʼ�ļ�
	fpInp = _tfopen(pInp,_T("rt"));
	if(!fpInp)
	{
		_tprintf(_T("ERROR:Open Input File %s Failed!\r\n"),pInp);
		PRESS_ANY_KEY_TO_CONTINUE();
		return -1;
	}

	_tprintf(_T("Open Output File %s\r\n"),pOut);
	//	Ŀ���ļ�
	fpOut = _tfopen(pOut,_T("w+t"));
	if(!fpOut)
	{
		_tprintf(_T("ERROR:Open Output File %s Failed!\r\n"),pOut);
		_fcloseall();
		PRESS_ANY_KEY_TO_CONTINUE();
		return -2;
	}

	//	ת��
	while(!feof(fpInp))
	{
		memset(buf,0,MAX_LINE_BUF);
		memset(arrLable,0,MAX_STRING);
		memset(arrCommand,0,MAX_STRING);
		memset(arrParament,0,MAX_STRING);
		memset(arrComment,0,MAX_COMMENT);

		//	��ȡһ��
		fgets(buf,MAX_LINE_BUF,fpInp);
		
		//	�ֽ�
		sCheckLine(buf,arrLable,arrCommand,arrParament,arrComment);

#ifdef _DEBUG
		printf("Line:%s\r\n",buf);
		printf("arrLable:%s\r\n",arrLable);
		printf("arrCommand:%s\r\n",arrCommand);
		printf("arrParament:%s\r\n",arrParament);
		printf("arrComment:%s\r\n",arrComment);
#endif

		if(iCaseMode == CASE_LOWER)
		{
			strLower(arrLable);
			strLower(arrCommand);
			strLower(arrParament);
		}
		else if(iCaseMode == CASE_UPPER)
		{
			strUpper(arrLable);
			strUpper(arrCommand);
			strUpper(arrParament);
		}

		l = strlen(arrLable);
		if(l >= w1)			arrLable[l++] = ' ';
		while(l < w1)		arrLable[l++] = ' ';
		arrLable[l] = 0;

		l = strlen(arrCommand);
		if(l >= w2)			arrCommand[l++] = ' ';
		while(l < w2 && l)	arrCommand[l++] = ' ';
		arrCommand[l] = 0;

		l = strlen(arrParament);
		if(l >= w3)			arrParament[l++] = ' ';
		while(l < w3 && l)	arrParament[l++] = ' ';
		arrParament[l] = 0;

		fprintf(fpOut,"%s%s%s%s\n",arrLable,arrCommand,arrParament,arrComment);
#ifdef _DEBUG
		printf("%s %s %s %s",arrLable,arrCommand,arrParament,arrComment);
#endif
	}

	fclose(fpInp);
	fclose(fpOut);
	_tprintf(_T(" Convert File Done! \r\n"));
	PRESS_ANY_KEY_TO_CONTINUE();
	return 0;
}

