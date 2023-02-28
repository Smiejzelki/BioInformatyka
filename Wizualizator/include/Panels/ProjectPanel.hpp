#pragma once
#include "PanelBase.hpp"

class Canvas;
class Camera;

class ProjectPanel final : public PanelBase
{
private:
	using AssetCallbackFunction = std::function<void(const std::filesystem::path&)>;
public:
	explicit ProjectPanel() noexcept;
	virtual ~ProjectPanel() noexcept = default;

	virtual void OnGUIRender() override final;
private:
	void DrawDirectoryFromRoot(const std::filesystem::path& rootPath, const std::string_view filter, const AssetCallbackFunction& function);
	void DrawDirectoriesRecursively(const std::filesystem::path& directoryPath, const std::string_view filter, const AssetCallbackFunction& function);
};