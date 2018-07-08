#pragma once

#include <husky/Log.hpp>
#include <cstdio>
#include <stdio.h>
#include <stdarg.h>

#if defined(HUSKY_WINDOWS)
#include <Windows.h>
#endif

namespace husky {

static const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void Log::debug(const char *format, ...)
{
#if defined(HUSKY_WINDOWS)
  SetConsoleTextAttribute(hConsole, 10); // Lime green
#endif

  va_list argptr;
  va_start(argptr, format);
  vprintf(format, argptr);
  va_end(argptr);

  printf("\n");
}

void Log::info(const char *format, ...)
{
#if defined(HUSKY_WINDOWS)
  SetConsoleTextAttribute(hConsole, 7); // Light gray (Windows default)
#endif

  va_list argptr;
  va_start(argptr, format);
  vprintf(format, argptr);
  va_end(argptr);

  printf("\n");
}

void Log::warning(const char *format, ...)
{
#if defined(HUSKY_WINDOWS)
  SetConsoleTextAttribute(hConsole, 14); // Yellow
#endif

  va_list argptr;
  va_start(argptr, format);
  vprintf(format, argptr);
  va_end(argptr);

  printf("\n");
}

void Log::error(const char *format, ...)
{
#if defined(HUSKY_WINDOWS)
  SetConsoleTextAttribute(hConsole, 12); // Red
#endif

  va_list argptr;
  va_start(argptr, format);
  vprintf(format, argptr);
  va_end(argptr);

  printf("\n");
}

}
