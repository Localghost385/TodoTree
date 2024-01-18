#include <converter.hxx>
#include <curses.h>
#include <fstream>
#include <treeHandler.hxx>

vector<TreeNode*> trees = {};
vector<string> treeNames = {};

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

    void scollWindow(int lines)
    {
        wscrl(window, lines);
        refresh();
    }
};

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
ui(string source_file)
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
    init_pair(4, COLOR_RED, COLOR_BLACK);

    //////////////////////////////////////////////////////////////////////////////////////////
    getch();
    //////////////////////////////////////////////////////////////////////////////////////////

    // create windows
    windowSmartPtr tree(LINES, 20, 0, 1);
    windowSmartPtr contents(LINES, COLS - 22, 0, 21);
    windowSmartPtr treeOptions(tree.window, LINES - 2, 17, 1, 2);
    windowSmartPtr ContentsPrint(contents.window, LINES - 2, COLS - 26, 1, 23);
    // scrollbars
    windowSmartPtr treeScroll(tree.window, LINES - 2, 1, 1, 19);
    windowSmartPtr contentsScroll(contents.window, LINES - 2, 1, 1, COLS - 3);

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

    treeOptions.box();
    wbkgd(treeOptions.window, COLOR_PAIR(1));
    treeOptions.refresh();
    ContentsPrint.refresh();

    wbkgd(treeScroll.window, COLOR_PAIR(1));
    wbkgd(contentsScroll.window, COLOR_PAIR(1));
    treeScroll.refresh();
    contentsScroll.refresh();

    // trees = fileToTreeNodes(source_file);
    trees.push_back(generateTree(2, 20));

    // populate treeNames
    for (TreeNode* tree : trees) {
        treeNames.push_back(tree->data);
    }

    ContentsPrint.refresh();

    // write tree to file
    // treeNodesToFile(trees, source_file);

    windowSmartPtr quit(6, 20, LINES / 2 - 3, COLS / 2 - 10);
    TreeNode* selectedTree = trees[0];
    // selectedTree->children[0]->children[0]->toggleCompletion();
    // printw("%s", selectedTree->children[0]->completed ? "true" : "false");
    // selectedTree->children[0]->children[1]->collapsed = true;

    vector<TreeNode*> orderedTrees = orderTrees(selectedTree);
    vector<string> buffer;

    ofstream file;
    file.open("orderedTrees.txt", ios::out | ios::trunc);
    for (TreeNode* tree : orderedTrees) {
        file << tree->data << "\n";
    }
    file.close();

    refresh();
    getch();
    int selector = 0;
    int cursor = 0;

    char input;
    // add loop
    while (true) {
        input = getch();
        if (input == 'q' || input == 'Q') {
            quit.box();
            wbkgd(quit.window, COLOR_PAIR(1));
            mvwprintw(quit.window, 1, 3, "Are you sure?");
            mvwprintw(quit.window, 3, 2, "Y - Yes | N - No");
            quit.refresh();
            char answer = getch();
            if (answer == 'y' || answer == 'Y') {
                endwin();
                exit(0);
            } else {
                werase(quit.window);
                // treeOptions.refresh();
            }
        }

        switch (input) {
            case 'w':
            case 'W':
                // move selector up
                if (cursor > -10) {
                    cursor--;
                } else if (selector > 0) {
                    selector--;
                } else if (cursor > -((LINES - 2) / 2)) {
                    cursor--;
                }
                break;
            case 's':
            case 'S':
                if (cursor < 10) {
                    cursor++;

                } else if (selector < buffer.size() - LINES + 2) {
                    selector++;
                } else if ((LINES - 2 > orderedTrees.size() &&
                            cursor < ((LINES - 2) / 2) - 1) ||
                           (LINES - 2 <= orderedTrees.size() &&
                            cursor < 10)) {
                    cursor++;
                }
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
            case 'd':
                // complete/incomplete
                printw("%s", orderedTrees[selector]->data.c_str());
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

        tree.box();
        contents.box();
        mvwprintw(
          contents.window,
          LINES - 1,
          ((COLS - 22) / 2) - 64,
          "| W - ↑ | S - ↓ | Tab - Switch | Q - Exit | E - Edit | C "
          "- Collapse/Expand | Enter - Complete/Incomplete | A - Add | R - "
          "Remove |");
        wbkgd(tree.window, COLOR_PAIR(1));
        wbkgd(contents.window, COLOR_PAIR(2));
        tree.refresh();
        contents.refresh();

        // editor update
        buffer = generateBuffer(selectedTree);
        orderedTrees = orderTrees(selectedTree);

        // print the right portion of the buffer based on selector
        werase(ContentsPrint.window);
        for (int i = 0;
             i < (orderedTrees.size() < LINES - 2 ? orderedTrees.size()
                                                  : LINES - 2);
             i++) {

            mvwprintw(
              ContentsPrint.window, i, 0, "%s", buffer[i + selector].c_str());
            if (orderedTrees[i] != nullptr && orderedTrees[i]->completed) {
                wattron(ContentsPrint.window, COLOR_PAIR(3));
                wprintw(ContentsPrint.window, " dd");
                wattron(ContentsPrint.window, COLOR_PAIR(1));
                wprintw(ContentsPrint.window, " dd");
            } else {
                wattron(ContentsPrint.window, COLOR_PAIR(4));
                wprintw(ContentsPrint.window, " X");
                wattron(ContentsPrint.window, COLOR_PAIR(1));
                wprintw(ContentsPrint.window, " X");
            }

            if (i == ((LINES - 2) / 2) + cursor) {
                wattron(ContentsPrint.window, COLOR_PAIR(2));
                wprintw(ContentsPrint.window, "          <");
                wattroff(ContentsPrint.window, COLOR_PAIR(2));
            }
        }

        ContentsPrint.refresh();

        werase(contentsScroll.window);
        int scrollbarSize = buffer.size() / (LINES + 2);
        for (int i = 0; i < scrollbarSize; i++) {
            mvwprintw(contentsScroll.window,
                      i + selector /
                            (buffer.size() / ((LINES - scrollbarSize) + 1)),
                      0,
                      "█");
        }
    }
}