#include "GUI.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "implot.h"

namespace GUI
{
	template<typename Enum>
	constexpr bool operator &(const Enum lhs, const Enum rhs) noexcept
	{
		return static_cast<int>(lhs) & static_cast<int>(rhs);
	}

	float CalculateTextWidth(const std::string_view text)
	{
		return ImGui::CalcTextSize(text.data()).x;
	}

	bool TextInput(const char* label, std::string& buffer)
	{
		buffer.resize(512);
		return ImGui::InputText(label, buffer.data(), buffer.size()) && ImGui::IsItemDeactivatedAfterEdit();
	}

	void Text(const std::string_view text)
	{
		ImGui::Text(text.data());
	}

	void TextCentered(const std::string_view text)
	{
		const float windowWidth = ImGui::GetWindowSize().x;
		const float textWidth = ImGui::CalcTextSize(text.data()).x;

		constexpr float g_MinimalIndent = 20.0f;
		float textIndentation = (windowWidth - textWidth) * 0.5f;

		if (textIndentation <= g_MinimalIndent)
			textIndentation = g_MinimalIndent;

		ImGui::SameLine(textIndentation);
		ImGui::PushTextWrapPos(windowWidth - textIndentation);
		ImGui::TextWrapped(text.data());
		ImGui::PopTextWrapPos();
	}

	void TextWrapped(const std::string_view text)
	{
		ImGui::TextWrapped(text.data());
	}

	void HorizontalSeparator()
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
	}

	void VerticalSeparator()
	{
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	}

	bool ButtonCentered(const std::string_view label)
	{
		const ImGuiStyle& style{ ImGui::GetStyle() };

		const float size{ ImGui::CalcTextSize(label.data()).x + style.FramePadding.x * 2.0f };
		const float available{ ImGui::GetContentRegionAvail().x };
		const float offset{ (available - size) * 0.5f };

		if (offset > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

		return ImGui::Button(label.data());
	}

	void SearchBar(std::string& outFilter, const bool spanAvailableWidth, const std::string_view hint)
	{
		std::array<char, 512U> inputBuffer{ 0U };
		const std::size_t size = outFilter.size() > 512U - 1 ? 512U - 1U: outFilter.size();
		std::copy_n(outFilter.begin(), size, inputBuffer.data());

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);

		if (spanAvailableWidth)
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		
		if (ImGui::InputTextWithHint("Search bar", hint.data(), inputBuffer.data(), inputBuffer.size() - 1U, ImGuiInputTextFlags_AllowTabInput))
			outFilter = std::move(std::string(inputBuffer.data()));

		ImGui::PopStyleVar();
		if (spanAvailableWidth)
			ImGui::PopItemWidth();

	}
}