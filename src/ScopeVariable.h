#pragma once

#include <string>
#include <vector>
#include "Connection.h"
#include "VariableDefinition.h"
#include "Function.h"


// var during building, valid in a scope
class ScopeVariable : public VariableDefinition {
public:
	std::vector<Connection*> bits; // curetn vue. emtpy if var not yet assigned
public:
	// constructor
	ScopeVariable(Type* t, std::string n) : VariableDefinition(t, n) { }

	// set variable value
	void set_value(const std::vector<Connection*>& value);
	// is the variable assigned ?
	bool is_set(void) const;
};
// all the variables in a scope
class ScopeVariables : private std::vector<ScopeVariable> {
public:
	// find a variable by name
	ScopeVariable* find_by_name(std::string name);
	// declare a new local variable
	void declare_local_var(Type* var_type, std::string var_name);
	// copy a variable for a sub-scope
	void copy_var(const ScopeVariable& var_source);


	// Init from function defintion ans parameters
	void init_from_function_parameters(const Function::Definition& definition, std::vector<Connection*>& current_input);
};

