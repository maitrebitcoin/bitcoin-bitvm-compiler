#pragma once

// Compiler class definition

#include <string>
#include <vector>
#include <functional>
#include <map>
#include "BtcContract.h"

using TokenType = int;
static const TokenType INVALID_TOKEN  = - 1;

//using TokenValue = std::string;
union TokenValue {

	std::string*		 string_value;
	Type*				 type_value;
	Function::Parameter* function_paramter_value;
	Function*			 function_value;
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
enum RuleType;
struct RuleDefinition {
	// rule to produce
	RuleType rule_id;
	// conditions to produce the rules
	std::vector<TokenType> tokens;
	// callback function if the rule is matched
	std::function<void (TokenValue& result, std::vector<TokenValue> values)> action;
};
class GrammarRule : public RuleDefinition  {
public:
	bool is_root	 = false;
	bool is_terminal = false;
	// LR(1) parsing :
	// right conditions to produce the rules.  empty  = all is valid
	std::vector<TokenType> right_token_conditions;

public:
	// constructor
	GrammarRule(RuleDefinition definition) {
		rule_id = definition.rule_id;
		tokens  = definition.tokens;
		action  = definition.action;
	}
};

class CLexer {
protected:
	// all source code
	std::istream* source_code=nullptr;
	// current line code
	std::string source_code_current_line;
	// what remains to be read in current line
	std::string remaining_ligne;
public:
	// Init
	void init(std::istream& source);

	// read one line of code
	bool read_line(void);
	// is the line empty ?
	bool is_current_line_empty(void) const { return source_code_current_line.size() == 0; }
	// get 1 token from code
	enum class ReadOption {
		remove,
		keep
	};
	CToken get_next_token(ReadOption option);
	// get le this of code (for error messages)
	std::string get_source_code_current_line(void) const { return source_code_current_line; }
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
	// token lexer : get tokens from the code
	CLexer lexer;

	// grammar of the compiler 
	std::vector <GrammarRule> grammar_rules;
	// find rules from its id
	std::map< TokenType, std::vector<GrammarRule*>> rules_map;

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
	// initgrammar : right conditions (récursive)
	void _init_grammar_right_conditions(TokenType token, TokenType right_token_required);

	// compile 1 line of code
	enum class Result {
		nextLine,
		syntaxError,
		endOfCode,
	};
	Result _compile_line(void);
	// test if a rule is matching the current stack
	bool is_rule_matching_stack(GrammarRule& rule) ;
	// find a matchin rule from the token stack
	GrammarRule* _find_matching_rule_from_stack(void);
	// call a rule and remove the token from the stack
	void _execute_rule(GrammarRule& rule);
};


