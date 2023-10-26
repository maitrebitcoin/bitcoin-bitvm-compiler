// Implentation of the BtcContract class and its sub-classes

#include "Program.h"
#include <assert.h>
#include "Error.h"
#include "Circuit.h"

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
	Type returned_type = get_type();

	// check the return type
	Function* parent_function= parent_bloc->get_parent_function();
	if (!returned_type.is_same_type( parent_function->get_return_type() ))
		throw Error("Return type mismatch");
}
// init
void BinaryOperation::init(CodeBloc* parent_bloc) {
	left_operand->init(parent_bloc);
	right_operand->init(parent_bloc);
	// left and right operand must have the same type
	if (!left_operand->get_type().is_same_type(right_operand->get_type()))
		throw Error("Type mismatch");
	result_type = left_operand->get_type();

}

// opérand Variable init
void Variable::init(CodeBloc* parent_bloc)
{
	// get the variable type by namae
	const Type* variable_type = parent_bloc->find_variable_by_name(var_name);
	if (variable_type == nullptr)
		throw Error("Variable not found", var_name);
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
// get the return statement of the bloc
Statement_Return* CodeBloc::get_return_statement(void) const {
	// always last in bloc
	Statement* last_statement = statements.back();
	return dynamic_cast<Statement_Return*>(last_statement);
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

// var during building
class VarBuild {
public:
	Type type; // type of the variable
	std::string name;
	std::vector<Connection*> bits; // curetn vue. emtpy if var not yet assigned
};
class KnownVar : public std::vector<VarBuild> {
public:
	// find a variable by name
	VarBuild* find_by_name(std::string name) {
		for (VarBuild& var_i : *this)
		{
			if (var_i.name == name)
				return &var_i;
		}
		return nullptr;
	}
	// declare a new local variable
	void declareLocalVar(Type var_type, std::string var_name) {
		// TODO
		VarBuild new_var{ var_type, var_name };
		push_back(new_var);
	}
};


// context for building the circuit
class BuildContext {


public:
	Circuit& circuit; // the circuit to build
	KnownVar& variables; // current known variables
//	std::vector<Connection*> inputs;  // input bits 
};


// build the circuit for the  expression
std::vector<Connection*> Variable::build_circuit(BuildContext& ctx) {

	// get the variable type by name
	VarBuild* var= ctx.variables.find_by_name(var_name);
	if (var== nullptr)
		throw Error("Unknonwn variable : ", var_name);
	// if var not set
	if (var->bits.size() == 0)
		throw Error("Uninitialized variable : ", var_name);

	// set outputs to get the value of the variable
	return var->bits;

}
std::vector<Connection*> Literal::build_circuit(BuildContext& ctx) {
	assert(false); // TODO
	std::vector<Connection*> result;
	return result;
}
// constructor
UnaryOperation::UnaryOperation(Operator op, Expression* exp) : operation(op), operand(exp) {
	// check
	assert(operand != nullptr);
}
// init
void UnaryOperation::init(CodeBloc* parent_bloc) {
	operand->init(parent_bloc);
	result_type = operand->get_type();
}
// build the circuit for the binairy expression
std::vector<Connection*> UnaryOperation::build_circuit(BuildContext& ctx) {
	// build the  operand
	std::vector<Connection*> output_ = operand->build_circuit(ctx);

	// create the gate for the operation
	UnaryGate* gate = nullptr;
	switch (operation)
	{
	case  Operator::op_not:
		gate = new Gate_NOT();
		break;
	default:
		assert(false);
		throw Error("Internal error : unexpected operator");
	}

	// IN
	std::array<Connection*, 1> input_1_bit = { output_[0] };
	// OUT = A 
	std::array<Connection*, 1> bits_result = gate->add_to_circuit(ctx.circuit, input_1_bit);
	//TODO
	//delete gate;
	std::vector<Connection*> result;
	result.assign(bits_result.begin(), bits_result.end());
	return result;
}

// build the circuit for the binairy expression
std::vector<Connection*> BinaryOperation::build_circuit(BuildContext& ctx) {

	// build the L& R operands
	std::vector<Connection*> output_left  = left_operand->build_circuit(ctx);
	std::vector<Connection*> output_right = right_operand->build_circuit(ctx);

	// create the gate for the operation
	BinaryGate* gate = nullptr;
	switch (operation)
	{
	case  Operator::op_and:
		gate = new Gate_AND();
		break;
	case Operator::op_or:
		gate = new Gate_OR();
		break;
	case Operator::op_xor:
		gate = new Gate_XOR();
		break;
	default:
		assert(false);
		throw Error("Internal error : unexpected operator");
	}

	// IN
	std::array<Connection*, 2> input_2_bit = { output_left[0], output_right[0] };
	// OUT = A OP B
	std::array<Connection*, 1> bits_result = gate->add_to_circuit(ctx.circuit, input_2_bit);
	//TODO
	//delete gate;

	std::vector<Connection*> result;
	result.assign(bits_result.begin(), bits_result.end());
	return result;
}


// build the circuit for the return statement
void Statement_Return::build_circuit( BuildContext& ctx) const {
	
	// build the expression
	std::vector<Connection*> outputs  =  expression->build_circuit(ctx);
	int nb_bit_out = (int)outputs.size();
	assert(nb_bit_out == get_type().size_in_bit());
	// connect the output of the expression to the output of the circuit
	ctx.circuit.add_output(outputs);
}
// build the circuit for the declaration statement
void Statement_DeclareVar::build_circuit(BuildContext& ctx) const {
	// if the variable is already known
	if (ctx.variables.find_by_name(var_name) != nullptr)
		throw Error("Variable already declared : ", var_name);
	// declare the variable
	ctx.variables.declareLocalVar(var_type, var_name );
}


// build a circuit that represents the fuidl
void Function::build_circuit(class Circuit& circuit) {
	// declare inputs
	int nb_bits_in = size_in_bit_input();
	circuit.set_inputs(nb_bits_in);
	// get input
	std::vector<Connection*> current_input = circuit.getInputs();
	
	// init known variables
	KnownVar variables;
	int index = 0;
	for (const Parameter& param_i : definition.parameters)
	{
		VarBuild var_i;
		int size = param_i.type.size_in_bit();
		var_i.bits.assign(  current_input.begin() + index, 
							current_input.begin() + index + size) ;
		var_i.type		  = param_i.type;
		var_i.name		  = param_i.name;
		variables.push_back(var_i);
		index += var_i.type.size_in_bit();
	}
	
	// create context
	BuildContext ctx{ circuit, variables };

	// build the body
	for (int i=0;i< body->statements.size()-1;i++) {
		Statement* statement = body->statements[i];
		try {
			statement->build_circuit(ctx);
		}
		catch (Error& e) {
			//add info to the error
			e.line_number   = statement->num_line;
			e.function_name = definition.name;
			throw e;
		}
	}
	// las statement : return 
	Statement_Return* return_statement =  body->get_return_statement();
	return_statement->build_circuit(ctx);
}

// build a circuit that represents the program
void Program::build_circuit(class Circuit& circuit_to_build) {

	// getk main function
	Function& fn_main = *main_function();

	// build the circuit for the main function
	fn_main.build_circuit(circuit_to_build);
	

}
