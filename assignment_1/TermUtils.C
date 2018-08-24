#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <string>
#include "TermUtils.H"
using namespace std;

struct termios orig_term_settings;
struct termios new_term_settings;

int setup_alternate_terminal()
{
    int rc = SUCCESS;
    tcgetattr(STDIN_FILENO, &orig_term_settings);

    new_term_settings = orig_term_settings;
    new_term_settings.c_lflag &= ~ECHO;
    new_term_settings.c_lflag &= ~ICANON;
    new_term_settings.c_cc[VMIN] = 1;
    new_term_settings.c_cc[VTIME] = 0;

    // Save the cursor and switch to alternate screen buffer
    cout << SWITCH_ALT_SCREEN_BUFFER << flush;
    cout << ENABLE_ALT_SCREEN_SCROLL << flush;

    rc = tcsetattr(STDIN_FILENO, TCSANOW, &new_term_settings);

    if (rc != SUCCESS)
    {
        cerr << "Could Not Set New Terminal Attributes" << endl;
        rc = FAILURE;
    }
    return rc;
}


int restore_terminal()
{
    int rc = SUCCESS;
    // Switch back from the alternate screen buffer
    cout << SWITCH_NORM_SCREEN_BUFFER << flush;

    // Restore the original terminal
    rc = tcsetattr(STDIN_FILENO, TCSANOW, &orig_term_settings);

    if (rc != SUCCESS)
    {
        cerr << "Could Not Restore the Terminal Attributes" << endl;
        rc = FAILURE;
    }
    return rc;
}

void evaluate_arrow_keys(int& cpos, int& cbound)
{
    char c;
    read (STDIN_FILENO, &c, 1);
    if (c == ESC_SEQ_START)
    {
        read (STDIN_FILENO, &c, 1);
        if (c == KEY_UP)
        {
            if (cpos > 1)
            {
                cout << CURSOR_UP << flush;
                cpos--;
            }
        }
        else if (c == KEY_DOWN)
        {
            if (cpos < cbound)
            {
                cout << CURSOR_DOWN << flush;
                cpos++;
            }
        }
        else
        {
            // do nothing - right and left arrow keys are disabled
        }
    }
}

