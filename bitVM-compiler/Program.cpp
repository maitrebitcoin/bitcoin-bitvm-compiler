// Implentation of the BtcContract class and its sub-classes

#include "Program.h"
#include <assert.h>


// return the size in bits of a type
int Type::size_in_bit(void) const
{
	switch (native_type)
	{
	case Type::Native::undefined:
		assert(false);
		return 0;
	case Type::Native::bit:    return 1;
	case Type::Native::int8:   return 8;
	case Type::Native::uint8:  return 8;
	case Type::Native::int64:  return 64;
	case Type::Native::uint64: return 64;
	case Type::Native::uint256:return 256;
	default:
		assert(false);
		return 0;
	}
}

// return the number of bits needed for the input parameters
int Function::size_in_bit_input(void) const
{
	// addd the size of each parameter
	int nb_bit = 0;
	for (Parameter param_i : definition.parameters)
		nb_bit += param_i.type.size_in_bit();
	return nb_bit;
}
// return the number of bits needed to store the return value
int Function::size_in_bit_output(void) const {
	// size of the return type
	return definition.return_type.size_in_bit();
}

// Function Definition constructor
Function::Definition::Definition(Type type, std::string function_name, Function::AllParameter* all_params)
	: return_type(type)
	, name(function_name)
{
	assert(function_name.size() > 0);
	// copy the parameters
	for (Parameter param_i : all_params->parameters)
		parameters.push_back(param_i);
}

// constructor for BinaryOperation
BinaryOperation::BinaryOperation(Operator op, Operand* left, Operand* right)
	: operation(op), left_operand(left), right_operand(right) {
	//	TODO
	// build the expression for debug purposes
}

// function constructor
Function::Function(Definition* def, CodeBloc* fn_body)
	: definition(*def) 
	, body(fn_body) {
}


// add a function to the program
void Program::add_function(Function* f) {
	// si the function name is not already used
	if (find_function_by_name(f->get_name()))
	{
		throw "Function name already used";
	}
	// add the functions vector
	functions.push_back(f);
}
// get a function by name
Function* Program::find_function_by_name(std::string name) const  {
	for (Function* f : functions)
	{
		if (f->get_name() == name)
			return f;
	}
	return nullptr;
}
// get main function
Function* Program::main_function(void) const {
	return find_function_by_name("main");
}

// build a circuit that represents the program
void Program::build_circuit(class Circuit& circuit_out) {
	// get main function
	Function* fn_main = main_function();
	if (fn_main==nullptr)
		throw "No main function";
	// TODO build the circuit
}
