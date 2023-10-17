// bitVM-compiler.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include "Circuit.h"
#include "Gate.h"
#include "Bits.h"


const char* sTest = "bool main(bool a,bool b) { return a|b; }";



// convert bits  to string
std::string bits_to_string(const std::vector<Bit>& bits) {
	std::string s = "";
	for (Bit bit : bits) {
		s += bit ? '1' : '0'; // std::to_string(bit->value);
	}
	return s;
}





int main()
{
	Circuit main_circuit;
	// compile the circuit

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
	std::cout << "Runnig...\n";
	// set the inputs
	CInputs test_input;
	test_input.add_bit(true);
	test_input.add_bit(true);
	//test_input.add_bit(false);
	std::vector<Bit> test_result = main_circuit.run(test_input);
	std::string s = bits_to_string(test_result);
	std::cout << "\n";
	std::cout << s;

	// done
	std::cout << "\n\n...Done";

}

