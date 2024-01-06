#include <deps.hxx>

struct TreeNode
{
    string data;
    vector<TreeNode*> children;

    TreeNode(const string& value)
      : data(value)
    {
    }

    // Add a child to the current node
    void addChild(TreeNode* child) { children.push_back(child); }

    // Destructor to deallocate memory used by children
    ~TreeNode()
    {
        for (auto child : children) {
            delete child;
        }
    }
};
