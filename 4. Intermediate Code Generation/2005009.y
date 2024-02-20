%{
#include "2005009_syntax.h"
#include "2005009_icg.h"

int inCondition = 0;

void yyerror(const char *s) {
	// cout << "Error in line no " << lineCount<<" : " << s << endl;
}
int yyparse(void);
int yylex(void);

%}

%union{
    SymbolInfo* symbolValue;
}

%token <symbolValue> IF FOR WHILE DO BREAK INT FLOAT CHAR DOUBLE VOID RETURN SWITCH CASE DEFAULT CONTINUE PRINTLN
%token <symbolValue> LPAREN RPAREN LCURL RCURL LSQUARE RSQUARE SEMICOLON COMMA
%token <symbolValue> ID CONST_INT CONST_FLOAT CONST_CHAR
%token <symbolValue> ELSE
%token <symbolValue> ASSIGNOP BITOP LOGICOP RELOP ADDOP MULOP NOT INCOP DECOP

%type <symbolValue> array rparen else
%type <symbolValue> start program unit func_declaration func_definition parameter_list compound_statement
%type <symbolValue> var_declaration type_specifier declaration_list statements statement expression_statement
%type <symbolValue> variable expression logic_expression rel_expression simple_expression term unary_expression
%type <symbolValue> factor argument_list arguments started_LCURL

%right RPAREN
%left LOGICOP
%left RELOP
%left ADDOP
%left MULOP
%right ASSIGNOP
%left BITOP
%right NOT
%right INCOP DECOP

%nonassoc ELSE

%%

start : program
	{
		$$ = initialProcess({$1}, "start" ,"program");
		$$->printTree(pout, 0);
		asmCodeGenerator(table, $$, errCount);
	}
	;

program : program unit
	{
		$$ = initialProcess({$1, $2}, "program", "program unit");
	}
	| unit
	{
		$$ = initialProcess({$1}, "program","unit");
	}
	;
	
unit : var_declaration
	{
		$$ = initialProcess({$1}, "unit", "var_declaration");
	}
	| func_declaration
	{
		$$ = initialProcess({$1}, "unit", "func_declaration");
	}
	| func_definition
	{
		$$ = initialProcess({$1}, "unit", "func_definition");
	}
	| error
	{
		$$ = initialProcess({},"unit","error");
		syntaxError(0,"unit");
	}
	;
	
func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON
	{
		$$ = initialProcess({$1, $2, $3, $4, $5, $6}, "func_declaration", "type_specifier ID LPAREN parameter_list RPAREN SEMICOLON");
		insertFunction($2, $4->getParameters(), $1->getTypeSpecifier(), SymbolType::FUNCTION_DECLARATION);
	}
	| type_specifier ID LPAREN RPAREN SEMICOLON
	{
		$$ = initialProcess({$1, $2, $3, $4, $5}, "func_declaration", "type_specifier ID LPAREN RPAREN SEMICOLON");
		insertFunction($2, {}, $1->getTypeSpecifier(), SymbolType::FUNCTION_DECLARATION);
	}
	;
		 
func_definition : type_specifier ID LPAREN parameter_list RPAREN 
	{
		insertFunction($2, $4->getParameters(), $1->getTypeSpecifier(), SymbolType::FUNCTION_DEFINITION);
	}
	compound_statement
	{
		$$ = initialProcess({$1, $2, $3, $4, $5, $7}, "func_definition","type_specifier ID LPAREN parameter_list RPAREN compound_statement");
	}
	| type_specifier ID LPAREN RPAREN
	{
		insertFunction($2, {}, $1->getTypeSpecifier(), SymbolType::FUNCTION_DEFINITION);
	}
	compound_statement
	{
		$$ = initialProcess({$1, $2, $3, $4, $6}, "func_definition", "type_specifier ID LPAREN RPAREN compound_statement");
	}
	;				

parameter_list : parameter_list COMMA type_specifier ID
	{
		$$ = initialProcess({$1, $2, $3, $4}, "parameter_list", "parameter_list COMMA type_specifier ID");
		updateParameterList($$, $1->getParameters(), $3, $4);
	}
	| parameter_list COMMA type_specifier
	{
		$$ = initialProcess({$1, $2, $3},"parameter_list","parameter_list COMMA type_specifier");
		updateParameterList($$, $1->getParameters(), $3);
	}
	| type_specifier ID
	{
		$$ = initialProcess({$1, $2},"parameter_list","type_specifier ID");
		updateParameterList($$, {}, $1, $2);
	}
	| type_specifier
	{
		$$ = initialProcess({$1},"parameter_list","type_specifier");
		updateParameterList($$, {}, $1);
	}
	| error
	{
		$$ = initialProcess({},"parameter_list","error");
		syntaxError(0,"parameter");
	}
	;

compound_statement : started_LCURL statements RCURL
	{
		$$ = initialProcess({$1,$2,$3},"compound_statement","LCURL statements RCURL");
		$$->setOffset(table->getCurrentScope()->getTableOffset());
		table->_printAllScope(lout);
		if(!inCondition)
			table->_exit();
	}
	| started_LCURL RCURL
	{
		$$ = initialProcess({$1,$2},"compound_statement","LCURL RCURL");
		$$->setOffset(table->getCurrentScope()->getTableOffset());
		table->_printAllScope(lout);
		if(!inCondition)
			table->_exit();
	}
	;
 		    
var_declaration : type_specifier declaration_list SEMICOLON
	{
		$$ = initialProcess({$1,$2,$3},"var_declaration","type_specifier declaration_list SEMICOLON");
		$$->setScopeID(table->getCurrentScope()->getId());
		// cout << "At table " << table->getCurrentScope()->getId() << " with offset " << table->getCurrentScope()->getTableOffset() << endl;
		for(SymbolInfo* declaration : $2->getDeclarations()) {
			declaration->setScopeID(table->getCurrentScope()->getId());
			if(table->getCurrentScope()->getId() == 1)
				declaration->setOffset(0);
			else {
				if(declaration->getSymbolType() == SymbolType::ARRAY)
					declaration->setOffset(table->getCurrentScope()->getTableOffset() - 2*declaration->getArraySize());
				else
					declaration->setOffset(table->getCurrentScope()->getTableOffset() - 2);
			}
			if(isValidTypeSpecifier($1, declaration->getName())) {
				declaration->setTypeSpecifier($1->getTypeSpecifier());
				if(!table->_insert(declaration->getName(), declaration->getType(), declaration))
					errorPrint(declaration->getStartLine(), "Redeclaration of variable \'" + declaration->getName()+"\'");
				else
					table->getCurrentScope()->setTableOffset(declaration->getOffset());
			}
			// cout << "Variable \'" << declaration->getName() << "\' at offset " << declaration->getOffset() << " table id " << table->getCurrentScope()->getId() <<endl;
		}
	}
	;
 		 
type_specifier	: INT
	{
		$$ = initialProcess({$1},"type_specifier","INT");
		$$->setTypeSpecifier(TypeSpecifier::_INT);
	}
	| FLOAT
	{
		$$ = initialProcess({$1},"type_specifier","FLOAT");
		$$->setTypeSpecifier(TypeSpecifier::_FLOAT);
	}
	| VOID
	{
		$$ = initialProcess({$1},"type_specifier","VOID");
		$$->setTypeSpecifier(TypeSpecifier::_VOID);
	}
	;
 		
declaration_list : declaration_list COMMA ID
	{
		$$ = initialProcess({$1,$2,$3},"declaration_list","declaration_list COMMA ID");
		$$->setDeclarations($1->getDeclarations());
		$$->addDeclaration($3);
	}
	| declaration_list COMMA ID LSQUARE CONST_INT RSQUARE
	{
		$$ = initialProcess({$1,$2,$3,$4,$5,$6},"declaration_list","declaration_list COMMA ID LSQUARE CONST_INT RSQUARE");
		$$->setDeclarations($1->getDeclarations());
		$3->setSymbolType(SymbolType::ARRAY);
		$3->setArraySize(stoi($5->getName()));
		$$->addDeclaration($3);
	}
	| declaration_list COMMA ID LSQUARE CONST_INT RSQUARE array
	{
		$$ = initialProcess({$1,$2,$3,$4,$5,$6,$7},"declaration_list","declaration_list COMMA ID LSQUARE CONST_INT RSQUARE array_list");
		$$->setDeclarations($1->getDeclarations());
		$3->setSymbolType(SymbolType::ARRAY);
		$3->setArraySize($7->getArrayList().size());
		$3->setArrayList($7->getArrayList());
		if($7->getArrayList().size() != stoi($5->getName()))
			errorPrint($3->getStartLine(), "Array size mismatch with initialization");
		$$->addDeclaration($3);
	}
	| declaration_list COMMA ID LSQUARE RSQUARE array
	{
		$$ = initialProcess({$1,$2,$3,$4,$5,$6},"declaration_list","declaration_list COMMA ID LSQUARE CONST_INT RSQUARE array_list");
		$$->setDeclarations($1->getDeclarations());
		$3->setSymbolType(SymbolType::ARRAY);
		$3->setArraySize($6->getArrayList().size());
		$3->setArrayList($6->getArrayList());
		$$->addDeclaration($3);
	}
	| ID LSQUARE CONST_INT RSQUARE array
	{
		$$ = initialProcess({$1,$2,$3,$4,$5},"declaration_list","ID LSQUARE CONST_INT RSQUARE array_list");
		$1->setSymbolType(SymbolType::ARRAY);
		$1->setArraySize($5->getArrayList().size());
		$1->setArrayList($5->getArrayList());
		if($5->getArrayList().size() != stoi($3->getName()))
			errorPrint($1->getStartLine(), "Array size mismatch with initialization");
		$$->addDeclaration($1);
	}
	| ID
	{
		$$ = initialProcess({$1},"declaration_list","ID");
		$$->addDeclaration($1);
	}
	| ID LSQUARE CONST_INT RSQUARE
	{
		$$ = initialProcess({$1,$2,$3,$4},"declaration_list","ID LSQUARE CONST_INT RSQUARE");
		$1->setSymbolType(SymbolType::ARRAY);
		$1->setArraySize(stoi($3->getName()));
		$$->addDeclaration($1);
	}
	| ID LSQUARE RSQUARE array
	{
		$$ = initialProcess({$1,$2,$3,$4},"declaration_list","ID LSQUARE RSQUARE array_list");
		$1->setSymbolType(SymbolType::ARRAY);
		$1->setArraySize($4->getArrayList().size());
		$1->setArrayList($4->getArrayList());
		$$->addDeclaration($1);
	}
	| error
	{
		$$ = initialProcess({},"declaration_list","error");
		syntaxError(0,"declaration");
	}
	;
 		  
statements : statement
	{
		$$ = initialProcess({$1}, "statements", "statement");
	}
	| statements statement
	{
		$$ = initialProcess({$1,$2}, "statements","statements statement");
	}
	;

statement : var_declaration
	{
		$$ = initialProcess({$1},"statement","var_declaration");
	}
	| expression_statement
	{
		$$ = initialProcess({$1},"statement","expression_statement");
	}
	| compound_statement
	{
		$$ = initialProcess({$1},"statement","compound_statement");
	}
	| FOR LPAREN expression_statement expression_statement expression rparen statement
	{
		$$ = initialProcess({$1,$2,$3,$4,$5,$6,$7},"statement","FOR LPAREN expression_statement expression_statement expression RPAREN statement");
		int offset = table->getCurrentScope()->getTableOffset();
		table->_exit();
		offset -= table->getCurrentScope()->getTableOffset();
		$$->setOffset(offset);
		inCondition--;
		// cout << "FOR LOOP at line " << $$->getStartLine() << " to " << $$->getEndLine() << " with offset " << offset << endl;
	}
	| IF LPAREN expression rparen statement
	{
		$$ = initialProcess({$1,$2,$3,$4,$5},"statement","IF LPAREN expression RPAREN statement");
		int offset = table->getCurrentScope()->getTableOffset();
		table->_exit();
		offset -= table->getCurrentScope()->getTableOffset();
		$$->setOffset(offset);
		inCondition--;
		// cout << "IF at line " << $$->getStartLine() << " to " << $$->getEndLine() << " with offset " << offset << endl;
	}
	| IF LPAREN expression rparen statement else statement
	{
		$$ = initialProcess({$1,$2,$3,$4,$5,$6,$7},"statement","IF LPAREN expression RPAREN statement ELSE statement");
		int offset = table->getCurrentScope()->getTableOffset();
		table->_exit();
		table->_exit();
		offset -= table->getCurrentScope()->getTableOffset();
		$$->setOffset(offset);
		inCondition--;
		// cout << "IF ELSE at line " << $$->getStartLine() << " to " << $$->getEndLine() << " with offset " << offset << endl;
	}
	| WHILE LPAREN expression rparen statement
	{
		$$ = initialProcess({$1,$2,$3,$4,$5},"statement","WHILE LPAREN expression RPAREN statement");
		int offset = table->getCurrentScope()->getTableOffset();
		table->_exit();
		offset -= table->getCurrentScope()->getTableOffset();
		$$->setOffset(offset);
		inCondition--;
		// cout << "WHILE at line " << $$->getStartLine() << " to " << $$->getEndLine() << " with offset " << offset << endl;
	}
	| PRINTLN LPAREN ID RPAREN SEMICOLON
	{
		$$ = initialProcess({$1,$2,$3,$4,$5}, "statement","PRINTLN LPAREN ID RPAREN SEMICOLON");
		SymbolInfo* temp = table->_lookUp($3->getName());
		if(!temp) {
			errorPrint($3->getStartLine(), "Undeclared variable \'" + $3->getName()+"\'");
		} else {
			temp = temp->getPointTo();
			$$->setOffset(temp->getOffset());
			$$->setScopeID(temp->getScopeID());
		}
	}
	| RETURN expression SEMICOLON
	{
		$$ = initialProcess({$1,$2,$3},"statement","RETURN expression SEMICOLON");
	}
	;

rparen : RPAREN
	{
		$$ = $1;
		inCondition++;
		int offset = table->getCurrentScope()->getTableOffset();
		table->_enter();
		table->getCurrentScope()->setTableOffset(offset);
		// cout << "Table Id " << table->getCurrentScope()->getId() << " with offset " << offset << endl;
	}
	;

else : ELSE
	{
		$$ = $1;
		// table->_exit();
		int offset = table->getCurrentScope()->getTableOffset();
		table->_enter();
		table->getCurrentScope()->setTableOffset(offset);
		// cout << "Table Id " << table->getCurrentScope()->getId() << " with offset " << offset << endl;
	}
	;

expression_statement : SEMICOLON
	{
		$$ = initialProcess({$1},"expression_statement","SEMICOLON");
	}			
	| expression SEMICOLON 
	{
		$$ = initialProcess({$1,$2},"expression_statement","expression SEMICOLON");
		$$->setTypeSpecifier($1->getTypeSpecifier());
	}
	;
	  
variable : ID
	{
		$$ = initialProcess({$1},"variable","ID");
		SymbolInfo* temp = table->_lookUp($1->getName());
		if(!temp)
			errorPrint($1->getStartLine(), "Undeclared variable \'" + $1->getName()+"\'");
		else {
			temp = temp->getPointTo();
			$$->setScopeID(temp->getScopeID());
			$$->setOffset(temp->getOffset());
			if(temp->getSymbolType() == SymbolType::ARRAY) {	
				errorPrint($1->getStartLine(), "\'" + $1->getName() + "\' is an array");
				$$->setTypeSpecifier(TypeSpecifier::_ERROR);
			} else if(temp->getSymbolType() == SymbolType::FUNCTION_DEFINITION || temp->getSymbolType() == SymbolType::FUNCTION_DECLARATION) {
				errorPrint($1->getStartLine(), "\'" + $1->getName() + "\' is a function");
				$$->setTypeSpecifier(TypeSpecifier::_ERROR);
			} else
				$$->setTypeSpecifier(temp->getTypeSpecifier());
		}
	}
	| ID LSQUARE expression RSQUARE 
	{
		$$ = initialProcess({$1,$2,$3,$4},"variable","ID LSQUARE expression RSQUARE");
		SymbolInfo* temp = table->_lookUp($1->getName());
		if(!temp)
			errorPrint($1->getStartLine(), "Undeclared variable \'" + $1->getName()+"\'");
		else {
			temp = temp->getPointTo();
			$$->setScopeID(temp->getScopeID());
			$$->setOffset(temp->getOffset());
			if(temp->getSymbolType() == SymbolType::FUNCTION_DEFINITION || temp->getSymbolType() == SymbolType::FUNCTION_DECLARATION) {
				errorPrint($1->getStartLine(), "\'" + $1->getName() + "\' is a function");
				$$->setTypeSpecifier(TypeSpecifier::_ERROR);
			} else if(temp->getSymbolType() != SymbolType::ARRAY) {
				errorPrint($1->getStartLine(), "\'"+ $1->getName() + "\' is not an array");
				$$->setTypeSpecifier(TypeSpecifier::_ERROR);
			} else {
				$$->setSymbolType(SymbolType::ARRAY);
				$$->setTypeSpecifier(temp->getTypeSpecifier());
			}
			if($3->getTypeSpecifier() != TypeSpecifier::_INT) 
				errorPrint($3->getStartLine(), "Array subscript is not an integer");
		}
	}
	;

expression : logic_expression
	{
		$$ = initialProcess({$1},"expression","logic_expression");
		$$->setTypeSpecifier($1->getTypeSpecifier());
		if($1->getSymbolType() == SymbolType::ARRAY) {
			$$->setSymbolType(SymbolType::ARRAY);
		}
	}
	| variable ASSIGNOP logic_expression 	
	{
		$$ = initialProcess({$1,$2,$3},"expression","variable ASSIGNOP logic_expression");
		$$->setTypeSpecifier($1->getTypeSpecifier());
		if($1->getTypeSpecifier() == TypeSpecifier::_VOID || $3->getTypeSpecifier() == TypeSpecifier::_VOID) {
			errorPrint($1->getStartLine(), "Void cannot be used in expression");
			$$->setTypeSpecifier(TypeSpecifier::_ERROR);
		}
		else if($1->getTypeSpecifier() == TypeSpecifier::_FLOAT && $3->getTypeSpecifier() == TypeSpecifier::_INT) {}
		else if($3->getTypeSpecifier() == TypeSpecifier::_FLOAT && $1->getTypeSpecifier() == TypeSpecifier::_INT) 
			errorPrint($1->getStartLine(), "Warning: possible loss of data in assignment of FLOAT to INT");
		else if($1->getTypeSpecifier() == TypeSpecifier::_ERROR || $3->getTypeSpecifier() == TypeSpecifier::_ERROR) {
			// errorPrint($1->getStartLine(), "Type mismatch in assignment");
			$$->setTypeSpecifier(TypeSpecifier::_ERROR);
		}
	}
	| error
	{
		$$ = initialProcess({},"expression","error");
		syntaxError(0,"expression");
	}
	;

logic_expression : rel_expression 	
	{
		$$ = initialProcess({$1},"logic_expression","rel_expression");
		$$->setTypeSpecifier($1->getTypeSpecifier());
		if($1->getSymbolType() == SymbolType::ARRAY)
			$$->setSymbolType(SymbolType::ARRAY);
	}
	| rel_expression LOGICOP rel_expression 	
	{
		$$ = initialProcess({$1,$2,$3},"logic_expression","rel_expression LOGICOP rel_expression");
		setType($$,getType($1->getTypeSpecifier(), $3->getTypeSpecifier()),TypeSpecifier::_INT);
	}
	;

rel_expression : simple_expression 
	{
		$$ = initialProcess({$1},"rel_expression","simple_expression");
		$$->setTypeSpecifier($1->getTypeSpecifier());
		if($1->getSymbolType() == SymbolType::ARRAY)
			$$->setSymbolType(SymbolType::ARRAY);
	}
	| simple_expression RELOP simple_expression	
	{
		$$ = initialProcess({$1,$2,$3},"rel_expression","simple_expression RELOP simple_expression");
		setType($$,getType($1->getTypeSpecifier(), $3->getTypeSpecifier()),TypeSpecifier::_INT);
	}
	;

simple_expression : term 
	{
		$$ = initialProcess({$1},"simple_expression","term");
		$$->setTypeSpecifier($1->getTypeSpecifier());
		if($1->getSymbolType() == SymbolType::ARRAY)
			$$->setSymbolType(SymbolType::ARRAY);
	}
	| simple_expression ADDOP term 
	{
		$$ = initialProcess({$1,$2,$3},"simple_expression","simple_expression ADDOP term");
		setType($$,getType($1->getTypeSpecifier(), $3->getTypeSpecifier()));
	}
	;

term : unary_expression
	{
		$$ = initialProcess({$1},"term","unary_expression");
		$$->setTypeSpecifier($1->getTypeSpecifier());
		if($1->getSymbolType() == SymbolType::ARRAY)
			$$->setSymbolType(SymbolType::ARRAY);
	}
	| term MULOP unary_expression
	{
		$$ = initialProcess({$1,$2,$3},"term","term MULOP unary_expression");
		$$->setTypeSpecifier(getType($1->getTypeSpecifier(), $3->getTypeSpecifier()));
		if($$->getTypeSpecifier() == TypeSpecifier::_ERROR) {
			// errorPrint($1->getStartLine(), "Type mismatch in expression");
		} else if($$->getTypeSpecifier() == TypeSpecifier::_VOID) {
			errorPrint($1->getStartLine(), "Void cannot be used in expression");
			$$->setTypeSpecifier(TypeSpecifier::_ERROR);
		} else if( ($2->getName() == "%" || $2->getName() == "/") && $3->getName() == "0" ) {
			errorPrint($1->getStartLine(), "Warning: division by zero");
			$$->setTypeSpecifier(TypeSpecifier::_ERROR);
		} else if($2->getName() == "%" && ( $1->getTypeSpecifier() != TypeSpecifier::_INT || $3->getTypeSpecifier() != TypeSpecifier::_INT) ) {
			errorPrint($1->getStartLine(), "Operands of modulus must be integers");
			$$->setTypeSpecifier(TypeSpecifier::_ERROR);
		}
	}
	;

unary_expression : ADDOP unary_expression  
	{
		$$ = initialProcess({$1,$2},"unary_expression","ADDOP unary_expression");
		$$->setTypeSpecifier($2->getTypeSpecifier());
		if($2->getTypeSpecifier() == TypeSpecifier::_VOID) {
			errorPrint($1->getStartLine(), "Void cannot be used in expression");
			$$->setTypeSpecifier(TypeSpecifier::_ERROR);
		}
	}
	| NOT unary_expression
	{
		$$ = initialProcess({$1,$2},"unary_expression","NOT unary_expression");
		setType($$,$2->getTypeSpecifier(),TypeSpecifier::_INT);
	}
	| factor
	{
		$$ = initialProcess({$1},"unary_expression","factor");
		$$->setTypeSpecifier($1->getTypeSpecifier());
		if($1->getSymbolType() == SymbolType::ARRAY)
			$$->setSymbolType(SymbolType::ARRAY);
	}
	;

factor : variable
	{
		$$ = initialProcess({$1},"factor","variable");
		$$->setTypeSpecifier($1->getTypeSpecifier());
		if($1->getSymbolType() == SymbolType::ARRAY)
			$$->setSymbolType(SymbolType::ARRAY);
	}
	| ID LPAREN argument_list RPAREN
	{
		$$ = initialProcess({$1,$2,$3,$4},"factor","ID LPAREN argument_list RPAREN");
		SymbolInfo* temp = table->_lookUp($1->getName());
		if(!temp)
			errorPrint($1->getStartLine(), "Undeclared function \'" + $1->getName()+"\'");
		else {
			temp = temp->getPointTo();
			if(temp->getSymbolType() != SymbolType::FUNCTION_DEFINITION)
				errorPrint($1->getStartLine(), "Undefined function \'" + $1->getName() + "\'");
			else if(temp->getParameters().size() < $3->getParameters().size())
				errorPrint($1->getStartLine(), "Too many arguments to function \'" + $1->getName() + "\'");
			else if(temp->getParameters().size() > $3->getParameters().size())
				errorPrint($1->getStartLine(), "Too few arguments to function \'" + $1->getName() + "\'");
			else {
				vector<SymbolInfo*> parameters = temp->getParameters();
				vector<SymbolInfo*> arguments = $3->getParameters();
				for(int i = 0; i < parameters.size(); i++){
					if(parameters[i]->getTypeSpecifier() != arguments[i]->getTypeSpecifier() || parameters[i]->getSymbolType() != arguments[i]->getSymbolType())
						errorPrint($1->getStartLine(), "Type mismatch for argument " + to_string(i+1) + " of \'" + $1->getName() + "\'");
				}
			}
		}
		if(temp) $$->setTypeSpecifier(temp->getTypeSpecifier());
		else $$->setTypeSpecifier(TypeSpecifier::_ERROR);
	}
	| LPAREN expression RPAREN
	{
		$$ = initialProcess({$1,$2,$3},"factor","LPAREN expression RPAREN");
		$$->setTypeSpecifier($2->getTypeSpecifier());
	}
	| CONST_INT 
	{
		$$ = initialProcess({$1},"factor","CONST_INT");
		$$->setTypeSpecifier(TypeSpecifier::_INT);
	}
	| CONST_FLOAT
	{
		$$ = initialProcess({$1},"factor","CONST_FLOAT");
		$$->setTypeSpecifier(TypeSpecifier::_FLOAT);
	}
	| variable INCOP 
	{
		$$ = initialProcess({$1,$2},"factor","variable INCOP");
		setType($$,$1->getTypeSpecifier());
	}
	| variable DECOP
	{
		$$ = initialProcess({$1,$2},"factor","variable DECOP");
		setType($$,$1->getTypeSpecifier());
	}
	;
	
argument_list : arguments
	{
		$$ = initialProcess({$1},"argument_list","arguments");
		$$->setParameters($1->getParameters());
		$$->setOffset($1->getOffset());
		$$->setScopeID($1->getScopeID());
	}
	|
	{
		$$ = initialProcess({},"argument_list","");
	}
	;
	
arguments : arguments COMMA logic_expression
	{
		$$ = initialProcess({$1,$2,$3},"arguments","arguments COMMA logic_expression");
		$$->setParameters($1->getParameters());
		$$->addParameter($3);
		$$->setOffset($1->getOffset()+2);
	}
	| logic_expression
	{
		$$ = initialProcess({$1},"arguments","logic_expression");
		$$->addParameter($1);
		$$->setOffset(2);
	}
	;

started_LCURL : LCURL
	{
		$$ = $1;
		if(!inCondition) {
			table->_enter();
			int offset = 2;
			reverse(tempParameterList.begin(), tempParameterList.end());
			for(SymbolInfo* parameter : tempParameterList){
				parameter->setOffset(offset+2);
				if(parameter->getName() == "")
					errorPrint(parameter->getStartLine(), "Parameter name missing");
				else if(!table->_insert(parameter->getName(), parameter->getType(), parameter))
					errorPrint(parameter->getStartLine(), "Redefinition of parameter \'" + parameter->getName()+"\'");
				else offset += 2;
			}
			tempParameterList.clear();
			// cout << "New Func Table Id " << table->getCurrentScope()->getId() << " with offset " << table->getCurrentScope()->getTableOffset() << endl;
		}
	}
	;

array : ASSIGNOP LCURL argument_list RCURL
	{
		$$ = initialProcess({$1,$2,$3},"array","ASSIGNOP LCURL argument_list RCURL");
		$$->setArrayList($3->getParameters());
	}
	;
	
%%

int main(int argc,char *argv[]){
	FILE *fp;

	if((fp=fopen(argv[1],"r"))==NULL){
		printf("Cannot Open Input File.\n");
		exit(1);
	}

	lout.open("log.txt");
	/* eout.open("error.txt"); */
	pout.open("parsetree.txt");
	asmout.open("code.asm");
	optout.open("optimized_code.asm");

	yyin=fp;
	yyparse();
	
	lout << "Total lines: " << lineCount << endl;
    lout << "Total errors: " << errCount << endl;
	if(!errCount)
		cout << "Successfully Compiled" << endl;
	else 
		cout << "Compilation with Error" << endl;

	asmout.close();

	vector<vector<string>> code;
	getCodeList("code.asm", code);
	peepholeOptimization(optout, code);

	optout.close();

	fclose(yyin);
 	lout.close();
	eout.close();
	pout.close();
	return 0;
}