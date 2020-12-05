#pragma once

#include "Huffman.h"

class CEncode : public CHuffman
{
public:

	CEncode();
	virtual ~CEncode();
	void CompressFile(FILE* input, COMPRESSED_FILE* output);
	COMPRESSED_FILE* OpenOutputCompressedFile(const std::string& name);

private:

	void CloseOutputCompressedFile(COMPRESSED_FILE* bit_file);
	void EncodeSymbol(TREE* tree, unsigned int c, COMPRESSED_FILE* output);
	void OutputBits(COMPRESSED_FILE* bit_file, unsigned long code, int count);
};


