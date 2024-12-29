#include "GIT.h"

#include <iostream>

void GIT::printErrorAndShutdown(std::string text)
{
	std::cerr << text << std::endl;
	git_libgit2_shutdown();
	exit(EXIT_FAILURE);
}

void GIT::getLastError(std::string info)
{

	const git_error* lastError = git_error_last();
	if (lastError != nullptr)
		printErrorAndShutdown(info + lastError->message);
	else
		printErrorAndShutdown(info + "Unknown error messasge");
}

void GIT::appendGitIgnore(const std::vector<std::string>& ignorePatterns)
{
	std::string gitIgnorePath = repoPath + "/.gitignore";
	std::set<std::string> existingPatterns;

	std::ifstream gitIgnoreFileIn(gitIgnorePath);
	if (gitIgnoreFileIn.is_open())
	{
		std::string line;
		while (std::getline(gitIgnoreFileIn, line))
			existingPatterns.insert(line);
		gitIgnoreFileIn.close();
	}

	std::ofstream gitIgnoreFileOut(gitIgnorePath, std::ios::app);
	if (!gitIgnoreFileOut.is_open())
	{
		printErrorAndShutdown("gitIgnoreFileOut open failed!");
	}

	for (const auto& pattern : ignorePatterns)
	{
		if (existingPatterns.find(pattern) == existingPatterns.end())
		{
			gitIgnoreFileOut << pattern << "\n";
			existingPatterns.insert(pattern);
		}
	}
	gitIgnoreFileOut.close();
}


GIT::FileStatus GIT::collectRepoStatus()
{
	GIT::FileStatus fileStatus;

	git_status_options opts;
	if (git_status_options_init(&opts, GIT_STATUS_OPTIONS_VERSION) < 0)
		getLastError("git_status_options_init failed: ");
	opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR; // Include untracked files
	opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED; // Explicitly include untracked files

	git_status_list* statusList;
	if (git_status_list_new(&statusList, repo, &opts) < 0)
		getLastError("git_status_list_new failed: ");

	size_t statusCount = git_status_list_entrycount(statusList);
	std::cout << "git_status_list_entrycount: " + std::to_string(statusCount) << std::endl;
	for (size_t i = 0; i < statusCount; i++)
	{
		const git_status_entry* entry = git_status_byindex(statusList, i);
		if (entry->status & GIT_STATUS_WT_NEW)
		{
			if (entry->index_to_workdir)
				fileStatus.untracked.push_back(entry->index_to_workdir->new_file.path);
		}
		else if (entry->status & GIT_STATUS_WT_MODIFIED)
		{
			if (entry->head_to_index)
				fileStatus.modified.push_back(entry->head_to_index->new_file.path);
		}
		else if (entry->status & GIT_STATUS_WT_DELETED)
		{
			if (entry->head_to_index)
				fileStatus.deleted.push_back(entry->head_to_index->old_file.path);
		}
	}

	git_status_list_free(statusList);

	return fileStatus;
}


std::string GIT::printRepoStatus(const GIT::FileStatus& fileStatus)
{
	std::ostringstream oss;
	for (const auto& file : fileStatus.untracked)
		oss << "Untracked files:\t" << file << "\n";
	for (const auto& file : fileStatus.modified)
		oss << "Modified files:\t" << file << "\n";
	for (const auto& file : fileStatus.deleted)
		oss << "Deleted files:\t" << file << "\n";

	return oss.str();
}

void GIT::addUntrackedFilesToIndex(const GIT::FileStatus& fileStatus)
{
	git_index* index = nullptr;
	if (git_repository_index(&index, repo) < 0)
		getLastError("get_repository_index failed: ");

	for (const auto& filePath : fileStatus.untracked)
	{
		if (git_index_add_bypath(index, filePath.c_str()) < 0)
			getLastError("git_index_add_bypath failed at " + filePath + ": ");
		std::cout << "Added file to index: " + filePath << std::endl;
	}


	switch (git_index_write(index))
	{
		git_index_free(index);
	case 0:
		break;
	default:
		getLastError("git_index_write failed: ");
		break;
	}

}


GIT::GIT(std::string repoPath) : repoPath(repoPath)
{

	git_libgit2_init();
	auto t0 = std::chrono::system_clock::now();
	while (git_repository_open(&repo, repoPath.c_str()) != GIT_OK)
	{
		if (std::chrono::system_clock::now() - t0 > std::chrono::seconds(GIT_TIMEOUT))
		{
			printErrorAndShutdown("git_repository_open timeout!");
		}
		if (git_repository_init(&repo, repoPath.c_str(), false) < 0)
			getLastError("git_repository_init failed: ");
		std::cout << "git_repository_init success" << std::endl;

		appendGitIgnore({ ".vs", "x64" });




	}
	std::cout << "git_repository_open success" << std::endl;

	auto fileStatus = collectRepoStatus();
	std::cout << printRepoStatus(fileStatus) << std::endl;
	// addUntrackedFilesToIndex(fileStatus);

}

GIT::~GIT()
{
	git_repository_free(repo);
	git_libgit2_shutdown();
}