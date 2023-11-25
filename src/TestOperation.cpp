
#include <array>
#include "Program.h"
#include "Error.h"
#include "Circuit.h"

// init '=='
void TestOperation::init(Scope& parent_scope) {
	// init operands
	left_operand->init(parent_scope);
	right_operand->init(parent_scope);
	// left and right operand must have the same type
	if (!left_operand->get_type().is_same_type(right_operand->get_type()))
		throw Error("Type mismatch");
	result_type = TypeBasic(Type::Native::bit);
}


// build the circuit for the "== != < > <= >= " expressions
std::vector<Connection*> TestOperation::build_circuit(BuildContext& ctx) {
	// build the L& R operands
	std::vector<Connection*> op_left = left_operand->build_circuit(ctx);
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
		std::array<Connection*, 1> bit_not_equal = Gate_NOT().add_to_circuit(ctx.circuit(), input_1_bit);
		std::vector<Connection*> result(1, nullptr);
		result[0] = bit_not_equal[0];
		return result;
	}
	// >
	if (operation == Operator::op_test_greater) {
		// build the circuit for the "b-a" expression
		std::vector<Connection*> b_minus_a = build_circuit_sub(ctx, op_right, op_left);
		// get the bit sign of b-a : true if (b-a)<0 => a > b
		std::vector<Connection*> b_minus_a_is_negative(1, b_minus_a[op_right.size() - 1]);
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
		std::array<Connection*, 1> bit_result = Gate_NOT().add_to_circuit(ctx.circuit(), input_1_bit);
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
		std::array<Connection*, 1> bit_result = Gate_NOT().add_to_circuit(ctx.circuit(), input_1_bit);
		std::vector<Connection*> result(1, bit_result[0]);
		return result;
	}




	// TODO, implement other operators
	// not an available operators
	throw(Error("Internal error : unimplemened operator"));

}
// build the circuit for the "==" expression
std::vector<Connection*>  TestOperation::build_circuit_equal(BuildContext& ctx,
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
		std::array<Connection*, 1> bit_equal = Gate_XNOR().add_to_circuit(ctx.circuit(), input_2_bit);
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
		std::vector<Connection*> bit_equal_array_next(bit_equal_array.size() / 2);
		for (int i = 0; i < bit_equal_array_next.size(); i++)
		{
			std::array<Connection*, 2> input_2_bit = { bit_equal_array[i * 2], bit_equal_array[i * 2 + 1] };
			std::array<Connection*, 1> _equal = Gate_AND().add_to_circuit(ctx.circuit(), input_2_bit);
			bit_equal_array_next[i] = _equal[0];
		}
		// nesxt cascade
		bit_equal_array = bit_equal_array_next;
	}
	assert(bit_equal_array.size() == 1);
	result[0] = bit_equal_array[0];
	return result;
}
