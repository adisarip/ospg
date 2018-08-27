
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

    void run(); // traverse the path, clear screen and display
    int traverse();
    void display();
    void evaluateEnterKey();
    void evaluateArrowKeys(string sBuff);
    void moveUp();
    void restart();
    void snapshot();

    // Inline setter/getter functions
    inline void setTermDimensions(int h, int w)
    {
        mTermHeight = h;
        mTermWidth = w;
        mDisplayAreaSize  = (h - mFooterSize - 1);
        mDispStartIndex = 0;
        mDispEndIndex = mDisplayAreaSize-1;
    };
    inline void setPath(string dirPath) {mPath = dirPath;};

  private:
    inline int min(int x, int y) {return ((x<y) ? x : y);};
    void changeDir(string nextDir);
    void openFile(string fileName);
    void footer();
    int constructFileData();
    void clearAndDisplay();
    void normalDisplay();
    void extendedDisplay();

  private:
    string mPath;
    string mRootPath;
    int mTermHeight;
    int mTermWidth;
    int mDispStartIndex;
    int mDispEndIndex;
    int mDisplayAreaSize;
    int mFooterSize;
    vector<string> mDirEntries;  // List of all the entries files/directories in the current directory
    vector<string> mDirEntryDetails;
    vector<string> mBackDirStack;    // directory stack for "<-" (Left arrow) key
    vector<string> mFwdDirStack;     // directory stack for "->" (right arrow) key
};

#endif /* FILESYSTEM_H */

