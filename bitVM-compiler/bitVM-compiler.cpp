// bitVM-compiler.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <sstream>
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


const char* sTest = "bool main(bool a,bool b) { return a|b; }";

int main()
{
	Circuit main_circuit;
	// compile the circuit


	Compiler compiler;
	Compiler::Error error;
	std::istringstream input(sTest);
	if (!compiler.compile(input, error)) {

		std::cout << "Compilation failed.\n";
		std::cout << error.message << "\n";
		return 1;
	}

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

