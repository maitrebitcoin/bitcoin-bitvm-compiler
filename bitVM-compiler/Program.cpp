// Implentation of the BtcContract class and its sub-classes

#include "Program.h"
#include <assert.h>
#include "Error.h"

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
BinaryOperation::BinaryOperation(Operator op, Expression* left, Expression* right)
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
		throw Error("Function name already used");
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
// init a function
void Function::init(void) {
	body->init(this);
}
// init a bloc
void CodeBloc::init(Function* parent) {
	// init parent function
	parent_function = parent;
	// init statemtents
	for (Statement* statement_i : statements)
		statement_i->init(this);
	// check statements :
	if (statements.size() == 0)
		throw Error("Empty function");
	// last statement must be a return
	Statement* last_statement = statements.back();
	if (!last_statement->is_return())
		throw Error("Last statement must be a return");
}
// init a return statmenet
void Statement_Return::init(CodeBloc* parent_bloc) {
	// intialize the expression
	expression->init(parent_bloc);
	// get return type
	Type returned_type = expression->get_type();

	// check the return type
	Function* parent_function= parent_bloc->get_parent_function();
	if (!returned_type.is_same_type( parent_function->get_return_type() ))
		throw Error("Return type mismatch");
}
// init
void BinaryOperation::init(CodeBloc* parent_bloc) {
	left_operand->init(parent_bloc);
	right_operand->init(parent_bloc);
}

// opérand Variable init
void Variable::init(CodeBloc* parent_bloc)
{
	// get the variable type by namae
	const Type* variable_type = parent_bloc->find_variable_by_name(var_name);
	if (variable_type == nullptr)
		throw Error("Variable not found", var_name.c_str());
	// set the type
	assert(variable_type->is_defined());
	var_type = *variable_type;
}
// find a variable by name
const Type* CodeBloc::find_variable_by_name(std::string var_name) const {
	// is it a fiunction parameter ?
	auto function_param = parent_function->find_parameter_by_name(var_name);
	if (function_param != nullptr)
		return function_param;

	// is it a local variable ?
	//TODO

	// not found
	return nullptr;
}
// find a parameter by name
const Type* Function::find_parameter_by_name(std::string name) const {
	for (const Parameter& param_i : definition.parameters)
	{
		if (param_i.name == name)
			return &param_i.type;
	}
	return nullptr;
}

// init program tree, phase 2 of compilation
void Program::init_and_check_program_tree(void) {
	// init  check  functions
	for (Function* f : functions)
		f->init();
	// check main function
	if (main_function() == nullptr)
		throw Error("No main function");
	// OK
}

// build a circuit that represents the program
void Program::build_circuit(class Circuit& circuit_out) {


	// TODO build the circuit
}
