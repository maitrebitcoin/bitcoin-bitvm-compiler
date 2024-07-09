
#include "Scope.h"
#include "Function.h"

// init scope for a code bloc
void Scope::init_CodeBloc(Scope& parent_scp)
{
	// init parent function in the scope
	parent_function = parent_scp.get_parent_function();
	parent_for_loop = parent_scp.parent_for_loop;
	parent_scope = &parent_scp;
}


// create a new child scope
Scope* Scope::create_child_scope(void) {
	// create a new empty scope
	Scope* child_scope = new Scope();

	// init new scope
	child_scope->init_CodeBloc(*this);
	return child_scope;
}

// declare a local variable
void Scope::declare_local_variable(const VariableDefinition& def) {
	// add the variable to the list
	local_variables.push_back(VariableDefinition{ def });
}

// find a variable by name
const VariableDefinition* Scope::find_variable_by_name(std::string var_name) const {
	// is it a fiunction parameter ?
	if (parent_function != nullptr) // not in the global scope
	{
		const VariableDefinition* function_param = parent_function->find_parameter_by_name(var_name);
		if (function_param != nullptr)
			return function_param;
	}
	// is it a local variable ?
	for (const VariableDefinition& var_i : local_variables)
	{
		if (var_i.name == var_name)
			return &var_i;
	}
	// louk in the parent scope
	if (parent_scope != nullptr)
		return parent_scope->find_variable_by_name(var_name);

	// not found
	return nullptr;
}

// find a struct type by name
const TypeStruct* Scope::find_struct_by_name(std::string name) const {
	for (const TypeStruct& struct_i : know_types) {
		if (struct_i.name == name)
			return &struct_i;
	}
	// search the parent
	if (parent_scope != nullptr)
		return parent_scope->find_struct_by_name(name);
	
	// look in the parent scope
	if (parent_scope != nullptr)
		return parent_scope->find_struct_by_name(name);
	
	// not found
	return nullptr;
}

// find a function by name
const Function* Scope::find_function_by_name(std::string name) const
{
	// look in the known functions
	for (const Function* func_i : known_functions)
	{
		if (func_i->get_name() == name)
			return func_i;
	}
	// ask the parent scope
	if (parent_scope != nullptr)
		return parent_scope->find_function_by_name(name);


	// unknown function
	return nullptr;
}
