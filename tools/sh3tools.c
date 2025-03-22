/*
 *	A command-line tool for sh3 modding
 */

const char *no_args_msg = 
"USAGE: sh3tools [options] [<data_path>]\n\n"
"OPTIONS:\n"
//"\t--extract-cluster=<string>\n"
//"\t\tExtracts every virtual file inside the selected cluster.\n"
//"\t--extract-all\n"
//"\t\tExtracts all virtual files.\n"
"\t--list-clusters\n"
"\t\tPrints all cluster names.\n"
"\t--list-all-vfiles\n"
"\t\tPrints all virtual files.\n"
"\t--list-vfiles=<string>\n"
"\t\tPrints all virtual files inside <string> cluster.\n"
"\t--tree-all\n"
"\t\tPrints a tree representation of all virtual files.\n"
"\t--tree-cluster=<string>\n"
"\t\tPrints a tree representation of the cluster's virtual files.\n"
;

#include <sh3modlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct path_tree_node
{
	char name[64];
	struct path_tree_node *child;
	struct path_tree_node *sibling;
};

void free_table_tree(struct path_tree_node *root)
{
	if (root == NULL)
		return;
	free_table_tree(root->child);
	free_table_tree(root->sibling);
	free(root);
}

struct path_tree_node *create_table_tree(struct arc_table *table)
{
	// possible improvements:
	//	- store all path_tree_node contiguously
	//	- store all strings contiguously
	char tok_proxy[256];

	struct path_tree_node *root = malloc(sizeof(struct path_tree_node));
	root->child = NULL;
	root->sibling = NULL;
	strcpy(root->name, "root");

	for (int j = 0; j < table->vfile_count; j++)
	{
		char *str = table->strings + table->vfiles[j].string_index;
		strcpy(tok_proxy, str);

		char *pch;
		pch = strtok (tok_proxy,"\\/");

		struct path_tree_node **it = &root; 
		while (pch != NULL)
		{
			it = &((*it)->child);

			while ((*it) != NULL)
			{
				if (strcmp((*it)->name, pch) == 0)
					goto skip_alloc;
				it = &((*it)->sibling);
			}

			(*it) = malloc(sizeof(struct path_tree_node));
			(*it)->child = NULL;
			(*it)->sibling = NULL;
			strcpy((*it)->name, pch);
		skip_alloc:
			pch = strtok (NULL, "\\/");
		}
	}
	return root;
}

void print_tree_recursive(struct path_tree_node *node, int depth, uint64_t endmask)
{
#ifdef EXTENDED_CHARS
	#define TREE_DOWN 	"│   "
	#define TREE_DOWN_END 	"    "
	#define TREE_RIGHT 	"├───"
	#define TREE_RIGHT_END 	"└───"
#else
	#define TREE_DOWN 	"|   "
	#define TREE_DOWN_END 	"    "
	#define TREE_RIGHT 	"|---"
	#define TREE_RIGHT_END 	"|---"
#endif
	if (node == NULL) return;
	for (int i = 0; i < depth; i++)
		if (endmask & (1 << i))
			printf(TREE_DOWN);
		else
			printf(TREE_DOWN_END);
	uint64_t new_mask = endmask;
	if (node->sibling)
	{
		printf(TREE_RIGHT"%s\n", node->name);
	}
	else
	{
		printf(TREE_RIGHT_END"%s\n", node->name);
		new_mask &= ~(1 << depth);
	}
	print_tree_recursive(node->child, depth+1, new_mask);
	if (!node->sibling && endmask & (1 << (depth - 1)))
	{
		for (int i = 0; i < depth; i++)
			if (endmask & (1 << i))
				printf(TREE_DOWN);
			else
				printf(TREE_DOWN_END);
		printf("\n");
	}
	print_tree_recursive(node->sibling, depth, endmask);
}

int main(int argc, char **argv)
{
	if (argc <= 1)
		goto no_args;

	struct arc_ctx _arc_ctx;

	strncpy(_arc_ctx.data_path, argv[argc - 1], 255);
	_arc_ctx.data_path[255] = '\0';

	struct arc_table table = {0};
	if (arc_table_load(&_arc_ctx, &table) != 0)
	{
		printf("error: %s\n", _arc_ctx.msg);
		return 0;
	}

	for (int i = 1; i < argc-1;)
	{
		if (strncmp(argv[i], "--list-clusters", 16) == 0)
		{
			printf("%llu clusters found:\n", table.cluster_count);
			for (int j = 0; j < table.cluster_count; j++)
			{
				printf("\tcluster #%i = \"%s\"\n", j+1, table.strings + table.clusters[j].string_index);
			}
			fflush(stdout);
			return 0;
		}
		if (strncmp(argv[i], "--list-all-vfiles", 17) == 0)
		{
			printf("%llu virtual files found:\n", table.vfile_count);
			for (int j = 0; j < table.vfile_count; j++)
			{
				printf("\tvfile #%i = \"%s\"\n", j+1, table.strings + table.vfiles[j].string_index);
			}
			fflush(stdout);
			return 0;
		}
		if (strncmp(argv[i], "--list-vfiles=", 14) == 0)
		{
			int c_idx = arc_table_get_cluster_index(&table, (argv[i] + 14));
			printf("%i virtual files found:\n", table.clusters[c_idx].vfile_count);
			for (int j = 0; j < table.vfile_count; j++)
			{
				if (c_idx == table.vfiles[j].cluster_index)
					printf("\tvfile #%i = \"%s\"\n", table.vfiles[j].index+1, table.strings + table.vfiles[j].string_index);
			}
			fflush(stdout);
			return 0;
		}
		if (strncmp(argv[i], "--tree-all", 10) == 0)
		{
			struct path_tree_node *root = create_table_tree(&table);
			print_tree_recursive(root, 0, ~0);
			free_table_tree(root);
			printf("tree build\n");
			printf("\n");
			fflush(stdout);
			return 0;
		}
		if (strncmp(argv[i], "--tree-cluster", 14) == 0)
		{
			/*struct path_tree_node *root = create_table_tree(&table);
			print_tree_recursive(root, 0, ~0);
			free_table_tree(root);
			printf("tree build\n");
			printf("\n");
			fflush(stdout);
			return 0;*/
		}
		i++;
	}

no_args:
	printf("%s", no_args_msg);
	fflush(stdout);
	return 0;
}