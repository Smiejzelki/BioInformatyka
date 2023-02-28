#pragma once
#ifndef _WIN32 
#ifndef _WIN64
#error Windows build only
#endif
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#define BIO_LIKELY		[[likely]]
#define BIO_UNLIKELY	[[unlikely]]

#include <exception>
#include <vector>
#include <type_traits>
#include <string>
#include <assert.h>
#include <limits>
#include <array>
#include <atomic>
#include <thread>
#include <mutex>
#include <utility>
#include <iostream>
#include <fstream>
#include <functional>
#include <filesystem>
#include <unordered_map>
#include <numeric>
#include <string_view>
#include <variant>
#include <map>
#include <set>

#ifdef BIO_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <tchar.h>

constexpr WCHAR WIN32_API_WINDOW_CLASS_NAME[] = L"BIO_INFORMATYKA_WINDOW_CLASS";
#elif defined BIO_PLATFORM_LINUX
#endif

#define BIO_CONSTEVAL consteval
#define NON_COPY_CONSTRUCTIBLE(class_) class_(const class_&) = delete; 
#define NON_COPY_ASSIGNABLE(class_)	class_& operator=(const class_&) = delete;

#define NON_COPYABLE(class_)		\
NON_COPY_CONSTRUCTIBLE(class_)		\
NON_COPY_ASSIGNABLE(class_)

struct Exception
{
	constexpr Exception(const char* message, const char* fileName, const std::size_t fileLine) noexcept
		:
		Message(message),
		FileName(fileName),
		FileLine(fileLine)
	{}

	Exception() noexcept = default;

	const char* Message;
	const char* FileName;
	const std::size_t FileLine;
};

template<typename CharacterPointer>
void LogToConsole(CharacterPointer message);

template<>
inline void LogToConsole(char* message)
{
	fprintf(stdout, "%s\n", message);
}

template<>
inline void LogToConsole(wchar_t* message)
{
	std::wcout << message << L'\n';
}

template<>
inline void LogToConsole(const char* message)
{
	fprintf(stdout, "%s\n", message);
}

template<>
inline void LogToConsole(const wchar_t* message)
{
	std::wcout << message << L'\n';
}

#ifdef BIO_DEBUG
#define BIO_ASSERT(x) assert(x)
#else
#define BIO_ASSERT(x)  
#endif

#define LOG(message) LogToConsole(message)

#ifdef __FILE__
#define EXCEPTION_FILE_NAME __FILE__
#else
#define EXCEPTION_FILE_NAME "Unknown file name"
#endif

#ifdef __LINE__
#define EXCEPTION_FILE_LINE __LINE__
#else
#define EXCEPTION_FILE_LINE "Unknown file line"
#endif

#define THROW_EXCEPTION(message) throw Exception(message, EXCEPTION_FILE_NAME, EXCEPTION_FILE_LINE)

template<typename CharacterType>
class CommandLineArgumentsBase
{
private:
public:
	inline explicit CommandLineArgumentsBase(const uint32_t argumentCount, CharacterType** arguments) noexcept
		:
		m_Arguments(argumentCount)
	{
		for (uint32_t i{ 0U }; i < argumentCount; ++i)
			m_Arguments[i] = arguments[i];
	}

	~CommandLineArgumentsBase() noexcept = default;

	inline std::vector<CharacterType*>::const_iterator begin() const noexcept
	{
		return m_Arguments.cbegin();
	}

	inline std::vector<CharacterType*>::const_iterator end() const noexcept
	{
		return m_Arguments.cend();
	}
private:
	std::vector<CharacterType*> m_Arguments;
};

inline void HandleExceptions()
{
	try
	{
		/* Re-throw current exception */
		throw;
	}
	catch (const std::exception& exception)
	{
		LOG("A standard exception has been thrown:");
		LOG(exception.what());
	}
	catch (const Exception& exception)
	{
		LOG("An application exception has been thrown:");
		fprintf(stdout, "Message: %s | In File: %s | On Line: %d\n", exception.Message, exception.FileName, static_cast<int>(exception.FileLine));
	}
}

#ifdef BIO_PLATFORM_WINDOWS
using CommandLineArguments = CommandLineArgumentsBase<std::wstring::value_type>;
#elif defined BIO_PLATFORM_LINUX
using CommandLineArguments = CommandLineArgumentsBase<std::string::value_type>;
#endif

inline namespace VolumeLiterals
{
	constexpr size_t operator ""_Kb(const size_t volume) noexcept
	{
		return volume * 1000U;
	}

	constexpr size_t operator ""_Mb(const size_t volume) noexcept
	{
		return volume * 1000'000U;
	}
}