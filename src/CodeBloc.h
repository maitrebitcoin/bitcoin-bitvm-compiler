#pragma once

#include <vector>
#include "VariableDefinition.h"
#include "TypeStruct.h"
class Statement;
class Function;

class CodeBloc {
public:
	// code statements
	std::vector<Statement*> statements;
	Function* parent_function = nullptr;
	// declared local variables
	std::vector<VariableDefinition> local_variables;
	// declared struct
	std::vector<TypeStruct> know_types;

public:
	// constructor
	CodeBloc(Statement* first_statement) { statements.push_back(first_statement); }
	// add a statement
	void add_statement(Statement* s) { statements.push_back(s); }
	// init a bloc
	void init(Function* parent_function);
	// get the parent function
	Function* get_parent_function(void) { return parent_function; }
	// find a variable by name
	const VariableDefinition* find_variable_by_name(std::string name) const;
	// declare a local variable
	void declare_local_variable(const VariableDefinition& def);
	// find a struct type by name
	const TypeStruct* find_struct_by_name(std::string name) const;
	// declare a type struct
	void declare_struct(const TypeStruct& struct_type) { know_types.push_back(struct_type); }


	// get the return statement of the bloc
	class Statement_Return* get_return_statement(void) const;

};