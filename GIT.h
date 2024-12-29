#pragma once

#include <git2.h>

#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <vector>
#include <set>

#define GIT_TIMEOUT 1

class GIT
{
private:
	std::string repoPath;
	git_repository* repo = nullptr;
	git_oid tree_oid;

	struct FileStatus
	{
		std::vector<std::string> untracked;
		std::vector<std::string> modified;
		std::vector<std::string> deleted;
	};

	void printErrorAndShutdown(std::string text);

	void getLastError(std::string info = "");

	void appendGitIgnore(const std::vector<std::string>& ignorePatterns);


	FileStatus collectRepoStatus();


	std::string printRepoStatus(const FileStatus& fileStatus);

	void addUntrackedFilesToIndex(const FileStatus& fileStatus);

public:
	GIT(std::string repoPath);
	static bool isRepoExist(std::string repoPath);
	
	~GIT();


};

