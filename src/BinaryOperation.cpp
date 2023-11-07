
#include "Program.h"
#include <assert.h>
#include "Error.h"
#include "Circuit.h"
#include "LangageGrammar.h"
#include "BuildContext.h"


// constructor for BinaryOperation
BinaryOperation::BinaryOperation(Operator op, Expression* left, Expression* right)
	: operation(op), left_operand(left), right_operand(right) {
	//	TODO
	// build the expression for debug purposes
}
// reorg epxresion tree to ensure operator precedence
// ex: "a + b * c" must be calculad as "a + (b * c)"
// retrun the new root of the expression
BinaryOperation* BinaryOperation::BinaryOperation::reorg_for_precedence(LangageAttributes& language) {
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
	this->left_operand = b;
	this->right_operand = c;
	left_operand_2op->right_operand = this;
	return result;

}
// true is the expression has a higher precedence than the othe expression
bool BinaryOperation::has_higher_precedence(const BinaryOperation& other_expression, LangageAttributes& language) const {
	// compare precedence with the other expression
	int cmp = language.compare_operator_precedence((int)operation, (int)other_expression.operation);
	return cmp < 0;
}



// build the circuit for the binairy expression
std::vector<Connection*> BinaryOperation::build_circuit(BuildContext& ctx) {

	// build the L& R operands
	std::vector<Connection*> output_left = left_operand->build_circuit(ctx);
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
	for (int i = 0; i < size; i++) {
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
	std::vector<Connection*>& in_b) {
	assert(in_a.size() == in_b.size());
	std::vector<Connection*> result;
	int size = (int)in_a.size();
	Gate_ADD  gate_add;  // add 2 birs, return carry + 1 bit 
	Gate_ADDC gate_addc; // add 2 birs + carry, return carry + 1 bit 
	// start with 1st bit addition
	std::array<Connection*, 2> input_2_bit = { in_a[0], in_b[0] };
	std::array<Connection*, 2> low_bits = gate_add.add_to_circuit(ctx.circuit, input_2_bit);
	result.push_back(low_bits[0]);
	Connection* carry = low_bits[1];
	for (int i = 1; i < size; i++) {
		// IN
		std::array<Connection*, 3> input_3_bit = { in_a[i], in_b[i], carry };
		std::array<Connection*, 2> bits_ = gate_addc.add_to_circuit(ctx.circuit, input_3_bit);
		result.push_back(bits_[0]);
		carry = bits_[1];
	}
	assert(result.size() == size);
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
