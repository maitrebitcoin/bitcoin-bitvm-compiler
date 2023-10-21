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
	while (lexer.read_next_line()) {
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
			error_out.line_number = lexer.get_current_line_number();
			return false;
		case Result::endOfCode:
			// compilation success
			return true;
		}
		// next line
	}
	// compilation failed
	error_out.message = "syntax error : unexpected end of code";
	error_out.line_number = lexer.get_current_line_number();
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

	// init terminal flag
	for (GrammarRule& a_rule : grammar_rules) {
		// if only 1 token
		// ex : " RULE_TYPE , { TOKEN_TYPE_BOOL } 
		if (a_rule.tokens.size() == 1 && is_token(a_rule.tokens[0])) {
			// if a a terminal leaf
			a_rule.is_terminal = true;
			continue;
		}
	}

	// init Left token possible
	// from last rule to first
	for (int i = (int)grammar_rules.size()-1; i >= 0;i--) {
		GrammarRule& rule = grammar_rules[i];
		// if the 1st rule si a basic token
		TokenOrRuleId first_token = rule.tokens[0];
		if (is_token(first_token) && !rule.is_terminal ) {
			// left token possible is the 1st token
			rule.add_unique_left_token_possible(first_token);
		}
		if (is_rule(first_token)) {
			// add the left possible tokens are all the left possible tokens of the first_token rule
			_visit_prediction_rules(first_token, [&](GrammarRule* first_token_rule) {
				// add the left possible tokens of first_token_rule
				for (TokenId token_id : first_token_rule->left_token_possible)
					rule.add_unique_left_token_possible(token_id);
			});
		}
	}

	// init R conditions
	for (GrammarRule& rule : grammar_rules) {
		//for de first R tokens
		for (int i = 0; i < rule.tokens.size() - 1; i++) {
			//  token & next token
			// ex :  RULE_TYPE, TOKEN_IDENTIFIER
			TokenOrRuleId token_or_rule_i	   = rule.tokens[i ];
			TokenOrRuleId token_or_rule_next   = rule.tokens[i + 1];
			// if token i is another rule
			if (is_rule(token_or_rule_i))
				_init_grammar_right_conditions(token_or_rule_i, token_or_rule_next);
	
		}
	}


}
// init right conditions (récursive)
void Compiler::_init_grammar_right_conditions(RuleId rule_id, TokenOrRuleId right_required){

	// for all rules that can produce <rule_id>
	_visit_prediction_rules(rule_id, [&](GrammarRule* rule) {
		// if right_required is a token
		if (is_token(right_required)) {
			// add 1 token if non present
			rule->add_unique_right_token_conditions(right_required);
		}
		else {
			//  right_required is a rule
			assert(is_rule(right_required));
			// add its right conditions to <child_rule>
			_visit_prediction_rules(right_required, [&](GrammarRule* child_rule) {
				for (TokenId left_tok_id : child_rule->left_token_possible)
					rule->add_unique_right_token_conditions(left_tok_id);
				});
		}
		// recursive call if the last token is a rule
		TokenOrRuleId token_last = rule->tokens.back();
		if (is_rule(token_last))
			_init_grammar_right_conditions(token_last, right_required);

	});//


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
	for (TokenId right_token : rule.right_token_conditions) {
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
	std::string all_str_value;
	for (int i = 0; i < rule_size; i++) {
		values.push_back(token_stack[stack_top + i].value);
		all_str_value += token_stack[stack_top + i].str_value + ' ';
	}
	// call the rule to set token_result.value 
	CToken token_result(rule.rule_id);
	token_result.str_value = all_str_value;
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
	// read al sources lines
	char line_raw[max_line_size];
	while (source_code->getline(line_raw, max_line_size))
		source_code_lines.push_back(line_raw);
	// start at line 0
	current_line_number = -1;

}

// read one line of code
bool CLexer::read_next_line(void) {
	// no more lines ?
	if (current_line_number+1 >= source_code_lines.size())
		return false; // end of file
	current_line_number++;
	// set the current line in working buffer
	std::string line = source_code_lines[current_line_number];
	source_code_current_line = line;
	remaining_ligne          = line;
	// ok
	return true;
}
// is a line empty = no token
bool CLexer::_is_line_empty(std::string line) const {
	// remove white space and comments
	_remove_white_space_and_comments(line);
	if (line.size() == 0)
		return true;
	// not empty
	return false;
}

// get the next first next non exmpty ligne
std::string CLexer::_get_next_non_empty_line(void) {
	int next_line_number = current_line_number+1;
	while (next_line_number < source_code_lines.size()) {
		const std::string& next_line = source_code_lines[next_line_number];
		// if the line is not empty
		if (!_is_line_empty(next_line))
			return next_line;
	}
	// no more lines
	return "";
}

// construct an empty token
CToken::CToken(int t) 
	: type(t) {
	value.string_value = nullptr;
}

// contruct a new string token
CToken::CToken(int t, std::string v) 
	: type(t) {
	str_value = v;
	value.string_value = BitVM_C_Grammar.new_string(v.c_str());// & value_buffer;
}

// remove spaces, tabs and comments
void CLexer::_remove_white_space_and_comments(std::string& code_in_out) const {
	// remove white space and tab at the begining
	while (code_in_out[0] == ' ' || code_in_out[0] == '\t')
		code_in_out.erase(0, 1);
	// if start with commen mark e: //xxx
	if ( code_in_out[0] == '/' && code_in_out[1] == '/')
		code_in_out = "";

}

// get 1 token from the line
CToken CLexer::get_next_token(ReadOption option) {
	// working string
	std::string &code_in_out = remaining_ligne;
	std::string temp;	
	if (option == ReadOption::keep) {
		// work on a copy
		temp = remaining_ligne;
		// if the remaining line is empty, read a new line
		if (_is_line_empty(remaining_ligne))
		{
			temp = _get_next_non_empty_line();
		}
		code_in_out = temp;
	}

	// ignore white space, tab and comments
	_remove_white_space_and_comments(code_in_out);

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
			// right reduce : call the rule, remove the token from the stack and push the result
			_execute_rule(*rule);
			// right reduce until the no more match
			if (rule->is_root)
				break;
			rule = _find_matching_rule_from_stack();
		}
		// if found the finale rule
		if (rule && rule->is_root)
		{
			// check if the stack is empty
			if (token_stack.size()>1)
				return Result::syntaxError;
			// sucess
			return Result::endOfCode;
		}
	}

}
