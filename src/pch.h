#pragma once

//disable library warnings when using Visual Studio
//using /wall in VS produces a large amount of warnings
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <cmath>
#include <filesystem>

//reenable warnings for VS
#ifdef _MSC_VER
#pragma warning(pop)
#endif