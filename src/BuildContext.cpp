
#include "BuildContext.h"
#include "Circuit.h"

// get a new sub circuit
Circuit& BuildContext::get_new_sub_circuit(void) {
	Circuit* sub_circuit =  new Circuit();
	return *sub_circuit;
}

// visit all the circuits
void BuildContext::visit_circuits(std::function<void(Circuit&)> fnVisit) {
	// main circuit
	fnVisit(circuit);
	// sub circuits
	circuit.visit_sub_circuits(fnVisit);
}

