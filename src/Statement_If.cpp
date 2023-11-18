
#include <assert.h>
#include "Program.h"
#include "Error.h"
#include "Gate.h"


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
	std::array<Connection*, 0> void_result = gate_if->add_to_circuit(ctx.circuit, input_1_bit);


}


