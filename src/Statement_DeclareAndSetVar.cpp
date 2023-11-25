
#include "Program.h"
#include "Error.h"
#include "BuildContext.h"

// declare and set init
void Statement_DeclareAndSetVar::init(Scope& parent_scope) {
	// init set and assign
	declaration.init(parent_scope);
	affectation.init(parent_scope);

}

// build the circuit for "int a=3"
void Statement_DeclareAndSetVar::build_circuit(BuildContext& ctx) const {
	// declare the variable
	declaration.build_circuit(ctx);
	// set variable value
	affectation.build_circuit(ctx);
}

