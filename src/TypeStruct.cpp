
#include "TypeStruct.h"

// return the size in bits of the type
int TypeStruct::size_in_bit(void) const {
	// the summ of the size of all members
	int sum = 0;
	for (const VariableDefinition& member : members) {
		sum += member.type.size_in_bit();
	}
	return sum;
}

