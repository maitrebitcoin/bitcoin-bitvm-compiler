// variables during cirtuit building.

#include "ScopeVariable.h"

// set variable value
void ScopeVariable::set_value(std::vector<Connection*>& value) {
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
	for (ScopeVariable& var_i : *this)
	{
		if (var_i.name == name)
			return &var_i;
	}
	return nullptr;
}
// declare a new local variable
void ScopeVariables::declare_local_var(Type* var_type, std::string var_name) {

	ScopeVariable new_var{ var_type, var_name };
	push_back(new_var);
}

