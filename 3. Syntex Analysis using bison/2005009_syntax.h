#include "2005009_SymbolTable.h"
#include <typeinfo>

using namespace std;

ofstream lout;
ofstream eout;
ofstream pout;

int prevLine = 1;

extern int lineCount;
extern int errCount;
extern FILE *yyin;

vector<SymbolInfo*> tempParameterList;

int tableSize = 11;
SymbolTable *table = new SymbolTable(tableSize);

void errorPrint(int lineNo, string s) {
    eout << "Line# " << lineNo << ": " << s << endl;
	errCount++;
}

void syntaxError(int lineNo, string type) {
	if(lineNo == 0) lineNo = prevLine;
	lout << "Error at line no "<< lineNo <<" : syntax error" << endl;
	if(type == "parameter")
		eout << "Line# " << lineNo << ": Syntax error at parameter list of function definition" << endl;
	else if(type == "declaration")
		eout << "Line# "<<lineNo<<": Syntax error at declaration list of variable declaration" << endl;
	else if(type == "expression")
		eout << "Line# "<< lineNo <<": Syntax error at expression of expression statement" << endl;
	errCount++;
}

bool isValidTypeSpecifier(SymbolInfo* typeSpecifier, string name) {
	if( typeSpecifier->getTypeSpecifier() == TypeSpecifier::_VOID ){
		if(name == "")
			errorPrint(typeSpecifier->getStartLine(), "Variable or field declared void");
		else 
			errorPrint(typeSpecifier->getStartLine(), "Variable or field \'"+ name+ "\' declared void");
		return false;
	}
	return true;
}

SymbolInfo* initialProcess(vector<SymbolInfo*> children, string type, string rule) {
	string name = "";
	if(children.size()>0) name += children[0]->getName();
	for(int i=1; i<children.size(); i++) name += " " + children[i]->getName();
    lout << rule << endl;
	SymbolInfo* root;
	if(children.size()>0) 
	 	root= new SymbolInfo(name, type, children[0]->getStartLine(), children[children.size()-1]->getEndLine(), rule);
	else
		root= new SymbolInfo(name, type, prevLine, prevLine, rule);
	root->setChildren(children);
	prevLine = lineCount;
	return root;
}

void logPrint(string output) {
	lout << output << endl;
}

TypeSpecifier getType(TypeSpecifier leftSymbol, TypeSpecifier rightSymbol) {
	if( leftSymbol == TypeSpecifier::_VOID || rightSymbol == TypeSpecifier::_VOID ) return TypeSpecifier::_VOID;
	if( leftSymbol == TypeSpecifier::_ERROR || rightSymbol == TypeSpecifier::_ERROR ) return TypeSpecifier::_ERROR;
	if( leftSymbol == TypeSpecifier::_FLOAT || rightSymbol == TypeSpecifier::_FLOAT ) return TypeSpecifier::_FLOAT;
	return TypeSpecifier::_INT;
}

void insertFunction(SymbolInfo* function, vector<SymbolInfo*> parameterList, TypeSpecifier typeSpecifier, SymbolType symbolType) {
	function->setTypeSpecifier(typeSpecifier);
	function->setParameters(parameterList);
	function->setSymbolType(symbolType);

	if(table->getCurrentScope()->getParentScope()!=NULL)
		errorPrint(function->getStartLine(), "Wrong scoping of function \'" + function->getName()+ "\'");

	if(table->_insert(function->getName(),function->getType(), function)) return;
	
    if(symbolType == SymbolType::FUNCTION_DECLARATION) {
        errorPrint(function->getStartLine(), "Redeclaration of function \'" + function->getName()+ "\'");
        return;
    }

    SymbolInfo* prevSymbol = table->_lookUp(function->getName())->getPointTo();
    if( prevSymbol->getSymbolType() != SymbolType::FUNCTION_DECLARATION ) { 
		errorPrint(function->getStartLine(), "\'" + function->getName() + "\' redeclared as different kind of symbol");
		return;
	}
	if( prevSymbol->getTypeSpecifier() != function->getTypeSpecifier() )
		errorPrint(function->getStartLine(), "Conflicting types for \'" + function->getName() +"\'");
	if( prevSymbol->getParameters().size() != function->getParameters().size() )
		errorPrint(function->getStartLine(), "Conflicting types for \'" + function->getName()+"\'");
	else {
		prevSymbol->setSymbolType(SymbolType::FUNCTION_DEFINITION);
		vector<SymbolInfo*> paramsOfDeclaration = prevSymbol->getParameters(), paramsOfDefinition = function->getParameters();
		int i = 0;
		for(SymbolInfo* param: paramsOfDefinition) {
			if( param->getTypeSpecifier() != paramsOfDeclaration[i]->getTypeSpecifier())
				errorPrint(function->getStartLine(), to_string(i+1) + "th parameter mismatch in function " + function->getName());
			else if( paramsOfDeclaration[i]->getSymbolType() != param->getSymbolType())
				errorPrint(function->getStartLine(), to_string(i+1) + "th parameter mismatch in function " + function->getName());
			i++;
		}
		prevSymbol->setParameters(paramsOfDefinition);
	}
}

void updateParameterList(SymbolInfo* parent, vector<SymbolInfo*> parameterList, SymbolInfo* type, SymbolInfo* child = NULL) {
	if(child == NULL)
		child = new SymbolInfo("", "ID", type->getStartLine(), type->getEndLine());
	child->setTypeSpecifier(type->getTypeSpecifier());
	parent->setParameters(parameterList);
	parent->addParameter(child);
	isValidTypeSpecifier(type, child->getName());
	tempParameterList = parent->getParameters();
}

void setType(SymbolInfo* symbol, TypeSpecifier type, TypeSpecifier trivial = TypeSpecifier::_NONE) {
	if(type == TypeSpecifier::_VOID) {
		errorPrint(symbol->getStartLine(), "Void cannot be used in expression");
		symbol->setTypeSpecifier(TypeSpecifier::_ERROR);
	} else if(type == TypeSpecifier::_ERROR) {
		// errorPrint($1->getStartLine(), "Type mismatch in expression");
		symbol->setTypeSpecifier(type);
	} else if(trivial != TypeSpecifier::_NONE) {
		symbol->setTypeSpecifier(trivial);
	} else {
		symbol->setTypeSpecifier(type);
	}
}