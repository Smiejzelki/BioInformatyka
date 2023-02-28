#ifdef BIO_PLATFORM_WINDOWS
#include "Platform.hpp"
#include <commdlg.h>
#include <shellapi.h>
#include <Psapi.h>

#ifdef max
#undef max
#endif

namespace Platform {
	std::wstring StringToWideString(const std::string& input)
	{
		BIO_UNLIKELY
		if (input.empty())
			return std::wstring{};
	
		BIO_UNLIKELY
		if (input.size() + 1 >= std::numeric_limits<int>::max())
			THROW_EXCEPTION("String is too big to convert");
	
		std::wstring result{};
		result.resize(input.size());
	
		BIO_UNLIKELY
		if(!MultiByteToWideChar(
			CP_ACP,
			MB_PRECOMPOSED,
			input.c_str(),
			static_cast<int>(input.size()),
			&result[0],
			static_cast<int>(result.size())))
		{
			THROW_EXCEPTION("Failed converting string to wide string");
		}
	
		return result;
	}
	
	std::string WideStringToString(const std::wstring& input)
	{
		BIO_UNLIKELY
		if (input.empty())
			return std::string{};
	
		BIO_UNLIKELY
		if (input.size() + 1 >= std::numeric_limits<int>::max())
			THROW_EXCEPTION("String is too big to convert");
	
		std::string result;
		result.resize(input.size());
	
		BIO_UNLIKELY
		if (!WideCharToMultiByte(
			CP_UTF8,
			0,
			input.c_str(),
			static_cast<int>(input.size()),
			&result[0],
			static_cast<int>(input.size()),
			nullptr,
			nullptr))
		{
			THROW_EXCEPTION("Failed converting wide string to string");
		}
	
		return result;
	}

	std::optional<std::filesystem::path> OpenFile(const std::string_view filter)
	{
		constexpr std::size_t maxFileNameSize{ MAX_PATH };
		CHAR szFile[maxFileNameSize]{ '\0' };

		OPENFILENAMEA openFileName;
		ZeroMemory(&openFileName, sizeof(OPENFILENAMEA));

		openFileName.lStructSize = sizeof(OPENFILENAMEA);
		openFileName.hwndOwner = nullptr;
		openFileName.lpstrFile = szFile;
		openFileName.nMaxFile = sizeof(szFile);
		openFileName.lpstrFilter = filter.data();
		openFileName.nFilterIndex = 1;
		openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&openFileName) == TRUE)
			return openFileName.lpstrFile;

		return std::nullopt;
	}

	std::optional<std::filesystem::path> SaveFile(const std::string_view filter)
	{
		OPENFILENAMEA openFileName;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&openFileName, sizeof(OPENFILENAME));
		openFileName.lStructSize = sizeof(OPENFILENAME);
		openFileName.hwndOwner = nullptr; 
		openFileName.lpstrFile = szFile;
		openFileName.nMaxFile = sizeof(szFile);
		openFileName.lpstrFilter = filter.data();
		openFileName.nFilterIndex = 1;
		openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		// Sets the default extension by extracting it from the filter
		openFileName.lpstrDefExt = strchr(filter.data(), '\0') + 1;

		if (GetSaveFileNameA(&openFileName) == TRUE)
			return openFileName.lpstrFile;

		return std::nullopt;
	}

	bool PushConfirmationWindow(const std::string_view title, const std::string_view windowText)
	{
		return MessageBoxA
		(
			NULL,
			reinterpret_cast<LPCSTR>(windowText.data()),
			reinterpret_cast<LPCSTR>(title.data()),
			MB_YESNO | MB_ICONWARNING
		) == IDYES;
	}

	void CopyTextToClipboard(const std::string_view text)
	{
		try
		{
			BIO_UNLIKELY
			if (text.size() > static_cast<size_t>(std::numeric_limits<DWORD>::max()))
				THROW_EXCEPTION("Buffer too large to copy to clipboard");

			BIO_LIKELY
			if (OpenClipboard(NULL))
			{
				EmptyClipboard();
				const DWORD textSize{ static_cast<DWORD>(text.size()) };
				const HGLOBAL clipboardDataHandle{ GlobalAlloc(GMEM_DDESHARE | GMEM_ZEROINIT, textSize + 1U) };
				char* pchData{ reinterpret_cast<char*>(GlobalLock(clipboardDataHandle)) };

				BIO_LIKELY
				if (pchData)
				{
					memcpy(pchData, text.data(), textSize);
					pchData[textSize] = '\0';
				}

				GlobalUnlock(clipboardDataHandle);
				SetClipboardData(CF_TEXT, clipboardDataHandle);
				CloseClipboard();
			}
			else
				THROW_EXCEPTION("Failed to open clipboard");
		}
		catch (...)
		{
			HandleExceptions();
		}
	}

	void Execute(const std::wstring_view operation, const std::wstring_view arguments)
	{
		constexpr size_t BufferSize{ 512U };
		WCHAR currentDirectoryBuffer[BufferSize]{ L'\0' };

		if(GetCurrentDirectory(static_cast<DWORD>(BufferSize - 1), reinterpret_cast<LPWSTR>(currentDirectoryBuffer)))
			ShellExecute(
				nullptr,
				operation.data(),
				arguments.data(),
				currentDirectoryBuffer,
				nullptr,
				SW_HIDE);
	}

	void ClearConsoleBuffer()
	{
		system("cls");
	}
}

#endif