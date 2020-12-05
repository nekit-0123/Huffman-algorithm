#pragma once

#include "Huffman.h"

class CDecode : public CHuffman
{
public:

	CDecode();
	virtual ~CDecode();	
	void ExpandFile(COMPRESSED_FILE* input, FILE* output);
	COMPRESSED_FILE* OpenInputCompressedFile(const std::string& name);

private:

	void CloseInputCompressedFile(COMPRESSED_FILE* bit_file);
	int DecodeSymbol(TREE* tree, COMPRESSED_FILE* input);
	int InputBit(COMPRESSED_FILE* bit_file);
	unsigned long InputBits(COMPRESSED_FILE* bit_file, int bit_count);
};

