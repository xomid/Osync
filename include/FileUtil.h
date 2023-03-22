#pragma once
#include <string>
#include <vector>
#include <filesystem>

typedef std::filesystem::path FSPath;

namespace Osync {
	// returns the paths inside a directory, non-recursive
	size_t get_path_list(FSPath directory, std::vector<FSPath>& paths);

	// success: the relative path respecing a base path 
	// failure: return path itself
	FSPath to_relative_path(FSPath& root, FSPath path);

	// convert dstPath to a path inside rootSourceDir
	FSPath get_src_path(FSPath rootSourceDir, FSPath rootDestDir, FSPath dstPath);

	// convert srcPath to a path inside rootDestDir
	FSPath get_dst_path(FSPath rootDestDir, FSPath rootSrcDir, FSPath srcPath);

	// return the md5 of a file
	// it is also suitable for large files
	std::string get_file_md5(std::string path);

	// copy a file or directory into dstDirectory
	int copy(FSPath path, FSPath targetDirectory);

	// remove a file or directory shown by filePath
	int remove_all(FSPath filePath);

	// create a directory
	int create_directory(FSPath directory);

	// returns true if path exists, 
	// returns false if path not found or an error occurs
	bool path_exists(FSPath path);

}