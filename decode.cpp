#include "decode.h"

CDecode::CDecode()
{
}

CDecode::~CDecode()
{
}

//===========================================================================
COMPRESSED_FILE* CDecode::OpenInputCompressedFile(const std::string& name)
{

	 COMPRESSED_FILE*  compressed_file = (COMPRESSED_FILE*)
		calloc(1, sizeof(COMPRESSED_FILE));
	if (compressed_file == nullptr)
		return(compressed_file);
	compressed_file->file = fopen(name.c_str(), "rb");
	compressed_file->rack = 0;
	compressed_file->mask = 0x80;
	compressed_file->pacifier_counter = 0;
	return(compressed_file);
}

//===========================================================================
void CDecode::ExpandFile(COMPRESSED_FILE* input, FILE* output)
{
	InitializeTree(&Tree);
	int c(0);

	while ((c = DecodeSymbol(&Tree, input)) != END_OF_STREAM)
	{
		if (putc(c, output) == EOF)
			fatal_error("Error on output");
		UpdateModel(&Tree, c);
	}

	CloseInputCompressedFile(input);
}

//===========================================================================
void CDecode::CloseInputCompressedFile(COMPRESSED_FILE* compressed_file)
{
	fclose(compressed_file->file);
	free((char*)compressed_file);
}

//===========================================================================
int CDecode::DecodeSymbol(TREE* tree, COMPRESSED_FILE* input)
{
	int current_node = ROOT_NODE;
	while (!tree->nodes[current_node].child_is_leaf)
	{
		current_node = tree->nodes[current_node].child;
		current_node += InputBit(input);
	}
	int c = tree->nodes[current_node].child;
	if (c == ESCAPE)
	{
		c = (int)InputBits(input, 8);
		add_new_node(tree, c);
	}
	return(c);
}

//===========================================================================
int CDecode::InputBit(COMPRESSED_FILE* compressed_file)
{
	if (compressed_file->mask == 0x80)
	{
		compressed_file->rack = getc(compressed_file->file);
		if (compressed_file->rack == EOF)
			fatal_error("Error on InputBit!");
		if ((compressed_file->pacifier_counter++ &
			PACIFIER_COUNT) == 0)
			putc('.', stdout);
	}
	int value = compressed_file->rack & compressed_file->mask;
	compressed_file->mask >>= 1;
	if (compressed_file->mask == 0)
		compressed_file->mask = 0x80;
	return(value ? 1 : 0);
}

//===========================================================================
unsigned long CDecode::InputBits(COMPRESSED_FILE* compressed_file, int bit_count)
{
	unsigned long mask = 1L << (bit_count - 1);
	unsigned long return_value = 0;
	while (mask != 0)
	{
		if (compressed_file->mask == 0x80)
		{
			compressed_file->rack = getc(compressed_file->file);
			if (compressed_file->rack == EOF)
				fatal_error("Error on InputBits!");
			if ((compressed_file->pacifier_counter++ &
				PACIFIER_COUNT) == 0)
				putc('.', stdout);
		}
		if (compressed_file->rack & compressed_file->mask)
			return_value |= mask;
		mask >>= 1;
		compressed_file->mask >>= 1;
		if (compressed_file->mask == 0)
			compressed_file->mask = 0x80;
	}
	return (return_value);
}