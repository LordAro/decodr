#ifndef BINARYREADER_HPP
#define BINARYREADER_HPP

#include <string>
#include <vector>

class BinaryReader {
public:
	BinaryReader(const std::string &path);

	int8_t   read_int8();
	uint8_t  read_uint8();
	int16_t  read_int16();
	uint16_t read_uint16();
	int32_t  read_int32();
	uint32_t read_uint32();
	int64_t  read_int64();
	uint64_t read_uint64();

	std::vector<uint8_t> read_blob(size_t len);
	std::string read_str(size_t len);

	size_t size();
	size_t pos();
	void seek(size_t p);
	void skip(size_t n);

private:
	std::vector<uint8_t> buf;
	size_t buf_pos;
};

#endif /* BINARYREADER_HPP */
