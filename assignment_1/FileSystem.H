
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>
using namespace std;

class FileSystem
{
  public:
    FileSystem(string dirPath);
    ~FileSystem();

    void setPath(string dirPath);
    int traverse();
    void display();
    void evaluateEnterKey();
    void evaluateArrowKeys(string sBuff);
    void moveUp();
    void restart();
    void snapshot();

  private:
    void run(); // re-traverse the new path and display
    void changeDir(string nextDir);
    void openFile(string fileName);
    void footer();
    int constructFileData();

  private:
    string mPath;
    string mRootPath;
    vector<string> mDirEntries;  // List of all the entries files/directories in the current directory
    vector<string> mDirEntryDetails;
    vector<string> mBackDirStack;    // directory stack for "<-" (Left arrow) key
    vector<string> mFwdDirStack;     // directory stack for "->" (right arrow) key
};

#endif /* FILESYSTEM_H */

