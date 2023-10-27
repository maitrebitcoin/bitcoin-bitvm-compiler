// Creator: MaitreBitcoin
// Creation date: 23/10/2023
// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.

// Usage :
//   bitVM-compiler <source_file_name>

// compilee a program compatible with bitVM

#include <iostream>
#include <sstream>
#include <fstream>
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
	// source file name
	std::string source_file_name;
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
	// get file name from command line : 1s argument
	source_file_name = argv[1];

}


// main entry point
int main(int argc, char* argv[])
{
	// analyse command line
	CommandLine command_line(argc, argv);

	// need help ?
	if (command_line.show_help) {
		std::cout << "Usage : bitVM-compiler <source_file_name>\n";
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

	// TODO
	// save the circuit in a file
	auto s = result.circuit.export_to_string();
	std::cout << s;
	return 0;
}

