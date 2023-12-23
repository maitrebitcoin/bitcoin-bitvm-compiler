
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
// visit all expression ueed in the statement
void Statement_If::visit_Expression(std::function<void(Expression& expr)> visitor) const  {
	visitor(*expression); 
	if (bloc_if_true != nullptr)
		bloc_if_true->visit_Expression(visitor);
	if (bloc_if_false != nullptr)
		bloc_if_false->visit_Expression(visitor);
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

	// init blocs of code
	if (parent_scope.is_in_for_loop())	{
		// insida a for loop, special rules : brek is ok, and the else bloc do not need a return
		bloc_if_true->init_ex( parent_scope, CodeBloc::InitOption::return_or_break_must_be_present);
		if (bloc_if_false) bloc_if_false->init_ex(parent_scope, CodeBloc::InitOption::return_not_required);
	}
	else {
		bloc_if_true->init( parent_scope);
		if (bloc_if_false) bloc_if_false->init(parent_scope);
	}
}

// build the circuit for the return statement
BuildContext::NextAction Statement_If::build_circuit(BuildContext& ctx) const
{
	// build the condition expression
	std::vector<Connection*> expression_value = expression->build_circuit(ctx);
	assert(expression_value.size() == 1);

	if (bloc_if_true == nullptr)
		throw Error("Internal error : if (true) bloc is null");

	// create 2 contexts with 2 new sub circuits 
	BuildContext ctx_if_true(ctx,  BuildContext::Caller::if_statement );
	ctx_if_true.debug_description = "ctx_if_true";
	BuildContext ctx_if_false(ctx, BuildContext::Caller::if_statement);
	ctx_if_false.debug_description = "ctx_if_false";
	// créate a new gate IF
	Gate_IF* gate_if = new Gate_IF(&ctx_if_true.circuit(), &ctx_if_false.circuit());

	// init true case
	ctx_if_true.init_variables_if_gate(ctx, gate_if,  true);
	BuildContext::NextAction next_action = bloc_if_true->build_circuit(ctx_if_true);
	// if no return or break statement, add the end of the bloc
	if (next_action != BuildContext::NextAction::Return ) {
		// add the end of the bloc
		BuildContext::NextAction action_1;
		if (next_action == BuildContext::NextAction::Break) 
			action_1 = ctx.build_on_break(ctx_if_true);
		else
			action_1 = ctx.build_all_next_statements(ctx_if_true);
		assert(action_1 == BuildContext::NextAction::Return);
	}

	// init false case
	ctx_if_false.init_variables_if_gate(ctx, gate_if, false);
	assert(bloc_if_false == nullptr);// TODO: else
	// add the end of the bloc 
	assert(ctx.build_all_next_statements != nullptr);
	BuildContext::NextAction action_0 = ctx.build_all_next_statements(ctx_if_false);
	assert(action_0 == BuildContext::NextAction::Return); //build_all_next_statements muut havec reachted a return statement-

	// add the gate to the circuit
	std::array<Connection*, 1> input_1_bit = { expression_value[0] };
	std::array<Connection*, 0> void_result =
	gate_if->add_to_circuit(ctx.circuit(), input_1_bit);
	input_1_bit[0]->debug_description = "gate_if intput";

	// tell the ciruit outupt sizz, without real connexion.
	// the real output will be from or circuit_if_true ou ctx_if_false
	NbBit nb_bit_out1 = ctx_if_true.circuit().nb_bits_output();
	NbBit nb_bit_out0 = ctx_if_false.circuit().nb_bits_output();
	// the 2 circuits must have the same output size
	if (nb_bit_out1 != nb_bit_out0)
		throw Error("If : 2 Return statements with different size");

	// tell the ciruit outupt sizz, without real connexion. they are those og ctx_if_true 
	ctx.circuit().set_output_size_child(nb_bit_out1);
	// circuit is build fot both cas, no need to build anything
	return BuildContext::NextAction::Return;
}


