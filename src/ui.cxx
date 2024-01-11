#include <converter.hxx>
#include <cstddef>
#include <curses.h>
#include <deps.hxx>
#include <iterator>
#include <vector>

vector<TreeNode*> trees = {};
vector<string> treeNames = {};

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

class windowSmartPtr
{
  public:
    WINDOW* window;
    // Constructor for creating a main window
    windowSmartPtr(int height, int width, int starty, int startx)
    {
        window = newwin(height, width, starty, startx);
    }

    // Constructor for creating a child window
    windowSmartPtr(WINDOW* parent,
                   int height,
                   int width,
                   int starty,
                   int startx)
    {
        window = subwin(parent, height, width, starty, startx);
    }

    // Destructor
    ~windowSmartPtr() { delwin(window); }

    // Function to refresh the window
    void refresh() { wrefresh(window); }

    // Function to draw a box around the window
    void box() { ::box(window, 0, 0); }
};

// RECURSIVELy print nodetree and its children

/**
 * Prints the tree structure represented by the given TreeNode object to the
 * specified window. This function recursively traverses the tree and prints
 * each node along with appropriate indentation symbols. The tree is printed
 * in a hierarchical manner, with each level indented by three spaces. Each
 * node is printed on a new line.
 *
 * @param node The TreeNode object representing the root of the tree to be
 *             printed.
 * @param window The window to which the tree structure should be printed.
 * @param starty The y-coordinate of the starting position for printing the
 *               tree structure.
 * @param startx The x-coordinate of the starting position for printing the
 *               tree structure.
 * @param depth The current depth of the tree traversal. This parameter is
 *              used for indentation purposes.
 * @param parentIsLast A boolean value indicating whether the current node is
 *                     the last child of its parent. This parameter is used
 *                     for printing appropriate indentation symbols.
 *
 * @throws None
 */
void
printTreeNode(TreeNode* node,
              WINDOW* window,
              int depth = 0,
              bool parentIsLast = true)
{
    if (node == nullptr) {
        return;
    }

    int branchesToLeft = 0;
    bool blankBranch = false;
    // Indentation for current node
    // Iterate through each level of the tree
    for (int i = 0; i < depth; i++) {
        // Check if it's the last level
        if (i == depth - 1) {
            // Print the appropriate symbol based on whether the parent is the
            // last child
            wprintw(window, parentIsLast ? "├─ " : "└─ ");
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
                wprintw(window, "   ");
                branchesToLeft++;
                blankBranch = true;

            } else {
                wprintw(window, "│  ");
                branchesToLeft++;
                blankBranch = false;
            }
        }
    }

    // check if node is completed
    // Print current node data
    if (node->data.length() < COLS - 44 - branchesToLeft * 3) {
        if (node->completed) {
            wattron(window, COLOR_PAIR(3));
        }
        wprintw(window, "%s\n", node->data.c_str());
        wattron(window, COLOR_PAIR(1));
    } else {
        while (node->data.length() > COLS - 44 - branchesToLeft * 3) {
            wprintw(
              window,
              "%s\n",
              node->data.substr(0, COLS - 44 - branchesToLeft * 3).c_str());
            node->data =
              node->data.substr((window->_maxx - 2) - branchesToLeft * 3);
            if (blankBranch) {
                for (int i = 0; i < branchesToLeft + 1; i++) {
                    wprintw(window, "   ");
                }
            } else {
                for (int i = 0; i < branchesToLeft + 1; i++) {
                    wprintw(window, "│  ");
                }
                if (node->completed) {
                    wattron(window, COLOR_PAIR(3));
                }
                wprintw(window, "%s\n", node->data.substr(20).c_str());
                wattron(window, COLOR_PAIR(1));
            }
        }
    }
    // Reset color
    // wattron(window, COLOR_PAIR(0));
    wattron(window, COLOR_PAIR(1));

    // Recursively print children
    size_t numChildren = node->children.size();
    for (size_t i = 0; i < numChildren; i++) {
        bool isLastChild = (i == numChildren - 1);
        printTreeNode(node->children[i], window, depth + 1, !isLastChild);
    }
}

vector<TreeNode*> orderTrees(TreeNode* trees){
    //create a vector of treeNodes, based on their vertical position in the 2d representation.
}

void
treeOptionsUpdate(windowSmartPtr* options, int index)
{
    // clear  options
    wclear(options->window);
    wrefresh(options->window);

    // for each tree in treeNames print the name if not too long
    for (int i = 0; i < treeNames.size(); i++) {
        if (treeNames[i].size() > 20 - 6) {
            if (i == index) {
                // if tree is selected print its nam in red
                wattron(options->window, COLOR_PAIR(COLOR_RED));
                mvwprintw(
                  options->window,
                  i + 1,
                  1,
                  "%s",
                  (treeNames[i].substr(0, (COLS / 10) - 6) + "...").c_str());
                wattroff(options->window, COLOR_PAIR(COLOR_RED));
            } else if (trees[i]->completed) {
                // if tree is completed print its name in blue
                wattron(options->window, COLOR_PAIR(COLOR_BLUE));
                mvwprintw(
                  options->window,
                  i + 1,
                  1,
                  "%s",
                  (treeNames[i].substr(0, (COLS / 10) - 6) + "...").c_str());
                wattroff(options->window, COLOR_PAIR(COLOR_BLUE));
            } else {
                // if tree is not completed print its name in white
                wattron(options->window, COLOR_PAIR(COLOR_WHITE));
                mvwprintw(
                  options->window,
                  i + 1,
                  1,
                  "%s",
                  (treeNames[i].substr(0, (COLS / 10) - 6) + "...").c_str());
                wattroff(options->window, COLOR_PAIR(COLOR_WHITE));
            }
        } else {
            if (i == index) {
                wattron(options->window, COLOR_PAIR(COLOR_RED));
                mvwprintw(
                  options->window, i + 1, 1, "%s", treeNames[i].c_str());
                wattroff(options->window, COLOR_PAIR(COLOR_RED));
            } else if (trees[i]->completed) {
                wattron(options->window, COLOR_PAIR(COLOR_BLUE));
                mvwprintw(
                  options->window, i + 1, 1, "%s", treeNames[i].c_str());
                wattroff(options->window, COLOR_PAIR(COLOR_BLUE));
            } else {
                wattron(options->window, COLOR_PAIR(COLOR_WHITE));
                mvwprintw(
                  options->window, i + 1, 1, "%s", treeNames[i].c_str());
                wattroff(options->window, COLOR_PAIR(COLOR_WHITE));
            }
        }
    }
}

void
ui_init(string source_file)
{
    setlocale(LC_CTYPE, "");
    // init ncurses
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    refresh();

    // init colors
    start_color();
    // black: #1e1e2e
    init_color(COLOR_BLACK, (0x1e) * 3.9, (0x1e) * 3.9, (0x2e) * 3.9);
    // white: #cdd6f4
    init_color(COLOR_WHITE, (0xcd) * 3.9, (0xd6) * 3.9, (0xf4) * 3.9);
    // blue: #89b4fa
    init_color(COLOR_BLUE, (0x89) * 3.9, (0xb4) * 3.9, (0xfa) * 3.9);
    // green: #a6e3a1
    init_color(COLOR_GREEN, (0xa6) * 3.9, (0xe3) * 3.9, (0xa1) * 3.9);

    // set color_pairs
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);

    //////////////////////////////////////////////////////////////////////////////////////////
    getch();
    //////////////////////////////////////////////////////////////////////////////////////////

    // init windows
    windowSmartPtr tree(LINES, 20, 0, 1);
    windowSmartPtr contents(LINES, COLS - 22, 0, 21);

    tree.box();
    wbkgd(tree.window, COLOR_PAIR(2));
    tree.refresh();

    contents.box();
    wbkgd(tree.window, COLOR_PAIR(1));
    mvwprintw(contents.window,
              LINES - 1,
              ((COLS - 22) / 2) - 64,
              "| W - ↑ | S - ↓ | Tab - Switch | Q - Exit | E - Edit | C "
              "- Collapse/Expand | Enter - Complete/Incomplete | A - Add | R - "
              "Remove |");
    contents.refresh();

    // read file

    // create tree subwindows
    windowSmartPtr treeOptions(tree.window, 5, 5, 1, 1);
    windowSmartPtr ContentsPrint(contents.window, LINES - 2, COLS - 26, 1, 23);

    treeOptions.box();
    wbkgd(treeOptions.window, COLOR_PAIR(1));
    treeOptions.refresh();

    ContentsPrint.refresh();
    scrollok(ContentsPrint.window, TRUE);

    // trees = fileToTreeNodes(source_file);
    trees.push_back(generateTree(2, 100));

    // populate treeNames
    for (TreeNode* tree : trees) {
        treeNames.push_back(tree->data);
    }

    printTreeNode(trees[0], ContentsPrint.window);
    ContentsPrint.refresh();

    // write tree to file
    treeNodesToFile(trees, source_file);
}

void
ui(string source_file)
{
    // start ui
    ui_init(source_file);

    // update tree options

    // add loop
    while (char input = getch() != 'q' && input != 'Q') {
        int selector = 0;

        switch (input) {

            case 'w':
            case 'W':
                selector--;
                break;
            case 's':
            case 'S':
                selector++;
                break;
            case '\t':
                // switch
                break;
            case 'e':
            case 'E':
                // edit
                break;
            case 'c':
            case 'C':
                // collapse/expand
                break;
            case '\n':
                // complete/incomplete
                break;
            case 'a':
            case 'A':
                // add
                break;
            case 'r':
            case 'R':
                // remove
                break;
        }
    }

    getch();
}