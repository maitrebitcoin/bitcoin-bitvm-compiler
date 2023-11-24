
#include "Program.h"
#include "Error.h"
#include "BuildContext.h"

// declare and set init
void Statement_DeclareAndSetVar::init(Scope& parent_scope) {
	// init set and assign
	declaration.init(parent_scope);
	affectation.init(parent_scope);
	// check the type
	if (!declaration.get_type().is_same_type(affectation.get_type())) {
		// if R argument si a litteral, ex:  int256 = 0; try to cast it
		Literal* affectation_literal = affectation.expression->cast_to_literal();
		if (affectation_literal != nullptr && declaration.get_type().is_integer()) {
			// cast the litteral
			affectation_literal->set_native_type(declaration.get_type().cast_to_TypeBasic()->get_native_type());
			return;
		}
		// error
		throw Error("Type mismatch");
	}
}

// build the circuit for "int a=3"
void Statement_DeclareAndSetVar::build_circuit(BuildContext& ctx) const {
	// declare the variable
	declaration.build_circuit(ctx);
	// set variable value
	affectation.build_circuit(ctx);
}

