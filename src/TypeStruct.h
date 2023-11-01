#pragma once

#include <string>
#include <vector>
#include "Type.h"
#include "VariableDefinition.h"

// usert type structure
class TypeStruct : public Type {
public:
	// name of the struct
	std::string name;
	// meberes of the struct
	std::vector<VariableDefinition> members;

	// return the size in bits of the type
	virtual int size_in_bit(void) const override;
	// cast to "TypeStruct"
	virtual TypeStruct* cast_to_TypeStruct(void) override { return this; }
};
