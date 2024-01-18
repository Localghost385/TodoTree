#include <tree.hxx>
#include <vector>

/**
 * Generates a buffer of strings representing a tree structure.
 *
 * @param node The root node of the tree.
 * @param depth The current depth of the tree (default is 0).
 * @param parentIsLast Indicates if the parent node is the last child (default
 * is true).
 *
 * @return A vector of strings representing the tree structure.
 *
 * @throws None.
 */
vector<string>
generateBuffer(TreeNode* node, int depth = 0, bool parentIsLast = true)
{
    vector<string> lines;

    if (node == nullptr) {
        return lines;
    }

    int branchesToLeft = 0;
    bool blankBranch = false;
    string line;

    // Indentation for current node
    // Iterate through each level of the tree
    for (int i = 0; i < depth; i++) {
        // Check if it's the last level
        if (i == depth - 1) {
            // Print the appropriate symbol based on whether the parent is the
            // last child
            line += parentIsLast ? "├─ " : "└─ ";
        } else {
            // Check if the ancestor at this depth is the last child
            const TreeNode* ancestor = node;
            for (int j = 1; j < depth - i; ++j) {
                // Traverse up the tree to the ancestor at this depth
                ancestor = ancestor->parent;
            }
            // Print the appropriate symbol based on whether the ancestor is the
            // last child of its parent
            if (ancestor->parent &&
                ancestor == ancestor->parent->children.back()) {
                line += "   ";
                branchesToLeft++;
                blankBranch = true;
            } else {
                line += "│  ";
                branchesToLeft++;
                blankBranch = false;
            }
        }
    }

    // Print current node data
    if (node->data.length() < COLS - 44 - branchesToLeft * 3) {
        line += node->data;
        lines.push_back(line);
    } else {
        while (node->data.length() > COLS - 44 - branchesToLeft * 3) {
            lines.push_back(
              line + node->data.substr(0, COLS - 44 - branchesToLeft * 3));
            node->data = node->data.substr((COLS - 44 - branchesToLeft * 3) -
                                           branchesToLeft * 3);
            if (blankBranch) {
                for (int i = 0; i < branchesToLeft + 1; i++) {
                    line += "   ";
                }
            } else {
                for (int i = 0; i < branchesToLeft + 1; i++) {
                    line += "│  ";
                }
                line += node->data.substr(20);
                lines.push_back(line);
            }
            line.clear();
        }
    }

    // Recursively print children
    if (!node->collapsed) {
        size_t numChildren = node->children.size();
        for (size_t i = 0; i < numChildren; i++) {
            bool isLastChild = (i == numChildren - 1);
            vector<string> childLines =
              generateBuffer(node->children[i], depth + 1, !isLastChild);
            lines.insert(lines.end(), childLines.begin(), childLines.end());
        }
    }

    return lines;
}
/**
 * Generates the order of the nodes in a tree using depth-first search.
 *
 * @param root The root of the tree.
 *
 * @return A vector of TreeNodes representing the order of the nodes.
 *
 * @throws None.
 */
vector<TreeNode*>
orderTrees(TreeNode* root, vector<TreeNode*> order = {})
{
    order.push_back(root);

    for (TreeNode* child : root->children) {
        for (TreeNode* node : orderTrees(child)) {
            order.push_back(node);
        }
    }

    return order;
}

/**
 * Generates a binary tree with the specified number of levels and total nodes.
 *
 * @param levels The number of levels in the tree.
 * @param totalNodes The total number of nodes in the tree.
 *
 * @return A pointer to the root node of the generated tree.
 *
 * @throws None.
 */
TreeNode*
generateTree(int levels, int totalNodes)
{
    TreeNode* root = new TreeNode("1");
    vector<TreeNode*> queue = { root };
    int count = 1;

    while (count < totalNodes) {
        TreeNode* currentNode = queue[0];
        queue.erase(queue.begin());

        for (int i = 0; i < levels; i++) {
            count++;
            TreeNode* newNode = new TreeNode(to_string(count));
            currentNode->addChild(newNode);
            queue.push_back(newNode);
            if (count == totalNodes) {
                break;
            }
        }
    }

    return root;
}
