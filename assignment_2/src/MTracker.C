/*
Tracker Main
Usage:
    ./tracker -i this_tracker_ip:port  -o other_tracker_ip:port  -s seeders_list  -f log_file
*/

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include "Tracker.H"
#include "MTUtils.H"
using namespace std;


void printUsageAndExit()
{
    cout << "USAGE: " << flush;
    cout << "./tracker" << " -t <current_tracker_ip:port>"
                        << " -o <other_tracker_ip:port>"
                        << " -s <seeders_list_file>"
                        << " -f <log_file>" << endl;
    exit(EXIT_FAILURE);
}


void parseInputArgs(IN  int argcParm,
                    IN  char* argvParm[],
                    OUT struct TrackerArgs_t& inputArgsParm)
{
    int option;
    string sOptionString;
    int sPortNum;
    string sIpAddr;
    while ((option = getopt(argcParm, argvParm, "t:o:s:f:")) != -1 )
    {
        sOptionString.clear();
        switch(option)
        {
            case 't':
                sOptionString = string(optarg);
                sIpAddr = sOptionString.substr(0, sOptionString.find_last_of(":"));
                sPortNum = stoi(sOptionString.substr(sOptionString.find_last_of(":")+1));
                inputArgsParm.mCurrentTrackerPortNum = sPortNum;
                inputArgsParm.mCurrentTrackerIpAddr = sIpAddr;
                break;
            case 'o':
                sOptionString = string(optarg);
                sIpAddr = sOptionString.substr(0, sOptionString.find_last_of(":"));
                sPortNum = stoi(sOptionString.substr(sOptionString.find_last_of(":")+1));
                inputArgsParm.mOtherTrackerPortNum = sPortNum;
                inputArgsParm.mOtherTrackerIpAddr = sIpAddr;
                break;
            case 's':
                inputArgsParm.mSeedersListFileName = string(optarg);
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
    // parse the input arguments
    struct TrackerArgs_t sInputArgs;
    parseInputArgs(IN  argc,
                   IN  argv,
                   OUT sInputArgs);

    // create a Tracker object
    // This will also instantiate the server instance of Tracker
    Tracker sTracker(IN sInputArgs);

    // initialize the tracker.
    sTracker.init();

    // Start the Tracker
    sTracker.run();

    return 0;
}