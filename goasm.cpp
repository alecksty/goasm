// goasm.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string.h>

//==========================================================================================
// 使用说明：
//
// LABLES:[标号]
//              COMMAND[指令]
//                           PARAMENTS[参数]
//                                       COMMENT[注释]
// 不同的宽度
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
//	最长字串
#define MAX_STRING			256

//	最长注释
#define MAX_COMMENT			1024

//	行缓冲
#define MAX_LINE_BUF		1024

//	不转换
#define CASE_NULL			0

//	转换小写
#define CASE_LOWER			1

//	转换大写
#define CASE_UPPER			2

//==========================================================================================
//
//==========================================================================================

//	分段宽度
int w1 = 20;
int w2 = 20;
int w3 = 20;

//	注释符号
char cComment = ';';
//	标号
char arrLable[MAX_STRING];
//	命令
char arrCommand[MAX_STRING];
//	参数
char arrParament[MAX_STRING];
//	注释
char arrComment[MAX_COMMENT];

_TCHAR *pInp = _T("i.asm");
_TCHAR *pOut = _T("o.asm");
FILE *fpInp;
FILE *fpOut;

//	模式,0-不处理,1-小写，2-大写
int iCaseMode = CASE_NULL;

//==========================================================================================
//	空格
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
//	读取单词
//==========================================================================================
int sReadWord(char *p,char *d)
{
	char *pOld = p;

	if(!p)
		return 0;
	if(!d)
		return 0;

	//	过滤空格
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
//	包含字符
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
//	去掉首尾空格
//==========================================================================================
int strLimit(char *s)
{
	char *p = s;
	int l = strlen(s);
	
	//	尾部找空格
	while(l > 1 && isSpace(s[l])){
		l--;
	}
	s[l] = 0;

	//	前面找空格
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
//	小写
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
//	大写
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
//	分解行中各字段
//==========================================================================================
int sCheckLine(const char buf[],char sLable[],char sCommand[],char sParament[],char sComment[])
{
	int i = 0;
	char *p = (char *)buf;

	sLable[0]		= 0;
	sCommand[0]		= 0;
	sParament[0]	= 0;
	sComment[0]		= 0;

	//	去掉末尾空格
	int l = strlen(p);

	if((p[l-1] == '\r')||( p[l-1] == '\n'))
		p[l-1] = 0;
	
	if((p[l-2] == '\r')|| (p[l-2] == '\n'))
		p[l-2] = 0;

	//	过滤前空格
	while(isSpace(*p))
		p++;
	
	//	查找注释 
	i = 0;
	while((p[i] != ';') && p[i])
		i++;

	//	找到注释
	if(p[i] == ';'){
		strcpy(sComment,&p[i]);
		p[i] = 0;
	}

	//	查找标号
	i = 0;
	while((p[i] != ':') && p[i])
		i++;
	
	//	找到标号
	if(p[i] == ':'){
		i++;
		strncpy(sLable,p,i);
		p += i;
	}

	//	找到命令
	i = sReadWord(p,sCommand);

	//	剩下的是参数
	p += i;

	//	过滤空格
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
//	盘古开天地
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
		_tprintf(_T("LABLES:[标号]\n"));
		_tprintf(_T("             COMMAND:[指令]\n"));
		_tprintf(_T("                         PARAMENTS:[参数]\n"));
		_tprintf(_T("                                      COMMENT:[注释]\n"));
		_tprintf(_T("(4 diffrent widths:不同的宽度)\r\n"));
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

	//	原始文件
	fpInp = _tfopen(pInp,_T("rt"));
	if(!fpInp)
	{
		_tprintf(_T("ERROR:Open Input File %s Failed!\r\n"),pInp);
		PRESS_ANY_KEY_TO_CONTINUE();
		return -1;
	}

	_tprintf(_T("Open Output File %s\r\n"),pOut);
	//	目标文件
	fpOut = _tfopen(pOut,_T("w+t"));
	if(!fpOut)
	{
		_tprintf(_T("ERROR:Open Output File %s Failed!\r\n"),pOut);
		_fcloseall();
		PRESS_ANY_KEY_TO_CONTINUE();
		return -2;
	}

	//	转换
	while(!feof(fpInp))
	{
		memset(buf,0,MAX_LINE_BUF);
		memset(arrLable,0,MAX_STRING);
		memset(arrCommand,0,MAX_STRING);
		memset(arrParament,0,MAX_STRING);
		memset(arrComment,0,MAX_COMMENT);

		//	读取一行
		fgets(buf,MAX_LINE_BUF,fpInp);
		
		//	分解
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

