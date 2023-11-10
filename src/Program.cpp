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

// init
void BinaryOperation::init(Scope& parent_scope) {
	// init operands
	left_operand->init(parent_scope);
	right_operand->init(parent_scope);
	// left and right operand must have the same type
	if (!left_operand->get_type().is_same_type(right_operand->get_type()))
		throw Error("Type mismatch");
	// type must ba a basic type
	const TypeBasic* type_basic =	left_operand->get_type().cast_to_TypeBasic();
	if (type_basic == nullptr)
		throw Error("Type must be a basic type");
	// copy basic type
	result_type = *type_basic;
}
// init << and >>
void ShiftOperation::init(Scope& parent_scope)  {
	// init left operands
	left_operand->init(parent_scope);
	// type must ba a basic type
	const TypeBasic* type_basic = left_operand->get_type().cast_to_TypeBasic();
	if (type_basic == nullptr)
		throw Error("Type must be a basic type");
	// copy basic type
	result_type = *type_basic;


	// right operand must be a constant interger
	Literal* rigth_litteral = right_operand->cast_to_literal();
	if (rigth_litteral == nullptr)
		throw Error("Right operand must be a constant");
	if (!rigth_litteral->get_type().is_integer())
		throw Error("Right operand must be an integer");
	// get the shift value
	nb_sihft = rigth_litteral->get_int_value();
	if (nb_sihft<0)
		throw Error("Shift value must be positive");
}
// init '=='
void TestOperation::init(Scope& parent_scope) {
	// init operands
	left_operand->init(parent_scope);
	right_operand->init(parent_scope);
	// left and right operand must have the same type
	if (!left_operand->get_type().is_same_type(right_operand->get_type()))
		throw Error("Type mismatch");
	result_type = TypeBasic( Type::Native::bit );
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

// build the circuit for the shift expression
std::vector<Connection*> ShiftOperation::build_circuit(BuildContext& ctx) {
	// build the L operands
	std::vector<Connection*> operand = left_operand->build_circuit(ctx);
	// if 0 sihft : return operand with modificatiton
	if (nb_sihft == 0)
		return operand;
	assert(nb_sihft > 0);
	int op_size = (int)operand.size();

	//int nb_bit_out = op_size - nb_sihft;
	// init result with nullptr 
	std::vector<Connection*> result(op_size,  nullptr);

	//  nb of bit in the output for the operand
	Connection* _0 = ctx.circuit.get_literal_values(false);
	for (int i = 0; i< op_size; i++) {
		// right shift : >>
		if (operation == Operator::op_right_shift ) {
			int revese_i = op_size-1- i;
			if (i < nb_sihft)
				result[revese_i] = _0;
			else
				result[revese_i] = operand[revese_i + nb_sihft];
		}
		// left shift : <<
		else {
			if (i < nb_sihft)
				result[i] = _0;
			else
				result[i] = operand[i-nb_sihft];
		}
	}
	return result;

}
// build the circuit for the "== != < > <= >= " expressions
std::vector<Connection*> TestOperation::build_circuit(BuildContext& ctx) {
	// build the L& R operands
	std::vector<Connection*> op_left  = left_operand->build_circuit(ctx);
	std::vector<Connection*> op_right = right_operand->build_circuit(ctx);

	// ==
	if (operation == Operator::op_test_equal) {
		return build_circuit_equal(ctx, op_left, op_right);
	}
	// !=
	if (operation == Operator::op_test_not_equal) {
		// build the circuit for ==
		std::vector<Connection*> equal = build_circuit_equal(ctx, op_left, op_right);
		// negate the result
		std::array<Connection*, 1> input_1_bit = { equal[0] };
		std::array<Connection*, 1> bit_not_equal = Gate_NOT().add_to_circuit(ctx.circuit, input_1_bit);
		std::vector<Connection*> result(1, nullptr);
		result[0] = bit_not_equal[0];
		return result;
	}
	// >
	if (operation == Operator::op_test_greater) {
		// build the circuit for the "b-a" expression
		std::vector<Connection*> b_minus_a = build_circuit_sub(ctx, op_right, op_left );
		// get the bit sign of b-a : true if (b-a)<0 => a > b
		std::vector<Connection*> b_minus_a_is_negative(1, b_minus_a[op_right.size()-1]);
		return b_minus_a_is_negative;
	}
	// <=
	if (operation == Operator::op_test_lower_or_equal) {
		// build the circuit for the "a-b" expression
		std::vector<Connection*> b_minus_a = build_circuit_sub(ctx, op_right, op_left);
		// get the bit sign of b-a : true if (b-a)<0 => a > b
		Connection* b_minus_a_is_negative = b_minus_a[op_right.size() - 1];
		// a<=b = !(a>b) 
		std::array<Connection*, 1> input_1_bit = { b_minus_a_is_negative };
		std::array<Connection*, 1> bit_result = Gate_NOT().add_to_circuit(ctx.circuit, input_1_bit);
		std::vector<Connection*> result(1, bit_result[0]);
		return result;
	}
	// <
	if (operation == Operator::op_test_lower) {
		// build the circuit for the "a-b" expression
		std::vector<Connection*> a_minus_b = build_circuit_sub(ctx, op_left, op_right);
		// get the bit sign of b-a : true if (b-a)<0 => a > b
		std::vector<Connection*> a_minus_b_is_negative(1, a_minus_b[op_right.size() - 1]);
		return a_minus_b_is_negative;
	}
	// >=
	if (operation == Operator::op_test_greater_or_equal) {
		// build the circuit for the "a-b" expression
		std::vector<Connection*> a_minus_b = build_circuit_sub(ctx, op_left, op_right);
		// get the bit sign of b-a : true if (b-a)<0 => a > b
		Connection* a_minus_b_is_negative = a_minus_b[op_right.size() - 1];
		// a<=b = !(a>b) 
			std::array<Connection*, 1> input_1_bit = { a_minus_b_is_negative };
		std::array<Connection*, 1> bit_result = Gate_NOT().add_to_circuit(ctx.circuit, input_1_bit);
		std::vector<Connection*> result(1, bit_result[0]);
		return result;
	}




	// TODO, implement other operators
	// not an available operators
	throw(Error("Internal error : unimplemened operator"));

}
// build the circuit for the "==" expression
std::vector<Connection*>  TestOperation::build_circuit_equal(BuildContext & ctx,
															std::vector<Connection*>& op_left,
															std::vector<Connection*>& op_right)
{
	int nb_bit = (int)op_left.size();

	// build the circuit for the test
	std::vector<Connection*> result(1, nullptr);

	// build a array with each bit comparison
	std::vector<Connection*> bit_equal_array(nb_bit);
	for (int i = 0; i < nb_bit; i++)
	{
		std::array<Connection*, 2> input_2_bit = { op_left[i], op_right[i] };
		std::array<Connection*, 1> bit_equal = Gate_XNOR().add_to_circuit(ctx.circuit, input_2_bit);
		bit_equal_array[i] = bit_equal[0];
	}
	// easy case: compare 2 bools
	if (nb_bit == 1) //op_left.size() == 1))
	{
		result[0] = bit_equal_array[0];
		return result;
	}
	// nb_bit must be a power of 2
	if ((nb_bit & (nb_bit - 1)) != 0)
		throw(Error("Internal error : operand size must be a power of 2")); // TODO, implement array with nb_bit not power of 2

	// cascade test 2 by 2 with AND operator 
	while (bit_equal_array.size() > 1) {
		std::vector<Connection*> bit_equal_array_next(bit_equal_array.size()/2);
		for (int i = 0; i < bit_equal_array_next.size(); i++)
		{
			std::array<Connection*, 2> input_2_bit = { bit_equal_array[i*2], bit_equal_array[i*2+1] };
			std::array<Connection*, 1> _equal = Gate_AND().add_to_circuit(ctx.circuit, input_2_bit);
			bit_equal_array_next[i] = _equal[0];
		}
		// nesxt cascade
		bit_equal_array = bit_equal_array_next;
	}
	assert(bit_equal_array.size()== 1);
	result[0] = bit_equal_array[0];
	return result;
}


// build a circuit that represents the fuidl
void Function::build_circuit(class Circuit& circuit) {
	// declare inputs
	int nb_bits_in = size_in_bit_input();
	InterfaceInputsMap* input_map = getInterfaceInputsMap();
	circuit.set_inputs(nb_bits_in, input_map);
	// get input
	std::vector<Connection*> current_input = circuit.getInputs();
	
	// init known variables
	KnownVar variables;
	int index = 0;
	for (const Parameter& param_i : definition.parameters)
	{
		VarBuild var_i(param_i.type, param_i.name);
		int size = param_i.type->size_in_bit();
		var_i.bits.assign(  current_input.begin() + index, 
							current_input.begin() + index + size) ;
		variables.push_back(var_i);
		index += var_i.type->size_in_bit();
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
	
	// init id for alla gates ands connections
	circuit_to_build.init_id();
}
