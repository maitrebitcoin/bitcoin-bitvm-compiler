#pragma once

#include <string>
#include "Type.h"

class VariableDefinition {
public:
	// type of the variable
	Type type;
	// name of the variable
	std::string name;

	// constructor
	VariableDefinition(Type t, std::string s) : type(t), name(s) {}
};