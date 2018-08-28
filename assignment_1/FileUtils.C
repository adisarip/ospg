#include <iostream>
#include "FileUtils.H"
using namespace std;

FileUtils::FileUtils()
{
    mCmd = "";
    mArgs = "";
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
}


FileUtils::~FileUtils();
{
    mCmd.clear();
    mArgs.clear();
}


FileUtils::void init(string cmd)
{
    // split string into command and args
}


FileUtils::void execute()
{
    // execute the command
}



