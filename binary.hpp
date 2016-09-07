#ifndef BINARY_HPP
#define BINARY_HPP

#include <array>
#include <memory>

#include "binaryreader.hpp"

struct ELFIdent {
	std::array<uint8_t, 4> ei_mag;
	uint8_t ei_class;
	uint8_t ei_data;
	uint8_t ei_version;
	uint8_t ei_osabi;
	uint8_t ei_abiversion;
};

struct ELFHeader {
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint64_t e_entry; // !
	uint64_t e_phoff; // !
	uint64_t e_shoff; // !
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
};

struct ProgHeader {
	uint32_t p_type;
	uint32_t p_flags; // !
	uint64_t p_offset;
	uint64_t p_vaddr;
	uint64_t p_paddr;
	uint64_t p_filesz;
	uint64_t p_memsz;
	uint64_t p_align;
};

struct SectionHeader {
	uint32_t sh_name;
	uint32_t sh_type;
	uint64_t sh_flags; // !
	uint64_t sh_addr; // !
	uint64_t sh_offset;
	uint64_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint64_t sh_addralign; // !
	uint64_t sh_entsize; // !
};

class Binary {
public:
	Binary(BinaryReader &br);

	bool decode();
	ELFIdent decode_elf_ident();
	ELFHeader decode_elf_header();
	ProgHeader decode_prog_header(size_t n);
	SectionHeader decode_section_header(size_t n);

	bool is_32bit();

	std::string elf_header();
	std::string prog_headers();
	std::string section_headers();

	std::string interpreter_path;
	std::string note;
private:
	ELFIdent ident;
	ELFHeader eheader;
	std::vector<ProgHeader> pheaders;
	std::vector<SectionHeader> sheaders;

	BinaryReader reader;
};

#endif /* BINARY_HPP */
