#include "Application.hpp"

/* Defined in user application */
extern Application* CreateApplication(CommandLineArguments&& arguments) noexcept;
/* Platform independent application entry point */
int CommonEntryPoint(CommandLineArguments&& arguments)
{
	try
	{
		std::unique_ptr<Application> application{ CreateApplication(std::move(arguments)) };
		
		application->TryInitialize();
		application->TryRun();
		application->TryShutdown();
	}
	catch (...)
	{
		HandleExceptions();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
#ifdef BIO_PLATFORM_WINDOWS
#ifdef BIO_DEBUG
#define USE_CRT_MEMORY_LEAK_DETECTION 1
#else
#define USE_CRT_MEMORY_LEAK_DETECTION 0
#endif

#if USE_CRT_MEMORY_LEAK_DETECTION
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <new>
#ifdef new
#undef new
#endif
extern "C++"
{
	void* operator new(const size_t size)
	{
		const auto memory{ malloc(size) };

		BIO_UNLIKELY
		if (!memory)
			throw std::bad_alloc{};

		return memory;
	}

	void* operator new[](const size_t size)
	{
		const auto memory{ malloc(size) };

		BIO_UNLIKELY
		if (!memory)
			throw std::bad_alloc{};

		return memory;
	}
}
/* Using memory checkpoint to prevent static object initialization and deinitialization */
#define MEMORY_CHECKPOINT(checkpoint)	\
_CrtMemState checkpoint{};				\
_CrtMemCheckpoint(&checkpoint);

#define MEMORY_SUMMARY(checkpoint)								\
_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	\
_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);				\
_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);				\
_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);				\
_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);				\
_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);				\
_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);			\
_CrtMemCheckpoint(&checkpoint);
#endif
static_assert(sizeof(WCHAR) == sizeof(std::wstring::value_type), "Invalid wide character size");
#include <shellapi.h>
INT APIENTRY wWinMain(
	[[maybe_unused]] _In_ HINSTANCE hInstance,
	[[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
	[[maybe_unused]] _In_ LPWSTR lpCmdLine,
	[[maybe_unused]] _In_ int nShowCmd)
{
	/* Using a checkpoint to ignore static initialization */
#if USE_CRT_MEMORY_LEAK_DETECTION	
	MEMORY_CHECKPOINT(memoryState);
#endif
	/* Application code */
	INT argumentCount{ 0U };
	LPWSTR* arguments{ CommandLineToArgvW(GetCommandLineW(), &argumentCount) };
#ifdef BIO_PLATFORM_WINDOWS
#ifndef _DEBUG
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	FreeConsole();
#endif
#endif
	
	const int result{ CommonEntryPoint(CommandLineArguments(argumentCount, arguments)) };
	/* Print memory leaks if any were detected */
#if USE_CRT_MEMORY_LEAK_DETECTION
	MEMORY_SUMMARY(memoryState);
#endif
	return result;
}
#elif defined BIO_PLATFORM_LINUX
int main(const int argc, const char** argv)
{
	//return CommonEntryPoint(argc, argv);
}
#endif
