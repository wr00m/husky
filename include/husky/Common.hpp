#pragma once

#include <cstdint>
//#include <memory>

namespace husky {

#if defined(__linux__)
#define HUSKY_LINUX
#elif defined(_WIN32)
#define HUSKY_WINDOWS
#else
#error "Unsupported platform"
#endif

#if defined(HUSKY_STATIC)
#define HUSKY_DLL __declspec(dllimport)
#else
#define HUSKY_DLL __declspec(dllexport)
#endif

}
