#pragma once
#include <string>
#include <vector>
#include "BtcContract.h"

struct TokenDefinition;
struct RuleDefinition;

// to keep all string an objet crated during parsing alive
class ObjetKeeper {
protected:
	// strings
	std::vector<std::string *> strings;
	// types
	std::vector<Type*> Types;
	// 1 function parameters
	std::vector<Function::Parameter*> function_parameters;
	// all function parameters
	std::vector<Function::AllParameter*> function_all_parameters;
	// finction definitions
	std::vector<Function::Definition*> function_definitions;

public:
	// destuctor
	~ObjetKeeper(void) {
		free_all();
	}

	// get a new string
	std::string* new_string(const char *c_string) {
		std::string* new_string = new std::string(c_string);
		strings.push_back(new_string);
		return new_string;
	}	
	// get a new Type 
	Type* new_Type(Type::Native t) {
		Type* new_type = new Type(t);
		Types.push_back(new_type);
		return new_type;
	}
	// get a new function parameter
	Function::Parameter* new_function_parameter(Type type, std::string name) {
		Function::Parameter* new_parameter = new Function::Parameter(type, name);
		function_parameters.push_back(new_parameter);
		return new_parameter;
	}
	// get a new function definition
	Function::Definition* new_function_definition(Type type, std::string name, Function::AllParameter* all_params ) {
		Function::Definition* new_definition = new Function::Definition(type, name, all_params);
		function_definitions.push_back(new_definition);
		return new_definition;
	}
	// get a new function all parameters
	Function::AllParameter* new_function_all_parameters(Function::Parameter* p0) {
		Function::AllParameter* new_all_parameters = new Function::AllParameter(*p0);
		function_all_parameters.push_back(new_all_parameters);
		return new_all_parameters;
	}

	// free all 
	void free_all(void) {
		for (std::string* s: strings) {
			delete s;
		}
		for (Type* t : Types) {
			delete t;
		}
		for (Function::Parameter* p : function_parameters) {
			delete p;
		}
		for (Function::AllParameter* p : function_all_parameters) {
			delete p;
		}
	}
};

// definition of the language grammar
class LangageGrammar : public ObjetKeeper {

public:
	// get the token definition
	std::pair<TokenDefinition*, int> get_token_definition(void);
	// get the grammar definition
	std::vector<RuleDefinition> get_grammar_definition(void);
};

