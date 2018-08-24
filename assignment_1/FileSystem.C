
#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include "FileSystem.H"
using namespace std;

FileSystem::FileSystem(string dirPath)
:mPath(dirPath)
,mRootPath(dirPath)
{
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
        while (NULL != (pDirEntry = readdir(pDir)))
        {
            //cout << pDirEntry->d_fileno << " : "<< pDirEntry->d_name << " : " << (int)pDirEntry->d_type << endl;
            sName = pDirEntry->d_name;
            //mDirEntries.push_back(sName);
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

    //cout << "DEBUG:" <<endl;
    //cout << "DT_UNKNOWN:" << DT_UNKNOWN << endl;
    //cout << "DT_REG" << DT_REG << endl;
    //cout << "DT-DIR" << DT_DIR << endl;
    //cout << "END" << endl;

    return sRC;
}

int FileSystem::display()
{
    // display the contents of the directory
    for (string& s : mDirEntries)
    {
        cout << s << endl;
    }
    return mDirEntries.size();
}

void FileSystem::snapshot()
{
}

