#include "binary.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>


template<class T>
std::string to_hexstring(T num)
{
	constexpr char hexmap[] = "0123456789abcdef";
	std::string out = "";
	while (num > 0) {
		T rem = num % 16;
		num /= 16;
		out += hexmap[rem];
	}
	if (out.size() == 0) out += '0';
	for (int i = 0, j = out.size() - 1; i < j; i++, j--) {
		char c = out[i];
		out[i] = out[j];
		out[j] = c;
	}
	return "0x" + out;
}

Binary::Binary(BinaryReader &br) : reader(br)
{
}

bool Binary::decode()
{
	this->ident  = this->decode_elf_ident();
	this->eheader = this->decode_elf_header();
	for (uint64_t i = 0; i < this->eheader.e_phnum; i++) {
		this->pheaders.push_back(this->decode_prog_header(i));
	}
	for (uint64_t i = 0; i < this->eheader.e_shnum; i++) {
		this->sheaders.push_back(this->decode_section_header(i));
	}
	return true;
}

/** Consumes 16 bytes */
ELFIdent Binary::decode_elf_ident()
{
	assert(this->reader.pos() == 0);
	ELFIdent ident;
	auto blob = this->reader.read_blob(4);
	std::copy_n(blob.begin(), 4, ident.ei_mag.begin());

	ident.ei_class      = this->reader.read_uint8();
	ident.ei_data       = this->reader.read_uint8();
	ident.ei_version    = this->reader.read_uint8();
	ident.ei_osabi      = this->reader.read_uint8();
	ident.ei_abiversion = this->reader.read_uint8();

	this->reader.skip(7);
	assert(this->reader.pos() == 16);
	return ident;
}

ELFHeader Binary::decode_elf_header()
{
	assert(this->reader.pos() == 16);
	ELFHeader header;
	header.e_type = this->reader.read_uint16();
	header.e_machine = this->reader.read_uint16();
	header.e_version = this->reader.read_uint32();
	if (this->is_32bit()) {
		header.e_entry = this->reader.read_uint32();
		header.e_phoff = this->reader.read_uint32();
		header.e_shoff = this->reader.read_uint32();
	} else {
		header.e_entry = this->reader.read_uint64();
		header.e_phoff = this->reader.read_uint64();
		header.e_shoff = this->reader.read_uint64();
	}
	header.e_flags     = this->reader.read_uint32();
	header.e_ehsize    = this->reader.read_uint16();
	header.e_phentsize = this->reader.read_uint16();
	header.e_phnum     = this->reader.read_uint16();
	header.e_shentsize = this->reader.read_uint16();
	header.e_shnum     = this->reader.read_uint16();
	header.e_shstrndx  = this->reader.read_uint16();
	return header;
}

ProgHeader Binary::decode_prog_header(size_t n)
{
	this->reader.seek(this->eheader.e_phoff + (n * this->eheader.e_phentsize));
	ProgHeader header;
	header.p_type = this->reader.read_uint32();
	// Everything else different
	if (this->is_32bit()) {
		header.p_offset = this->reader.read_uint32();
		header.p_vaddr  = this->reader.read_uint32();
		header.p_paddr  = this->reader.read_uint32();
		header.p_filesz = this->reader.read_uint32();
		header.p_memsz  = this->reader.read_uint32();
		header.p_flags  = this->reader.read_uint32();
		header.p_align  = this->reader.read_uint32();
	} else {
		header.p_flags  = this->reader.read_uint32();
		header.p_offset = this->reader.read_uint64();
		header.p_vaddr  = this->reader.read_uint64();
		header.p_paddr  = this->reader.read_uint64();
		header.p_filesz = this->reader.read_uint64();
		header.p_memsz  = this->reader.read_uint64();
		header.p_align  = this->reader.read_uint64();
	}
	assert(this->reader.pos() == this->eheader.e_phoff + ((n + 1) * this->eheader.e_phentsize));

	this->reader.seek(header.p_offset);
	switch (header.p_type) {
		case 0x03: // PT_INTERP
			this->interpreter_path = this->reader.read_str(header.p_filesz);
			break;
		case 0x04: // PT_NOTE
			this->note = this->reader.read_str(header.p_filesz);
			break;
		case 0x00: // PT_NULL
		case 0x01: // PT_LOAD
		case 0x02: // PT_DYNAMIC
		case 0x05: // PT_SHLIB
		case 0x06: // PT_PHDR
		case 0x07: // PT_TLS
			break;
		default:
			std::cerr << "Unknown program header type: " << to_hexstring(header.p_type) << "\n";
			break;
	}
	return header;
}

SectionHeader Binary::decode_section_header(size_t n)
{
	this->reader.seek(this->eheader.e_shoff + (n * this->eheader.e_shentsize));
	SectionHeader header;

	header.sh_name = this->reader.read_uint32();
	header.sh_type = this->reader.read_uint32();
	if (this->is_32bit()) {
		header.sh_flags     = this->reader.read_uint32();
		header.sh_addr      = this->reader.read_uint32();
		header.sh_offset    = this->reader.read_uint32();
		header.sh_size      = this->reader.read_uint32();
		header.sh_link      = this->reader.read_uint32();
		header.sh_info      = this->reader.read_uint32();
		header.sh_addralign = this->reader.read_uint32();
		header.sh_entsize   = this->reader.read_uint32();
	} else {
		header.sh_flags     = this->reader.read_uint64();
		header.sh_addr      = this->reader.read_uint64();
		header.sh_offset    = this->reader.read_uint64();
		header.sh_size      = this->reader.read_uint64();
		header.sh_link      = this->reader.read_uint32();
		header.sh_info      = this->reader.read_uint32();
		header.sh_addralign = this->reader.read_uint64();
		header.sh_entsize   = this->reader.read_uint64();
	}
	assert(this->reader.pos() == this->eheader.e_shoff + ((n + 1) * this->eheader.e_shentsize));
	return header;
}

bool Binary::is_32bit()
{
	return this->ident.ei_class == 1;
}

std::string Binary::elf_header()
{
	std::string out;
	out += "ELF Header:\n";
	out += "  ELF Identification:\n";
	auto magic_str = std::string(this->ident.ei_mag.begin(), this->ident.ei_mag.end());
	out += "    Magic:       " + magic_str + "\n";
	out += "    Class:       " + std::to_string(this->ident.ei_class)      + "\n";
	out += "    Data:        " + std::to_string(this->ident.ei_data)       + "\n";
	out += "    Version:     " + std::to_string(this->ident.ei_version)    + "\n";
	out += "    OS ABI:      " + to_hexstring(this->ident.ei_osabi)        + "\n";
	out += "    ABI Version: " + std::to_string(this->ident.ei_abiversion) + "\n";
	out += "  Type:                 " + std::to_string(this->eheader.e_type)      + "\n";
	out += "  Machine:              " + to_hexstring(this->eheader.e_machine)     + "\n";
	out += "  Version:              " + std::to_string(this->eheader.e_version)   + "\n";
	out += "  Entry point:          " + std::to_string(this->eheader.e_entry)     + "\n";
	out += "  Program header off:   " + std::to_string(this->eheader.e_phoff)     + "\n";
	out += "  Section header off:   " + std::to_string(this->eheader.e_shoff)     + "\n";
	out += "  Flags:                " + to_hexstring(this->eheader.e_flags)       + "\n";
	out += "  ELF header size:      " + std::to_string(this->eheader.e_ehsize)    + "\n";
	out += "  Program header size:  " + std::to_string(this->eheader.e_phentsize) + "\n";
	out += "  Program header num:   " + std::to_string(this->eheader.e_phnum)     + "\n";
	out += "  Section header size:  " + std::to_string(this->eheader.e_shentsize) + "\n";
	out += "  Section header num:   " + std::to_string(this->eheader.e_shnum)     + "\n";
	out += "  Section header index: " + std::to_string(this->eheader.e_shstrndx)  + "\n";
	return out;
}

std::string Binary::prog_headers()
{
	std::string out;
	for (size_t i = 0; i < this->pheaders.size(); i++) {
		ProgHeader *header = &this->pheaders[i];
		uint32_t flags = header->p_flags;
		out += "Program Header " + std::to_string(i) + "\n";
		out += "  Type:         " + to_hexstring(header->p_type)   + "\n";
		out += "  Flags:        " + to_hexstring(header->p_flags);
		out += std::string(" (") + (flags & 0x04 ? 'R' : ' ') + (flags & 0x02 ? 'W' : ' ') + (flags & 0x01 ? 'X' : ' ') + ")\n";
		out += "  Offset:       " + to_hexstring(header->p_offset) + "\n";
		out += "  Virt address: " + to_hexstring(header->p_vaddr)  + "\n";
		out += "  Phys address: " + to_hexstring(header->p_paddr)  + "\n";
		out += "  File size:    " + to_hexstring(header->p_filesz) + "\n";
		out += "  Memory size:  " + to_hexstring(header->p_memsz) + "\n";
		out += "  Alignment:    " + to_hexstring(header->p_align)  + "\n";
	}
	return out;
}

std::string section_name(const std::vector<uint8_t> &string_names, size_t n)
{
	// ick.
	const char *p = reinterpret_cast<const char *>(string_names.data() + n);
	return std::string(p);
}

std::string Binary::section_headers()
{
	SectionHeader *strheader = &this->sheaders[this->eheader.e_shstrndx];
	this->reader.seek(strheader->sh_offset);
	auto str_names = this->reader.read_blob(strheader->sh_size);

	std::string out;
	for (size_t i = 0; i < this->sheaders.size(); i++) {
		SectionHeader *header = &this->sheaders[i];
		std::string name = section_name(str_names, header->sh_name);
		out += "Section Header " + std::to_string(i) + "\n";
		out += "  Name index: " + to_hexstring(header->sh_name)      + "\n";
		out += "  Name:       " + name + "\n";
		out += "  Type:       " + to_hexstring(header->sh_type)      + "\n";
		out += "  Flags:      " + to_hexstring(header->sh_flags)     + "\n";
		out += "  Address:    " + to_hexstring(header->sh_addr)      + "\n";
		out += "  Offset:     " + to_hexstring(header->sh_offset)    + "\n";
		out += "  Size:       " + to_hexstring(header->sh_size)      + "\n";
		out += "  Link:       " + to_hexstring(header->sh_link)      + "\n";
		out += "  Info:       " + to_hexstring(header->sh_info)      + "\n";
		out += "  Align:      " + to_hexstring(header->sh_addralign) + "\n";
		out += "  Entry size: " + to_hexstring(header->sh_entsize)   + "\n";
		if (name == ".comment") {
			this->reader.seek(header->sh_offset);
			auto content = this->reader.read_blob(header->sh_size);
			for (auto &i : content) if (i == '\0') i = ' ';
			const char *p = reinterpret_cast<const char *>(content.data());
			out += "  Content:    \"" + std::string(p) + "\"\n";
		}
	}
	return out;
}
