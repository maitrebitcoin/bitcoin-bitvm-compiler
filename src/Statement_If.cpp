
#include <assert.h>
#include "Program.h"
#include "Error.h"
#include "Gate.h"
#include "Circuit.h"


// constructor
Statement_If::Statement_If(int line, Expression* condidtion, CodeBloc* if_true, CodeBloc* if_false) 
	: Statement(line)
	, expression(condidtion) 
	, bloc_if_true(if_true)
	, bloc_if_false(if_false)
{
	assert(bloc_if_true != nullptr || bloc_if_false != nullptr  );
}


// init if statmenet
void Statement_If::init(Scope& parent_scope) {
	// intialize the expression
	expression->init(parent_scope);

	// get expression type
	const Type& expresrion_type = expression->get_type();
	// must be a boolean
	if (!expresrion_type.is_bool())
		throw Error("If condition must be boolean");

	// both blocs must exists
	if (bloc_if_true == nullptr)
		throw Error("Internal error : if (true) bloc is null");
	if (bloc_if_false == nullptr)
		throw Error("Internal error : if (false) bloc is null");

	// init blocs of code
	bloc_if_true->init(parent_scope);
	bloc_if_false->init(parent_scope);
}

// add ircuits inputs requirements from a bloc
void Statement_If::_add_all_bloc_input(BuildContext& ctx, CodeBloc* code_bloc, Gate_IF* gate  ) const {
	
	// visite all variables used in the bloc
	code_bloc->visit_all_used_variable([&](IVariableToConnexion& var2cnx) {
		// get var connexion
		std::vector<Connection *> var_inputs = var2cnx.get_var_connexion(ctx);
		// copy all inputs
		for (Connection* input : var_inputs)
			gate->add_input(input);
		});

}

// build the circuit for the return statement
void Statement_If::build_circuit(BuildContext& ctx) const
{
	// build the condition expression
	std::vector<Connection*> expression_value = expression->build_circuit(ctx);
	assert(expression_value.size() == 1);

	// both blocs must exists
	if (bloc_if_true == nullptr)
		throw Error("Internal error : if (true) bloc is null");
	if (bloc_if_false == nullptr)
		throw Error("Internal error : if (false) bloc is null");

	// creat 2 circuits :
	// true case
	Circuit& circuit_if_true = ctx.get_new_sub_circuit();
	BuildContext ctx_if_true(circuit_if_true, ctx.variables);
	bloc_if_true->build_circuit(ctx_if_true);
	// false case
	Circuit& circuit_if_false = ctx.get_new_sub_circuit();;
	BuildContext ctx_if_false( circuit_if_false, ctx.variables);
	bloc_if_false->build_circuit(ctx_if_false);

	// créate a new gate IF
	Gate_IF * gate_if = new Gate_IF(&circuit_if_true, &circuit_if_false);
	// add the gate to the circuit
	std::array<Connection*, 1> input_1_bit = { expression_value[0] };
	std::array<Connection*, 0> void_result =
		gate_if->add_to_circuit(ctx.circuit, input_1_bit);

	// set the input of the circuit : union of the 2 circuits inputs requirements
	_add_all_bloc_input(ctx, bloc_if_true, gate_if);
	_add_all_bloc_input(ctx, bloc_if_false, gate_if);

	// tell the ciruit outupt sizz, without real connexion.
	// the real output will be from or circuit_if_true ou ctx_if_false
	NbBit nb_bit_out = circuit_if_true.nb_bits_output();
	// the 2 circuits must have the same output size
	if (nb_bit_out != circuit_if_false.nb_bits_output())
		throw Error("If : 2 Return statements with different size");

	// connect the output of the expression to the output of the circuit
	ctx.circuit.set_output_size_child(nb_bit_out);
}


