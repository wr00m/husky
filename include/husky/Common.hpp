#pragma once

namespace husky {

#ifdef HUSKY_STATIC
#define HUSKY_DLL __declspec(dllimport)
#else
#define HUSKY_DLL __declspec(dllexport)
#endif

}
