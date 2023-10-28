// Creator: MaitreBitcoin
// Creation date: 23/10/2023
// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.

// Usage :
//   bitVM-compiler <source_file_name> <output_file_name>

// compilee a program compatible with bitVM

// TODO List

// export format
// add to github
// readme.md
// literals
// array
//  + - * / % << >> & | ^ ~
// == != < > <= >=
// if
// loop

#include <iostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include "Circuit.h"
#include "Gate.h"
#include "Bits.h"
#include "Compiler.h"
#include "SufaceTest.h"

class CommandLine {
public:
	// -help : show help mesage and exit
	bool show_help = false;
	// -test : run all surface tests
	bool run_test = false;
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
	// run tests ?
	if (strcmp(argv[1], "-test") == 0) {
		run_test = true;
		return;
	}
	// get file names from command line
	source_file_name = argv[1];
	if (argc>2)
		destination_file_name = argv[2];
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
	if (command_line.run_test) {
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

	// if not file name : show the circuit on the console
	if (command_line.destination_file_name == "")
	{
		result.circuit.export_to_stream(std::cout);
		return 0; // OK
	}

	// save the circuit in a file
	// open the file to write and remplace if already exists
	std::ofstream  destination_file(command_line.destination_file_name, std::ios::out | std::ios::trunc);
	if (!destination_file) {
		std::cout << "Cannot save file " << command_line.destination_file_name;
		return 1;
	}
	result.circuit.export_to_stream(destination_file);
	destination_file.close();
	// succes, show stats about the circuit
	Circuit::Stats stats = result.circuit.get_stats();
	std::cout << "Compilation ok\n";
	std::cout << command_line.destination_file_name << " generated.\n";
	std::cout << "# inputs : " << stats.nb_inputs		<< "\n";
	std::cout << "# outpus : " << stats.nb_outputs		<< "\n";
	std::cout << "# gates  : " << stats.nb_gates		<< "\n";
	std::cout << "# wires  : " << stats.nb_connections  << "\n";

	
	return 0;
}

