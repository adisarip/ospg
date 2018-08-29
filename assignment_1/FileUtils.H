
#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <string>
#include <vector>
using namespace std;

#define SUCCESS 0
#define FAILURE -1


class FileUtils
{
  public:

    enum
    {
        COPY = 0,
        MOVE = 1,
        RENAME = 2,
        CREATE_FILE = 3,
        CREATE_DIR = 4,
        DELETE_FILE = 5,
        DELETE_DIR = 6,
        GOTO = 7,
        SEARCH = 8,
        SNAPSHOT = 9,
    };

    // Constructor/Destructor
    FileUtils();
    ~FileUtils();

    // Public methods
    void init(string cmdString, string mFxPath);
    int execute();

    int fxCopy();
    int fxMove();
    int fxRename();
    int fxCreateFile();
    int fxCreateDir();
    int fxDeleteFile();
    int fxDeleteDir();
    int fxGoto();
    int fxSearch();
    int fxSnapshot();
    void fxDump();

  private:
    void tokenize(string cmdString);
    int deleteFolderTree(string dirPath);
    int searchFolderTree(string sEntry, string dirPath = "");
    void displaySearchResults();
    bool isDirectory(string path);
    string getUserHome();

  private:
    string mCmd;
    string mFxPath;
    vector<string> mArgs;
    vector<string> mCmdsList; // List of commands supported.
    vector<string> mFoundList;
};



#endif /* FILEUTILS_H */

