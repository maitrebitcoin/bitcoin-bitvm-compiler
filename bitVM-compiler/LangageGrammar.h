#pragma once
#include <string>
#include <vector>
struct TokenDefinition;
struct RuleDefinition;

// definition of the language grammar
class LangageGrammar {
public:
	// get the token definition
	std::pair<TokenDefinition*, int> get_token_definition(void);
	// get the grammar definition
	std::vector<RuleDefinition> get_grammar_definition(void);
};