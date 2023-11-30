// variables during cirtuit building.

#include "ScopeVariable.h"

// set variable value
void ScopeVariable::set_value(const std::vector<Connection*>& value) {
	assert(value.size() == type->size_in_bit());
	// set bits
	bits = value;
}
// is the variable assigned ?
bool ScopeVariable::is_set(void) const {
	return bits.size() > 0;
}


// find a variable by name
ScopeVariable* ScopeVariables::find_by_name(std::string name) {
	for (ScopeVariable* var_i : *this)
	{
		if (var_i->name == name)
			return var_i;
	}
	return nullptr;
}
// Init from function defintion ans parameters
void ScopeVariables::init_from_function_parameters(
	const Function::Definition& definition,
	std::vector<Connection*>& current_input)
{

	// init known variables
	int index = 0;
	for (const Function::Parameter& param_i : definition.parameters)
	{
		ScopeVariable* var_i = new ScopeVariable(param_i.type, param_i.name);
		int size = param_i.type->size_in_bit();
		var_i->bits.assign(current_input.begin() + index,
			current_input.begin() + index + size);
		this->push_back(var_i);
		index += var_i->type->size_in_bit();
	}
}

// declare a new local variable
void ScopeVariables::declare_local_var(Type* var_type, std::string var_name) {

	ScopeVariable* new_var = new ScopeVariable( var_type, var_name );
	push_back(new_var);
}

// visit all variables
void ScopeVariables::visit_all_variables(std::function<void(const ScopeVariable&)> visitor) const {
	for (auto var_i : *this)
		visitor(*var_i);

}

// copy a variable for a sub-scope a retrun the copy
ScopeVariable* ScopeVariables::copy_var(const ScopeVariable& var_source)
{
	// deep copy : if the var is modified in the sub-scope, the parent scope var is not modified
	ScopeVariable* new_var = new ScopeVariable(var_source.type, var_source.name );
	// if the source var ia assigned, copy source bits to the new variable
	if (var_source.is_set())
		new_var->set_value(var_source.bits);

	push_back(new_var);
	return new_var;

}

