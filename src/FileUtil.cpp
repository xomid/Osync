#include "FileUtil.h"
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <fstream>

namespace Osync {

	size_t get_path_list(FSPath directory, std::vector<FSPath>& paths)
	{
		for (const auto& entry : std::filesystem::directory_iterator(directory))
			paths.push_back(entry.path());
		return paths.size();
	}

	FSPath to_relative_path(FSPath& root, FSPath path) {
		std::error_code Ec;
		auto res = std::filesystem::relative(path, root, Ec);
		if (Ec) // returns 
			return path;
		return res;
	}

	FSPath get_src_path(FSPath rootSourceDir, FSPath rootCloneDir, FSPath dstPath) {
		auto relativeDstPath = Osync::to_relative_path(rootCloneDir, dstPath);
		return rootSourceDir / relativeDstPath;
	}

	FSPath get_dst_path(FSPath rootCloneDir, FSPath rootSrcDir, FSPath srcPath) {
		auto relativeSrcPath = Osync::to_relative_path(rootSrcDir, srcPath);
		return rootCloneDir / relativeSrcPath;
	}

	std::string get_file_md5(std::string fileName) {

		std::ifstream file(fileName, std::ifstream::binary);
		if (!file.is_open())
			return "";

		EVP_MD_CTX* mdctx;
		unsigned int md5_digest_len = EVP_MD_size(EVP_md5());

		// MD5_Init
		mdctx = EVP_MD_CTX_new();
		EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

		constexpr int buf_size = 1024 * 16;
		char buf[buf_size];
		while (file.good()) {
			auto count = file.read(buf, sizeof(buf)).gcount();
			EVP_DigestUpdate(mdctx, buf, count);
		}

		auto md5_digest = (unsigned char*)OPENSSL_malloc(md5_digest_len);
		EVP_DigestFinal_ex(mdctx, md5_digest, &md5_digest_len);
		EVP_MD_CTX_free(mdctx);

		std::string strResult;

		for (unsigned int i = 0; i < md5_digest_len; ++i) {
			char buf[32];
			sprintf_s(buf, "%02x", md5_digest[i]);
			strResult.append(buf);
		}

		OPENSSL_free(md5_digest);
		return strResult;
	}

	int copy(FSPath sourceFile, FSPath targetParent) {
		auto target = targetParent / sourceFile.filename(); // extracts the filename.

		std::error_code Ec;
		std::filesystem::create_directories(targetParent, Ec); // Recursively create target directory if not existing.

		if (Ec)
			return 1;

		std::filesystem::copy(sourceFile, target, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, Ec);

		if (Ec)
			return 1;

		return 0;
	}

	int remove_all(FSPath directory) {
		std::error_code Ec;
		std::filesystem::remove_all(directory, Ec);

		if (Ec)
			return 1;
		return 0;
	}

	int create_directory(FSPath directory) {
		std::error_code Ec;
		std::filesystem::create_directory(directory, Ec);

		if (Ec)
			return 1;
		return 0;
	}

	bool path_exists(FSPath path) {
		std::error_code Ec;
		if (std::filesystem::exists(path, Ec))
			return true;
		// discard the error
		return false;
	}
}