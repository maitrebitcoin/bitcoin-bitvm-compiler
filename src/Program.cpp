// Implentation of the BtcContract class and its sub-classes

#include "Program.h"
#include <assert.h>
#include "Error.h"
#include "Circuit.h"
#include "LangageGrammar.h"
#include "BuildContext.h"




// add structures to the program as bloc before the main function
void Program::add_struct_definition(CodeBloc* bloc ) {
	// check that there are only struct definitions
	for (Statement* statement : bloc->statements)
	{
		// if the instruction is not a struct definition
		if (statement->cast_to_Statement_DeclareStruct() == nullptr)
			throw Error("Only struct definitions are allowed before functions");
	}
	// keep the bloc
	struct_definitions = bloc;

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



// find a parameter by name
const VariableDefinition* Function::find_parameter_by_name(std::string name) const {
	for (const Parameter& param_i : definition.parameters)
	{
		if (param_i.name == name)
			return &param_i;
	}
	return nullptr;
}

// init program tree, phase 2 of compilation
void Program::init_and_check_program_tree(void) {
	// init struct
	if (struct_definitions != nullptr)
	{
		for (Statement* statement_struct : struct_definitions->statements)
			statement_struct->init(*this);
	}

	// init  check  functions
	for (Function* f : functions)
		f->init(*this);
	// check main function
	if (main_function() == nullptr)
		throw Error("No main function");
	// OK
}



std::vector<Connection*> Literal::build_circuit(BuildContext& ctx) {
	std::vector<Connection*> result;

	assert(type.size_in_bit() == value_bits.size());
	// get a 0 ou 1 connexion for each bit
	for (int i = 0; i < type.size_in_bit(); i++) {
		// get the bit value
		bool b = value_bits[i];
		// get literal value as a vector of bits
		Connection* connection_to_0or1 = ctx.circuit.get_literal_values(b);
		result.push_back(connection_to_0or1);
	}

	return result;
}
// constructor
UnaryOperation::UnaryOperation(Operator op, Expression* exp) : operation(op), operand(exp) {
	// check
	assert(operand != nullptr);
}
// init
void UnaryOperation::init(Scope &parent_scope) {
	operand->init(parent_scope);
	// type must ba a basic type
	const TypeBasic* type_basic = operand->get_type().cast_to_TypeBasic();
	if (type_basic == nullptr)
		throw Error("Type must be a basic type");
	// copy basic type
	result_type = *type_basic;
}
// build the circuit for the binairy expression
std::vector<Connection*> UnaryOperation::build_circuit(BuildContext& ctx) {
	// build the  operand
	std::vector<Connection*> inputs = operand->build_circuit(ctx);

	// create the gate for the operation
	UnaryGate* gate = nullptr;
	switch (operation)
	{
	case Operator::op_not:
		// type bool only
		if (!get_type().is_bool())
			throw Error("Operator ! is reserved for bool type only");
		return build_circuit_not(ctx, inputs);
	case Operator::op_negate:
		if (get_type().is_bool())
			throw Error("Operator - is not defined for bool type");
		return build_circuit_negation(ctx, inputs);
	case Operator::op_complement:
		return build_circuit_not(ctx, inputs);
	default:
		assert(false);
		throw Error("Internal error : unexpected operator");
	}
}
// build the circuit for the not expression
std::vector<Connection*> UnaryOperation::build_circuit_not(BuildContext & ctx, std::vector<Connection*>& inputs) {

	// OUT = !A for each bit
	std::vector<Connection*> result;
	for (int i = 0; i < inputs.size(); i++) {
		std::array<Connection*, 1> input_1_bit = { inputs[i] };
		std::array<Connection*, 1> bits_result = Gate_NOT().add_to_circuit(ctx.circuit, input_1_bit);
		result.push_back(bits_result[0]);
	}
	return result;
}

// build the circuit for the negate expression
std::vector<Connection*> UnaryOperation::build_circuit_negation(BuildContext & ctx, std::vector<Connection*>& inputs) {
	std::vector<Connection*> result;
	// create a 2's complement
	std::vector<Connection*> not_inputs = build_circuit_not(ctx, inputs);
	// get 1 with the right size
	std::vector<Connection*> _1;
	for (int i = 0; i < inputs.size(); i++) {
		_1.push_back( ctx.circuit.get_literal_values( i==0 ));
	}
	// add 1
	return BinaryOperation::build_circuit_add(ctx, not_inputs, _1);

}


// build a circuit that represents the program
void Program::build_circuit(BuildContext& build_context) {

	// getk main function
	Function& fn_main = *main_function();

	// build the circuit for the main function
	fn_main.build_circuit(build_context);

	// init id for all gates and connections of all circuits
	int id_circuit = 20000; // start a 20000 for debug purposes
	build_context.visit_circuits([&](Circuit& circuit) {
		circuit.set_id(id_circuit);
		circuit.init_id_gates_and_connexions();
		id_circuit++;
	});
}
