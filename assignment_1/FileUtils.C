#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "FileUtils.H"
#include "TermUtils.H"
using namespace std;

FileUtils::FileUtils(FileSystem* p)
:pFx(p)
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
    mCmdsList.push_back("delete");
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
        case FileUtils::DELETE:      rc = fxDelete();     break;
        case FileUtils::GOTO:        rc = fxGoto();       break;
        case FileUtils::SEARCH:      rc = fxSearch();     break;
        case FileUtils::SNAPSHOT:    rc = fxSnapshot();   break;
        default: break;
    }
    return rc;
}


int FileUtils::fxCopy()
{
    int rc = SUCCESS;
    string sDestPath;

    if (mArgs.size() < 2)
    {
        rc = FAILURE;
    }
    else
    {
        string darg = mArgs.back();
        mArgs.pop_back();

        // Evaluating / Validating the destination argument
        rc = evaluateDirectoryArg(darg, sDestPath);

        if (rc == SUCCESS)
        {
            for (string& arg : mArgs)
            {
                string sPath;
                if (evaluateDirectoryArg(arg, sPath) == SUCCESS)
                {
                    rc = copyDirectory(sPath, sDestPath);
                }
                else
                {
                    string srcFile = arg;
                    string destFile = sDestPath + arg;
                    ifstream input(srcFile.c_str(), ios_base::binary | ios_base::in);
                    ofstream output(destFile.c_str(), ios_base::binary | ios_base::out);
                    input >> output.rdbuf();
                }
            }
        }
    }

    return rc;
}


int FileUtils::fxMove()
{
    int rc = SUCCESS;

    rc = fxCopy();

    if (rc == SUCCESS)
    {
        rc = fxDelete();
    }

    return rc;
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
        string arg = mArgs.back();
        mArgs.pop_back();

        // Evaluating / Validating the destination argument
        rc = evaluateDirectoryArg(arg, sFilePath);
        if (rc == SUCCESS)
        {
            string sPath;
            for (string& sFile : mArgs)
            {
                sPath = sFilePath + sFile;
                int fd = open(sPath.c_str(),
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
        string arg = mArgs.back();
        mArgs.pop_back();

        // Evaluating / Validating the destination argument
        rc = evaluateDirectoryArg(arg, sDirPath);

        if (rc == SUCCESS)
        {
            string sPath;
            for (string& sDir : mArgs)
            {
                sPath = sDirPath + sDir;
                rc = mkdir(sPath.c_str(),
                           S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
            }
        }
    }

    return rc;
}


int FileUtils::fxGoto()
{
    int rc = SUCCESS;
    string sDirPath;
    bool isAppend = false;

    if (mArgs.size() != 1)
    {
        rc = FAILURE;
    }
    else
    {
        string arg = mArgs[0];
        // Evaluating / Validating the destination argument
        rc = evaluateDirectoryArg(arg, sDirPath);
    }

    if (rc == SUCCESS)
    {
        pFx->changeDir(sDirPath, isAppend);
    }

    return rc;
}


int FileUtils::fxDelete()
{
    int rc = SUCCESS;

    if (mArgs.size() < 1)
    {
        rc = FAILURE;
    }
    else
    {
        for (string& arg : mArgs)
        {
            string sDirPath;
            string sFilePath;
            if (evaluateDirectoryArg(arg, sDirPath) == SUCCESS)
            {
                rc = deleteFolderTree(sDirPath);
            }
            else
            {
                sFilePath = mFxPath + arg;
                rc = remove(sFilePath.c_str());
                if (rc < 0) break;
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

    if (mArgs.size() < 1)
    {
        rc = FAILURE;
    }
    else
    {
        for (string& arg : mArgs)
        {
            string sDirPath;
            rc = evaluateDirectoryArg(arg, sDirPath);
            if (rc == SUCCESS)
            {
                rc = deleteFolderTree(sDirPath);
            }
        }
    }

    return rc;
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


int FileUtils::fxSnapshot()
{
    int rc = SUCCESS;
    string sDirPath;
    ofstream dump;

    if (mArgs.size() != 2)
    {
        rc = FAILURE;
    }
    else
    {
        string arg = mArgs[0]; // Cmd:(0)snapshot Args:(0)DirectoryName (1)DumpFile
        ofstream dump(mArgs[1]);

        // Evaluating / Validating the directory argument
        rc = evaluateDirectoryArg(arg, sDirPath);

        if (rc == SUCCESS)
        {
            dump << "[" << sDirPath << "]" << endl;
            rc = scanAndDumpResults(dump, sDirPath, 0);
        }
        dump.close();
    }

    return rc;
}


/****************************/
/*     Helper Functions     */
/****************************/

bool FileUtils::isDirectory(string path)
{
    struct stat fileStat;

    // adjust in case of relative paths
    if (path.find("./") == 0)
    {
        path = mFxPath + path.substr(2);
    }
    else if (path.find("../") == 0)
    {
        path = mFxPath + path;
    }

    stat(path.c_str(), &fileStat);
    cout << S_ISDIR(fileStat.st_mode) << "::" << path << endl;
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


int FileUtils::evaluateDirectoryArg(string arg, string& sPath)
{
    int rc = SUCCESS;

    if (arg == "." || arg == "..")
    {
        arg = arg + "/";   // for consistency
    }

    if (arg == "/" && arg.find("/") == 0)
    {
        // goto file explorer root / or absolute path
        sPath = (arg.size() > 1) ? (arg) : (pFx->getRootPath());
    }
    else if (arg == "~" || arg.find("~/") == 0)
    {
        // path w.r.t user home directory
        int pos = (arg.size() > 1) ? 2 : 1;
        string sHomeDir = getUserHome();
        sPath = sHomeDir + arg.substr(pos);
    }
    else if (arg.find("./") == 0)
    {
        //sPath = arg;
        sPath = mFxPath + arg.substr(2);
    }
    else if (arg.find("../") == 0)
    {
        if (mCmd == "goto")
        {
            sPath = arg;
        }
        else
        {
            int pos;
            sPath = mFxPath;
            while (arg.find("../") == 0 && sPath.size() >= 1)
            {
                if (sPath.back() == '/')
                {
                    pos = sPath.find_last_of("/");
                    sPath = sPath.substr(0, pos);
                }
                pos = sPath.find_last_of("/");
                sPath = sPath.substr(0, pos+1);
                arg = arg.substr(3);
            }
            sPath = sPath + arg;
        }
    }
    else
    {
        // relative path w/o starting with "./"
        sPath = mFxPath + arg;
    }

    if (sPath.back() != '/')
    {
        sPath = sPath + "/";
    }

    // Finally the destination should be a valid existing directory
    if (!isDirectory(sPath))
    {
        rc = FAILURE;
    }
    return rc;
}


int FileUtils::scanAndDumpResults(ofstream& dump, string dirPath, int formatDepth)
{
    int rc = SUCCESS;
    DIR *pDir;
    struct dirent *entry;
    struct stat fileStat;

    if((pDir = opendir(dirPath.c_str())) == NULL)
    {
        rc = FAILURE;
    }
    else
    {
        while((entry = readdir(pDir)) != NULL)
        {
            string sEntryName(entry->d_name);
            if(entry->d_type == DT_DIR)
            {
                // skip "." and ".." directories
                if (sEntryName != "." && sEntryName != "..")
                {
                    dump << setw(formatDepth) << "" << "[" << sEntryName << "]" << endl;
                    string sDir = dirPath + sEntryName + "/";
                    rc = scanAndDumpResults(dump, sDir, formatDepth+4);
                }
            }
            else
            {
                dump << setw(formatDepth) << "" << sEntryName << endl;
            }
        }
        closedir(pDir);
    }
    return rc;
}


int FileUtils::copyDirectory(string sourceDirPath, string destDirPath)
{
    int rc = SUCCESS;
    DIR *pDir;
    struct dirent *entry;
    struct stat fileStat;
    string sDestPath;
    string sourceDirName;

    // create and empty sourceDir first in the destination path
    if (sourceDirPath.back() == '/')
    {
        sourceDirName = sourceDirPath.substr(0, sourceDirPath.find_last_of("/"));
    }
    sourceDirName = sourceDirName.substr(sourceDirName.find_last_of("/")+1);
    sDestPath = destDirPath + sourceDirName + "/";

    rc = mkdir(sDestPath.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

    if (rc == SUCCESS)
    {
        if((pDir = opendir(sourceDirPath.c_str())) == NULL)
        {
            rc = FAILURE;
        }
        else
        {
            while((entry = readdir(pDir)) != NULL)
            {
                string sEntryName(entry->d_name);
                if(entry->d_type == DT_DIR)
                {
                    // skip "." and ".." directories
                    if (sEntryName != "." && sEntryName != "..")
                    {
                        string srcPath = sourceDirPath + sEntryName + "/";
                        rc = copyDirectory(srcPath, sDestPath);
                    }
                }
                else
                {
                    string srcFile = sEntryName;
                    string destFile = sDestPath + sEntryName;
                    ifstream input(srcFile.c_str(), ios_base::binary | ios_base::in);
                    ofstream output(destFile.c_str(), ios_base::binary | ios_base::out);
                    input >> output.rdbuf();
                }
            }
            closedir(pDir);
        }
    }
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
                if (pEntry->d_type == DT_DIR)
                {
                    mFoundList.push_back(dirPath + sName + "/");
                }
                else
                {
                    mFoundList.push_back(dirPath + sName);
                }
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
            if (pEntry->d_type == DT_DIR)
            {
                sPath = dirPath + sName + "/";
                deleteFolderTree(sPath);
            }
            else
            {
                sPath = dirPath + sName;
                unlink(sPath.c_str());
            }
        }
    }
    closedir(pDir);
    rmdir(dirPath.c_str());

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

    char c;
    char in_buff[8];
    while(1)
    {
        cin.clear();
        read (STDIN_FILENO, &c, 1);
        if (c == BACKSPACE)
        {
            break;
        }
        else if (c == KEY_ESC)
        {
            cin.clear();
            read (STDIN_FILENO, &in_buff, 8);
            evaluateArrowKeysInSearchResults(string(in_buff));
        }
        else if (c == KEY_ENTER)
        {
            evaluateEnterKeyInSearchResults();
            break;
        }
        else
        {
            // invalid input - try again
        }
    }
}


void FileUtils::evaluateArrowKeysInSearchResults(string buff)
{
    int sCursorPos = fetch_cursor_position();
    int sNumEntries = mFoundList.size() + 2;  // additional 2 for search results header

    if (buff == KEY_UP)
    {
        if (sCursorPos > CURSOR_START_POS)
        {
            cout << MOVE_CURSOR_UP << flush;
            sCursorPos--;
        }
    }
    else if (buff == KEY_DOWN)
    {
        if (sCursorPos < sNumEntries)
        {
            cout << MOVE_CURSOR_DOWN << flush;
            sCursorPos++;
        }
    }
    else
    {
        // KEY_RIGHT & KEY_LEFT are disabled in search results screen
    }
    return;
}


void FileUtils::evaluateEnterKeyInSearchResults()
{
    int sCursorPos = fetch_cursor_position();
    int sIndex = sCursorPos - 3;

    if (sIndex < 0)
    {
        return;
    }

    if (sIndex < mFoundList.size())
    {
        string sCurrentEntry = mFoundList[sIndex];
        if (sCurrentEntry.back() == '/')
        {
            pFx->changeDir(sCurrentEntry, false, false);
        }
        else
        {
            // Open the directory containing the file
            int pos = sCurrentEntry.find_last_of("/");
            sCurrentEntry = sCurrentEntry.substr(0, pos+1);
            pFx->changeDir(sCurrentEntry, false, false);
        }
    }
}


