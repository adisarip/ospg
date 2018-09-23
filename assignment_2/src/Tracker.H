// Class for the mtorrent Tracker server

#ifndef TRACKER_H
#define TRACKER_H

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <fstream>
#include "MTUtils.H"
#include "Server.H"
using namespace std;

class Tracker : public Server
{
  public:
    Tracker(struct TrackerArgs_t inputArgsParm);
    ~Tracker();

    void init();
    void run();

  private:
    void processRequest(NodeRequestMessage_t& msgParm);
    void processSeedingRequest(IN int portNumParm,
                               IN char* nodeIpAddressParm,
                               IN char* fileHashParm);

    void processDownloadRequest(IN int portNumParm,
                                IN char* nodeIpAddressParm,
                                IN char* fileHashParm);

    void processRemoveRequest(IN int portNumParm,
                              IN char* nodeIpAddressParm,
                              IN char* fileHashParm);

    void sendResponse();

    // Tracker Data
    TrackerArgs_t mArgs;
    NodeRequestMessage_t mMsg;
    fstream mSeedersListFile;
    vector<thread> mWorkers;
    map<string, vector<string>> mNodesData;
};


#endif /* TRACKER_H */