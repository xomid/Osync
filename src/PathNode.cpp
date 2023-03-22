#include "PathNode.h"

void copy_path(PathNode* node, FSPath rootCloneDir) {

	auto dstParentPath = Osync::get_dst_path(rootCloneDir, node->root, node->path).parent_path();
	Osync::copy(node->path, dstParentPath);
}

void remove_path(PathNode* node, FSPath rootCloneDir) {

	auto dstPath = Osync::get_dst_path(rootCloneDir, node->root, node->path);
	Osync::remove_all(dstPath);
}

PathNode::PathNode(FSPath root, FSPath path, PathNodeType type)
	: root(root), path(path), type(type), checksum("0")
{
	filename = path.filename();
}

PathNode::~PathNode() {
	clear();
}

int PathNode::calculate_checksum() {
	if (type == PathNodeType::File) {
		checksum = Osync::get_file_md5(path.string());
		if (checksum.length() == 0)
			return 1;
	}
	else
		checksum = "0";
	return 0;
}

int PathNode::parse_path() {
	std::vector<FSPath> paths;
	Osync::get_path_list(path, paths);

	for (auto& path : paths) {
		add_node(path);
	}

	return 0;
}

std::map<FSPath, PathNode*>& PathNode::get_nodes() {
	return nodes;
}

FSPath& PathNode::get_path() {
	return path;
}

void PathNode::clear() {
	for (auto n : nodes) {
		delete n.second;
	}
}

PathNode* PathNode::add_node(FSPath path) {
	auto isDirectoy = std::filesystem::is_directory(path);
	auto newNode = new PathNode(root, path, isDirectoy ? PathNodeType::Directory : PathNodeType::File);

	if (isDirectoy) {
		newNode->parse_path();
	}
	else if (newNode->calculate_checksum()) {
		//delete newNode;
		//throw;
	}

	nodes[path] = newNode;
	return newNode;
}

int PathNode::remove_node(FSPath path) {
	auto it = nodes.find(path);
	if (it != nodes.end()) {
		if (it->second) {
			delete it->second;
		}
		nodes.erase(it);
	}
	return 0;
}

PathNode* PathNode::find_node(FSPath path) {
	auto it = nodes.find(path);
	if (it != nodes.end())
		return it->second;
	return NULL;
}

int PathNode::update_checksum() {
	if (type == PathNodeType::File) {
		auto newChecksum = Osync::get_file_md5(path.string());
		if (newChecksum != checksum) {
			checksum = newChecksum;
			return 1;
		}
		return 0;
	}
	return 0;
}

bool PathNode::is_directory() const {
	return type == PathNodeType::Directory;
}

bool PathNode::is_file() const {
	return type == PathNodeType::File;
}

std::string PathNode::get_checksum() const {
	return checksum;
}

void PathNode::set_checksum(std::string newChecksum) {
	checksum = newChecksum;
}