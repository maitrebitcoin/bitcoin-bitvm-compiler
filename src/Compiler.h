#pragma once
// Compiler.h
// Compiler class definition

#include <string>
#include <vector>
#include <functional>
#include <map>
#include "Program.h"
#include "Error.h"
#include "Circuit.h"

using TokenId = int; // <255 1 char token ex ':', >255 = user defined token
using RuleId  = int; // over 1000
using TokenOrRuleId = int; // TokenId ou TokenId, depending on the value 
static const TokenId INVALID_TOKEN  = - 1;
static bool is_token(TokenOrRuleId id) { return id < 1000; }
static bool is_rule(TokenOrRuleId id) { return id >= 1000; }
class LangageDefinitionAndContext;

// types for the node during parsign
union TokenValue {

	std::string*		    string_value;
	Type*				    type_value;
	Function::Parameter*    function_paramter_value;
	Function::AllParameter* function_all_paramters_value;
	Function::Definition*   function_definition_value;
	Function*			    function_value;
	Expression*				expression_value;
	Program*				program_value;
	CodeBloc*				code_block_value;
	Statement*				statement_value;
};
// what a token is for the lexer part of the compiler
struct TokenDefinition {
	TokenId	    token_type;
	const char* token_value;
	const char* regex;
	std::function< bool(char next_char)> condition; // for conditionnal tokens
};

class ObjetKeeper;
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
	CToken(int t, std::string v, ObjetKeeper& allocator);

};
enum RuleType;
struct RuleDefinition {
	// rule to produce
	RuleId rule_id;
	// conditions to produce the rules
	std::vector<TokenOrRuleId> tokens;
	// callback function if the rule is matched
	std::function<void (TokenValue& result, std::vector<TokenValue> values)> action;
	// pre-condiciton to reduce the rule. null = no pre-condition
	std::function<bool(void)> pre_condition;

};
class GrammarRule : public RuleDefinition  {
public:
	bool is_root	  = false;
	bool is_terminal  = false;
	bool is_recursive = false;
	// LR(1) parsing :
	// left basic possible tokens.  
	std::vector<TokenId> left_token_possible;
	// right conditions to produce the rules.  empty  = all is valid
	std::vector<TokenId> right_token_conditions;

public:
	// constructor
	GrammarRule(RuleDefinition definition) {
		rule_id		  = definition.rule_id;
		tokens		  = definition.tokens;
		action		  = definition.action;
		pre_condition = definition.pre_condition;
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
	// langue defintion & context
	LangageDefinitionAndContext& language_context;
public:
	// constructor
	CLexer(LangageDefinitionAndContext& language);
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
	// get 1 token from a string <code_in_out>
	CToken _get_next_token_from_line(std::string& code_in_out, ReadOption option) const;
	// is a line empty = no token
	bool _is_line_empty(std::string line) const;
	// remove spaces, tabs and comments
	void _remove_white_space_and_comments(std::string& code_in_out) const;
	// get first non space/tab char afet a match
	char _get_next_nonspace_char(std::string code_in_out, std::string match_string) const;
};

class Compiler
{
protected:
	// token lexer : get tokens from the code
	CLexer lexer;
	// langue defintion & context
	LangageDefinitionAndContext& language_context;

	// grammar of the compiler 
	std::vector <GrammarRule> grammar_rules;
	// find rules from its id
	std::map<RuleId, std::vector<GrammarRule*>> rules_map;

	// tokenss waiting to be processed by the compiler
	std::vector<CToken> token_stack;
public:
	//==>Main entry point
	// compile and build the circuit from a file
	class Result {
	public:
		bool ok=false;
		// if result is not ok, the error
		Error	error;
	protected:
		// if result is ok, the circuits. 0 is main circuit
		std::vector<Circuit*> circuits;
	public:
		// Constructor if error
		Result(Error e) : ok(false), error(e) {}
		// Constructor if ok
		Result(std::vector<Circuit*> c) : ok(true), circuits(c) {}

		// export the resut to a stream
		void export_to_stream(std::ostream& out) const;
		// get stats
		struct Stats {
			int nb_circuit = 0;
			int nb_gate = 0;
			int nb_connection = 0;
			int nb_input = 0;
			int nb_output = 0;
		};
		// getd stats about the circuits
		Stats get_stats(void) const;
		// get the main circuits
		Circuit& main_circuit(void) { return *circuits[0]; }

	};
	static Result compile_circuit_from_file(std::string file_name);

	// constructor
	Compiler(LangageDefinitionAndContext& language);
	// compile a memory stream
	bool compile(std::istream& source_code, Error& error_out);
	// get the program afer compilation
	Program& get_programm(void);



protected:
	// init the grammar
	void _init_grammar(void);
	// initgrammar : right conditions (récursive)
	struct RuleAndRToken {
		RuleId	rule_id;
		TokenId right_token_required;
		bool operator ==(const RuleAndRToken& other) const {
			return rule_id == other.rule_id && right_token_required == other.right_token_required;
		}
		bool operator <(const RuleAndRToken& other) const {
			return rule_id < other.rule_id || (rule_id == other.rule_id && right_token_required < other.right_token_required);
		}
	};
	using MapVisited = std::map<RuleAndRToken, bool>;
	void _init_grammar_right_conditions(RuleId rule_id, TokenId right_token_required, MapVisited & visited);
	// set resurively all child rules as recursive
	using MapVisited2 = std::map<RuleId, bool>;
	void _set_child_rule_recusive(GrammarRule& rule, MapVisited2& visited);
	// 1s phase : compile a memory stream
	bool _compile_build_tree(std::istream& source_code_stream, Error& error_out);
	// compile 1 line of code
	enum class ResultforLine {
		nextLine,
		syntaxError,
		endOfCode,
	};
	ResultforLine _compile_line(void);
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


