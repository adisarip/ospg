#include <iostream>
#include <string>
#include <unistd.h>
#include "TermUtils.H"
#include "FileSystem.H"
using namespace std;

int main(int argc, char* argv[])
{
    char c;
    char in_buff[8];
    string sDirPath;

    if (argc > 2)
    {
        cerr << "ERROR: Invalid Command" << endl;
        return FAILURE;
    }
    else if (argc == 2)
    {
        sDirPath = string(argv[1]);
        if (sDirPath[0] == '.')
        {
            cout << "Provide Absolute Path or Nothing" << endl;
            return -1;
        }
    }
    else
    {
        // get the current working directory
        char* pCwd = getenv("PWD");
        if (pCwd != NULL)
        {
            sDirPath = string(pCwd) + "/";
        }
        else
        {
            sDirPath = "./";
        }
    }

    FileSystem fs(sDirPath);

    // Switch to alternate screen buffer
    setup_alternate_terminal();

    // Display initial directory listing
    fs.traverse();
    fs.display();
    cout << CURSOR_TOP << flush;

    // Read each character in non-cannonical mode
    // Infinite loop - press 'q' or Ctrl-C to break.
    while(1)
    {
        cin.clear();
        read (STDIN_FILENO, &c, 1);
        if (c == CTRL_C || c == QUIT)
        {
            cout << CLEAR_ALT_SCREEN_BUFFER << flush;
            break;
        }
        else if (c == KEY_ESC)
        {
            cin.clear();
            read (STDIN_FILENO, &in_buff, 8);
            fs.evaluateArrowKeys(string(in_buff));
        }
        else if (c == KEY_ENTER)
        {
            fs.evaluateEnterKey();
        }
        else if (c == BACKSPACE)
        {
            // Move up a directory
            fs.moveUp();
        }
        else if (c == 'h' || c == 'H')
        {
            fs.restart();
        }
        else
        {
            // Do nothing
        }
    }

    // Revert from the alternate screen buffer and restore the Terminal
    restore_terminal();
    return 0;
}
