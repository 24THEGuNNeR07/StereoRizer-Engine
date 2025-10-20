#pragma once

#include <iostream>

// Simple logging macros. Wrap std::cout / std::cerr to make it easy to change later.
#define LOG_INFO(msg) do { std::cout << msg << std::endl; } while(0)
#define LOG_ERROR(msg) do { std::cerr << msg << std::endl; } while(0)
