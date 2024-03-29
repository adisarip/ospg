
/*
    MTorrent Application Message Structures & Other Common Utilities
*/

#ifndef MTUTILS_H
#define MTUTILS_H

#include <string>
#include <stdio.h>
using namespace std;

// defines for function parameters readability
#define IN
#define OUT

#define SUCCESS  0
#define FAILURE -1
#define CLOSE_AND_EXIT 2

#define SIZE_IP_ADDRESS  16
#define SIZE_FILENAME    256
#define SIZE_SHA1_HASH   64
#define SIZE_NODE_LIST   4096


// Other global defines
#define INPUT_ARGS_COUNT 9

const long int gChunkSize  = 65536; // 64KB = 65536 bytes

// Type of request for the client node to start with
// Type of requests the Tracker will receive
enum MessageType
{
    // Types of requests
    SEEDING_REQUEST  = 0x01,
    DOWNLOAD_REQUEST = 0x02,
    REMOVE_REQUEST   = 0x03,
    
    // Types of responses
    START_SEEDING      = 0x11,
    START_DOWNLOAD     = 0x12,
    NO_PEERS_AVAILABLE = 0x13,
    DUPLICATE_ENTRY    = 0x14,
    ENTRY_REMOVED      = 0x15,
    CONNECT_FOR_TRFR   = 0x16,
    INITIATE_TRANSFER  = 0x17,
    REQUEST_APPROVED   = 0x18,
    TRFR_IN_PROGRESS   = 0x19,
    LAST_PACKET        = 0x20,
    SEND_PACKET        = 0x21,
    STOP_TRFR          = 0x22
};

// Struct representing all the input args of Tracker
struct TrackerArgs_t
{
    int mCurrentTrackerPortNum;
    int mOtherTrackerPortNum;
    string mCurrentTrackerIpAddr;
    string mOtherTrackerIpAddr;
    string mSeedersListFileName;
    string mLogFile;
};

// structure representing P2P Client Node arguments
struct NodeArgs_t
{
    // args
    int mPortNumNode;
    int mPortNumTracker1;
    int mPortNumTracker2;
    string mIpAddressNode;
    string mIpAddressTracker1;
    string mIpAddressTracker2;
    string mLogFile;
};

// Data File Metadata
struct DataFileInfo_t
{
    long int mFileSize;     // Data File Size in bytes
    string mFile;           // Absolute path with FileName
    string mFileHash;       // Hash of file in chunks = multiple of 20 characters
    string mHashOfFileHash; // Hash of Hash 40 characters
    string mTorrentFile;    // torrent file for the seeding/downloading data file
};

// Node-Tracker Request Message Format
struct NodeRequestMessage_t
{
    int  mRequestType;                      // Type of request seeding/download from Node-Tracker
    int  mNodePortNum;                      // Port number of the Node
    char mNodeIpAddress[SIZE_IP_ADDRESS+1]; // Ip Address of the Node
    char mFileHash[SIZE_SHA1_HASH+1];       // Hash of Hash 40 characters
};

// Tracker acknowledgement/response message format
struct TrackerResponseMessage_t
{
    int  mResponseType;                     // Tracker response message type
    char mNodeInfoList[SIZE_NODE_LIST+1];     // Comma seperated list of the seeders info ip:port,ip:port,...
};

// Node Listener request/response message
struct ListenerMessage_t
{
    int mMessageType;
    int mNewPortNum;
};

// Node Listener response message
struct ListenerResponseMessage_t
{
    int mResponseType;
    int mNewPortNum;
};

// Peer-Peer Request Message Format
struct P2PRequestMessage_t
{
    int  mRequestType;
    char mHashOfHash[SIZE_SHA1_HASH+1];
};

// Peer-Peer Response Message Format
struct P2PResponseMessage_t
{
    int  mResponseType;                 // response type
    long int mFileSize;                 // Data File Size in bytes
    char mSeedingFile[SIZE_FILENAME+1]; // Absolute path with FileName
};

// Peer-Peer Data transfer packet Format
struct P2PDataRequest_t
{
    int  mRequestType;
    char mReqChunkHash[SIZE_SHA1_HASH/2+1];
};

struct P2PDataResponse_t
{
    long int  mIndex;
    long int  mChunkSize;
    char mData[gChunkSize];
};

/*
struct FileHashTableEntry_t
{
    int mChunkIndex;
    string mHashOfFileHash; // 40 chars
};*/


inline void error(const char* error_message)
{
    perror(error_message);
    exit(EXIT_FAILURE);
};

#endif /* MTUTILS_H */