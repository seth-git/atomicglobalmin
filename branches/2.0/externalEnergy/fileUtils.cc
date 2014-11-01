
#include "fileUtils.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

bool FileUtils::exists(const char* fileOrDirectory) {
	struct stat fileStatistics;
	return stat(fileOrDirectory, &fileStatistics) == 0; // If no errors occurred in getting stats, the file exists.
}

bool FileUtils::makeDirectory(const char* dir) {
	std::string command;
	command.append("mkdir ").append(dir);
	return executeCommand(command.c_str());
}

bool FileUtils::deleteFile(const char* fileOrDirectory) {
	std::string command;
	command.append("rm -r ").append(fileOrDirectory);
	return executeCommand(command.c_str());
}

bool FileUtils::changeDirectory(const std::string &filePrefix, const std::string &newDir) {
	std::string command;
	command.append("mv ").append(filePrefix).append(".* ").append(newDir);
	return executeCommand(command.c_str());
}

bool FileUtils::changeFilePrefix(const std::string &currentPrefix, const std::string &newPrefix, const std::string &directory) {
	std::string command;
	command.append("rename 's/").append(currentPrefix).append("/").append(newPrefix).append("/' ")
			.append(directory).append("/").append(currentPrefix).append(".*");
	return executeCommand(command.c_str());
}

bool FileUtils::deletePrefixFiles(const std::string &prefix) {
	std::string command;
	command.append("rm -f ").append(prefix).append(".*");
	return executeCommand(command.c_str());
}

bool FileUtils::executeCommand(const char* command) {
	if (system(command) == -1) {
		printf(strings::UnableToExecuteCommand, command);
		return false;
	}
	return true;
}
