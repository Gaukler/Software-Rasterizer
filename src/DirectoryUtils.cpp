#include "pch.h"
#include "DirectoryUtils.h"

namespace DirectoryUtils {

	namespace fs = std::filesystem;

	fs::path getWorkingDirectory() {
		return fs::current_path();
	}

	fs::path searchResourceDirectory() {
		fs::path current = getWorkingDirectory();

		//parent_path of C: will return C: -> loop terminates
		while (current != current.parent_path()) {
			fs::directory_iterator it(current);

			for (auto& entry : it) {
				if (!entry.is_directory()) continue;
				//end of path is always "", second to last is folder
				if (*(--entry.path().end()) == "resources") {
					return entry;
				}
			}
			current = (current.parent_path());
		}

		return current;
	}
}