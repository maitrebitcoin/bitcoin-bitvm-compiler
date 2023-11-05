
#include "TypeStruct.h"
#include "BuildContext.h"



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

// get the member by name
TypeStruct::Member TypeStruct::get_member_by_name(const std::string& name) const {
	
	int index = 0;
	int sum = 0;
	for (const VariableDefinition& member : members) {
		// is it the member we are looking for ?
		if (member.name == name) {
			Member result;
			result.is_valid = true;
			result.index = index;
			result.type = member.type;
			result.offest_in_bit = sum;
			return result;
		}

		// next membre
		sum += member.type->size_in_bit();
		index++;

	}
	// not found
	Member result_notfound;
	return result_notfound;
}

