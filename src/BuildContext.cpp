
#include "BuildContext.h"
#include "Circuit.h"

// constructor
BuildContext::BuildContext(Caller caller) 
{
	assert(caller == Caller::main_body); 
	// alloc main circuit
	ctx_circuit = new Circuit();
}


// copy constructor
BuildContext::BuildContext(const BuildContext& source, Caller caller) 
{
	assert(caller != Caller::main_body);
	switch (caller) 
	{
		case Caller::if_statement:
			// create a new sub circuit
			ctx_circuit = new Circuit();
			break;
		case Caller::for_statement:
			// share the same circuit in a for statement
			ctx_circuit = source.ctx_circuit;
			// copy all vaiables
			variables = source.variables;
			break;
		default:
			assert(false);
			throw Error("internal error : unknown caller");
	}

}

// visit all the circuits
void BuildContext::visit_circuits(std::function<void(Circuit&)> fnVisit) {
	// main circuit
	fnVisit(circuit());
	// sub circuits
	circuit().visit_sub_circuits(fnVisit);
}

