// Osync.cpp
// Write an app that synchronizes two folders and their contents

#include <iostream>
#include "ArgumentParser.h"
#include "AppInfo.h"
#include "Synchronizer.h"

static constexpr clock_t MAX_SYNC_INTERVAL = 24L * 3600L * 1000L; // 24 hours
static constexpr clock_t DEFAULT_SYNC_INTERVAL = 1000;

int main(int argc, char* argv[])
{
    ArgumentParser parser;
    parser.parse(argc, argv);

    // looking for simple tasks like help or version
    if (argc == 2) {
        if (parser.exists("-help") || parser.exists("-h")) {
            // show the user how to use the app
            std::cout << "sync \t[-s | -sourcedir] [-c | -clonedir] [-l | -logfile]\n\t[-help | -help] [-v | -version]" << std::endl;
            return 0;
        }
        else if (parser.exists("-version") || parser.exists("-v")) {
            // show the version and info
            std::cout << "Osync " << get_app_version() << std::endl;
            return 0;
        }
    }

    clock_t syncInterval = DEFAULT_SYNC_INTERVAL;
    std::string sourceDir, cloneDir, logFilePath;

    if ((parser.find("-s", sourceDir) || parser.find("-sourcedir", sourceDir)) &&
        (parser.find("-c", cloneDir) || parser.find("-clonedir", cloneDir)) &&
        (parser.find("-l", logFilePath) || parser.find("-logfile", logFilePath)))
    {
        // parsing optional interval parameter
        std::string strSyncInterval;
        if (parser.find("-i", strSyncInterval) || parser.find("-interval", strSyncInterval)) {
            clock_t interval = (clock_t)atoi(strSyncInterval.c_str());
            syncInterval = std::min(MAX_SYNC_INTERVAL, std::max(1L, interval));
        }

        bool shouldContinue = true;
        auto lastTime = clock();
        Log log(std::cout, logFilePath);

        Synchronizer synchronizer(log);
        auto error = synchronizer.init(sourceDir, cloneDir, logFilePath);

        while (!error) {
            if (clock() - lastTime >= syncInterval) {
                error = synchronizer.sync();
                lastTime = clock();
            }
        }

        synchronizer.terminate();
        return error;
    }

    // failed to receive the required options and values
    std::cout << "not enough arguments" << std::endl;
    return 1;
}
