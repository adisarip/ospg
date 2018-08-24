#include <cstring>
#include <unistd.h>
#include <iostream>
#include <string>
#include "TermUtils.H"
#include "FileSystem.H"
using namespace std;

int main(int argc, char* argv[])
{
    char in_buff;
    int sCursorBound = 0;
    int sCursorPos = 0;
    int rc = SUCCESS;
    string sDirPath("./");

    if (argc > 2)
    {
        cerr << "ERROR: Invalid Command" << endl;
        return FAILURE;
    }
    else if (argc == 2)
    {
        sDirPath = string(argv[1]);
    }
    else
    {
        sDirPath = "./";
    }

    FileSystem fs(sDirPath);

    // Switch to alternate screen buffer
    setup_alternate_terminal();

    // Display initial directory listing
    fs.traverse();
    sCursorBound = fs.display();
    cout << CURSOR_TOP << flush;
    sCursorPos = CURSOR_START_POS;

    while(1)
    {
        // Read each character in non-cannonical mode
        read (STDIN_FILENO, &in_buff, 1);
        if (in_buff == CTRL_C || in_buff == QUIT)
        {
            cout << CLEAR_ALT_SCREEN_BUFFER << flush;
            break;
        }
        else if (in_buff == KEY_ESC)
        {
            evaluate_arrow_keys(sCursorPos, sCursorBound);
        }
        else if (in_buff == KEY_ENTER)
        {
            // Evaluate and display the directory contents
            //cout << "Enter" << flush;
            fs.evaluateAndDisplay(sCursorPos, sCursorBound);
        }
        else
        {
            cout << in_buff << flush;
        }
    }

    // Revert from the alternate screen buffer and restore the Terminal
    restore_terminal();

    return(0);
}
