#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>
#include <time.h>
#include <pwd.h>
#include "FileSystem.H"
#include "TermUtils.H"
using namespace std;

FileSystem::FileSystem(string dirPath)
:mPath(dirPath)
,mRootPath(dirPath)
,mFooterSize(4)
{
    mDirEntries.clear();
    mDirEntryDetails.clear();
    mBackDirStack.clear();
    mFwdDirStack.clear();
}

FileSystem::~FileSystem()
{
    // Good Bye !!
}


// traverse the directory pointed by mPath
int FileSystem::traverse()
{
    int sRC = SUCCESS;
    struct dirent* pDirEntry;

    DIR* pDir = opendir(mPath.c_str());
    if (NULL == pDir)
    {
        cerr << "ERROR: Cannot open the directory " << mPath << endl;
        sRC = FAILURE;
    }

    if (sRC == SUCCESS)
    {
        string sName;
        mDirEntries.clear();
        while (NULL != (pDirEntry = readdir(pDir)))
        {
            sName = pDirEntry->d_name;
            if (pDirEntry->d_type == DT_DIR)
            {
                mDirEntries.push_back(sName + "/");
            }
            else
            {
                mDirEntries.push_back(sName);
            }
        }
    }

    sRC = constructFileData();

    return sRC;
}


int FileSystem::constructFileData()
{
    int rc = SUCCESS;
    string sDirEntry;
    string sMonthEntries[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    struct stat fileStat;
    // Now get details of each entry
    for (string& entry : mDirEntries)
    {
        sDirEntry.clear();
        string sPath = mPath + entry;
        rc = stat(sPath.c_str(), &fileStat);
        if (rc < 0)
        {
            rc = FAILURE;
        }
        else
        {
            // create the permissions string
            sDirEntry += (S_ISDIR(fileStat.st_mode) ? "d" : "-");
            sDirEntry += ((fileStat.st_mode & S_IRUSR) ? "r" : "-");
            sDirEntry += ((fileStat.st_mode & S_IWUSR) ? "w" : "-");
            sDirEntry += ((fileStat.st_mode & S_IXUSR) ? "x" : "-");
            sDirEntry += ((fileStat.st_mode & S_IRGRP) ? "r" : "-");
            sDirEntry += ((fileStat.st_mode & S_IWGRP) ? "w" : "-");
            sDirEntry += ((fileStat.st_mode & S_IXGRP) ? "x" : "-");
            sDirEntry += ((fileStat.st_mode & S_IROTH) ? "r" : "-");
            sDirEntry += ((fileStat.st_mode & S_IWOTH) ? "w" : "-");
            sDirEntry += ((fileStat.st_mode & S_IXOTH) ? "x" : "-");
            sDirEntry += "    ";

            // add user name
            struct passwd* usr_t = getpwuid(fileStat.st_uid);
            sDirEntry += string(usr_t->pw_name);
            sDirEntry += "    ";

            // add group name
            struct group* grp_t = getgrgid(fileStat.st_gid);
            sDirEntry += string(grp_t->gr_name);
            sDirEntry += "    ";

            // add last modified time
            struct tm* tm_t = localtime(&(fileStat.st_mtime));
            string s_mon(sMonthEntries[tm_t->tm_mon]);
            string s_min = string(to_string(tm_t->tm_min));
            string s_hour = string(to_string(tm_t->tm_hour));
            string s_day = string(to_string(tm_t->tm_mday));
            s_day = (s_day.size() == 1) ? (string("0") + s_day) : s_day;
            s_hour = (s_hour.size() == 1) ? (string("0") + s_hour) : s_hour;
            s_min = (s_min.size() == 1) ? (string("0") + s_min) : s_min;
            sDirEntry += s_mon  + " " + s_day + " " + s_hour + " " + s_min;
            sDirEntry += "    ";

            // add file size
            sDirEntry += string(to_string(fileStat.st_size));
            sDirEntry += "    ";
        }
        mDirEntryDetails.push_back(sDirEntry);
    }

    return rc;
}


void FileSystem::display()
{
    // check number of entries for display
    int sNumEntries = mDirEntries.size();
    if(sNumEntries <= mDisplayAreaSize)
    {
        normalDisplay();
    }
    else
    {
        extendedDisplay();
    }
    footer();
    return;
}


void FileSystem::normalDisplay()
{
    int sNumEntries = mDirEntries.size();
    // display the contents of the directory
    for (int index = 0; index < sNumEntries; index++)
    {
        cout << setw(25) << left << mDirEntries[index] << mDirEntryDetails[index] << endl;
    }
}


void FileSystem::extendedDisplay()
{
    // display the contents of the directory - handling overflow
    for (int index = mDispStartIndex; index <= mDispEndIndex; index++)
    {
        cout << setw(25) << left << mDirEntries[index] << mDirEntryDetails[index] << endl;
    }
}


void FileSystem::run()
{
    // re-traverse, clear screen and display the contents
    traverse();
    clearAndDisplay();
    return;
}


void FileSystem::clearAndDisplay()
{
    cout << CLEAR_SCREEN << flush;
    cout << MOVE_CURSOR_TOP << flush;
    display();
    cout << MOVE_CURSOR_TOP << flush;
    return;
}


void FileSystem::evaluateArrowKeys(string sBuff)
{
    int sCursorIndex = fetch_cursor_position();
    int sNumEntries = mDirEntries.size();

    if (sBuff == KEY_UP)
    {
        if (sCursorIndex == CURSOR_START_POS && mDispStartIndex > 0)
        {
            mDispStartIndex--;
            mDispEndIndex--;
            clearAndDisplay();
        }
        if (sCursorIndex > CURSOR_START_POS)
        {
            cout << MOVE_CURSOR_UP << flush;
            sCursorIndex--;
        }
    }
    else if (sBuff == KEY_DOWN)
    {
        if (sCursorIndex == mDisplayAreaSize && mDispEndIndex < sNumEntries)
        {
            mDispStartIndex++;
            mDispEndIndex++;
            cout << CLEAR_SCREEN << flush;
            cout << MOVE_CURSOR_TOP << flush;
            display();
            cout << MOVE_CURSOR_UP_5 << flush;
        }
        if (sCursorIndex < min(mDisplayAreaSize, sNumEntries))
        {
            cout << MOVE_CURSOR_DOWN << flush;
            sCursorIndex++;
        }
    }
    else if (sBuff == KEY_LEFT)
    {
        // goto to previously visited directory - backward
        if (mBackDirStack.size() > 0)
        {
            string nextDir = mBackDirStack.back();
            mBackDirStack.pop_back();
            mFwdDirStack.push_back(mPath);
            mPath = nextDir;
            run();
        }
    }
    else if (sBuff == KEY_RIGHT)
    {
        // goto to previously visited directory - forward
        if (mFwdDirStack.size() > 0)
        {
            string nextDir = mFwdDirStack.back();
            mFwdDirStack.pop_back();
            mBackDirStack.push_back(mPath);
            mPath = nextDir;
            run();
        }
    }
    else
    {
        // Do Nothing for other entries.
    }
    return;
}


void FileSystem::evaluateEnterKey()
{
    int cpos = fetch_cursor_position();
    string sCurrentEntry = mDirEntries[cpos-1];

    if (sCurrentEntry.back() == '/')
    {
        changeDir(sCurrentEntry);
    }
    else
    {
        // Open the file
        openFile(mPath + sCurrentEntry);
    }
    return;
}


void FileSystem::changeDir(string nextDir)
{
    // change the directory to nextDir.
    if (nextDir == "./")
    {
        // skip - no need to append current directory
        return;
    }
    else if (nextDir == "../")
    {
        // save the current directory in backward dir stack
        mBackDirStack.push_back(mPath);

        // Slice the last directory
        int pos = mPath.find_last_of("/");
        mPath = mPath.substr(0, pos);
        pos = mPath.find_last_of("/");
        mPath = mPath.substr(0, pos+1);
    }
    else
    {
        // save the current directory in backward dir stack
        mBackDirStack.push_back(mPath);
        mPath = mPath + nextDir;
    }
    run();
    return;
}


void FileSystem::openFile(string fileName)
{
    int pid = fork();
    if (pid == 0) // if child process
    {
        execl(EXEC_PATH, EXEC_NAME, fileName.c_str(), (char *)0);
        exit(1);
    }
}


void FileSystem::moveUp()
{
    changeDir("../");
    return;
}


// Goto to Home directory of the file explorer.
void FileSystem::restart()
{
    mPath = mRootPath;
    mBackDirStack.clear();
    mFwdDirStack.clear();
    run();
    return;
}

void FileSystem::snapshot()
{
}


void FileSystem::footer()
{
    cout << "====================" << endl;
    cout << "Entries: " << mDirEntries.size() << endl;
    cout << "Current Dir: " << mPath << endl;
    cout << "====================" << endl;
    return;
}

