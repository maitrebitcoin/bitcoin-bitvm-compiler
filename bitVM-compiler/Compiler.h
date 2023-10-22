#pragma once
// Compiler.h
// Compiler class definition

#include <string>
#include <vector>
#include <functional>
#include <map>
#include "Program.h"

using TokenId = int; // <255 1 char token ex ':', >255 = user defined token
using RuleId  = int; // over 1000
using TokenOrRuleId = int; // TokenId ou TokenId, depending on the value 
static const TokenId INVALID_TOKEN  = - 1;
static bool is_token(TokenOrRuleId id) { return id < 1000; }
static bool is_rule(TokenOrRuleId id) { return id >= 1000; }

// types for the node during parsign
union TokenValue {

	std::string*		    string_value;
	Type*				    type_value;
	Function::Parameter*    function_paramter_value;
	Function::AllParameter* function_all_paramters_value;
	Function::Definition*   function_definition_value;
	Function*			    function_value;
};
// what a token is for the lexer part of the compiler
struct TokenDefinition {
	TokenId	    token_type;
	const char* token_value;
	const char* regex;
};


class CToken {
public:
	// type of the token. ex '=' or TOKEN_TYPE_BOOL
	TokenId	    type;
	// value of the token. ex "bool"
	std::string str_value; // source string in the code
	TokenValue  value;
public:
	// constructors
	CToken(int t);
	CToken(int t, std::string v);

};
enum RuleType;
struct RuleDefinition {
	// rule to produce
	RuleId rule_id;
	// conditions to produce the rules
	std::vector<TokenOrRuleId> tokens;
	// callback function if the rule is matched
	std::function<void (TokenValue& result, std::vector<TokenValue> values)> action;
};
class GrammarRule : public RuleDefinition  {
public:
	bool is_root	 = false;
	bool is_terminal = false;
	// LR(1) parsing :
	// left basic possible tokens.  
	std::vector<TokenId> left_token_possible;
	// right conditions to produce the rules.  empty  = all is valid
	std::vector<TokenId> right_token_conditions;
	

public:
	// constructor
	GrammarRule(RuleDefinition definition) {
		rule_id = definition.rule_id;
		tokens  = definition.tokens;
		action  = definition.action;
	}
	// add left basic possible tokens if not already present
	void add_unique_left_token_possible(TokenId token_id) {
		if (std::find(left_token_possible.begin(), left_token_possible.end(), token_id) == left_token_possible.end()) {
			left_token_possible.push_back(token_id);
		}
	}
	// add right conditions to produce the rules if not already present
	void add_unique_right_token_conditions(TokenId token_id) {
		if (std::find(right_token_conditions.begin(), right_token_conditions.end(), token_id) == right_token_conditions.end()) {
			right_token_conditions.push_back(token_id);
		}
	}
};

class CLexer {
protected:
	static const int max_line_size = 1000;
	// tokens definition
	std::vector<TokenDefinition> token_definition;
	// all source code
	std::istream* source_code=nullptr;
	// all source split in lines
	std::vector<std::string> source_code_lines;
	// current line number
	int current_line_number = 0;
	// current line code
	std::string source_code_current_line;
	// what remains to be read in current line
	std::string remaining_ligne;
public:
	// constructor
	CLexer(void);
	// Init
	void init(std::istream& source);
	// read one line of code
	bool read_next_line(void);
	// get the current line number
	int get_current_line_number(void) const { return current_line_number; }
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

protected:
	// get the next first next non exmpty ligne
	std::string _get_next_non_empty_line(void);
	// is a line empty = no token
	bool _is_line_empty(std::string line) const;
	// remove spaces, tabs and comments
	void _remove_white_space_and_comments(std::string& code_in_out) const;
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
	std::map<RuleId, std::vector<GrammarRule*>> rules_map;

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
	void _init_grammar_right_conditions(RuleId rule_id, TokenId right_token_required);

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
	// visit all prpdiction rules form id
	void _visit_prediction_rules(RuleId rule_id, std::function<void(GrammarRule* production_rule) > fn)	{
		for (auto& rule : rules_map[rule_id]) {
			fn(rule);
		}
	}

};


