/*
    MTorrent peer-peer Client Node.
*/

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
:Client(inputArgsParm.mIpAddressTracker1,
        inputArgsParm.mPortNumTracker1)
{
    mArgs = inputArgsParm;
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
    readTorrentFile();
    sendDownloadRequest();
    //selectPeersForDownload();
    //startDownload();
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


/*     PRIVATE HELPER METHODS     */

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


// send a request to Tracker for seeding a file.
void Node::sendSeedingRequest()
{
    // prepare the Node-Tracker Message
    struct NodeRequestMessage_t sMsg;
    bzero(&sMsg, sizeof(NodeRequestMessage_t));

    sMsg.mRequestType = SEEDING_REQUEST;
    sMsg.mNodePortNum = mArgs.mPortNumNode;
    mArgs.mIpAddressNode.copy(sMsg.mNodeIpAddress,
                                  mArgs.mIpAddressNode.size());
    mSeedFileInfo.mFile.copy(sMsg.mFile,
                             mSeedFileInfo.mFile.size());
    string sHashOfHash = constructHashOfHash(mSeedFileInfo.mFileHash);
    sHashOfHash.copy(sMsg.mFileHash,
                     sHashOfHash.size());

    // Connect to the Tracker Server
    int sSocketFd = connectToServer();

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
}


void Node::startSeeding()
{
    // @TODO
    cout << "Seeding Started ..." << endl;
}


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
        mDownFileInfo.mTorrentFile = sTorrentFile;
    }
    else
    {
        // remove
        // Fill the DataFileInfo_t structure
        mRemoveFileInfo.mFile = sConfigData["filepath"];
        mRemoveFileInfo.mFileSize = stoi(sConfigData["filesize"]);
        mRemoveFileInfo.mFileHash = sConfigData["filehash"]; // multiple of 20
        mRemoveFileInfo.mTorrentFile = sTorrentFile;
    }
}


void Node::sendRemoveRequest()
{
    // prepare the Node-Tracker Message (Remove Entry)
    struct NodeRequestMessage_t sMsg;
    bzero(&sMsg, sizeof(NodeRequestMessage_t));

    sMsg.mRequestType = REMOVE_REQUEST;
    sMsg.mNodePortNum = mArgs.mPortNumNode;
    mArgs.mIpAddressNode.copy(sMsg.mNodeIpAddress,
                                  mArgs.mIpAddressNode.size());
    mRemoveFileInfo.mFile.copy(sMsg.mFile,
                               mRemoveFileInfo.mFile.size());
    string sHashOfHash = constructHashOfHash(mRemoveFileInfo.mFileHash);
    sHashOfHash.copy(sMsg.mFileHash,
                     sHashOfHash.size());

    // connect to the Tracker server
    int sSocketFd = connectToServer();

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


// Send a request to Tracker for downloading a file.
void Node::sendDownloadRequest()
{
    // prepare the Node-Tracker Message
    struct NodeRequestMessage_t sMsg;
    bzero(&sMsg, sizeof(NodeRequestMessage_t));
    
    sMsg.mRequestType = DOWNLOAD_REQUEST;
    sMsg.mNodePortNum = mArgs.mPortNumNode;
    mArgs.mIpAddressNode.copy(sMsg.mNodeIpAddress,
                                  mArgs.mIpAddressNode.size());
    mDownFileInfo.mFile.copy(sMsg.mFile,
                             mDownFileInfo.mFile.size());
    string sHashOfHash = constructHashOfHash(mDownFileInfo.mFileHash);
    sHashOfHash.copy(sMsg.mFileHash,
                     sHashOfHash.size());

    // connect to the Tracker server
    int sSocketFd = connectToServer();

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

    cout << "Node: Tracker Response is ... ##"
         << sResponseMsg.mResponseType << "##"
         << sResponseMsg.mNodeInfoList <<   endl;
    
    // Disconnect to the Tracker Server
    disconnectFromServer(sSocketFd);
}

void Node::selectPeersForDownload()
{
    // @TODO
    cout << "Peers Selected for download ..." << endl;
}

void Node::startDownload()
{
    // @TODO
    cout << "Download Started ..." << endl;
}
