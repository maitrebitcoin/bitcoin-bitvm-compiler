
#include <assert.h>
#include "Program.h"
#include "Error.h"


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

	// init blocs of code
	if (bloc_if_true != nullptr)
		bloc_if_true->init(parent_scope);
	if (bloc_if_false != nullptr)
		bloc_if_false->init(parent_scope);
}

// build the circuit for the return statement
void Statement_If::build_circuit(BuildContext& ctx) const
{
	// build the condition expression
	std::vector<Connection*> expression_value = expression->build_circuit(ctx);


	//@TODO
	throw Error("Non implemented");
}


