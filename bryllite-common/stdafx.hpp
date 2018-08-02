#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#endif //_WIN32

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstdarg>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <chrono>
#include <ctime>
#include <time.h>
#include <map>
#include <vector>
#include <array>

#include <thread>
#include <atomic>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>


// top level header
#include "version.hpp"
#include "types.hpp"
#include "assert.hpp"
#include "utils.hpp"
#include "exception.hpp"
#include "singleton.hpp"
#include "lockable.hpp"

// dependent header
#include "clog.hpp"
#include "threadable.hpp"
#include "fsm.hpp"
#include "timer.hpp"


