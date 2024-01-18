#ifndef TREE_INCLUDED
#define TREE_INCLUDED
#include <tree.hxx>
#endif

string
treeToText(TreeNode* tree);

TreeNode*
stringToTreeNode(const string& str);

vector<TreeNode*>
fileToTreeNodes(const string& filePath, bool decompress = true);

void
treeNodesToFile(const vector<TreeNode*>& trees,
                const string& filePath,
                bool compress = true);