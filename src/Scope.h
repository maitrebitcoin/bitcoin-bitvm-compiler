// represents a scope in the program
// .variables
// .structures

#pragma once

#include <vector>
#include "VariableDefinition.h"
#include "TypeStruct.h"
class Function;

class Scope {
public:
	// declared local variables
	std::vector<VariableDefinition> local_variables;
	// declared struct
	std::vector<TypeStruct> know_types;

	// parent scrope. null for global scope
	Scope* parent_scope = nullptr;
	// null for global scope, parent function for local scope
	Function* parent_function = nullptr;


public:

	// find a variable by name
	const VariableDefinition* find_variable_by_name(std::string name) const;
	// declare a local variable
	void declare_local_variable(const VariableDefinition& def);
	// find a struct type by name
	const TypeStruct* find_struct_by_name(std::string name) const;
	// declare a type struct
	void declare_struct(const TypeStruct& struct_type) { know_types.push_back(struct_type); }

	// get the parent function
	// return nullptr if global scope
	Function* get_parent_function(void) { return parent_function; }

};

