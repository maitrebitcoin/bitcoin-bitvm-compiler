
#include "Program.h"
#include "Error.h"


// init statmenet
void Statement_Break::init(Scope& parent_scope)  {
	// nothing
}
// build the circuit 
void Statement_Break::build_circuit(BuildContext& ctx) const {
	// must be in a loop
	if (!ctx.is_in_for_loop()) {
		throw Error("break statement must be in a loop");
	}

	// if we reach this point, the rest of the ciruit is the part after the loop
	Statement_For* for_loop = ctx.for_statement;
	assert(for_loop != nullptr);

}
