#include <deps.hxx>

struct TreeNode
{
    string data;
    TreeNode* parent = nullptr;
    bool completed = false;
    bool collapsed = false;
    vector<TreeNode*> children;

    TreeNode(const string& value)
      : data(value)
    {
    }

    // Add a child to the current node
    void addChild(TreeNode* child)
    {
        children.push_back(child);
        child->addParent(this);
    }
    void addParent(TreeNode* parent) { this->parent = parent; }

    // Destructor to deallocate memory used by children
    ~TreeNode()
    {
        for (auto child : children) {
            delete child;
        }
    }
};
