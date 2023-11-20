// Creator: MaitreBitcoin
// Creation date: 23/10/2023
// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.

// Usage :
//   bitVM-compiler <source_file_name> <output_file_name>
//   bitVM-compiler <source_file_name> -run  a=1234,b=5678

// compile a program compatible with bitVM

#include <iostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>
#include "Circuit.h"
#include "Gate.h"
#include "Bits.h"
#include "Compiler.h"
#include "Test.h"

class CommandLine {
public:
	// -help : show help mesage and exit
	bool show_help = false;
	// -test : run all surface tests
	bool auto_test = false;
	// -run : run the program simulation
	bool run_simuation = false;
	std::string simulation_input;
	// source & destination file names
	std::string source_file_name;
	std::string destination_file_name;

public:
	// constructor
	CommandLine(int argc, char* argv[]);
};
// constructor : anlayse command line
CommandLine::CommandLine(int argc, char* argv[]) {
	// need help ?
	if (argc < 2
		|| strcmp(argv[1], "-help") == 0) {
		show_help = true;
		return ;
	}
	// run auto  ests ?
	if (strcmp(argv[1], "-test") == 0) {
		auto_test = true;
		return;
	}
	// run the progamm ?
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-run") == 0) {
			run_simuation = true;
			// the next arg should be input values
			if (i+1 < argc)
				simulation_input = argv[i + 1];
		}
	}
	// get file names from command line
	source_file_name = argv[1];
	if (argc>2)
		destination_file_name = argv[2];
}

// convert a string to a array of string 
// ex : "a,b,c" -> {"a","b","c"}
std::vector<std::string>  string_to_arrtay(std::string stringToSplit, char separator_char=',')
{
	// build a simple regex with 1 char
	char separator_str[] = { separator_char, 0 };
	std::regex separator_regex{ separator_str };
	// slitting
	std::vector<std::string> result( std::sregex_token_iterator(stringToSplit.begin(), stringToSplit.end(), separator_regex, -1), {} );
	return result;
}

// run a simumation circuit
//<inputs> : ex : "a=0xF234,b=true,c=12"
std::string _run_circuit(Circuit& circuit, std::string inputs) {
	
	if (inputs.size() == 0)
		throw Error("Missing inputs");
	
	// set the inputs
	CRunInputs test_input = circuit.get_run_inputs();
	std::vector<std::string> params = string_to_arrtay(inputs);
	for (std::string param : params) {
		// get the name and the value
		std::vector<std::string> param_parts = string_to_arrtay(param, '=');
		if (param_parts.size() != 2)
			throw Error("Invalid input : ", param);
		std::string input_name = param_parts[0];
		std::string input_value = param_parts[1];
		// set the value in the input
		test_input.set_varaible_value(input_name, input_value);
	}

	// run the circuit
	circuit.reset();
	std::vector<Bit> test_result = circuit.run(test_input);
	if (test_result.size() == 0) {
		return "void (no ouput)";
	}

	// convert résul to string
	if (test_result.size() == 1)
	{
		return test_result[0] ? "true" : "false";
	}
	// convert to hexa
	std::string result_hexa = "0x";
	for (int i = (int)test_result.size()-4; i>=0;i-=4) {
		// get 4 bits
		int n = test_result[i] + test_result[i+1] * 2 + test_result[i + 2] * 4 + test_result[i + 3] * 8;
		// convert to hexa
		char bit_hexa = n < 10 ? '0' + n : 'A' + n - 10;
		result_hexa += bit_hexa;
	}
	return result_hexa;
}
// run a simumation circuit
std::string run_circuit(Circuit& circuit, std::string inputs) {
	try {
		return _run_circuit(circuit, inputs);
	}
	catch (Error& error) {
		// FAILED
		std::cout << "Error : " << error.toString() << "\n";
		exit(1);
	}
}

// main entry point
int main(int argc, char* argv[])
{
	// analyse command line
	CommandLine command_line(argc, argv);

	// need help ?
	if (command_line.show_help) {
		std::cout << "Usage : bitVM-compiler <source_file_name> <output_file_name>\n";
		return 1;
	}
	// run tests ?
	if (command_line.auto_test) {
		run_all_test();
		return 0;
	}

	// ccmpile  the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file(command_line.source_file_name);
	if (!result.ok) {
		std::cout << "Compilation failed.\n";
		std::cout << result.error.toString() << "\n";
		return 1;
	}

	// if a simulation is requested
	if (command_line.run_simuation) {
		// run the simulation
		std::cout << "Running with : " << command_line.simulation_input << "\n";
		std::string simulation_result = _run_circuit(result.main_circuit(), command_line.simulation_input);
		// show the result
		std::cout << "Simulation result : " << simulation_result << "\n";
		return 0;
	}

	// if not file name : show the circuit on the console
	if (command_line.destination_file_name == "")
	{
		result.export_to_stream(std::cout);
		return 0; // OK
	}

	// save the circuit in a file
	// open the file to write and remplace if already exists
	std::ofstream  destination_file(command_line.destination_file_name, std::ios::out | std::ios::trunc);
	if (!destination_file) {
		std::cout << "Cannot save file " << command_line.destination_file_name;
		return 1;
	}
	result.export_to_stream(destination_file);
	destination_file.close();
	// succes, show stats about the circuit
	Compiler::Result::Stats stats = result.get_stats();
	std::cout << "Compilation ok\n";
	std::cout << command_line.destination_file_name << " generated.\n";
	std::cout << "# circuit : " << stats.nb_circuit		<< "\n";
	std::cout << "# inputs  : " << stats.nb_input		<< "\n";
	std::cout << "# outpus  : " << stats.nb_output		<< "\n";
	std::cout << "# gates   : " << stats.nb_gate		<< "\n";
	std::cout << "# wires   : " << stats.nb_connection  << "\n";
	
	return 0;
}

