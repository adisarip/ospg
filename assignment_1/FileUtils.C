#include <iostream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
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

    if (mArgs.size() > 0)
    {
        mCmd = mArgs[0];
        mArgs.erase(mArgs.begin());
    }
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
    int rc = SUCCESS;
    string sFilePath;

    if (mArgs.size() < 2)
    {
        rc = FAILURE;
    }
    else
    {
        string sLastArg = mArgs.back();
        mArgs.pop_back();
        if (sLastArg[0] == '~')
        {
            string sHomeDir = getUserHome();
            sLastArg = sHomeDir + sLastArg.substr(1);
        }
        if (sLastArg[0] == '.')
        {
            sLastArg = mFxPath + sLastArg.substr(1);
        }
        if (sLastArg.back() != '/')
        {
            sLastArg += "/";
        }

        // The last argument should be a directory
        if (!isDirectory(sLastArg))
        {
            rc = FAILURE;
        }
        else
        {
            for (string& sFile : mArgs)
            {
                sFilePath = sLastArg + sFile;
                int fd = open(sFilePath.c_str(),
                              O_RDWR | O_CREAT,
                              S_IRWXU | S_IRGRP | S_IROTH);
                if (fd < 0)
                {
                    rc = FAILURE;
                }
                else
                {
                    close(fd);
                }
            }
        }
    }

    return rc;
}


bool FileUtils::isDirectory(string path)
{
    struct stat fileStat;
    stat(path.c_str(), &fileStat);
    return (S_ISDIR(fileStat.st_mode));
}


string FileUtils::getUserHome()
{
    // get the user home directory
    char* pHomeDir = getenv("HOME");
    string sHomeDir;
    if (pHomeDir != NULL)
    {
        sHomeDir = string(pHomeDir) + "/";
    }
    else
    {
        sHomeDir = "";
    }
    return sHomeDir;
}


int FileUtils::fxCreateDir()
{
    int rc = SUCCESS;
    string sDirPath;

    if (mArgs.size() < 2)
    {
        rc = FAILURE;
    }
    else
    {
        string sLastArg = mArgs.back();
        mArgs.pop_back();
        if (sLastArg[0] == '~')
        {
            string sHomeDir = getUserHome();
            sLastArg = sHomeDir + sLastArg.substr(1);
        }
        if (sLastArg[0] == '.')
        {
            sLastArg = mFxPath + sLastArg.substr(1);
        }
        if (sLastArg.back() != '/')
        {
            sLastArg += "/";
        }

        // The last argument should be a directory
        if (!isDirectory(sLastArg))
        {
            rc = FAILURE;
        }
        else
        {
            for (string& sDir : mArgs)
            {
                sDirPath = sLastArg + sDir;
                rc = mkdir(sDirPath.c_str(),
                           S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            }
        }
    }

    return rc;
}


int FileUtils::fxDeleteFile()
{
    int rc = SUCCESS;
    string sFilePath;

    if (mArgs.size() < 1)
    {
        rc = FAILURE;
    }
    else
    {
        for (string& sFile : mArgs)
        {
            sFilePath = mFxPath + sFile;
            rc = remove(sFilePath.c_str());
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

    if (mArgs.size() < 1)
    {
        rc = FAILURE;
    }
    else
    {
        for (string& sEntry : mArgs)
        {
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
    string sPath;

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
                sPath = dirPath + sName + "/";
                searchFolderTree(sEntry, sPath);
            }
        }
    }
    closedir(pDir);

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


