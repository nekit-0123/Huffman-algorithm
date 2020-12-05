#include "Huffman.h"

CHuffman::CHuffman()
{

}
CHuffman::~CHuffman() 
{

}

//===========================================================================
void CHuffman::InitializeTree(TREE* tree)
{
	tree->nodes[ROOT_NODE].child = ROOT_NODE + 1;
	tree->nodes[ROOT_NODE].child_is_leaf = FALSE;
	tree->nodes[ROOT_NODE].weight = 2;
	tree->nodes[ROOT_NODE].parent = -1;
	tree->nodes[ROOT_NODE + 1].child = END_OF_STREAM;
	tree->nodes[ROOT_NODE + 1].child_is_leaf = TRUE;
	tree->nodes[ROOT_NODE + 1].weight = 1;
	tree->nodes[ROOT_NODE + 1].parent = ROOT_NODE;
	tree->leaf[END_OF_STREAM] = ROOT_NODE + 1;
	tree->nodes[ROOT_NODE + 2].child = ESCAPE;
	tree->nodes[ROOT_NODE + 2].child_is_leaf = TRUE;
	tree->nodes[ROOT_NODE + 2].weight = 1;
	tree->nodes[ROOT_NODE + 2].parent = ROOT_NODE;
	tree->leaf[ESCAPE] = ROOT_NODE + 2;
	tree->next_free_node = ROOT_NODE + 3;

	for (int i = 0; i < END_OF_STREAM; i++)
		tree->leaf[i] = -1;
}

//===========================================================================
void CHuffman::fatal_error(const std::string& fmt)
{
	std::cout<< "Fatal error: " << fmt << std::endl;
	exit(-1);
};

//===========================================================================
void CHuffman::UpdateModel(TREE* tree, int c)
{
	int new_node(0);
	if (tree->nodes[ROOT_NODE].weight == MAX_WEIGHT)
		RebuildTree(tree);
	int current_node = tree->leaf[c];
	while

		(current_node != -1)
	{
		tree->nodes[current_node].weight++;
		for (new_node = current_node; new_node > ROOT_NODE; new_node--)
			if (tree->nodes[new_node - 1].weight >=
				tree->nodes[current_node].weight)
				break;
		if (current_node != new_node)
		{
			swap_nodes(tree, current_node, new_node);
			current_node = new_node;
		}
		current_node = tree->nodes[current_node].parent;
	}
}

//===========================================================================
void CHuffman::RebuildTree(TREE* tree)
{
	int i(0);
	int k(0);
	unsigned int weight(0);

	int j = tree->next_free_node - 1;
	for (i = j; i >= ROOT_NODE; i--)
	{
		if (tree->nodes[i].child_is_leaf)
		{
			tree->nodes[j] = tree->nodes[i];
			tree->nodes[j].weight =
				(tree->nodes[j].weight + 1) / 2;
			j--;
		}
	}

	for (i = tree->next_free_node - 2; j >= ROOT_NODE; i -= 2, j--)
	{
		k = i + 1;
		tree->nodes[j].weight =
			tree->nodes[i].weight + tree->nodes[k].weight;
		weight = tree->nodes[j].weight;
		tree->nodes[j].child_is_leaf = FALSE;
		for (k = j + 1; weight < tree->nodes[k].weight; k++)
			;
		k--;
		memmove(&tree->nodes[j], &tree->nodes[j + 1],
			(k - j) * sizeof(struct node));
		tree->nodes[k].weight = weight;
		tree->nodes[k].child = i;
		tree->nodes[k].child_is_leaf = FALSE;
	}

	for (i = tree->next_free_node - 1; i >= ROOT_NODE; i--)
	{
		if (tree->nodes[i].child_is_leaf)
		{
			k = tree->nodes[i].child;
			tree->leaf[k] = i;
		}
		else
		{
			k = tree->nodes[i].child;
			tree->nodes[k].parent =
				tree->nodes[k + 1].parent = i;
		}
	}
}

//===========================================================================
void CHuffman::swap_nodes(TREE* tree, int i, int j)
{
	if (tree->nodes[i].child_is_leaf)
		tree->leaf[tree->nodes[i].child] = j;
	else
	{
		tree->nodes[tree->nodes[i].child].parent = j;
		tree->nodes[tree->nodes[i].child + 1].parent = j;
	}
	if (tree->nodes[j].child_is_leaf)
		tree->leaf[tree->nodes[j].child] = i;
	else
	{
		tree->nodes[tree->nodes[j].child].parent = i;
		tree->nodes[tree->nodes[j].child + 1].parent = i;
	}
	node temp = tree->nodes[i];
	tree->nodes[i] = tree->nodes[j];
	tree->nodes[i].parent = temp.parent;
	temp.parent = tree->nodes[j].parent;
	tree->nodes[j] = temp;
}

void CHuffman::add_new_node(TREE* tree, int c)
{
	int lightest_node = tree->next_free_node - 1;
	int new_node = tree->next_free_node;
	int zero_weight_node = tree->next_free_node + 1;
	tree->next_free_node += 2;

	tree->nodes[new_node] = tree->nodes[lightest_node];
	tree->nodes[new_node].parent = lightest_node;
	tree->leaf[tree->nodes[new_node].child] = new_node;

	tree->nodes[lightest_node].child = new_node;
	tree->nodes[lightest_node].child_is_leaf = FALSE;

	tree->nodes[zero_weight_node].child = c;
	tree->nodes[zero_weight_node].child_is_leaf = TRUE;
	tree->nodes[zero_weight_node].weight = 0;
	tree->nodes[zero_weight_node].parent = lightest_node;
	tree->leaf[c] = zero_weight_node;
}

//===========================================================================
long CHuffman::file_size(const std::string& name)
{
	FILE* file(nullptr);
	file = fopen(name.c_str(), "r");
	if (file == nullptr)
		return(0);
	fseek(file, 0L, SEEK_END);
	long eof_ftell = ftell(file);
	fclose(file);
	return(eof_ftell);
}

//===========================================================================
void CHuffman::print_ratios(std::string& input, const std::string& output)
{
	long input_size = file_size(input);
	if (input_size == 0)
		input_size = 1;
	long output_size = file_size(output);
	int ratio = 100 - (int)(output_size * 100L / input_size);
	std::cout << "Source file size:" << input_size << std::endl;;
	std::cout << "Target Filesize:" << output_size << std::endl;;

	if (output_size == 0)
		output_size = 1;
	std::cout << "Compression ratio:" << ratio << std::endl;;
}