// implémentation of the compiler

#include "Compiler.h"
#include "LangageGrammar.h"
#include <sstream>
#include <vector>
#include <assert.h>
#include <regex>
#include <functional>

LangageGrammar BitVM_C_Grammar;

// constructor
Compiler::Compiler(void) {
	_init_grammar();
}


// compile a memory stream
bool Compiler::compile(std::istream& source_code_stream, Error& error_out)
{
	// init the lexer
	lexer.init(source_code_stream);

	// read the source code
	int current_line_number = 0;
	while (lexer.read_line()) {
		// compile 1 line
		Result result =	_compile_line();
		switch (result)
		{	
		case Result::nextLine:
			// continue compilation
			break;
		case Result::syntaxError:
			// compilation failed
			error_out.message     = "syntax error : " + lexer.get_source_code_current_line(); 
			error_out.line_number = current_line_number;
			return false;
		case Result::endOfCode:
			// compilation success
			return true;
		}
		// next line
		current_line_number++;
	}
	// compilation failed
	error_out.message = "syntax error : unexpected end of code";
	error_out.line_number = current_line_number;
	return false;
}



void Compiler::_init_grammar(void) 
{


	// init the grammar array
	auto rules_definition = BitVM_C_Grammar.get_grammar_definition();
	for (int i = 0; i < rules_definition.size() ; i++) {
		GrammarRule grammar_rule_i(rules_definition[i]);
		grammar_rules.push_back(grammar_rule_i);
	}
	// init map to rules
	for (GrammarRule& rule : grammar_rules)	{
		rules_map[rule.rule_id].push_back( & rule );
	}

	// 1sr rule is the start rule
	grammar_rules[0].is_root = true;

	// init terminals and R conditions
	for (GrammarRule& a_rule : grammar_rules)
	{
		// if only 1 token
		if (a_rule.tokens.size() == 1) {
			// if a a terminal leaf
			a_rule.is_terminal = true;
			continue;
		}
		// init right_token_conditions
		for (int i = 0; i < a_rule.tokens.size() - 1; i++) {
			//  token & next token
			// ex :  RULE_TYPE, TOKEN_IDENTIFIER
			TokenType token_i	  = a_rule.tokens[i ];
			TokenType right_token = a_rule.tokens[i + 1];
			// if token i is another rule
			_init_grammar_right_conditions(token_i, right_token);
		}
	}


}
// init right conditions (récursive)
void Compiler::_init_grammar_right_conditions(TokenType token, TokenType right_token_required){

	//  only if token is another rule
	if (rules_map.count(token) == 0)
		return;
	for (GrammarRule* sub_rule_i : rules_map[token])
	{
		// if <right_token_required> is not alredy present
		std::vector<TokenType>& R_conditions= sub_rule_i->right_token_conditions;
		bool is_already_present = std::find(R_conditions.begin(), R_conditions.end(), right_token_required) != R_conditions.end();
		if (!is_already_present)
			sub_rule_i->right_token_conditions.push_back(right_token_required);
		// récursive for the rightmost rule
		_init_grammar_right_conditions(sub_rule_i->tokens.back(), right_token_required);
	}
}


// test if a rule is matching the current stack
bool Compiler::is_rule_matching_stack(GrammarRule& rule)  {
	// check if the stack is big enough
	size_t rule_size = rule.tokens.size();
	if (token_stack.size()< rule_size)
		return false;
	// check if the rule match the top of the stack
	size_t stack_top = token_stack.size() - rule_size;
	for (int i=0; i< rule_size; i++) {
		if (rule.tokens[i] != token_stack[stack_top+i].type)
			return false;
	}
	// terminal rule
	if (rule.is_terminal )
		// matching ok
		return true;
	// no post condition .
	if (rule.right_token_conditions.size()==0)
		// matching ok
		return true;

	// Read one token after the rule
	CToken next_token = lexer.get_next_token( CLexer::ReadOption::keep );
	// chekc if if match the post condition
	for (TokenType right_token : rule.right_token_conditions) {
		if (right_token == next_token.type)
			return true; // one R condition is matching
	}
	// no R condition is matching
	return false;

}	

// find a matching rule from the token stack
GrammarRule* Compiler::_find_matching_rule_from_stack(void)  {
	// for each rule in the grammar
	for (GrammarRule& rule : grammar_rules) {
		// check if the rule match the token stack
		if (is_rule_matching_stack(rule))
			return &rule;
	}
	// no rule found
	return nullptr;
}
// call a rule and remove the token from the stack
void Compiler::_execute_rule(GrammarRule& rule) {
	size_t rule_size = rule.tokens.size();
	size_t stack_top = token_stack.size() - rule_size;
	assert(stack_top >= 0);
	// get values form the stack
	std::vector<TokenValue> values;
	for (int i = 0; i < rule_size; i++) {
		values.push_back(token_stack[stack_top + i].value);
	}
	// call the rule to set token_result.value 
	CToken token_result(rule.rule_id);
	if (rule.action != nullptr)
		rule.action(token_result.value, values);

	// remove the token from the top of the stack
	for (int i = 0; i < rule_size; i++) {
		token_stack.pop_back();
	}
	// push the result on the stack
	token_stack.push_back(token_result);

}

// lexer constructor
CLexer::CLexer(void) {
	auto [definitions, len] = BitVM_C_Grammar.get_token_definition();
	token_definition.assign(definitions, definitions + len);
}

// Init lexer
void CLexer::init(std::istream& source) {
	source_code = &source;
}

// read one line of code
bool CLexer::read_line(void) {
	static const int max_line_size = 1024;
	char line_raw[max_line_size];
	int cureent_line_number = 0;
	if (!source_code->getline(line_raw, max_line_size))
		return false;
	std::string line(line_raw);
	source_code_current_line = line;
	remaining_ligne          = line;
	// ok
	return true;
}


// get 1 token from the line
CToken CLexer::get_next_token(ReadOption option) {
	// working string
	std::string &code_in_out = remaining_ligne;
	std::string temp;	
	if (option == ReadOption::keep) {
		// work on a copy
		temp = remaining_ligne;
		code_in_out = temp;
	}

	// ignore white space and tab
	while (code_in_out[0] == ' ' || code_in_out[0] == '\t')
		code_in_out.erase(0, 1);

	// if the string is empty, return an empty token
	if (code_in_out.size() == 0)
		return CToken(0, "");

	// check if the string star wiht of of the known token
	for (TokenDefinition definition : token_definition) {
		// ex "bool"
		if (definition.token_value != nullptr) {
			// check if the string start with the token
			if (code_in_out.compare(0, strlen(definition.token_value), definition.token_value) == 0) {
				// remove the token from the string
				if (option == ReadOption::remove)
					code_in_out.erase(0, strlen(definition.token_value));
				// return the token
				return CToken(definition.token_type, definition.token_value);
			}
		}
		// ex "[0-9*]"
		else {
			assert(definition.regex != nullptr);
			//  check if the string start with the token with definition.regex
			std::string regex_definition = definition.regex;
			std::regex regex_i(regex_definition);
			std::smatch regex_match;
			if (std::regex_search(code_in_out, regex_match, regex_i)) {
				std::string match_string = regex_match[0];
				// check if the string start with the matched part in the regexp 
				if (match_string.size() > 0 && code_in_out.compare(0, match_string.size(), match_string) == 0) {
					// remove the token from the string
					if (option == ReadOption::remove)
						code_in_out.erase(0, match_string.size());
					// return the token
					return CToken(definition.token_type, match_string);
				}
			}
		}
	} // for each token definition

	//TODO : return INVALID_TOKEN if the token is not in the grammar

	// send the first char as a token
	char c = code_in_out[0];
	char tok_c[] = { c,0 };
	if (option == ReadOption::remove)
		code_in_out.erase(0, 1);
	return CToken(c, tok_c);
}


// compile 1 line of code
Compiler::Result Compiler::_compile_line(void) {
	// ignore empty lines
	if (lexer.is_current_line_empty()) 
		return Result::nextLine;;

	// read all the tokens
	while (1) {
		// get a token for the line
		CToken token = lexer.get_next_token( CLexer::ReadOption::remove);
		if (token.type == 0)
			return Result::nextLine; // End of line, read next line
		if (token.type == INVALID_TOKEN)
			return Result::syntaxError;
		// add to stack	
		token_stack.push_back(token);
		// check if we have a matching rule
		GrammarRule* rule = _find_matching_rule_from_stack();
		while (rule != nullptr) {
			// call the rule, remove the token from the stack and push the result
			_execute_rule(*rule);
			// do it utile the no more match
			rule = _find_matching_rule_from_stack();
		}
		// if found the finale rule
		if (rule && rule->is_terminal)
		{
			// check if the stack is empty
			if (token_stack.size()>1)
				return Result::syntaxError;
			// sucess
			return Result::endOfCode;
		}
	}

}
