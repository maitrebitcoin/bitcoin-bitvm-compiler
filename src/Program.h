#pragma once
// code tree after source code parsing

#include <string>
#include <vector>
#include <assert.h>	
#include "Type.h"
#include "VariableDefinition.h"
#include "Expression.h"
#include "Expression_StructMember.h"
#include "CodeBloc.h"
#include "Scope.h"

class Function;
class CodeBloc;
class BuildContext;
class Connection;
class Literal;
class LangageAttributes;

#include "Literal.h"
#include "Expression_Variable.h"
#include "BinaryOperation.h"
#include "ShiftOperation.h"
#include "UnaryOperation.h"
#include "TestOperation.h"
#include "ShiftOperation.h"
#include "Statement.h"
#include "Statement_Return.h"
#include "Statement_DeclareVar.h"
#include "Statement_SetVar.h"
#include "Statement_DeclareAndSetVar.h"
#include "Statement_DeclareStruct.h"
#include "Statement_Increment.h"
#include "Statement_If.h"
#include "Statement_For.h"
#include "Statement_Break.h"
#include "Function.h"
#include "Scope.h"

// logical representation of de contract 
class Program : public Scope {
	// structs definitions
	CodeBloc *struct_definitions;
	// body of the function
	std::vector<Function*> functions;
public:

	// add structures to the program as bloc before the main function
	void add_struct_definition(CodeBloc* bloc);
	// add a N function to the program
	void add_array_function(std::vector<Function*>* tabf);
	// get a function by name
	Function* find_function_by_name( std::string name ) const;
	// get main function
	Function* main_function(void) const;
	// init program tree, phase 2 of compilation
	void init_and_check_program_tree(void);


	// build a circuit that represents the program
	void build_circuit(class BuildContext &build_context);

private:
	// add a function to the program
	void add_function(Function* f);

};

