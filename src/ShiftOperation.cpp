
#include "Program.h"
#include "Error.h"
#include "Circuit.h"

// init << and >>
void ShiftOperation::init(Scope& parent_scope) {
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
	if (nb_sihft < 0)
		throw Error("Shift value must be positive");
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
	std::vector<Connection*> result(op_size, nullptr);

	//  nb of bit in the output for the operand
	Connection* _0 = ctx.circuit().get_literal_values(false);
	for (int i = 0; i < op_size; i++) {
		// right shift : >>
		if (operation == Operator::op_right_shift) {
			int revese_i = op_size - 1 - i;
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
				result[i] = operand[i - nb_sihft];
		}
	}
	return result;

}

