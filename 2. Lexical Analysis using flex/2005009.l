/* flex -o 2005009.cpp 2005009.l
g++ 2005009.cpp -lfl -o 2005009.out
./2005009.out input.txt */

%option noyywrap

%x STRING
%x SINGLE_COMMENT
%x MULTI_COMMENT

%{

#include "2005009.h"
#define callInitial {lineCount += nl_inside; nl_inside = 0; BEGIN(INITIAL);}

string res, res_plain, comment;

%}

LETTER          [a-zA-Z_]
DIGIT           [0-9]
NEWLINE         [\r]?\n
LINEBREAK		\\[\r]?\n
SPACES		  	[ \t]*
WHITESPACE      [\v\r\f]+
KEYWORD         if|else|for|while|do|break|int|char|float|double|void|return|switch|case|default|continue
IDENTIFIER      {LETTER}({LETTER}|{DIGIT})*
CONST_INT       {DIGIT}+
SIMPLE_FLOAT	(({DIGIT}*(\.)?{DIGIT}+)|({DIGIT}+(\.)?{DIGIT}*))
CONST_FLOAT     {SIMPLE_FLOAT}([Ee][-+]?{DIGIT}+)?
ADDOP           [+-]
MULOP           [*/%]
INCOP           \+\+|\-\-
RELOP           [<>]=?|[=!]=
LOGICOP         &&|\|\|
BITOP           &|\||\^|<<|>>
TOO_MANY_DEC    {DIGIT}*(\.{DIGIT}+)\.(\.|{DIGIT})*([Ee][+-]?)?(\.|{DIGIT})*
ILL_FORMATED    {SIMPLE_FLOAT}[Ee][+-]?({DIGIT}*\.{DIGIT}*)*
INVALID_ID_NUM  {CONST_FLOAT}{LETTER}({LETTER}|{DIGIT})*
SPECIAL_CHAR   	\\[(\\)(\')(\")abfnrtv0]
CHAR_CONST 		{SPECIAL_CHAR}|[^\\'(\r\n)]
CHARACTER 		'{CHAR_CONST}'
EMPTY_CHAR 		''
MULTI_CHAR 		'{CHAR_CONST}{CHAR_CONST}+'
UNFINISHED_CHAR '({CHAR_CONST}|\\)*

%%

{SPACES}        	{
						spaces = tabs = 0;
						if(isnewline)
							for(char s: (string)yytext)
								tabs += (s == '\t'), spaces += (s == ' ');
					}
{WHITESPACE}		{}
{NEWLINE}           {lineCount++; isnewline = true; spaces = tabs = 0;}
{KEYWORD}           {addToken(yytext);}
{CONST_INT}         {addToken(yytext,"CONST_INT");}
{CONST_FLOAT}       {addToken(yytext,"CONST_FLOAT");}
{TOO_MANY_DEC}      {printError(yytext,"TOO_MANY_DECIMAL_POINTS");}
{ILL_FORMATED}      {printError(yytext,"ILLFORMED_NUMBER");}
{IDENTIFIER}        {insertToken(yytext,"ID");}
{INVALID_ID_NUM}    {printError(yytext,"INVALID_ID_SUFFIX_NUM_PREFIX");}
{ADDOP}             {addToken(yytext,"ADDOP");}
{MULOP}             {addToken(yytext,"MULOP");}
{INCOP}            	{addToken(yytext,"INCOP");}
{RELOP}             {addToken(yytext,"RELOP");}
{LOGICOP}           {addToken(yytext,"LOGICOP");}
{BITOP}             {addToken(yytext,"BITOP");}
"="          		{addToken(yytext,"ASSIGNOP");}
"!"					{addToken(yytext,"NOT");}
"("					{addToken(yytext,"LPAREN");}
")"					{addToken(yytext,"RPAREN");}
"{"					{addToken(yytext,"LCURL");}
"}"					{addToken(yytext,"RCURL");}
"["					{addToken(yytext,"LSQUARE");}
"]"					{addToken(yytext,"RSQUARE");}
";"					{addToken(yytext,"SEMICOLON");}
","					{addToken(yytext,"COMMA");}

{CHARACTER}			{addToken(yytext,"CONST_CHAR");}
{UNFINISHED_CHAR}	{
						string temp = yytext; 
						int size = min((temp[1]=='\\')+2, (int)temp.size()); 
						yyless(size); 
						printError(yytext,"UNFINISHED_CONST_CHAR");
					}
{EMPTY_CHAR}		{printError(yytext,"EMPTY_CONST_CHAR");}
{MULTI_CHAR}		{printError(yytext,"MULTICHAR_CONST_CHAR");}

\"				  		{res.clear(); res_plain="\""; nl_inside = 0; BEGIN STRING;}
<STRING>{SPECIAL_CHAR} 	{res += replaceText(yytext); res_plain += yytext;}
<STRING>{LINEBREAK}     {nl_inside++; res_plain += yytext ;}
<STRING>{NEWLINE} 		{nl_inside++; printError(res_plain, "UNFINISHED_STRING"); callInitial;}
<STRING><<EOF>>      	{printError(res_plain, "UNFINISHED_STRING"); callInitial;}
<STRING>\"          	{	
							res_plain += "\""; 
							if(nl_inside == 0) addString(res_plain, res, "SINGLE LINE STRING");
							else addString(res_plain, res, "MULTI LINE STRING"); 
							callInitial;
						}
<STRING>.            	{res += yytext[0]; res_plain += yytext[0];}

\/\/	             		{comment = "//"; nl_inside = 0; BEGIN SINGLE_COMMENT;}
<SINGLE_COMMENT>{NEWLINE} 	{nl_inside++; addComment(comment, "SINGLE LINE COMMENT"); callInitial;}
<SINGLE_COMMENT>{LINEBREAK} {nl_inside++; comment += yytext ;}
<SINGLE_COMMENT>\\ 			{comment += "\\";}
<SINGLE_COMMENT>. 			{comment += yytext[0];}

\/\* 						{comment = "/*"; nl_inside = 0; BEGIN MULTI_COMMENT;}
<MULTI_COMMENT>\*\/ 		{comment += "*/"; addComment(comment, "MULTI LINE COMMENT"); callInitial;}
<MULTI_COMMENT><<EOF>> 		{printError(comment, "UNFINISHED_COMMENT"); callInitial;}
<MULTI_COMMENT>\n 			{nl_inside++; comment += "\n";}
<MULTI_COMMENT>\r 			{comment += "\r";}
<MULTI_COMMENT>. 			{comment += yytext[0];}

. 					{printError(yytext,"UNRECOGNIZED_CHAR");}

%%

int main(int argc, char *argv[]){
	if(argc < 2){
		cout << "Please specify the input file" << endl;
		exit(1);
	}
	
	FILE *fin = fopen(argv[1],"r");
	if(fin == NULL){
		cout << "Cannot open specified file" << endl;
		exit(1);
	}

	tout.open("2005009_token.txt");
	lout.open("2005009_log.txt");

	yyin = fin;
	yylex();
	fclose(yyin);

	table._printAllScope(lout);
	lout << "Total lines: " << lineCount << endl;
	lout << "Total errors: " << errCount << endl;
	lout << "Total warnings: " << warnCount << endl;

	tout.close();
	lout.close();
	return 0;
}