
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
}

FileSystem::~FileSystem()
{
    // Good Bye !!
}

void FileSystem::setPath(string dirPath)
{
    mPath = dirPath;
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

int FileSystem::display()
{
    // display the contents of the directory
    for (string& s : mDirEntries)
    {
        cout << s << endl;
    }
    //cout << mDirEntries.size() << endl; // DEBUG
    //cout << mPath << endl;              // DEBUG
    return mDirEntries.size();
}

void FileSystem::evaluateAndDisplay(int& cpos, int& cbound)
{
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
            // Slice the last directory
            int pos = mPath.find_last_of("/");
            mPath = mPath.substr(0, pos);
            pos = mPath.find_last_of("/");
            mPath = mPath.substr(0, pos+1);
        }
        else
        {
            mPath = mPath + sCurrentDir;
        }

        traverse();
        cout << CLEAR_SCREEN << flush;
        cout << CURSOR_TOP << flush;
        cbound = display();
        cout << CURSOR_TOP << flush;
        cpos = CURSOR_START_POS;
    }
    return;
}

void FileSystem::snapshot()
{
}

