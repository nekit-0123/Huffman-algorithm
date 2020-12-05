#include "encode.h"

CEncode::CEncode()
{
}

CEncode::~CEncode()
{
}

//===========================================================================
void CEncode::CompressFile(FILE* input, COMPRESSED_FILE* output)
{
	int c(0);

	InitializeTree(&Tree);
	while ((c = getc(input)) != EOF)
	{
		EncodeSymbol(&Tree, c, output);
		UpdateModel(&Tree, c);
	}
	EncodeSymbol(&Tree, END_OF_STREAM, output);

	CloseOutputCompressedFile(output);
}

//===========================================================================
void CEncode::EncodeSymbol(TREE* tree, unsigned int c, COMPRESSED_FILE* output)
{
	unsigned long code = 0;
	unsigned long current_bit = 1;
	int code_size = 0;
	int current_node = tree->leaf[c];
	if (current_node == -1)
		current_node = tree->leaf[ESCAPE];
	while (current_node != ROOT_NODE)
	{
		if ((current_node & 1) == 0)
			code |= current_bit;
		current_bit <<= 1;
		code_size++;
		current_node = tree->nodes[current_node].parent;
	}
	OutputBits(output, code, code_size);
	if (tree->leaf[c] == -1)
	{
		OutputBits(output, (unsigned long)c, 8);
		add_new_node(tree, c);
	}
}

//===========================================================================
void CEncode::OutputBits(COMPRESSED_FILE* compressed_file, unsigned long code, int count)
{
	unsigned long mask = 1L << (count - 1);
	while (mask != 0)
	{
		if (mask & code)
			compressed_file->rack |= compressed_file->mask;
		compressed_file->mask >>= 1;
		if (compressed_file->mask == 0)
		{
			if (putc(compressed_file->rack, compressed_file->file) !=
				compressed_file->rack)
				fatal_error("Error on OutputBits!");
			else if ((compressed_file->pacifier_counter++ &
				PACIFIER_COUNT) == 0)
				putc('.', stdout);
			compressed_file->rack = 0;
			compressed_file->mask = 0x80;
		}
		mask >>= 1;
	}
}

//===========================================================================
void CEncode::CloseOutputCompressedFile(COMPRESSED_FILE* compressed_file)
{
	if (compressed_file->mask != 0x80)
		if (putc(compressed_file->rack, compressed_file->file) !=
			compressed_file->rack)
			fatal_error("Error on close compressed file.");
	fclose(compressed_file->file);
	free((char*)compressed_file);
}

//===========================================================================
COMPRESSED_FILE* CEncode::OpenOutputCompressedFile(const std::string& name)
{
	 COMPRESSED_FILE*  compressed_file = (COMPRESSED_FILE*)
		calloc(1, sizeof(COMPRESSED_FILE));
	if (compressed_file == nullptr)
		return(compressed_file);
	compressed_file->file = fopen(name.c_str(), "wb");
	compressed_file->rack = 0;
	compressed_file->mask = 0x80;
	compressed_file->pacifier_counter = 0;
	return(compressed_file);
}