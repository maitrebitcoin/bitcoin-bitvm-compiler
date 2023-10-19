#pragma once

// Compiler class definition

#include <string>
#include <vector>
#include <functional>

using TokenType = int;
//using TokenValue = std::string;
union TokenValue {
	std::string *string_value;
	class Type* type_value;
	class FunctionParameter* function_paramter_value;
};

class CToken {
public:
	// type of the token. ex '=' or TOKEN_TYPE_BOOL
	TokenType type;
	// value of the token. ex "bool"
	std::string value_buffer; // point value if type if a "string_value"
	TokenValue value;
public:
	// constructors
	CToken(int t) : type(t) {}
	CToken(int t, std::string v) : type(t)
	{
		value_buffer = v;
		value.string_value = &value_buffer;
	}

};
class RuleDefinition {
public:
	TokenType rule_id;
	std::vector<TokenType> tokens;
	// callback function if the rule is matched
	std::function<void (TokenValue& result, std::vector<TokenValue> values)> action;
};

class Compiler
{
public:
	class Error {
	public:
		// line number
		int line_number = 0;
		// error message
		std::string message;
	public:
		// constructor
		Error(void) {}
		Error(const char* mess) : message(mess) {}
	};
protected:
	// grammar of the compiler 
	std::vector <RuleDefinition> grammar_rules;

	// tokenss waiting to be processed by the compiler
	std::vector<CToken> token_stack;
public:
	// constructor
	Compiler(void);
	// compile a memory stream
	bool compile(std::istream& source_code, Error& error_out);
protected:
	// init the grammar
	void _init_grammar(void);
	// compile 1 line of code
	void _compile_line(std::string line_of_code);
	// get 1 token from code
	CToken _get_next_token(std::string& code_in_out);
	// test if a rule is matching the current stack
	bool is_rule_matching_stack(RuleDefinition& rule) const;
	// find a matchin rule from the token stack
	RuleDefinition* _find_matching_rule_from_stack(void);
	// call a rule and remove the token from the stack
	void _execute_rule(RuleDefinition& rule);
};


