
#ifndef __FILE_UTILS_H_
#define __FILE_UTILS_H_

#include <string>
#include "../translation/strings.h"

class FileUtils {
public:
	static bool exists(const char* fileOrDirectory);
	static bool makeDirectory(const char* dir);
	static bool deleteFile(const char* fileOrDirectory);
	static bool changeDirectory(const std::string &filePrefix, const std::string &newDir);
	static bool changeFilePrefix(const std::string &currentPrefix, const std::string &newPrefix, const std::string &directory);
	static bool deletePrefixFiles(const std::string &prefix);
	static bool executeCommand(const char* command);
};

#endif
