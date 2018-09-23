
/******************************************************************************************************/
/* Descripton: Class Implementation for a P2P Node in the MTorrent System. A P2P Node will serve as   */
/*             both Client & Server. Hence it is derived from "Client" and "Server" Classes using     */
/*             Multiple Inheritance in C++.                                                           */
/*                                         ________    ________                                       */
/*                                        | Server |  | Client |                                      */
/*                                        |________|  |________|                                      */
/*                                           /_\          /_\                                         */
/*                                            |            |                                          */
/*                                            |____________|                                          */
/*                                            |    Node    |                                          */
/*                                            |____________|                                          */
/* Author: Aditya Saripalli                                                                           */
/* Roll#:  20173071                                                                                   */
/******************************************************************************************************/


#include <string>
#include <vector>
#include <map>
#include <queue>
#include <thread>
#include "MTUtils.H"
#include "Server.H"
#include "Client.H"
using namespace std;

class Node : public Client, public Server
{
  public:
    Node(struct NodeArgs_t inputArgsParm);
    ~Node();

    int  init();
    void run();
    void initSeeding();
    void initDownload();
    void removeFileTorrentInfo();
    void showDownloads();

  protected:
    void displayMenu();
    void tokenize(IN  string cmdStringParm,
                  OUT vector<string>& cmdTokensList);

    string constructHashOfHash(IN string dataFileHashParm);
    void constructSha1Hash(IN  string fileNameParm,
                           IN  int fileSizeParm,
                           OUT string& fileHashParm);
    void createTorrentFile();
    void sendSeedingRequest();
    void startSeeding();

    void parseInputLine(IN  string lineParm,
                        OUT map<string, string>& configDataParm);
    void readTorrentFile();
    void sendDownloadRequest();
    void selectPeersForDownload();
    void startDownload();

    void sendRemoveRequest();

  private:
    string mCmd;
    string mArg1;
    string mArg2;
    vector<string> mSeedingPeersList;
    queue<int> mListeningPortsQ;
    map<string, string> mSeedingFilesList;
    NodeArgs_t mArgs; // Node Input Arguments
    DataFileInfo_t mSeedFileInfo;
    DataFileInfo_t mDownFileInfo;
    DataFileInfo_t mRemoveFileInfo;
};