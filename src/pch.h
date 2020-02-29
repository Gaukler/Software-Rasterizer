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
#include <stddef.h>
#include <cassert>
#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include <array>
#include <immintrin.h>

//reenable warnings for VS
#ifdef _MSC_VER
#pragma warning(pop)
#pragma warning(disable: 5045) //specter mitigation warning
#pragma warning(disable: 4514) //compiler removing inline functions
#endif