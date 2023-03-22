#include "Synchronizer.h"
#include "FileUtil.h"
#include "PathNode.h"
#include <iostream>

Synchronizer::Synchronizer(Log& log)
	: log(log), rootSourceDir(""), rootCloneDir(""), running(false), snapshotNode(NULL) {

}

Synchronizer::Synchronizer(std::string sourceDir, std::string cloneDir, std::string logFilePath, Log& log)
	: log(log), running(false), snapshotNode(NULL) {

	init(sourceDir, cloneDir, logFilePath);
}

int Synchronizer::init(std::string sourceDir, std::string cloneDir, std::string logFilePath) {

	if (Osync::path_exists(sourceDir)) {
		terminate();

		this->rootSourceDir = sourceDir;
		this->rootCloneDir = cloneDir;

		// take a sanpshot of the directory and all its hashes
		snapshotNode = new PathNode(this->rootSourceDir, this->rootSourceDir, PathNodeType::Directory);
		snapshotNode->parse_path();
		return 0;
	}
	else {
		log << "source directory not found" << std::endl;
	}

	return 1;
}

void Synchronizer::terminate() {

	if (snapshotNode) {
		delete snapshotNode;
		snapshotNode = NULL;
	}

	// log the termination of previous running operation
	if (running) {
		log << "current watch is aborted" << std::endl;
	}

	running = false;
}

int Synchronizer::detect_new_files(PathNode* newSnapshot, PathNode* oldSnapshot) {

	auto& newSnapshotChildren = newSnapshot->get_nodes();
	auto& oldSnapshotChildren = oldSnapshot->get_nodes();

	for (auto& newSnapshotChild : newSnapshotChildren) {
		auto newSnapshotChildNode = newSnapshotChild.second;
		auto newSnapshotChildNodePath = newSnapshotChild.first;
		auto oldSnapshotChildNode = oldSnapshot->find_node(newSnapshotChildNodePath);

		if (!oldSnapshotChildNode) {
			auto newNode = oldSnapshot->add_node(newSnapshotChildNodePath);
			copy_path(newNode, rootCloneDir);
			log << "created: " << newNode->get_path() << std::endl;
		}
		else if (newSnapshotChildNode->is_directory()) {
			detect_new_files(newSnapshotChildNode, oldSnapshotChildNode);
		}
	}

	return 0;
}

void Synchronizer::detect_deleted_files(PathNode* newSnapshot, PathNode* oldSnapshot) {

	auto& newSnapshotChildren = newSnapshot->get_nodes();
	auto& oldSnapshotChildren = oldSnapshot->get_nodes();

	auto it = oldSnapshotChildren.begin();
	for (; it != oldSnapshotChildren.end(); ) {
		auto oldSnapshotChildNode = it->second;
		auto oldSnapshotChildNodePath = it->first;
		auto newSnapshotChildNode = newSnapshot->find_node(oldSnapshotChildNodePath);

		if (!newSnapshotChildNode) {
			remove_path(oldSnapshotChildNode, rootCloneDir);
			log << "deleted: " << oldSnapshotChildNodePath.string() << std::endl;
			delete oldSnapshotChildNode;
			it = oldSnapshotChildren.erase(it++);
		}
		else {
			if (oldSnapshotChildNode->is_directory()) {
				detect_deleted_files(newSnapshotChildNode, oldSnapshotChildNode);
			}

			++it;
		}
	}
}

void Synchronizer::detect_modified_files(PathNode* snapshotNode) {

	if (snapshotNode) {
		for (auto& subNodeHandle : snapshotNode->get_nodes()) {
			auto subNode = subNodeHandle.second;
			if (subNode->is_directory()) {
				detect_modified_files(subNode);
			}
			else {
				if (subNode->update_checksum()) {
					copy_path(subNode, rootCloneDir);
					log << "modified: " << subNode->get_path() << std::endl;
				}
			}
		}
	}
}

void Synchronizer::remove_unwanted_files(PathNode* dstDir, PathNode* srcDir) {

	auto& dstDirChildren = dstDir->get_nodes();
	auto& srcDirChildren = srcDir->get_nodes();

	auto it = dstDirChildren.begin();
	for (; it != dstDirChildren.end(); ) {
		auto dstDirChildNode = it->second;
		auto dstDirChildNodePath = it->first;
		auto dstDirChildNodeSrcPath = Osync::get_src_path(rootSourceDir, rootCloneDir, dstDirChildNodePath);
		auto srcDirChildNode = srcDir->find_node(dstDirChildNodeSrcPath);

		if (!srcDirChildNode) {
			Osync::remove_all(dstDirChildNodePath);
			log << "unwanted: " << dstDirChildNodePath << std::endl;
			delete dstDirChildNode;
			it = dstDirChildren.erase(it++);
		}
		else {
			if (srcDirChildNode->is_directory()) {
				remove_unwanted_files(dstDirChildNode, srcDirChildNode);
			}
			++it;
		}
	}
}

void Synchronizer::copy_missing_clone_files(PathNode* dstDir, PathNode* srcDir) {

	auto& dstDirChildren = dstDir->get_nodes();
	auto& srcDirChildren = srcDir->get_nodes();


	for (auto& srcDirChild : srcDirChildren) {
		auto srcDirChildNode = srcDirChild.second;
		auto srcDirChildPath = srcDirChild.first;
		auto dstDirChildPath = Osync::get_dst_path(rootCloneDir, rootSourceDir, srcDirChildPath);
		auto dstDirChildNode = dstDir->find_node(dstDirChildPath);

		if (!dstDirChildNode) {
			copy_path(srcDirChildNode, rootCloneDir);
			log << "copied: " << dstDirChildPath << std::endl;
		}
		else if (dstDirChildNode->is_directory()) {
			copy_missing_clone_files(dstDirChildNode, srcDirChildNode);
		}
	}
}

int Synchronizer::update_modifed_clone_files(PathNode* dstDir, PathNode* srcDir) {
	// nodes must be non-empty
	if (!dstDir || !srcDir) {
		log << "error while updating destination directory" << std::endl;
		return 1;
	}

	if (dstDir->is_directory()) {
		for (auto& dstChild : dstDir->get_nodes()) {
			auto dstChildNode = dstChild.second;
			auto dstChildNodePath = dstChild.first;
			// find out what would path in src be
			auto matchingSrcChildPath = Osync::get_src_path(rootSourceDir, rootCloneDir, dstChildNodePath);
			// find the matching node in src directory
			// if not found log error and return; because it is guaranteed to receive two identical structures at least
			auto matchingSrcChildNode = srcDir->find_node(matchingSrcChildPath);
			if (matchingSrcChildNode)
				update_modifed_clone_files(dstChildNode, matchingSrcChildNode);
			else {
				// exception, stop!
				log << "error while updating destination directory" << std::endl;
				return 1;
			}
		}
	}
	else {
		auto srcDirChecksum = srcDir->get_checksum();
		if (dstDir->get_checksum() != srcDirChecksum) {
			// copy from src to dst
			copy_path(srcDir, rootCloneDir);
			log << "updated: " << dstDir->get_path() << std::endl;
			// assign the new checksum
			dstDir->set_checksum(srcDirChecksum);
		}
	}

	return 0;
}

int Synchronizer::sync() {

	// scan src directory
	PathNode newSrcSnapshot(rootSourceDir, rootSourceDir, PathNodeType::Directory);
	if (newSrcSnapshot.parse_path())
		return 1;

	detect_new_files(&newSrcSnapshot, snapshotNode);

	detect_deleted_files(&newSrcSnapshot, snapshotNode);

	detect_modified_files(snapshotNode);

	// scan dst directory
	PathNode newDstNode(rootCloneDir, rootCloneDir, PathNodeType::Directory);
	if (newDstNode.parse_path())
		return 1;

	remove_unwanted_files(&newDstNode, snapshotNode);

	copy_missing_clone_files(&newDstNode, snapshotNode);

	// after calling remove_unwanted and copy_missing, it is guranteed to have two
	// same structures but maybe different hash(content) which we will use to detect 
	// neccessary update from src to dst directory
	update_modifed_clone_files(&newDstNode, snapshotNode);

	return 0;
}


