
#ifndef FILEUTILS_H
#define FILEUTILS_H
using namespace std;

class FileUtils
{
  public:
    FileUtils();
    ~FileUtils();

    void init();
    void execute();

  private:
    string mCmd;
    string mArgs;
    string mCmdsList;
};



#endif /* FILEUTILS_H */

