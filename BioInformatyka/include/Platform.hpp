#pragma once
#include "Core.hpp"

namespace Platform {
	std::wstring StringToWideString(const std::string& input);
	std::string WideStringToString(const std::wstring& input);

	std::optional<std::filesystem::path> OpenFile(const std::string_view filter);
	std::optional<std::filesystem::path> SaveFile(const std::string_view filter);
	bool PushConfirmationWindow(const std::string_view title, const std::string_view windowText);
	void CopyTextToClipboard(const std::string_view text);

	void Execute(const std::wstring_view operation, const std::wstring_view arguments);
	void ClearConsoleBuffer();
}
