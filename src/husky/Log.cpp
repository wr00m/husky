#pragma once

#include <husky/Log.hpp>
#include <cstdio>
#include <stdio.h>
#include <stdarg.h>
#include <Windows.h>

namespace husky {

static const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void Log::debug(const char *format, ...)
{
  SetConsoleTextAttribute(hConsole, 10); // Lime green
  //printf("DEBUG: ");

  va_list argptr;
  va_start(argptr, format);
  vprintf(format, argptr);
  va_end(argptr);

  printf("\n");
}

void Log::info(const char *format, ...)
{
  SetConsoleTextAttribute(hConsole, 7); // Light gray (Windows default)
  //printf("INFO: ");

  va_list argptr;
  va_start(argptr, format);
  vprintf(format, argptr);
  va_end(argptr);

  printf("\n");
}

void Log::warning(const char *format, ...)
{
  SetConsoleTextAttribute(hConsole, 14); // Yellow
  //printf("WARNING: ");

  va_list argptr;
  va_start(argptr, format);
  vprintf(format, argptr);
  va_end(argptr);

  printf("\n");
}

void Log::error(const char *format, ...)
{
  SetConsoleTextAttribute(hConsole, 12); // Red
  //printf("ERROR: ");

  va_list argptr;
  va_start(argptr, format);
  vprintf(format, argptr);
  va_end(argptr);

  printf("\n");
}

}
