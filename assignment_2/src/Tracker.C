/*
Tracker Class Implementation
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <string>
#include <cstring>
#include "Tracker.H"
using namespace std;

Tracker::Tracker(struct TrackerArgs_t inputArgsParm)
{
    mNodesData.clear();
    mArgs = inputArgsParm;
    bzero((char*)&mMsg, sizeof(NodeRequestMessage_t));
}


Tracker::~Tracker()
{
    // Good Bye
}


// Initialize the Tracker
void Tracker::init()
{
    cout << "[INFO] Tracker listening on: "
    << mArgs.mCurrentTrackerIpAddr << ":" << mArgs.mCurrentTrackerPortNum << endl;
    mSocketFd = startListening(mArgs.mCurrentTrackerIpAddr,  mArgs.mCurrentTrackerPortNum);
    mSeedersListFile.open(mArgs.mSeedersListFileName,
                          ios::in | ios::out);
}


void Tracker::run()
{
    // Tracker instance will always be alive
    
    struct sockaddr_in sClientAddress;
    bzero(&sClientAddress, sizeof(sockaddr_in));
    int sClientAddrLen = sizeof(sockaddr_in);
    
    while(1)
    {
        // On Success - Connection established with a Node.
        mSocketConnFd = accept(mSocketFd,
                               (struct sockaddr*)&sClientAddress,
                               (socklen_t*)&sClientAddrLen);
        if (mSocketConnFd < 0)
        {
            error("Socket accept failed");
        }

        // fetch the request from the socket
        int nBytes = read(mSocketConnFd,
                          (NodeRequestMessage_t*)&mMsg,
                          sizeof(NodeRequestMessage_t));
        if (nBytes < 0)
        {
            error("[ERROR] Tracker: Failed receiving message from Node");
        }

        // Do Something with the received request
        processRequest(IN mMsg);
    }
}


// Process the request
void Tracker::processRequest(NodeRequestMessage_t& msgParm)
{
    cout << "[INFO] Processing the request ..." << endl;

    // check the type of in-coming request
    switch(msgParm.mRequestType)
    {
        case SEEDING_REQUEST:
            processSeedingRequest(IN msgParm.mNodePortNum,
                                  IN msgParm.mNodeIpAddress,
                                  IN msgParm.mFileHash);
            break;
        case DOWNLOAD_REQUEST:
            processDownloadRequest(IN msgParm.mNodePortNum,
                                   IN msgParm.mNodeIpAddress,
                                   IN msgParm.mFileHash);
            break;
        case REMOVE_REQUEST:
            processRemoveRequest(IN msgParm.mNodePortNum,
                                 IN msgParm.mNodeIpAddress,
                                 IN msgParm.mFileHash);
            break;
        default:
            break;
    };
}


void Tracker::processSeedingRequest(int portNumParm,
                                    char* nodeIpAddressParm,
                                    char* fileHashParm)
{
    cout << "Seeding Request Received ..." << endl;

    // save the seeder information for future use
    string sFileHash(fileHashParm);
    string sNodeInfo = string(nodeIpAddressParm) + ":" + to_string(portNumParm);

    // duplication check
    // Verify if the client has sent a duplicate seeding request
    vector<string> sNodeInfoList = mNodesData[sFileHash];
    bool isFileExists = false;
    for (string& s : sNodeInfoList)
    {
        if (sNodeInfo == s)
        {
            isFileExists = true;
            break;
        }
    }

    int sResponse;
    if (isFileExists)
    {
        sResponse = DUPLICATE_ENTRY;
    }
    else
    {
        mNodesData[sFileHash].push_back(sNodeInfo);
        sResponse = START_SEEDING;
    }

    cout << "FileHash: " << sFileHash << endl;
    for (string& sInfo : mNodesData[sFileHash])
    {
        cout << "NodeInfo: " << sInfo << endl;
    }

    // send acknowledgement
    int nBytes = write(mSocketConnFd, &sResponse, sizeof(int));
    if (nBytes < 0)
    {
        error("[ERROR] Tracker: Failed to send seeding acknowledgement");
    }
}


void Tracker::processDownloadRequest(int portNumParm,
                                     char* nodeIpAddressParm,
                                     char* fileHashParm)
{
    cout << "Download Request Received ..." << endl;

    string sFileHash(fileHashParm);

    TrackerResponseMessage_t sMsg;
    bzero(&sMsg, sizeof(TrackerResponseMessage_t));
    string sList;
    sList.clear();
    map<string, vector<string>> NodeMap;
    map<string, vector<string>>::iterator itNodeInfo = mNodesData.find(sFileHash);
    if (!mNodesData.empty() && itNodeInfo != NodeMap.end())
    {
        sMsg.mResponseType = START_DOWNLOAD;
        // creating nodes list ip:port,ip:port,...
        for (string& sInfo : itNodeInfo->second)
        {
            sList += sInfo + ",";
        }
        sList.pop_back(); // remove trailing ","
        if (sList.size() < SIZE_NODE_LIST)
        {
            sList.copy(sMsg.mNodeInfoList, sList.size());
        }
    }
    else
    {
        sMsg.mResponseType = NO_PEERS_AVAILABLE;
        cout << "NO_PEERS_AVAILABLE" << endl;
    }

    cout << "Tracker: Tracker Response is ... ##"
         << sMsg.mResponseType << "##"
         << sMsg.mNodeInfoList <<   endl;

    int nBytes = write(mSocketConnFd, &sMsg, sizeof(TrackerResponseMessage_t));
    if (nBytes < 0)
    {
        error("[ERROR] Tracker: Failed to send download acknowledgement");
    }
}


void Tracker::processRemoveRequest(int portNumParm,
                                   char* nodeIpAddressParm,
                                   char* fileHashParm)
{
    cout << "Remove Request Received ..." << endl;

    string sFileHash(fileHashParm);
    string sSearchKey = string(nodeIpAddressParm) + ":" + to_string(portNumParm);

    // get the entry to be removed
    vector<string> sNodeInfoList = mNodesData[sFileHash];
    for (vector<string>::iterator it = sNodeInfoList.begin(); it != sNodeInfoList.end(); it++)
    {
        if (sSearchKey == (*it).substr(0, (*it).find_last_of(":")))
        {
            // remove the entry
            sNodeInfoList.erase(it);
            break;
        }
    }

    if (sNodeInfoList.empty())
    {
        mNodesData.erase(sFileHash);
    }
    else
    {
        mNodesData[sFileHash] = sNodeInfoList;
    }

    cout << "FileHash: " << sFileHash << endl;
    for (string& sInfo : mNodesData[sFileHash])
    {
        cout << "NodeInfo: " << sInfo << endl;
    }

    // send acknowledgement
    int sResponse = ENTRY_REMOVED;
    int nBytes = write(mSocketConnFd, &sResponse, sizeof(int));
    if (nBytes < 0)
    {
        error("[ERROR] Tracker: Failed to send seeding acknowledgement");
    }
}
