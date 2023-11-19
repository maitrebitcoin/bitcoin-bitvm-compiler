


#include <map>
#include "Function.h"
#include "CodeBloc.h"
#include "Error.h"
#include "Circuit.h"
#include "BuildContext.h"

// function constructor
Function::Function(Definition* def, CodeBloc* fn_body)
	: definition(*def)
	, body(fn_body) {
}
Function::~Function() {
	if (function_scope!=nullptr)
		delete function_scope;
}

// init a function
void Function::init(Scope& parent_scope) {

	// create a new scope for the function
	function_scope = parent_scope.create_child_scope();
	function_scope->parent_function = this;

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
	
	// init bodu
	body->init(*function_scope);
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


// find a parameter by name
const VariableDefinition* Function::find_parameter_by_name(std::string name) const {
	for (const Parameter& param_i : definition.parameters)
	{
		if (param_i.name == name)
			return &param_i;
	}
	return nullptr;
}


// build a circuit that represents the fuidl
void Function::build_circuit(BuildContext &ctx) {
	// declare inputs
	int nb_bits_in = size_in_bit_input();
	InterfaceInputsMap* input_map = getInterfaceInputsMap();
	ctx.circuit.set_circuit_inputs(nb_bits_in, input_map);
	// get input
	std::vector<Connection*> current_input = ctx.circuit.getInputs();

	// init scope variables in the contente
	ctx.variables.init_from_function_parameters(definition, current_input);

	// build the body
	try {
		body->build_circuit(ctx);
	}
	catch (Error& e) {
		//add fucntion name info to the error
		e.function_name = definition.name;
		throw e;
	}

}

class FunctionInputsMap : public InterfaceInputsMap
{
public:
	std::map<std::string, InterfaceInputsMap::Info> map;
public:
	// get a parameter info by name
	virtual Info find_info_by_name(std::string name) const override {
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