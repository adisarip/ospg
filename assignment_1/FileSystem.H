
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>
using namespace std;

#define SUCCESS 0
#define FAILURE -1

class FileSystem
{
  public:
    FileSystem(string dirPath);
    ~FileSystem();

    void setPath(string dirPath);
    int traverse();
    int display(); // return the #lines displayed
    void snapshot();

  private:
    string mPath;
    string mRootPath;
    vector<string> mDirEntries;  // List of all the entries files/directories in the current directory
};

#endif /* FILESYSTEM_H */

