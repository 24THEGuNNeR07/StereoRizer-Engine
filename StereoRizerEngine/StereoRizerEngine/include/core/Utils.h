#pragma once

#include <iostream>
#include <sstream>
#include <fstream>

namespace stereorizer::core
{
	namespace FileUtils
	{
		inline std::string readFile(const char* path)
		{
			std::string str;
			std::ifstream file;
			file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try
			{
				file.open(path);
				std::stringstream strStream;
				strStream << file.rdbuf();
				file.close();
				str = strStream.str();
			}
			catch (std::ifstream::failure e)
			{
				std::cout << "ERROR::FILE_NOT_SUCCESFULLY_READ" << std::endl;
			}
			return str;
		}
	}
	namespace StrUtils
	{
		inline bool endsWith(const std::string& str, const std::string& suffix)
		{
			return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
		}

		inline bool startsWith(const std::string& str, const std::string& prefix)
		{
			return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
		}

		inline bool endsWith(const std::string& str, const char* suffix, uint32_t suffixLen)
		{
			return str.size() >= suffixLen && 0 == str.compare(str.size() - suffixLen, suffixLen, suffix, suffixLen);
		}

		inline bool endsWith(const std::string& str, const char* suffix)
		{
			return endsWith(str, suffix, (uint32_t)std::string::traits_type::length(suffix));
		}

		inline bool startsWith(const std::string& str, const char* prefix, uint32_t prefixLen)
		{
			return str.size() >= prefixLen && 0 == str.compare(0, prefixLen, prefix, prefixLen);
		}

		inline bool startsWith(const std::string& str, const char* prefix)
		{
			return startsWith(str, prefix, (uint32_t)std::string::traits_type::length(prefix));
		}

	}
}