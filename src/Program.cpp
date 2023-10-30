// Implentation of the BtcContract class and its sub-classes

#include "Program.h"
#include <assert.h>
#include "Error.h"
#include "Circuit.h"
#include "LangageGrammar.h"

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
// reorg epxresion tree to ensure operator precedence
// ex: "a + b * c" must be calculad as "a + (b * c)"
// retrun the new root of the expression
BinaryOperation* BinaryOperation::BinaryOperation::reorg_for_precedence(LangageAttributes &language) {
	// only if the 1st operand is a binary operation
	BinaryOperation* left_operand_2op = left_operand->cast_to_BinaryOperation();
	if (left_operand_2op == nullptr)
		return this; // do nothing and keep tree as it is
	// if the expression is in (), do nothing
	if (left_operand_2op->has_parentesis)
		return this; 
	// if the precedence is ok
	// ex: a*b+c
	if (left_operand_2op->has_higher_precedence(*this, language))
		return this; // do nothing and keep tree as it is
	// if the precedence is not ok
	// ex: a*b+c
	// regorganize the tree :
	//	             this
	//		  left    |        
	//		  a + b   *   c   
	// =>
	//            result
	// 	 	        |   this
	//		   a    +   b * c      
	auto a = left_operand_2op->left_operand;
	auto b = left_operand_2op->right_operand;
	auto c = this->right_operand;
	BinaryOperation* result = left_operand_2op;
	left_operand_2op->left_operand = a;
	this->left_operand             = b;
	this->right_operand            = c;
	left_operand_2op->right_operand = this;
	return result;

}
// true is the expression has a higher precedence than the othe expression
bool BinaryOperation::has_higher_precedence(const BinaryOperation& other_expression, LangageAttributes& language) const {
	// compare precedence with the other expression
	int cmp = language.compare_operator_precedence( (int)operation, (int)other_expression.operation );
	return cmp < 0;
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
	for (Statement* statement_i : statements) {
		try {
			statement_i->init(this);
		}
		catch (Error& e) {
			// add the line number
			e.line_number = statement_i->num_line;
			throw e;
		}
	}
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
	// init operands
	left_operand->init(parent_bloc);
	right_operand->init(parent_bloc);
	// left and right operand must have the same type
	if (!left_operand->get_type().is_same_type(right_operand->get_type()))
		throw Error("Type mismatch");
	result_type = left_operand->get_type();
}
// init << and >>
void ShiftOperation::init(CodeBloc* parent_bloc)  {
	// init left operands
	left_operand->init(parent_bloc);
	result_type = left_operand->get_type();

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
void TestOperation::init(CodeBloc* parent_bloc) {
	// init operands
	left_operand->init(parent_bloc);
	right_operand->init(parent_bloc);
	// left and right operand must have the same type
	if (!left_operand->get_type().is_same_type(right_operand->get_type()))
		throw Error("Type mismatch");
	result_type = Type(Type::Native::bit );

}



// init a statmenet
void Statement_DeclareVar::init(CodeBloc* parent_bloc) {
	// if variable namae already used
	const Type* variable_type = parent_bloc->find_variable_by_name(var_name);
	if (variable_type != nullptr)
		throw Error("Variable already declared : ", var_name);
	// declare the variable type
	parent_bloc->declare_local_variable(var_type, var_name);

}
// opérand Variable init
void VariableExpression::init(CodeBloc* parent_bloc) {
	// get the variable type by namae
	const Type* variable_type = parent_bloc->find_variable_by_name(var_name);
	if (variable_type == nullptr)
		throw Error("Variable not found", var_name);
	// set the type
	assert(variable_type->is_defined());
	var_type = *variable_type;
}
//  assignment statement init
void Statement_SetVar::init(CodeBloc* parent_bloc) {
	expression->init(parent_bloc);
}
// declare and set init
void Statement_DeclareAndSetVar::init(CodeBloc* parent_bloc) {
	// init set and assign
	declaration.init(parent_bloc);
	affectation.init(parent_bloc);
	// check the type
	if (!declaration.get_type().is_same_type(affectation.get_type()))
		throw Error("Type mismatch");
}
// convert the value of the literal to a vector of bits for bool type
std::vector<bool> Literal::_get_bits_value_bool( std::string str_val ) const {
	std::vector<bool> result;
	if (str_val == "true")
		result = { true };
	else if (str_val == "false")
		result = { false };
	else
		throw Error("Invalid boolean value : ", str_val);
	return result;
}
// convert the value of the literal to a vector of bits for byte type
// convert to bits in low endian (x86 format)
// result[0] is the least significant bit, result[7] is the most significant bit
std::vector<bool> Literal::_get_bits_value_int8(std::string str_val) const {
	std::vector<bool> result;
	// conveto signed integer
	int value_int8 = std::stoi(str_val);
	// value must be in 8bit signed range
	if (value_int8 > 127 || value_int8 < -128)
		throw Error("Invalid signed 8 bit value : ", str_val);

	// convert to bits in low endian (x86 format)
	for (int i = 0; i < 8; i++)
	{
		bool b= value_int8 & 1;
		result.push_back(b);
		value_int8 >>= 1;
	}
	return result;
}
// convert hex string to array of bits in low endian
std::vector<bool> Literal::hex_string_to_bits(std::string hex_string) {
	// for eah char in hex string, starting from the end
	std::vector<bool> result;
	for (int i = (int)hex_string.size() - 1; i >= 0; i--)
	{
		// get the char
		char c = hex_string[i];
		// convert to int
		int value = 0;
		if (c >= '0' && c <= '9')
			value = c - '0';
		else if (c >= 'a' && c <= 'f')
			value = c - 'a' + 10;
		else if (c >= 'A' && c <= 'F')
			value = c - 'A' + 10;
		else
			throw Error("Invalid hex string : ", hex_string);
		// convert to bits le
		for (int j = 0; j < 4; j++)
		{
			bool b = value & 1;
			result.push_back(b);
			value >>= 1;
		}
	}
	return result;
}

// lietral init
void Literal::init(CodeBloc* parent_bloc) {
	switch (type.get_native_type())
	{ 
		case Type::Native::bit:
			value_bits = _get_bits_value_bool(value_str);
			break;
		case Type::Native::int8:
			value_bits = _get_bits_value_int8(value_str);
			break;
		default:
			throw Error("Invalid literal type");
	}
}

// find a variable by name
const Type* CodeBloc::find_variable_by_name(std::string var_name) const {
	// is it a fiunction parameter ?
	auto function_param = parent_function->find_parameter_by_name(var_name);
	if (function_param != nullptr)
		return function_param;

	// is it a local variable ?
	for (const VariableDefinition& var_i : local_variables)
	{
		if (var_i.var_name == var_name)
			return &var_i.var_type;
	}

	// not found
	return nullptr;
}
// declare a local variable
void CodeBloc::declare_local_variable(Type& type, std::string name) {
	// add the variable to the list
	local_variables.push_back(VariableDefinition{ type, name });
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
class VarBuild : public VariableDefinition {
public:
	std::vector<Connection*> bits; // curetn vue. emtpy if var not yet assigned
public:
	// is the variable assigned ?
	bool is_set(void) const {
		return bits.size() > 0;
	}
	// set variable value
	void set_value(std::vector<Connection*>& value) {
		assert(value.size() == var_type.size_in_bit());
		// set bits
		bits = value;
	}
};
class KnownVar : public std::vector<VarBuild> {
public:
	// find a variable by name
	VarBuild* find_by_name(std::string name) {
		for (VarBuild& var_i : *this)
		{
			if (var_i.var_name == name)
 				return &var_i;
		}
		return nullptr;
	}
	// declare a new local variable
	void declare_local_var(Type var_type, std::string var_name) {
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
std::vector<Connection*> VariableExpression::build_circuit(BuildContext& ctx) {

	// get the variable type by name
	VarBuild* var= ctx.variables.find_by_name(var_name);
	if (var== nullptr)
		throw Error("Unknonwn variable : ", var_name);
	// if variable not set
	if (!var->is_set())
		throw Error("Uninitialized variable : ", var_name);

	// set outputs to get the value of the variable
	return var->bits;

}

std::vector<Connection*> Literal::build_circuit(BuildContext& ctx) {
	std::vector<Connection*> result;

	auto type = get_type();
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
void UnaryOperation::init(CodeBloc* parent_bloc) {
	operand->init(parent_bloc);
	result_type = operand->get_type();
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
	case Operator::op_add:
		// a+b is not bit to bit independant
		return build_circuit_add(ctx, output_left, output_right);
	case Operator::op_sub:
		// a-b is not bit to bit independant
		return build_circuit_sub(ctx, output_left, output_right);
	default:
		assert(false);
		throw Error("Internal error : unexpected operator");
	}

	// for each bit connect gate input and output
	std::vector<Connection*> result;
	int size = get_type().size_in_bit();
	assert(output_left.size() == size);
	assert(output_right.size() == size);
	for (int i= 0; i < size; i++) {
		// IN
		std::array<Connection*, 2> input_2_bit = { output_left[i], output_right[i] };
		// OUT = A OP B
		std::array<Connection*, 1> bits_result = gate->add_to_circuit(ctx.circuit, input_2_bit);
		//TODO
		//delete gate;
		result.insert(result.end(), bits_result.begin(), bits_result.end());
	}
	return result;
}
// build the circuit for the "a+b" expression
std::vector<Connection*> BinaryOperation::build_circuit_add(BuildContext& ctx,
															std::vector<Connection*>& in_a,
															std::vector<Connection*>& in_b ) {
	assert(in_a.size()== in_b.size());
	std::vector<Connection*> result;
	int size = (int)in_a.size();
	Gate_ADD  gate_add;  // add 2 birs, return carry + 1 bit 
	Gate_ADDC gate_addc; // add 2 birs + carry, return carry + 1 bit 
	// start with 1st bit addition
	std::array<Connection*, 2> input_2_bit = { in_a[0], in_b[0] };
	std::array<Connection*, 2> low_bits = gate_add.add_to_circuit(ctx.circuit, input_2_bit);
	result.push_back(   low_bits[0]);
	Connection* carry = low_bits[1];
	for (int i = 1; i < size; i++) {
		// IN
		std::array<Connection*, 3> input_3_bit = { in_a[i], in_b[i], carry };
		std::array<Connection*, 2> bits_ = gate_addc.add_to_circuit(ctx.circuit, input_3_bit);
		result.push_back(bits_[0]);
		carry	       = bits_[1];
	}
	assert( result.size() == size);
	return result;
}
// build the circuit for the "a-b" expression
std::vector<Connection*> BinaryOperation::build_circuit_sub(BuildContext& ctx,
															std::vector<Connection*>& in_a,
															std::vector<Connection*>& in_b) {
	assert(in_a.size() == in_b.size());
	// implent wita a-b = a+(-b)
	// build the circuit for -b
	std::vector<Connection*> neg_b = UnaryOperation::build_circuit_negation(ctx, in_b);
	// build the circuit for a+(-b)
	std::vector<Connection*> a_minus_b = build_circuit_add(ctx, in_a, neg_b);
	return a_minus_b;
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


// build the circuit for the return statem-ent
void Statement_Return::build_circuit( BuildContext& ctx) const {
	
	// build the expression
	std::vector<Connection*> outputs  =  expression->build_circuit(ctx);
	int nb_bit_out = (int)outputs.size();
	assert(nb_bit_out == get_type().size_in_bit());
	// connect the output of the expression to the output of the circuit
	ctx.circuit.set_output(outputs);
}



// build the circuit for the declaration statement
void Statement_DeclareVar::build_circuit(BuildContext& ctx) const {
	// if the variable is already known
	if (ctx.variables.find_by_name(var_name) != nullptr)
		throw Error("Variable already declared : ", var_name);
	// declare the variable
	ctx.variables.declare_local_var(var_type, var_name );
}
// build the circuit for the assignment statement
void Statement_SetVar::build_circuit(BuildContext& ctx) const {
	// get the variable type by name
	VarBuild* var = ctx.variables.find_by_name(var_name);
	if (var == nullptr)
		throw Error("Unknonwn variable : ", var_name);
	// check variable type
	if (!var->var_type.is_same_type( expression->get_type() ) )
		throw Error("Type mismatch : ", var_name);

	// build the R expression
	std::vector<Connection*> expression_value = expression->build_circuit(ctx);
	// connect the output of the expression to current value of the variable
	var->set_value(expression_value);
}
// build the circuit for the dérale dans set statement
void Statement_DeclareAndSetVar::build_circuit(BuildContext& ctx) const {
	// declare the variable
	declaration.build_circuit(ctx);
	// set variable value
	affectation.build_circuit(ctx);

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
		var_i.var_type	  = param_i.type;
		var_i.var_name	  = param_i.name;
		variables.push_back(var_i);
		index += var_i.var_type.size_in_bit();
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
