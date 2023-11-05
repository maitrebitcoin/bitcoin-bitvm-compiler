
#include "Scope.h"
#include "Function.h"

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
	
	// not found
	return nullptr;
}