#include <iostream>
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
    cout << CLEAR_ALT_SCREEN_BUFFER << flush;
    cout << SWITCH_NORM_SCREEN_BUFFER << flush;

    // Restore the original terminal
    rc = tcsetattr(STDIN_FILENO, TCSANOW, &orig_term_settings);

    if (rc != SUCCESS)
    {
        cerr << "Could Not Restore the Terminal Attributes" << endl;
        rc = FAILURE;
    }
    cout << flush << endl;
    return rc;
}


int fetch_cursor_position()
{
    struct termios save,raw;
    char buff[8];
    string s;
    int start_pos, end_pos;
    int sCursorPos;

    tcgetattr(0,&save);
    cfmakeraw(&raw);
    tcsetattr(0,TCSANOW,&raw);

    if (isatty(STDIN_FILENO))
    {
        write (STDOUT_FILENO, FETCH_CURSOR_POSITION, 4);
        read (STDIN_FILENO ,buff ,sizeof(buff));

        s = string(buff);
        start_pos = s.find_last_of("[")+1;
        end_pos = s.find_last_of(";");
        s = s.substr(start_pos, end_pos-start_pos);
        sCursorPos = atoi(s.c_str());
    }
    tcsetattr(0,TCSANOW,&save);
    return sCursorPos;
}

