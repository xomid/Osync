#pragma once
#include "FileUtil.h"
#include <map>

enum class PathNodeType {
	Directory,
	File
};

/// <summary>
/// This class correlate to a path
/// </summary>
class PathNode {
public:
	PathNode(FSPath root, FSPath path, PathNodeType type);
	~PathNode();

	int parse_path();
	int calculate_checksum();
	std::map<FSPath, PathNode*>& get_nodes();
	FSPath& get_path();
	PathNode* add_node(FSPath path);
	PathNode* find_node(FSPath path);
	int remove_node(FSPath path);
	int update_checksum();
	void clear();
	bool is_directory() const;
	bool is_file() const;
	std::string get_checksum() const;
	void set_checksum(std::string);

	FSPath root, path, filename;
private:
	std::string checksum;
	PathNodeType type;
	std::map<FSPath, PathNode*> nodes;
};

void copy_path(PathNode* node, FSPath rootCloneDir);

void remove_path(PathNode* node, FSPath rootCloneDir);
