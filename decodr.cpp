#include <iostream>
#include <iomanip>
#include <vector>

#include "binary.hpp"
#include "binaryreader.hpp"

static const std::string HELPTEXT = "decodr - a thing that decodes binary stuff\n"
	"\n"
	"usage: decodr file\n"
	"\n"
	"file - elf binary file to decode\n"
	"\n";


int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cout << HELPTEXT;
		return 1;
	}
	std::string fpath(argv[1]);
	if (fpath == "-h" || fpath == "--help") {
		std::cout << HELPTEXT;
		return 0;
	}

	try {
		auto bin_reader = BinaryReader(argv[1]);
		std::cout << "Decoding " << fpath << "\n";
		std::cout << "Size: " << bin_reader.size() << " bytes\n";

		auto binary = Binary(bin_reader);
		binary.decode();
		std::cout << binary.elf_header() << "\n";
		std::cout << binary.prog_headers() << "\n";
		std::cout << "Interpreter Path: " << binary.interpreter_path << "\n";
		std::cout << "Note text: " << binary.note << "\n";
		std::cout << binary.section_headers() << "\n";
	} catch (const std::string &e) {
		std::cerr << e << "\n";
		return 1;
	}
/*
	for (size_t i = 0; i < binary.size(); i++) {
		if (i % 16 == 0) std::cout << "\n";
		std::cout << std::hex << std::setfill('0') << std::setw(2) << (uint32_t)binary.read_uint8();
		if (i % 2 == 1) std:: cout << ' ';
	}*/
}
