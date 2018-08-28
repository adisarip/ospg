#include <iostream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include "FileUtils.H"
#include "TermUtils.H"
using namespace std;

FileUtils::FileUtils()
{
    mCmd.clear();
    mFxPath.clear();
    mArgs.clear();
    mCmdsList.push_back("copy");
    mCmdsList.push_back("move");
    mCmdsList.push_back("rename");
    mCmdsList.push_back("create_file");
    mCmdsList.push_back("create_dir");
    mCmdsList.push_back("delete_file");
    mCmdsList.push_back("delete_dir");
    mCmdsList.push_back("goto");
    mCmdsList.push_back("search");
    mCmdsList.push_back("snapshot");
    mFoundList.clear();
}


FileUtils::~FileUtils()
{
    mCmd.clear();
    mArgs.clear();
}


void FileUtils::init(string cmdString, string dirPath)
{
    // split string into command and args
    mFxPath = dirPath;
    tokenize(cmdString);
}


void FileUtils::tokenize(string cmdString)
{
    stringstream input(cmdString);
    string s;
    while(getline(input, s, ' '))
    {
        mArgs.push_back(s);
    }
    mCmd = mArgs[0];
    mArgs.erase(mArgs.begin());
}


int FileUtils::execute()
{
    int rc = SUCCESS;
    // execute the command
    int cmdIndex = -1;
    for (unsigned int index=0; index < mCmdsList.size(); index++)
    {
        if (mCmd == mCmdsList[index])
        {
            cmdIndex = index;
        }
    }
    switch(cmdIndex)
    {
        case FileUtils::COPY:        rc = fxCopy();       break;
        case FileUtils::MOVE:        rc = fxMove();       break;
        case FileUtils::RENAME:      rc = fxRename();     break;
        case FileUtils::CREATE_FILE: rc = fxCreateFile(); break;
        case FileUtils::CREATE_DIR:  rc = fxCreateDir();  break;
        case FileUtils::DELETE_FILE: rc = fxDeleteFile(); break;
        case FileUtils::DELETE_DIR:  rc = fxDeleteDir();  break;
        case FileUtils::GOTO:        rc = fxGoto();       break;
        case FileUtils::SEARCH:      rc = fxSearch();     break;
        case FileUtils::SNAPSHOT:    rc = fxSnapshot();   break;
        default: break;
    }
    return rc;
}


int FileUtils::fxCopy()
{
    return SUCCESS;
}


int FileUtils::fxMove()
{
    return SUCCESS;
}


int FileUtils::fxRename()
{
    int rc = SUCCESS;
    string sFile, dFile;

    if (mArgs.size() < 2)
    {
        rc = FAILURE;
    }
    else
    {
        sFile = mFxPath + mArgs[0];
        dFile = mFxPath + mArgs[1];
        rc = rename(sFile.c_str(), dFile.c_str());
    }

    return rc;
}


int FileUtils::fxCreateFile()
{
    return SUCCESS;
}


int FileUtils::fxCreateDir()
{
    return SUCCESS;
}


int FileUtils::fxDeleteFile()
{
    int rc = SUCCESS;
    string sFile;

    if (mArgs.size() < 1)
    {
        rc = FAILURE;
    }
    else
    {
        for (string& sFile : mArgs)
        {
            rc = remove(sFile.c_str());
            if (rc < 0) break;
        }
    }

    return rc;
}


int FileUtils::fxDeleteDir()
{
    int rc = SUCCESS;
    string sDirPath;

    if (mArgs.size() < 1)
    {
        rc = FAILURE;
    }
    else
    {
        for (string& sEntry : mArgs)
        {
            sDirPath = mFxPath + sEntry;
            rc = deleteFolderTree(sDirPath);
        }
    }

    return rc;
}


int FileUtils::deleteFolderTree(string dirPath)
{
    DIR* pDir;
    struct dirent* pEntry;
    string sPath;

    pDir = opendir(dirPath.c_str());
    if (pDir == NULL)
    {
        return -1;
    }

    while ((pEntry = readdir(pDir)) != NULL)
    {
        string sName(pEntry->d_name);
        if (sName != "." && sName != "..")
        {
            sPath = dirPath + "/" + sName;
            if (pEntry->d_type == DT_DIR)
            {
                deleteFolderTree(sPath);
            }
            else
            {
                unlink(sPath.c_str());
            }
        }
    }
    closedir(pDir);
    rmdir(dirPath.c_str());

    return 0;
}


int FileUtils::fxGoto()
{
    return SUCCESS;
}


int FileUtils::fxSearch()
{
    int rc = SUCCESS;
    //string sDirPath;

    if (mArgs.size() < 1)
    {
        rc = FAILURE;
    }
    else
    {
        for (string& sEntry : mArgs)
        {
            //sDirPath = mFxPath + sEntry;
            rc = searchFolderTree(sEntry);
        }
    }
    displaySearchResults();

    return rc;
}


int FileUtils::searchFolderTree(string sEntry, string dirPath)
{
    DIR* pDir;
    struct dirent* pEntry;
    //vector<string> sFoundList;

    if (dirPath == "")
    {
        dirPath = mFxPath;
    }

    pDir = opendir(dirPath.c_str());
    if (pDir == NULL)
    {
        return -1;
    }

    while ((pEntry = readdir(pDir)) != NULL)
    {
        string sName(pEntry->d_name);
        if (sName != "." && sName != "..")
        {
            if (sName == sEntry)
            {
                mFoundList.push_back(dirPath + sName);
            }
            if (pEntry->d_type == DT_DIR)
            {
                dirPath = dirPath + sName + "/";
                searchFolderTree(sEntry, dirPath);
            }
        }
    }
    closedir(pDir);
    //rmdir(dirPath.c_str());

    return 0;
}


void FileUtils::displaySearchResults()
{
    cout << CLEAR_SCREEN << flush;
    cout << MOVE_CURSOR_TOP << flush;

    cout << "Results of the search strings: |" << flush;
    for (string& s : mArgs) cout << s << "|" << flush;
    cout << endl;
    cout << "==============================" << endl;

    for (string& s : mFoundList)
    {
        cout << s << endl;
    }
    cout << MOVE_CURSOR_TOP << flush;

    while(1)
    {
        cin.clear();
        char c;
        read (STDIN_FILENO, &c, 1);
        if (c == BACKSPACE) break;
    }
}

int FileUtils::fxSnapshot()
{
    return SUCCESS;
}


void FileUtils::fxDump()
{
    cout << "executing --> " << mCmd << " " << mArgs[0] << " " << mArgs[1] << flush;
}


