#pragma once

#include <vector>
#include <functional>
#include "VariableDefinition.h"

class Circuit;
class ScopeVariables;
class Connection;

// context for building the circuit
class BuildContext {


public:
	Circuit& circuit; // the circuit to build
	std::vector<Circuit*> sub_circuits; // sub circuits created pour if/loop/prorcedure
	ScopeVariables& variables; // current known variables in the current scope

public:
	// constructor
	BuildContext(Circuit& c, ScopeVariables& vars) : circuit(c), variables(vars) { }

	// visit all the circuits
	void visit_circuits(std::function<void (Circuit&)> fnVisit);

	// get a new sub circuit
	Circuit& get_new_sub_circuit(void);

};


// var during building, valid in a scope
class ScopeVariable : public VariableDefinition {
public:
	std::vector<Connection*> bits; // curetn vue. emtpy if var not yet assigned
public:
	// constructor
	ScopeVariable(Type* t, std::string n) : VariableDefinition(t, n) { }

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
	ScopeVariable* find_member_by_name(std::string name) {
		assert(false);// TODO
		return nullptr;
	}

};
class ScopeVariables : public std::vector<ScopeVariable> {
public:
	// find a variable by name
	ScopeVariable* find_by_name(std::string name) {
		for (ScopeVariable& var_i : *this)
		{
			if (var_i.name == name)
				return &var_i;
		}
		return nullptr;
	}
	// declare a new local variable
	void declare_local_var(Type* var_type, std::string var_name) {
		// TODO
		ScopeVariable new_var{ var_type, var_name };
		push_back(new_var);
	}

};
