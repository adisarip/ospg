#include <unistd.h>
#include <termios.h>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char* argv[])
{
    struct termios old_term_settings;
    struct termios new_term_settings;
    unsigned char in_buff [8];

    tcgetattr(STDIN_FILENO, &old_term_settings);

    new_term_settings = old_term_settings;
    //new_term_settings.c_lflag &= ~ECHO;
    new_term_settings.c_lflag &= ~ICANON;
    new_term_settings.c_cc[VMIN] = 1;
    new_term_settings.c_cc[VTIME] = 0;

    // Save the cursor and switch to alternate screen buffer
    cout << "\e[?1049h" << endl;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_term_settings) != 0)
    {
        cerr << "Could Not Set New Terminal Attributes" << endl;
    }
    else
    {
        cout << "New Terminal Attributes Set. ECHO is OFF" << endl;
        while(1)
        {
            // Read each character in non-cannonical mode
            read (STDIN_FILENO, &in_buff, 1);
            if (in_buff[0] == 3 || in_buff[0] == 'q')
            {
                break; // exit on Ctrl-C
            }
            else
            {
                // list the directories
            }
        }
    }

    write (STDOUT_FILENO, "\e[?1049l", 8);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_term_settings);
    return(0);
}
