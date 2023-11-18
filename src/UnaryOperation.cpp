
#include "Program.h"
#include "Error.h"
#include "Gate.h"
#include "Circuit.h"


// constructor
UnaryOperation::UnaryOperation(Operator op, Expression* exp) : operation(op), operand(exp) {
	// check
	assert(operand != nullptr);
}
// init
void UnaryOperation::init(Scope& parent_scope) {
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
std::vector<Connection*> UnaryOperation::build_circuit_not(BuildContext& ctx, std::vector<Connection*>& inputs) {

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
std::vector<Connection*> UnaryOperation::build_circuit_negation(BuildContext& ctx, std::vector<Connection*>& inputs) {
	std::vector<Connection*> result;
	// create a 2's complement
	std::vector<Connection*> not_inputs = build_circuit_not(ctx, inputs);
	// get 1 with the right size
	std::vector<Connection*> _1;
	for (int i = 0; i < inputs.size(); i++) {
		_1.push_back(ctx.circuit.get_literal_values(i == 0));
	}
	// add 1
	return BinaryOperation::build_circuit_add(ctx, not_inputs, _1);

}

