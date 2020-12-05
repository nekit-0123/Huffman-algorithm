#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <iostream>
#include <sstream>

#define END_OF_STREAM 256
#define ESCAPE 257
#define SYMBOL_COUNT 258 
#define NODE_TABLE_COUNT ((SYMBOL_COUNT * 2) - 1)
#define ROOT_NODE 0
#define MAX_WEIGHT 0x8000 
#define TRUE 1
#define FALSE 0
#define PACIFIER_COUNT 2047
#define NO_ERROR 0
#define BAD_FILE_NAME 1
#define BAD_ARGUMENT 2

struct node
{
	unsigned int weight; 
	int parent; 
	int child_is_leaf;
	int child;
};

typedef struct tree
{
	int leaf[SYMBOL_COUNT];
	int next_free_node; 	
	node nodes[NODE_TABLE_COUNT];
}
TREE;

typedef struct bit_file
{
	FILE* file;
	unsigned char mask;
	int rack;
	int pacifier_counter;
}
COMPRESSED_FILE;


#ifndef SEEK_END
#define SEEK_END 2
#endif

class CHuffman
{
public:
	CHuffman();
	virtual ~CHuffman();

	void fatal_error(const std::string& fmt);
	void print_ratios(std::string& input, const std::string& output);
protected:

	long file_size(const std::string& name);
	void InitializeTree(TREE* tree);
	void UpdateModel(TREE* tree, int c);
	void RebuildTree(TREE* tree);
	void swap_nodes(TREE* tree, int i, int j);
	void add_new_node(TREE* tree, int c);

	TREE Tree;
};


