#include "snappy.h"
#include <deps.hxx>
#include <fstream>
#include <ostream>
#include <tree.hxx>
#include <vector>

/**
 * Converts a given tree data structure into a text representation.
 *
 * @param tree The root node of the tree.
 *
 * @return The text representation of the tree.
 *
 * @throws None
 */
string
treeNodeToString(TreeNode* root)
{
    if (!root)
        return "0[]";

    string result = "\"" + root->data + "\"0[";
    for (const auto& child : root->children) {
        result += treeNodeToString(child);
    }
    result += "]";
    return result;
}

/**
 * Converts a string representation of a binary tree into a TreeNode object.
 *
 * @param str the string representation of the binary tree
 *
 * @return a TreeNode pointer representing the root of the binary tree
 *
 * @throws None
 */
TreeNode*
parseTreeNode(const string& str, size_t& pos)
{
    // Find the beginning of the node name
    size_t start = str.find('"', pos);
    if (start == string::npos)
        return nullptr; // No starting " found
    // Find the end of the node name
    size_t end = str.find('"', start + 1);
    if (end == string::npos)
        return nullptr; // No ending " found

    // Extract the node name
    string nodeName = str.substr(start + 1, end - start - 1);
    pos = end + 1; // Move past the end of the node name

    // Create a new TreeNode with the extracted name
    TreeNode* node = new TreeNode(nodeName);

    // Check for the completion status (0 or 1)
    if (str[pos] == '0') {
        node->completed = false;
    } else if (str[pos] == '1') {
        node->completed = true;
    }
    pos++; // Move past the completion status

    // Check if there is a children list starting with '['
    if (str[pos] == '[') {
        pos++; // Move past the '['
        // Recursively parse children until ']' is found
        while (str[pos] != ']') {
            TreeNode* childNode = parseTreeNode(str, pos);
            if (childNode != nullptr) {
                node->addChild(childNode);
            }
        }
        pos++; // Move past the ']'
    }

    return node;
}

/**
 * Converts a string representation of a TreeNode to an actual TreeNode object.
 *
 * @param str The string representation of the TreeNode.
 *
 * @return The TreeNode object converted from the string.
 *
 * @throws None.
 */
TreeNode*
stringToTreeNode(const string& str)
{
    size_t pos = 0;
    return parseTreeNode(str, pos);
}

/**
 * Reads a file and converts its contents into a vector of TreeNodes.
 *
 * @param filePath the path of the file to be read
 * @param decompress a boolean indicating whether the file contents should be
 * decompressed
 *
 * @return a vector of TreeNodes representing the contents of the file
 *
 * @throws runtime_error if the file cannot be opened or if decompression fails
 */
vector<TreeNode*>
fileToTreeNodes(const string& filePath, bool decompress = true)
{
    vector<TreeNode*> treeNodes;
    ifstream file(filePath, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Unable to open file: " + filePath);
    }

    // Read the file into a string
    string compressed((istreambuf_iterator<char>(file)),
                      (istreambuf_iterator<char>()));
    file.close();

    if (decompress) {
        // Decompress the string directly into uncompressed
        string uncompressed;
        if (!snappy::Uncompress(
              compressed.data(), compressed.size(), &uncompressed)) {
            throw runtime_error("Decompression failed for file: " + filePath);
        }

        istringstream ss(uncompressed);

        string line;
        while (getline(ss, line)) {
            TreeNode* node = stringToTreeNode(line);
            treeNodes.push_back(node);
        }
    } else {
        istringstream ss(compressed);

        string line;
        while (getline(ss, line)) {
            TreeNode* node = stringToTreeNode(line);
            treeNodes.push_back(node);
        }
    }

    return treeNodes;
}

/**
 * Writes the content of the given vector of TreeNodes to a file.
 *
 * @param trees the vector of TreeNodes to be written to the file
 * @param filePath the path of the file to write the TreeNodes to
 * @param compress optional parameter indicating whether to compress the file
 * (default: true)
 *
 * @throws ErrorType if unable to create or open the file
 */
void
treeNodesToFile(const vector<TreeNode*>& trees,
                const string& filePath,
                bool compress = true)
{
    ofstream file(filePath, ios::trunc);

    if (!file.is_open()) {
        cerr << "Unable to create or open file: " << filePath << endl;
        return;
    }

    for (TreeNode* tree : trees) {
        string treeString = treeNodeToString(tree);
        file << treeString << endl;
    }

    file.close();

    if (compress) {
        ifstream file2(filePath);

        string uncompressed((istreambuf_iterator<char>(file2)),
                            (istreambuf_iterator<char>()));
        file2.close();

        string compressed;
        snappy::Compress(uncompressed.data(), uncompressed.size(), &compressed);

        ofstream compressedFile(filePath, ios::trunc);
        compressedFile << compressed;
        compressedFile.close();
    }
}
