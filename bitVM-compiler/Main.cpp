// bitVM-compiler.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include "Circuit.h"
#include "Gate.h"
#include "Bits.h"
#include "Compiler.h"






// convert bits  to string
std::string bits_to_string(const std::vector<Bit>& bits) {
	std::string s = "";
	for (Bit bit : bits) {
		s += bit ? '1' : '0'; // std::to_string(bit->value);
	}
	return s;
}

// test the circuit
//<input_01> : ex : "01"
void _test_circuit(Circuit& circuit, const char* inputs, const char* expected_result) //, const std::vector<Bit>& expected_result)
{
	// test the circuit
	std::cout << "Input : " << inputs;

	// set the inputs
	CInputs test_input;
	while (*inputs)
	{
		if (*inputs == '1')
			test_input.add_bit(true);
		else
			test_input.add_bit(false);
		inputs++;
	}

	// run
	circuit.reset();
	std::vector<Bit> test_result = circuit.run(test_input);

	// check outputs
	int i = 0;
	while (expected_result[i])
	{
		bool expected_bit_i = (expected_result[i] == '1');
		if (expected_bit_i != test_result[i])
		{
			// FAILED
			std::cout << " - FAILED ***\n";
			return;

		}
		i++;
	}


	// output result
	std::cout << " - PASSED : ";
	std::string s = bits_to_string(test_result);
	std::cout << s << "\n";
}


//const char* sTest = "bool main(bool a,bool b) { return a|b; }";
const char* sTest = "bool main(bool a,  bool b) { \n return a; \n}";

int main(int argc, char* argv[])
{
	// echo the command line
	std::copy(argv, argv + argc, std::ostream_iterator<char*>(std::cout, "\n"));

	// need help ?
	if (   argc<2 
		|| strcmp(argv[1], "-help")==0) {
		std::cout << "Usage : bitVM-compiler <source_file_name>\n";
		return 1;
	}

	// get file name from command line : 1s argument
	const char* file_name = argv[1];

	// open the file
	std::ifstream source_file(file_name);
	if (!source_file) {
		std::cout << "Cannot open file " << file_name << "\n";
		return 1;
	}
	// compile the source file
	Compiler compiler;
	Error error;
	//std::istringstream input(sTest);
	if (!compiler.compile(source_file, error)) {

		std::cout << "Compilation failed.\n";
		std::cout << error.message << "\n";
		return 1;
	}
	std::cout << "Compilation success.\n";
	// get the compiled program 
	Program& program = compiler.get_programm();

	// compile the circuit
	Circuit main_circuit;
	program.build_circuit(main_circuit);


	// declare inputs
	main_circuit.set_inputs(2);

	std::vector<Connection*> current_input = main_circuit.getInputs();

	// declare gates
	//Gate_NOT* gate_1 = new Gate_NOT();
	Gate_OR* gate_1 = new Gate_OR();
	// IN
	//std::array<Connection*,1> input_1 = { current_input[0] };
	std::array<Connection*,2> input_2 = { current_input[0], current_input[1] };
	// OUT = NOT IN
	//std::array<Connection*, 1> bits_result = gate_1->add_to_circuit( main_circuit, input_1);
	std::array<Connection*, 1> bits_result = gate_1->add_to_circuit( main_circuit, input_2);

	// "return" opcode
	// tells the connexions they are the output of the circuit
	std::vector<Connection*> bits_return({ bits_result[0] });
	main_circuit.add_output(bits_return);

	// test the circuit
	_test_circuit(main_circuit, "00","0");
	_test_circuit(main_circuit, "01","1");
	_test_circuit(main_circuit, "10","1");
	_test_circuit(main_circuit, "11","1");


}

