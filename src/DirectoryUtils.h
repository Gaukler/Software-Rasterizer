#pragma once
#include "pch.h"

namespace DirectoryUtils {
	namespace fs = std::filesystem;

	fs::path getWorkingDirectory();

	//loops trough up from working directory and checks for "resource" folder
	//if none is found returns root of the path (C: or similar)
	fs::path searchResourceDirectory();
}