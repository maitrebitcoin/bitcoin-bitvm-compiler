
#include "TypeStruct.h"

// compare types
bool TypeStruct::is_same_type(const Type& other) const {
	// other must be a struct
	const TypeStruct *other_struct = other.cast_to_TypeStruct();
	if (other_struct == nullptr)
		return false;
	// compare the name
	return name == other_struct->name;
}


// return the size in bits of the type
int TypeStruct::size_in_bit(void) const {
	// the summ of the size of all members
	int sum = 0;
	for (const VariableDefinition& member : members) {
		sum += member.type->size_in_bit();
	}
	return sum;
}

