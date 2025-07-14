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
#include <crk/string.h>

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
/*
path_tree_node *create_table_tree(arc_Table &table)
{
	// possible improvements:
	//	- store all path_tree_node contiguously
	//	- store all strings contiguously
	char tok_proxy[256];

	path_tree_node *root = (path_tree_node*)malloc(sizeof(path_tree_node));
	root->child = NULL;
	root->sibling = NULL;
	strcpy(root->name, "root");

	//for (int j = 0; j < table.vfiles.size(); j++)
	for (auto &vf : table.vfiles)
	{
		//char *str = table->strings + table->vfiles[j].string_index;
		const char *str = table.strings[vf.string_index];
		strcpy(tok_proxy, str);

		char *pch;
		pch = strtok (tok_proxy,"\\/");

		path_tree_node **it = &root; 
		while (pch != NULL)
		{
			it = &((*it)->child);

			while ((*it) != NULL)
			{
				if (strcmp((*it)->name, pch) == 0)
					goto skip_alloc;
				it = &((*it)->sibling);
			}

			(*it) = (path_tree_node*)malloc(sizeof(path_tree_node));
			(*it)->child = NULL;
			(*it)->sibling = NULL;
			strcpy((*it)->name, pch);
		skip_alloc:
			pch = strtok (NULL, "\\/");
		}
	}
	return root;
}*/

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

#include <fstream>
#include <cassert>
#include <iostream>
#include <format>

int main(int argc, char **argv)
{
	mem_CreateContext();
	COM_REGISTER_CRK(string);

	if (argc <= 1)
		goto no_args;

	arc_CreateContext();
	arc_SetDataDirectory(argv[argc - 1]);
	arc_LoadData();
	if (!arc_IsDataLoaded())
	{
		std::cerr << "Could not load the game data!" << std::endl;
		return -1;
	}

	for (int i = 1; i < argc-1;)
	{
		if (strncmp(argv[i], "--list-clusters", 16) == 0)
		{
			auto clusters = arc_GetClusters();
			std::cout << std::format("{} clusters found:\n", clusters.count);
			for (int j = 0; j < clusters.count; j++)
			{
				std::cout << std::format("\tcluster #{} = \"{}\"\n", j, clusters[j].name);
			}
			std::flush(std::cout);
			return 0;
		}
		if (strncmp(argv[i], "--list-all-vfiles", 17) == 0)
		{
			auto vfiles = arc_GetVfiles();
			std::cout << std::format("{} virtual files found:\n", vfiles.count);
			for (int j = 0; j < vfiles.count; j++)
			{
				std::cout << std::format("\tvfile #{} = \"{}\"\n", j, vfiles[j].name);
			}
			std::flush(std::cout);
			return 0;
		}
		if (strncmp(argv[i], "--list-vfiles=", 14) == 0)
		{
			arc_Cluster cluster = arc_GetCluster(argv[i] + 14);
			printf("%i virtual files found:\n", cluster.vfile_count);
			auto vfiles = arc_GetVfiles();
			for (int j = 0; j < vfiles.count; j++)
			{
				if (cluster.id == vfiles[j].cluster_id)
					printf("\tvfile #%i = \"%s\"\n", vfiles[j].id+1, vfiles[j].name);
			}
			fflush(stdout);
			return 0;
		}
		if (strncmp(argv[i], "--tree-all", 10) == 0)
		{
			/*path_tree_node *root = create_table_tree(table);
			print_tree_recursive(root, 0, ~0);
			free_table_tree(root);
			printf("tree build\n");
			printf("\n");
			fflush(stdout);*/
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