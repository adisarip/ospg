/*
Create <filename>.mtorrent file for any file that needs to be shared
./create_torrent mtorrent.config
*/
#include <iomanip>
#include <iostream>
#include<fstream>
#include<sstream>
#include <map>
#include <vector>
#include <string>
#include <openssl/sha.h>
#include <stdio.h>
#include <sys/stat.h>
using namespace std;

#define IN
#define OUT
#define REF
const unsigned long gChunkSize = 524288; // 512KB = 524288 bytes


void constructSha1Hash(string fileNameParm,
                       int fileSizeParm,
                       string& fileHashParm)
                       //vector<string>& hashDataParm)
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
        if (sIndex == sNumChunks && sLastChunkSize > 0)
        {
            ifFileData.read(sDataChunk + sIndex, sLastChunkSize);
            // calculate the has of the logical chunk of 512KB
            SHA1((const unsigned char*)&sDataChunk,
                sLastChunkSize,
                sMessageDigest);
        }
        else
        {
            ifFileData.read(sDataChunk + sIndex, gChunkSize);
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


void createTorrentFile(map<string, string>& configDataParm)
{
    string sTorrentDirPath("/Users/aditya/DevHub/pgssp/2018_Monsoon/os/ospg/assignment_2/torrents/");
    string sFileName = configDataParm["filename"];
    string sTorrentFile = sTorrentDirPath +
                          sFileName.substr(sFileName.find_last_of("/")+1) +
                          ".mtorrent" ;
    ofstream ofTorrentFile;
    ofTorrentFile.open(sTorrentFile);
    ofTorrentFile << "tracker_1=" << configDataParm["tracker_1"] << endl;
    ofTorrentFile << "tracker_2=" << configDataParm["tracker_2"] << endl;

    // add the file path
    ofTorrentFile << "filepath=" << sFileName << endl;
    
    // get the size of the file to be shared
    struct stat sFileStat;
    stat(sFileName.c_str(), &sFileStat);
    int sFileSize = sFileStat.st_size;
    ofTorrentFile << "filesize=" << to_string(sFileSize) << endl;

    // construct SHA1-1 of the file
    string sFileHash;
    constructSha1Hash(IN  sFileName,
                      IN  sFileSize,
                      OUT sFileHash);

    // create the <filename>.torrent file
    ofTorrentFile << "filehash=" << sFileHash << endl;
    ofTorrentFile.close();
}


void parseInputLine(string lineParm,
                    map<string, string>& configDataParm)
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


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "[ERROR] Input config file missing" << endl;
        cerr << "[USAGE] ./create_torrent <config_file>" << endl;
        exit(EXIT_FAILURE);
    }

    // parse the config file
    map<string, string> sConfigData;
    ifstream ifConfigFile;

    ifConfigFile.open(argv[1]);
    if (!ifConfigFile)
    {
        cerr << "[ERROR] Unable to open the config file: " << argv[1] << endl;
        exit(EXIT_FAILURE);
    }
    
    string line;
    sConfigData.clear();
    while(getline(ifConfigFile, line))
    {
        parseInputLine(IN  line,
                       OUT sConfigData);
    }

    // create the torrent file
    createTorrentFile(REF sConfigData);

    return 0;
}
