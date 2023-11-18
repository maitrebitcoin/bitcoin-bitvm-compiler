
#include "BuildContext.h"
#include "Circuit.h"

// get a new sub circuit
Circuit& BuildContext::get_new_sub_circuit(void) {
	sub_circuits.push_back(new Circuit());
	return *sub_circuits.back();
}

// visit all the circuits
void BuildContext::visit_circuits(std::function<void(Circuit&)> fnVisit) {
	// main circuit
	fnVisit(circuit);
	// sub circuits
	for (Circuit* sub_circuit : sub_circuits)
		fnVisit(*sub_circuit);
}