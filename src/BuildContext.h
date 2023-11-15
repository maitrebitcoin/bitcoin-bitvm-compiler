#pragma once

#include <vector>
#include "VariableDefinition.h"

class Circuit;
class KnownVar;
class Connection;

// context for building the circuit
class BuildContext {


public:
	Circuit& circuit; // the circuit to build
	std::vector<Circuit*> sub_circuits; // sub circuits created pour if/loop/prorcedure
	KnownVar& variables; // current known variables in the current scope

public:
	// constructor
	BuildContext(Circuit& c, KnownVar& vars) : circuit(c), variables(vars) { }

	// get a new sub circuit
	Circuit& get_new_sub_circuit(void);

};


// var during building
class VarBuild : public VariableDefinition {
public:
	std::vector<Connection*> bits; // curetn vue. emtpy if var not yet assigned
public:
	// constructor
	VarBuild(Type* t, std::string n) : VariableDefinition(t, n) { }

	// is the variable assigned ?
	bool is_set(void) const {
		return bits.size() > 0;
	}
	// set variable value
	void set_value(std::vector<Connection*>& value) {
		assert(value.size() == type->size_in_bit());
		// set bits
		bits = value;
	}
	// it the var is a struct, get the member by name
	VarBuild* find_member_by_name(std::string name) {
		assert(false);// TODO
		return nullptr;
	}

};
class KnownVar : public std::vector<VarBuild> {
public:
	// find a variable by name
	VarBuild* find_by_name(std::string name) {
		for (VarBuild& var_i : *this)
		{
			if (var_i.name == name)
				return &var_i;
		}
		return nullptr;
	}
	// declare a new local variable
	void declare_local_var(Type* var_type, std::string var_name) {
		// TODO
		VarBuild new_var{ var_type, var_name };
		push_back(new_var);
	}

};
