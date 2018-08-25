
#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include "FileSystem.H"
#include "TermUtils.H"
using namespace std;

FileSystem::FileSystem(string dirPath)
:mPath(dirPath)
,mRootPath(dirPath)
{
    mDirEntries.clear();
    mBackDirStack.clear();
    mFwdDirStack.clear();
}

FileSystem::~FileSystem()
{
    // Good Bye !!
}

void FileSystem::setPath(string dirPath)
{
    mPath = dirPath;
    return;
}

int FileSystem::traverse()
{
    int sRC = SUCCESS;
    vector<string> sDirEntries;
    // traverse the directory pointed by mPath
    DIR* pDir = opendir(mPath.c_str());
    struct dirent* pDirEntry;
    if (NULL == pDir)
    {
        cerr << "ERROR: Cannot open the directory " << mPath << endl;
        sRC = FAILURE;
    }

    if (sRC == SUCCESS)
    {
        string sName;
        sDirEntries.clear();
        while (NULL != (pDirEntry = readdir(pDir)))
        {
            sName = pDirEntry->d_name;
            if (pDirEntry->d_type == DT_DIR)
            {
                sDirEntries.push_back(sName + "/");
            }
            else
            {
                sDirEntries.push_back(sName);
            }
        }
    }

    if (sDirEntries.size() > 0)
    {
        mDirEntries.clear();
        mDirEntries = sDirEntries;
    }

    return sRC;
}

void FileSystem::display()
{
    // display the contents of the directory
    for (string& s : mDirEntries)
    {
        cout << s << endl;
    }
    debug();
    return;
}


void FileSystem::run()
{
    // re-traverse and display the contents
    traverse();
    cout << CLEAR_SCREEN << flush;
    cout << CURSOR_TOP << flush;
    display();
    cout << CURSOR_TOP << flush;
    return;
}


void FileSystem::evaluateEnterKey()
{
    int cpos = fetch_cursor_position();
    string sCurrentDir = mDirEntries[cpos-1];

    if (sCurrentDir.back() == '/')
    {
        if (sCurrentDir == "./")
        {
            // skip - no need to append current directory
            return;
        }
        else if (sCurrentDir == "../")
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
            mPath = mPath + sCurrentDir;
        }
        run();
    }
    return;
}


void FileSystem::evaluateArrowKeys(string sBuff)
{
    int cpos = fetch_cursor_position();
    int cbound = mDirEntries.size();

    if (sBuff == KEY_UP)
    {
        if (cpos > 1)
        {
            cout << CURSOR_UP << flush;
            cpos--;
        }
    }
    else if (sBuff == KEY_DOWN)
    {
        if (cpos < cbound)
        {
            cout << CURSOR_DOWN << flush;
            cpos++;
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


void FileSystem::debug()
{
    cout << "===================================" << endl;
    cout << "Number Of Entries: " << mDirEntries.size() << endl;
    cout << "mPath: " << mPath << endl;
    cout << "mBackDirStack:" << endl;
    cout << "==============" << endl;
    for(string& s : mBackDirStack) cout << s << endl;
    cout << "mFwdDirStack:" << endl;
    cout << "=============" << endl;
    for(string& s : mFwdDirStack) cout << s << endl;
    cout << "===================================" << endl;
    return;
}

