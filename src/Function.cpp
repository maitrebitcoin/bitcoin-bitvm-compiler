
#include "Function.h"
#include "CodeBloc.h"
#include "Error.h"

// function constructor
Function::Function(Definition* def, CodeBloc* fn_body)
	: definition(*def)
	, body(fn_body) {
}
Function::~Function() {

}

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


#include <map>
class FunctionInputsMap : public InterfaceInputsMap
{
public:
	std::map<std::string, InterfaceInputsMap::Info> map;
public:
	// get a parameter info by name
	virtual Info find_by_name(std::string name) const override {
		InterfaceInputsMap::Info param_info;
		auto it = map.find(name);
		if (it == map.end()) {
			param_info.found = false;
			return param_info;
		}
		else {
			return it->second; 
		}
	}
};
// get a interface n InterfaceInputsMap 
InterfaceInputsMap* Function::getInterfaceInputsMap(void) const
{
	FunctionInputsMap * new_input_map = new FunctionInputsMap();
	// init map
	int offset_in_bit = 0;
	for (Parameter param_i : definition.parameters) {
		// int param info in map
		InterfaceInputsMap::Info param_info;
		param_info.found = true;
		param_info.type = param_i.type->get_copy();
		param_info.offset_in_bit = offset_in_bit;
		new_input_map->map[param_i.name] = param_info;
		// update offset
		offset_in_bit += param_i.type->size_in_bit();
	}
	return new_input_map;
}