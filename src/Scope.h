// represents a scope in the program
// .variables
// .structures

#pragma once

#include <vector>
#include "VariableDefinition.h"
#include "TypeStruct.h"
#include "TypeArray.h"
class Function;
class Statement_For;

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
	// for parent loop scope or null
	Statement_For* parent_for_loop = nullptr;

public:
	// init scope for a code bloc
	void init_CodeBloc(Scope& parent_scp);
	// create a new child scope
	Scope* create_child_scope(void);

	// find a variable by name
	const VariableDefinition* find_variable_by_name(std::string name) const;
	// declare a local variable
	void declare_local_variable(const VariableDefinition& def);
	// find a struct type by name
	const TypeStruct* find_struct_by_name(std::string name) const;
	// declare a type struct
	void declare_struct(const TypeStruct& struct_type) { know_types.push_back(struct_type); }
	// is the scope in a for loop ?
	bool is_in_for_loop(void) const { return parent_for_loop != nullptr; }

	// get the parent function
	// return nullptr if global scope
	Function* get_parent_function(void) { return parent_function; }

};

