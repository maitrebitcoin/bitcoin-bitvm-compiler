
#include "Program.h"
#include "Error.h"
#include "BuildContext.h"

// declare and set init
void Statement_DeclareAndSetVar::init(CodeBloc* parent_bloc) {
	// init set and assign
	declaration.init(parent_bloc);
	affectation.init(parent_bloc);
	// check the type
	if (!declaration.get_type().is_same_type(affectation.get_type()))
		throw Error("Type mismatch");
}


// build the circuit for "int a=3"
void Statement_DeclareAndSetVar::build_circuit(BuildContext& ctx) const {
	// declare the variable
	declaration.build_circuit(ctx);
	// set variable value
	affectation.build_circuit(ctx);
}

