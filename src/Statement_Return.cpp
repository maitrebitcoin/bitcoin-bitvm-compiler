
#include "Program.h"
#include "Error.h"
#include "BuildContext.h"
#include "Circuit.h"

// init a return statmenet
void Statement_Return::init(Scope& parent_scope) {
	// intialize the expression
	expression->init(parent_scope);
	// get return type
	const Type& returned_type = get_type();

	// check the return type
	Function* parent_function = parent_scope.get_parent_function();
	assert(parent_function != nullptr); // not allowed in global scope
	if (!returned_type.is_same_type(parent_function->get_return_type()))
		throw Error("Return type mismatch");
}

// build the circuit for the return statem-ent
BuildContext::NextAction Statement_Return::build_circuit(BuildContext& ctx) const {

	// build the expression
	std::vector<Connection*> outputs = expression->build_circuit(ctx);
	int nb_bit_out = (int)outputs.size();
	assert(nb_bit_out == get_type().size_in_bit());
	// connect the output of the expression to the output of the circuit
	ctx.circuit().set_output(outputs);
	return BuildContext::NextAction::Return;
}
