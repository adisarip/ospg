/*
Torrent P2P Node Main
Usage:
    ./client_node <node_config_file>
*/

#include <iostream>
#include <string>
#include <unistd.h>
#include "Node.H"
#include "MTUtils.H"
using namespace std;


void printUsageAndExit()
{
    cout << "USAGE: " << flush;
    cout << "./client_node" << " -c <client_node_ip:port>"
                            << " -x <tracker_1_ip:port>"
                            << " -y <tracker_2_ip:port>"
                            << " -f <log_file>" << endl;
    exit(EXIT_FAILURE);
}


void parseInputArgs(IN  int argcParm,
                    IN  char* argvParm[],
                    OUT struct NodeArgs_t& inputArgsParm)
{
    int option;
    string sOptionString;
    int sPortNum;
    string sIpAddr;
    while ((option = getopt(argcParm, argvParm, "c:x:y:f:")) != -1 )
    {
        sOptionString.clear();
        sIpAddr.clear();
        sPortNum = 0;
        switch(option)
        {
            case 'c':
                sOptionString = string(optarg);
                sIpAddr = sOptionString.substr(0, sOptionString.find_last_of(":"));
                sPortNum = stoi(sOptionString.substr(sOptionString.find_last_of(":")+1));
                inputArgsParm.mPortNumNode = sPortNum;
                inputArgsParm.mIpAddressNode = sIpAddr;
                break;
            case 'x':
                sOptionString = string(optarg);
                sIpAddr = sOptionString.substr(0, sOptionString.find_last_of(":"));
                sPortNum = stoi(sOptionString.substr(sOptionString.find_last_of(":")+1));
                inputArgsParm.mPortNumTracker1 = sPortNum;
                inputArgsParm.mIpAddressTracker1 = sIpAddr;
                break;
            case 'y':
                sOptionString = string(optarg);
                sIpAddr = sOptionString.substr(0, sOptionString.find_last_of(":"));
                sPortNum = stoi(sOptionString.substr(sOptionString.find_last_of(":")+1));
                inputArgsParm.mPortNumTracker2 = sPortNum;
                inputArgsParm.mIpAddressTracker2 = sIpAddr;
                break;
            case 'f':
                inputArgsParm.mLogFile = string(optarg);
                break;
            default:
                printUsageAndExit();
        }
    }

    if (optind < INPUT_ARGS_COUNT)
    {
        cerr << "[ERROR] Missing Input Arguments." << endl; 
        printUsageAndExit();
    }
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cerr << "[ERROR] Node: Input arguments missing" << endl;
        printUsageAndExit();
    }

    // parse the input arguments
    struct NodeArgs_t sInputArgs;
    parseInputArgs(IN  argc,
                   IN  argv,
                   OUT sInputArgs);

    Node sNode(IN sInputArgs);

    // Initialize Node
    int rc = sNode.init();
    if (rc == CLOSE_AND_EXIT)
    {
        cout << "Thanks for using MTorrent. See you ..." << endl;
        return 0;
    }

    return 0;
}

