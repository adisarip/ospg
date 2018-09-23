
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <iostream>
#include<sstream>
#include<fstream>
#include <cstring>
#include <string>
#include "Node.H"
using namespace std;


Node::Node(struct NodeArgs_t inputArgsParm)
//:Client(inputArgsParm.mIpAddressTracker1,
//        inputArgsParm.mPortNumTracker1)
//:Server(inputArgsParm.mIpAddressNode,
//        inputArgsParm.mPortNumNode)
{
    mArgs = inputArgsParm;
    // create a list of listening ports for seeding.
    for (int sPortNum = mArgs.mPortNumNode+1; sPortNum <= mArgs.mPortNumNode+16; sPortNum++)
    {
        mListeningPortsQ.push(sPortNum);
    }
}


Node::~Node()
{
    // destructor
}


void Node::displayMenu()
{
    cout << "==============================" << endl;
    cout << "Start Seeding      -> Command: start <data_file_with_path>" << endl;
    cout << "Start Download     -> Command: get <torrent_file_with_path> <destination_path>" << endl;
    cout << "Show Downloads     -> Command: show_downloads" << endl;
    cout << "Remove Shared File -> Command: remove <torrent_file_with_path>" << endl;
    cout << "Close Application  -> Command: close" << endl;
    cout << "==============================" << endl;
    cout << "Command: " << flush;
}

void Node::tokenize(IN  string cmdStringParm,
                    OUT vector<string>& cmdTokensList)
{
    stringstream input(cmdStringParm);
    string s;
    while(getline(input, s, ' '))
    {
        cmdTokensList.push_back(s);
    }
}

int Node::init()
{
    int rc = SUCCESS;
    cout << "==============================" << endl;
    cout << "===== Welcome to MTorrent ====" << endl;
    while(1)
    {
        displayMenu();
        // Enter your command
        string sCmdString;
        vector<string> sCmdTokensList;
        getline(cin, sCmdString);
        tokenize(IN  sCmdString,
                 OUT sCmdTokensList);

        // verifying the command
        if ((sCmdTokensList.size() == 0) ||
            (sCmdTokensList.size() >= 1 &&
             sCmdTokensList[0] != "close" &&
             sCmdTokensList[0] != "show_downloads" &&
             sCmdTokensList[0] != "start" &&
             sCmdTokensList[0] != "remove" &&
             sCmdTokensList[0] != "get"))
        {
            cout << "[ERROR] Node1: Invalid Input. Try Again." << endl;
            continue;
        }
        else if ((sCmdTokensList[0] == "close" && sCmdTokensList.size() > 1) ||
                 (sCmdTokensList[0] == "show_downloads" && sCmdTokensList.size() > 1) ||
                 (sCmdTokensList[0] == "start" && sCmdTokensList.size() > 2) ||
                 (sCmdTokensList[0] == "remove" && sCmdTokensList.size() > 2) ||
                 (sCmdTokensList[0] == "get" && sCmdTokensList.size() > 3))
        {
            cout << "[ERROR] Node2: Invalid Input. Try Again." << endl;
            continue;
        }
        else
        {
            if (sCmdTokensList[0] == "close")
            {
                rc = CLOSE_AND_EXIT;
                break;
            }

            if (sCmdTokensList.size() == 3)
            {
                mCmd = sCmdTokensList[0];
                mArg1 = sCmdTokensList[1];
                mArg2 = sCmdTokensList[2];
            }
            else if (sCmdTokensList.size() == 2)
            {
                mCmd = sCmdTokensList[0];
                mArg1 = sCmdTokensList[1];
                mArg2.clear();
            }
            else // size == 1
            {
                mCmd = sCmdTokensList[0];
                mArg1.clear();
                mArg2.clear();
            }
            run();
        }
    }
    return rc;
}


void Node::run()
{
    cout << "Processing the command ..." << endl;

    if (mCmd == "start")
    {
        initSeeding();
    }
    else if (mCmd == "get")
    {
        initDownload();
    }
    else if (mCmd == "remove")
    {
        removeFileTorrentInfo();
    }
    else if (mCmd == "show_downloads")
    {
        showDownloads();
    }
    else
    {
        // Invalid Command
        cout << "[ERROR] Node: Invalid Command" << endl;
        return;
    }
}

/******************************************************************************************************/
/*                                        Supported Operations                                        */
/******************************************************************************************************/

void Node::initSeeding()
{
    // create a torrent file
    createTorrentFile();
    
    // Send a seeding request to tracker
    sendSeedingRequest();
    
    // Start Seeding
    startSeeding();
}


void Node::initDownload()
{
    // read the torrent file obtained external from some client.
    readTorrentFile();

    // request the Tracker for the list of seeders
    sendDownloadRequest();

    // Select the peer for downloading the file
    selectPeersForDownload();

    // Start the download of the file
    startDownload();
}


void Node::removeFileTorrentInfo()
{
    readTorrentFile();
    sendRemoveRequest();
}

void Node::showDownloads()
{
    // @TODO
}


/******************************************************************************************************/
/*                                          Helper Functions                                          */
/******************************************************************************************************/

void Node::createTorrentFile()
{
    // Save the data file
    mSeedFileInfo.mFile = mArg1;

    // get the size of the file to be shared
    struct stat sFileStat;
    stat(mSeedFileInfo.mFile.c_str(), &sFileStat);
    mSeedFileInfo.mFileSize = sFileStat.st_size;

    // construct SHA1-1 of the file
    string sFileHash;
    constructSha1Hash(IN  mSeedFileInfo.mFile,
                      IN  mSeedFileInfo.mFileSize,
                      OUT sFileHash);
    mSeedFileInfo.mFileHash = sFileHash; // a multiple of 20

    // construct SHA1 Hash of File Hash (40 Characters)
    mSeedFileInfo.mHashOfFileHash = constructHashOfHash(mSeedFileInfo.mFileHash);

    string sTorrentDirPath("/Users/aditya/DevHub/pgssp/2018_Monsoon/os/ospg/assignment_2/torrents/");
    string sTorrentFile = sTorrentDirPath +
                          mSeedFileInfo.mFile.substr(mSeedFileInfo.mFile.find_last_of("/")+1) +
                          ".mtorrent";
    string s1,s2;
    ofstream ofTorrentFile;
    ofTorrentFile.open(sTorrentFile);
    s1 = mArgs.mIpAddressTracker1 + ":" + to_string(mArgs.mPortNumTracker1);
    s2 = mArgs.mIpAddressTracker2 + ":" + to_string(mArgs.mPortNumTracker2);

    // create the torrent file
    ofTorrentFile << "tracker_1=" << s1 << endl;
    ofTorrentFile << "tracker_2=" << s2 << endl;
    ofTorrentFile << "filepath=" << mSeedFileInfo.mFile << endl;
    ofTorrentFile << "filesize=" << to_string(mSeedFileInfo.mFileSize) << endl;
    ofTorrentFile << "filehash=" << mSeedFileInfo.mFileHash << endl;
    ofTorrentFile.close();
    mSeedFileInfo.mTorrentFile = sTorrentFile;
}


// send a request to Tracker for seeding a file.
void Node::sendSeedingRequest()
{
    // prepare the Node-Tracker Message
    struct NodeRequestMessage_t sMsg;
    bzero(&sMsg, sizeof(NodeRequestMessage_t));

    sMsg.mRequestType = SEEDING_REQUEST;
    sMsg.mNodePortNum = mArgs.mPortNumNode;
    mArgs.mIpAddressNode.copy(sMsg.mNodeIpAddress, mArgs.mIpAddressNode.size());
    //mSeedFileInfo.mFile.copy(sMsg.mFile, mSeedFileInfo.mFile.size());
    mSeedFileInfo.mHashOfFileHash.copy(sMsg.mFileHash, mSeedFileInfo.mHashOfFileHash.size());

    // Connect to the Tracker Server
    int sSocketFd = connectToServer(mArgs.mIpAddressTracker1, mArgs.mPortNumTracker1);

    // sending message to server
    cout << "[DEBUG] Node: sMsg.mFileHash: " << sMsg.mFileHash << endl;
    int nBytes = write(sSocketFd, &sMsg, sizeof(NodeRequestMessage_t));
    if (nBytes < 0)
    {
        error("[ERROR] Node: Write failed");
    }

    // receive acknowledgement from server
    int sResponse = 0;
    nBytes = read(sSocketFd, &sResponse, sizeof(int));
    if (nBytes < 0)
    {
        error("[ERROR] Node: Read from Tracker Socket failed");
    }

    cout << "Node: Tracker Response is " << sResponse << endl;

    // Disconnect to the Tracker Server
    disconnectFromServer(sSocketFd);

    // Add the file into the Node seeding list
    mSeedingFilesList[mSeedFileInfo.mHashOfFileHash] = mSeedFileInfo.mFile;
}


// Start seeding on a thread
void Node::startSeeding()
{
    //Server Instance of the P2P node starts here

    // Only Listening for download requests.
    // On Client port  provided through input args.
    while (1)
    {
        startListening(mArgs.mIpAddressNode, mArgs.mPortNumNode);

        // @TODO: update mSocketConnFd at the end -- No Need to update.
        // On Success - Connection established with a Node.
        struct sockaddr_in sClientAddress;
        bzero(&sClientAddress, sizeof(sockaddr_in));
        int sClientAddrLen = sizeof(sockaddr_in);

        int sSocketConnFd = accept(mSocketFd,
                                   (struct sockaddr*)&sClientAddress,
                                   (socklen_t*)&sClientAddrLen);
        if (sSocketConnFd < 0)
        {
            error("Node: Socket accept failed");
        }
        else
        {
            cout << "[INFO] Node: P2P Connection established." << endl;
        }

        // wait for the file transfer request from the P2P Client
        // 1. request received for the file download
        // 2. send response with
        //    a. new port number on while the file will be seeded.
        //    b. @TODO: the list of chunk indices available for seeding.

        P2PRequestMessage_t sReqMsg;
        bzero(&sReqMsg, sizeof(P2PRequestMessage_t));
        int nBytes = read(sSocketConnFd,
                        (P2PRequestMessage_t*)&sReqMsg,
                        sizeof(P2PRequestMessage_t));

        cout << nBytes << " Bytes received" << endl;
        cout << "[DEBUG] Node: mMsg.mRequestType: " << sReqMsg.mRequestType << endl;
        cout << "[DEBUG] Node: mMsg.mHashOfHash: " << sReqMsg.mHashOfHash << endl;

        string sSeedingFile;
        if (sReqMsg.mRequestType == DOWNLOAD_REQUEST)
        {
            // get the seeding file path from seeder based on file hash
            sSeedingFile = mSeedingFilesList[sReqMsg.mHashOfHash];
        }

        int sSeedingPortNum;
        if (mListeningPortsQ.empty())
        {
            cout << "Seeding Connections max limit reached." << endl;
        }
        else
        {
            sSeedingPortNum = mListeningPortsQ.front();
            mListeningPortsQ.pop();
        }

        // prepapre response to P2P node download request
        P2PResponseMessage_t sRspMsg;
        bzero(&sRspMsg, sizeof(P2PResponseMessage_t));

        sRspMsg.mSeedingPortNum = sSeedingPortNum;
        mArgs.mIpAddressNode.copy(sRspMsg.mSeedingIpAddress, mArgs.mIpAddressNode.size());
        sSeedingFile.copy(sRspMsg.mSeedingFile, sSeedingFile.size());

        // @TODO : Create a Listening Thread Here on port#sSeedingPortNum.

        // send response
        cout << "P2P Node Response is ... " << sRspMsg.mSeedingIpAddress << ":" << sRspMsg.mSeedingPortNum << endl;

        nBytes = write(sSocketConnFd, &sRspMsg, sizeof(P2PResponseMessage_t));
        if (nBytes < 0)
        {
            error("[ERROR] P2P Node: Failed to send download request acknowledgement");
        }
        // restart listening for next download request
    }

}


void Node::readTorrentFile()
{
    string sTorrentFile = mArg1; // torrent file
    string sDestPath = mArg2;    // destination path to save the downloaded file
    map<string, string> sConfigData;

    // parse the torrent file
    ifstream ifTorrentFile;
    ifTorrentFile.open(sTorrentFile.c_str());
    if (!ifTorrentFile)
    {
        cerr << "[ERROR] Unable to open the torrent file: " << sTorrentFile << endl;
        exit(EXIT_FAILURE);
    }

    string line;
    while(getline(ifTorrentFile, line))
    {
        parseInputLine(IN  line,
                       OUT sConfigData);
    }

    if (mCmd == "get")
    {
        // download
        // Fill the DataFileInfo_t structure
        mDownFileInfo.mFile = sConfigData["filepath"];
        mDownFileInfo.mFileSize = stoi(sConfigData["filesize"]);
        mDownFileInfo.mFileHash = sConfigData["filehash"]; // multiple of 20
        mDownFileInfo.mHashOfFileHash = constructHashOfHash(mDownFileInfo.mFileHash); // 40 Chars
        mDownFileInfo.mTorrentFile = sTorrentFile;
    }
    else
    {
        // remove
        // Fill the DataFileInfo_t structure
        mRemoveFileInfo.mFile = sConfigData["filepath"];
        mRemoveFileInfo.mFileSize = stoi(sConfigData["filesize"]);
        mRemoveFileInfo.mFileHash = sConfigData["filehash"]; // multiple of 20
        mRemoveFileInfo.mHashOfFileHash = constructHashOfHash(mRemoveFileInfo.mFileHash); // 40 Chars
        mRemoveFileInfo.mTorrentFile = sTorrentFile;
    }
}


// Send a request to Tracker for downloading a file.
void Node::sendDownloadRequest()
{
    // prepare the Node-Tracker Message
    struct NodeRequestMessage_t sMsg;
    bzero(&sMsg, sizeof(NodeRequestMessage_t));
    
    sMsg.mRequestType = DOWNLOAD_REQUEST;
    sMsg.mNodePortNum = mArgs.mPortNumNode;
    mArgs.mIpAddressNode.copy(sMsg.mNodeIpAddress, mArgs.mIpAddressNode.size());
    mDownFileInfo.mHashOfFileHash.copy(sMsg.mFileHash, mDownFileInfo.mHashOfFileHash.size());

    // connect to the Tracker server
    int sSocketFd = connectToServer(mArgs.mIpAddressTracker1, mArgs.mPortNumTracker1);

    // sending message to Tracker
    int nBytes = write(sSocketFd, &sMsg, sizeof(NodeRequestMessage_t));
    if (nBytes < 0)
    {
        error("[ERROR] Node: Write failed");
    }

    // receive response from server
    TrackerResponseMessage_t sResponseMsg;
    bzero(&sResponseMsg, sizeof(TrackerResponseMessage_t));
    nBytes = read(sSocketFd,
                  (TrackerResponseMessage_t*)&sResponseMsg,
                  sizeof(TrackerResponseMessage_t));
    if (nBytes < 0)
    {
        error("[ERROR] Node: Read from Tracker Socket failed");
    }

    cout << "Node: Tracker Response is ... ##" << sResponseMsg.mResponseType << "##" << sResponseMsg.mNodeInfoList << endl;
    
    // Disconnect to the Tracker Server
    disconnectFromServer(sSocketFd);
    
    if (sResponseMsg.mResponseType == START_DOWNLOAD)
    {
        // save the received nodes list in member data
        // ip_addr:port,ip_addr:port,...
        mSeedingPeersList.push_back(string(sResponseMsg.mNodeInfoList));
    }
}

void Node::selectPeersForDownload()
{
    // @TODO
    cout << "Selected Peers for download ..." << endl;
    string sSeedNodeInfo = mSeedingPeersList[0];
    cout << "sSeedNodeInfo: " << sSeedNodeInfo << endl;
    string sSeedNodeIpAddr = sSeedNodeInfo.substr(0, sSeedNodeInfo.find_last_of(":"));
    int sSeedNodePortNum = stoi(sSeedNodeInfo.substr(sSeedNodeInfo.find_last_of(":")+1));

    // Send a request to connect to the seeding peer node
    // test assuming 1 node seeding

    P2PRequestMessage_t sReqMsg;
    bzero(&sReqMsg, sizeof(P2PRequestMessage_t));

    sReqMsg.mRequestType = DOWNLOAD_REQUEST;
    mDownFileInfo.mHashOfFileHash.copy(sReqMsg.mHashOfHash, mDownFileInfo.mFileHash.size());

    // connect to a peer node
    int sSocketFd = connectToServer(sSeedNodeIpAddr, sSeedNodePortNum);

    // sending message to Peer node
    int nBytes = write(sSocketFd, &sReqMsg, sizeof(P2PRequestMessage_t));
    if (nBytes < 0)
    {
        error("[ERROR] Node: Write failed");
    }

    P2PResponseMessage_t sRspMsg;
    bzero(&sRspMsg, sizeof(P2PResponseMessage_t));

    nBytes = read(sSocketFd,
                  (P2PResponseMessage_t*)&sRspMsg,
                  sizeof(P2PResponseMessage_t));
    if (nBytes < 0)
    {
        error("[ERROR] Node: Read from Tracker Socket failed");
    }

    // disconnect from the peer node
    // as we need to connect to the new seeding port for file transfer
    // and release this socket for next requests.
    disconnectFromServer(sSocketFd);

    // extract peers information
    // assuming only one seeder now @TODO for list of seeders
    string sNewSeedingIpAddress(sRspMsg.mSeedingIpAddress);
    int sNewSeedingPortNum = sRspMsg.mSeedingPortNum;

    cout << "New Connection Info: " << (sNewSeedingIpAddress +":"+to_string(sNewSeedingPortNum)) << endl;
    cout << "File to download:    " << sRspMsg.mSeedingFile << endl;

    // re-connect to peer on new socket for file transfer
    //sSocketFd = connectToServer(sNewSeedingIpAddress, sNewSeedingPortNum);
}

void Node::startDownload()
{
    // @TODO
    cout << "Download Started ..." << endl;
}


void Node::sendRemoveRequest()
{
    // prepare the Node-Tracker Message (Remove Entry)
    struct NodeRequestMessage_t sMsg;
    bzero(&sMsg, sizeof(NodeRequestMessage_t));

    sMsg.mRequestType = REMOVE_REQUEST;
    sMsg.mNodePortNum = mArgs.mPortNumNode;
    mArgs.mIpAddressNode.copy(sMsg.mNodeIpAddress, mArgs.mIpAddressNode.size());
    mRemoveFileInfo.mHashOfFileHash.copy(sMsg.mFileHash, mRemoveFileInfo.mHashOfFileHash.size());

    // connect to the Tracker server
    int sSocketFd = connectToServer(mArgs.mIpAddressTracker1, mArgs.mPortNumTracker1);

    // sending message to Tracker
    int nBytes = write(sSocketFd, &sMsg, sizeof(NodeRequestMessage_t));
    if (nBytes < 0)
    {
        error("[ERROR] Node: Write failed");
    }

    // receive response from server
    int sResponse = 0;
    nBytes = read(sSocketFd, &sResponse,   sizeof(int));
    if (nBytes < 0)
    {
        error("[ERROR] Node: Read from Tracker Socket failed");
    }

    cout << "Node: Tracker Response is ... ##" << sResponse << endl;
    
    // Disconnect to the Tracker Server
    disconnectFromServer(sSocketFd);
}


/******************************************************************************************************/
/*                                   Utilities for Helper Functions                                   */
/******************************************************************************************************/

void Node::parseInputLine(IN  string lineParm,
                          OUT map<string, string>& configDataParm)
{
    stringstream sInput(lineParm);
    string sInputStr;
    vector<string> sFields;
    while(getline(sInput, sInputStr, '='))
    {
        sFields.push_back(sInputStr);
    }
    configDataParm[sFields[0]] = sFields[1];
}


// create hash-of-hash for the data file.
string Node::constructHashOfHash(string dataFileHashParm)
{
    int sHashStringSize = dataFileHashParm.size();
    unsigned char sMessageDigest[SHA_DIGEST_LENGTH];
    char sMdString[SHA_DIGEST_LENGTH*2+1];
    char sBuffer[sHashStringSize+1];
    
    bzero(sBuffer, sizeof(sBuffer));
    bzero(sMessageDigest, SHA_DIGEST_LENGTH);
    bzero(sMdString, SHA_DIGEST_LENGTH*2+1);

    dataFileHashParm.copy(sBuffer, sHashStringSize);

    SHA1((const unsigned char*)&sBuffer, sHashStringSize, sMessageDigest);

    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        sprintf(&sMdString[i*2], "%02x", (unsigned int)sMessageDigest[i]);
    }
    return string(sMdString);
}


// Constructing cummulative hash of the file with logical chunks
void Node::constructSha1Hash(IN  string fileNameParm,
                             IN  int fileSizeParm,
                             OUT string& fileHashParm)
{
    unsigned char sMessageDigest[SHA_DIGEST_LENGTH];
    char sMdString[SHA_DIGEST_LENGTH*2+1];
    char sDataChunk[gChunkSize];
    ifstream ifFileData;

    // calculate the number of logical chunks of size 512KB each
    int sNumChunks = (fileSizeParm / gChunkSize);
    int sLastChunkSize = (fileSizeParm % gChunkSize);

    fileHashParm.clear();
    ifFileData.open(fileNameParm, ifstream::binary);
    for (int sIndex=0; sIndex <= sNumChunks; sIndex++)
    {
        ifFileData.seekg(sIndex*gChunkSize);
        if (sIndex == sNumChunks && sLastChunkSize > 0)
        {
            ifFileData.read(sDataChunk, sLastChunkSize);
            // calculate the has of the logical chunk of 512KB
            SHA1((const unsigned char*)&sDataChunk,
                sLastChunkSize,
                sMessageDigest);
        }
        else
        {
            ifFileData.read(sDataChunk, gChunkSize);
            // calculate the has of the logical chunk of 512KB
            SHA1((const unsigned char*)&sDataChunk,
                gChunkSize,
                sMessageDigest);
        }

        for (int i = 0; i < SHA_DIGEST_LENGTH/2; i++)
        {
            sprintf(&sMdString[i*2], "%02x", (unsigned int)sMessageDigest[i]);
        }
        fileHashParm.append(sMdString);
    }
    ifFileData.close();
}



