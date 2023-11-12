#pragma once

#include <string>
#include <vector>
#include "VariableDefinition.h"
#include "InterfaceInputsMap.h"
class CodeBloc;
class Scope;


// represents a function, ex : bool main(bool a, bool b) { return a & b; }
class Function {
public:
	// 1 parameters of the function
	struct Parameter : VariableDefinition {
		// constructor
		Parameter(Type* t, std::string n) : VariableDefinition(t, n) {}
	};
	// all the parameters of the function
	struct AllParameter {
		std::vector<Parameter> parameters;
		// constructor : init with 1 parameter
		AllParameter(Parameter& p0) {
			parameters.push_back(p0);
		}
		// add 1 parameter
		void add(Parameter& p) {
			parameters.push_back(p);
		}
	};
	// Definition of the function
	struct Definition {
		// name of the function
		std::string name;
		// return type of the function
		Type* return_type;
		// parameters of the function
		std::vector<Parameter> parameters;
		// constructor
		Definition(Type* t, std::string n, Function::AllParameter* all_params);
	};

protected:
	// definiton of the function
	Definition definition;
	// body of the function
	CodeBloc* body;
	// scope of the function
	Scope* function_scope=nullptr;

public:
	// constructor
	Function(Definition* def, CodeBloc* body);
	~Function();
	// init a function
	void init(Scope& global_scope);
	// get the name of the function
	std::string get_name(void) const { return definition.name; }
	// get the return type of the function
	const Type& get_return_type(void) const { return *definition.return_type; }
	// return the number of bits needed for the input parameters
	int size_in_bit_input(void) const;
	// return the number of bits needed to store the return value
	int size_in_bit_output(void) const;
	// find a parameter by name
	const VariableDefinition* find_parameter_by_name(std::string name) const;
	// build a circuit that represents the fuidl
	void build_circuit(class Circuit& circuit);

	// get a interface n InterfaceInputsMap 
	InterfaceInputsMap* getInterfaceInputsMap(void) const;
	
};

