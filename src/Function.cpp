
#include "Function.h"
#include "CodeBloc.h"
#include "Error.h"

// init a function
void Function::init(Scope& parent_scope) {

	// init parameters typer
	for (Parameter& param : definition.parameters) {
		// if the type is just a name, get the real type
		if (!param.type->is_complete()) {
			const TypeStruct* real_type = parent_scope.find_struct_by_name(param.type->get_name());
			if (real_type == nullptr)
				throw Error("Unknown type : ", param.type->get_name());
			param.type = const_cast<TypeStruct*>(real_type);
		}
	}
	
	body->init(parent_scope, this);
}


// return the number of bits needed for the input parameters
int Function::size_in_bit_input(void) const
{
	// addd the size of each parameter
	int nb_bit = 0;
	for (Parameter param_i : definition.parameters)
		nb_bit += param_i.type->size_in_bit();
	return nb_bit;
}
// return the number of bits needed to store the return value
int Function::size_in_bit_output(void) const {
	// size of the return type
	return definition.return_type->size_in_bit();
}

// Function Definition constructor
Function::Definition::Definition(Type* type, std::string function_name, Function::AllParameter* all_params)
	: return_type(type)
	, name(function_name)
{
	assert(function_name.size() > 0);
	// copy the parameters
	for (Parameter param_i : all_params->parameters)
		parameters.push_back(param_i);
}