
#include "BuildContext.h"
#include "Circuit.h"

// get a new sub circuit
Circuit& BuildContext::get_new_sub_circuit(void) {
	sub_circuits.push_back(new Circuit());
	return *sub_circuits.back();
}