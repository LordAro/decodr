#include "binaryreader.hpp"

#include <fstream>
#include <iterator>

BinaryReader::BinaryReader(const std::string &path) : buf_pos(0)
{
	auto file = std::ifstream(path, std::ios::binary);
	if (!file) throw "Error opening " + path;
	// I'm reading binary file, why on earth would I want to skip whitespace?
	file.unsetf(std::ios_base::skipws);
	std::istream_iterator<uint8_t> begin(file), end;
	this->buf = std::vector<uint8_t>(begin, end);
}

uint8_t BinaryReader::read_uint8()
{
	return this->buf[this->buf_pos++];
}

int8_t BinaryReader::read_int8()
{
	return static_cast<int8_t>(this->read_uint8());
}

uint16_t BinaryReader::read_uint16()
{
	uint8_t b = this->read_uint8();
	return (static_cast<uint16_t>(this->read_uint8()) << 8) | b;
}

int16_t BinaryReader::read_int16()
{
	return static_cast<int16_t>(this->read_uint16());
}

uint32_t BinaryReader::read_uint32()
{
	uint16_t w = this->read_uint16();
	return (static_cast<uint32_t>(this->read_uint16()) << 16) | w;
}

int32_t BinaryReader::read_int32()
{
	return static_cast<int32_t>(this->read_uint32());
}

uint64_t BinaryReader::read_uint64()
{
	uint32_t dw = this->read_uint32();
	return (static_cast<uint64_t>(this->read_uint32()) << 32) | dw;
}

int64_t BinaryReader::read_int64()
{
	return static_cast<int64_t>(this->read_uint64());
}

std::vector<uint8_t> BinaryReader::read_blob(size_t len)
{
	auto cur_it = this->buf.begin() + this->buf_pos;
	auto blob = std::vector<uint8_t>(cur_it, cur_it + len);
	this->buf_pos += len;
	return blob;
}

std::string BinaryReader::read_str(size_t len)
{
	std::vector<uint8_t> blob = this->read_blob(len);
	return std::string(blob.begin(), blob.end());
}

size_t BinaryReader::size()
{
	return this->buf.size();
}

size_t BinaryReader::pos()
{
	return this->buf_pos;
}

void BinaryReader::seek(size_t p)
{
	if (p >= this->size()) throw "Seek out of bounds";
	this->buf_pos = p;
}

void BinaryReader::skip(size_t n)
{
	// TODO: bounds
	this->buf_pos += n;
}

