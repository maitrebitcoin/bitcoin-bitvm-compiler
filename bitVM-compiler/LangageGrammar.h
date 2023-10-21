#pragma once
#include <string>
#include <vector>
struct TokenDefinition;
struct RuleDefinition;

// to keep all string an objet crated during parsing alive
class ObjetKeeper {
protected:
	// strings
	std::vector<std::string *> strings;

public:
	// destuctor
	~ObjetKeeper(void) {
		free_all();
	}

	// get a new string
	std::string* get_new_string(const char *c_string) {
		std::string* s = new std::string(c_string);
		strings.push_back(s);
		return s;

	}	
	// free all strings
	void free_all(void) {
		for (std::string* s: strings) {
			delete s;
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

