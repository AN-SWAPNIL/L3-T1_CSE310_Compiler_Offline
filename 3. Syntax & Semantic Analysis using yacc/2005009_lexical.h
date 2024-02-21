#include "2005009_SymbolTable.h"
#include "y.tab.h"
using namespace std;

#define tabSize 4

extern ofstream lout;
extern ofstream eout;
extern YYSTYPE yylval;

int lineCount = 1, errCount = 0;
int scopeCount = 0, spaces = 0, tabs = 0;
bool isnewline = true;
int nl_inside = 0;

// ofstream tout, lout;
// SymbolTable table(tableSize);

void insertToTable(string token_symbol, string token_name)
{
	// cout << token_symbol << " " << token_name << endl;
	yylval.symbolValue = new SymbolInfo(token_symbol, token_name, lineCount, lineCount, token_name + " : " + token_symbol);
	// if (table._insert(yylval.symbolValue))
	// 	table._printAllScope(lout);
	// else
	// 	lout << "\t" << token_symbol << " already exists in the current ScopeTable" << endl;
}

string toUpper(string s)
{
	for (int i = 0; i < s.size(); i++)
	{
		s[i] = toupper(s[i]);
	}
	return s;
}

string replaceText(string text)
{
	if (text == "\\\\")
		return "\\";
	else if (text == "\\\'")
		return "\'";
	else if (text == "\\\"")
		return "\"";
	else if (text == "\\a")
		return "\a";
	else if (text == "\\b")
		return "\b";
	else if (text == "\\f")
		return "\f";
	else if (text == "\\n")
		return "\n";
	else if (text == "\\r")
		return "\r";
	else if (text == "\\t")
		return "\t";
	else if (text == "\\v")
		return "\v";
	else if (text == "\\0")
		return "\0";
	return text;
}

void checkWarning()
{
	if (isnewline && (tabs != scopeCount || spaces != 0))
	{
		string msg;
		if (spaces == 0)
			msg = "Warning, " + to_string(scopeCount) + " of tabs needed but got " + to_string(tabs) + " tabs.";
		else
			msg = "Warning, tab requrired but got space.";
		lout << "Warning at line no "<< lineCount <<" : indentation error" << endl;
		eout << "Line# " << lineCount << ": " << msg << endl;
		errCount++;
	}
	isnewline = false, spaces = tabs = 0;
}

void printError(string text, string msg)
{
	checkWarning();
	lout << "Error at line# " << lineCount << ": " << msg << " " << text << endl;
	errCount++;
}

void printToken(string text, string token_name)
{
	if (token_name == "CONST_CHAR")
	{
		text.erase(0, 1);
		text.erase(text.size() - 1, 1);
		text = replaceText(text);
	}
	// tout << "<" << token_name << ", " << text << ">" << endl;
	lout << "Line# " << lineCount << ": Token "
		 << "<" << token_name << "> Lexeme " << text << " found" << endl;
}

// void insertToken(string text, string token_name)
// {
// 	if (token_name.size() == 0)
// 		token_name = toUpper(text);
// 	if (token_name == "RCURL")
// 	{
// 		scopeCount--;
// 		// table._exit();
// 	}
// 	checkWarning();
// 	if (token_name == "LCURL")
// 	{
// 		scopeCount++;
// 		// table._enter();
// 	}
// 	printToken(text, token_name);
// 	insertTotable(text, token_name);
// }

void addToken(string text, string token_name = "")
{
	if (token_name.size() == 0)
		token_name = toUpper(text);
	if (token_name == "RCURL")
	{
		scopeCount--;
		// table._exit();
	}
	checkWarning();
	if (token_name == "LCURL")
	{
		scopeCount++;
		// table._enter();
	}
	printToken(text, token_name);
	insertToTable(text, token_name);
}

void addString(string ltext, string ttext, string token_name)
{
	checkWarning();
	// tout << "<" << token_name << ", " << ttext << ">" << endl;
	// lout << "Line# " << lineCount << ": Token "
	// 	 << "<" << token_name << "> Lexeme " << ltext << " found" << endl;
}

void addComment(string text, string token_name)
{
	// lout << "Line# " << lineCount << ": Token "
	// 	 << "<" << token_name << "> Lexeme " << text << " found" << endl;
}