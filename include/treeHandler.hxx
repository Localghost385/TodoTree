#ifndef TREE_INCLUDED
#define TREE_INCLUDED
#include <tree.hxx>
#include <vector>
#endif

vector<string>
generateBuffer(TreeNode* node, int depth = 0, bool parentIsLast = true);

vector<TreeNode*>
orderTrees(TreeNode* root, vector<TreeNode*> order = {});

TreeNode*
generateTree(int levels, int totalNodes);