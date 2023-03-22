#pragma once
#include <string>
#include <map>
#include "Log.h"
#include "FileUtil.h"
#include "PathNode.h"

class Synchronizer
{
public:
	Synchronizer(Log& log);
	Synchronizer(std::string sourceDir, std::string cloneDir, std::string logFilePath, Log& log);
	int init(std::string sourceDir, std::string cloneDir, std::string logFilePath);
	int sync();
	void terminate();
	int detect_new_files(PathNode* newSnapshot, PathNode* oldSnapshot);
	void detect_deleted_files(PathNode* newSnapshot, PathNode* oldSnapshot);
	void detect_modified_files(PathNode* snapshotNode);
	void remove_unwanted_files(PathNode* dstDir, PathNode* srcDir);
	void copy_missing_clone_files(PathNode* dstDir, PathNode* srcDir);
	int update_modifed_clone_files(PathNode* dstDir, PathNode* srcDir);

private:

	bool running;
	PathNode* snapshotNode;
	FSPath rootSourceDir, rootCloneDir;
	std::ofstream logFile;
	Log& log;
};

