#include "GUIRenderer.hpp"
#include "GUI.hpp"
#include "Window.hpp"
#include "Event.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "implot.h"

GUIRenderer::GUIRenderer(const std::unique_ptr<Window>& windowContext)
{
	IMGUI_CHECKVERSION();
	if (!ImGui::CreateContext())
		THROW_EXCEPTION("Failed to initialize ImGui context");
	
	ImGuiIO& io{ ImGui::GetIO() };
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	
	if (!io.Fonts->AddFontFromFileTTF("res/fonts/OpenSans-Regular.ttf", 17.0f))
		THROW_EXCEPTION("Failed to add font to ImGui");
	
	/* Upload icon font */
	{
		static const ImWchar iconRanges[]{ ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig config;
		config.OversampleH = config.OversampleV = 2;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced

		if (!io.Fonts->AddFontFromFileTTF("res/fonts/fa-solid-900.ttf", 13.0f, &config, iconRanges))
			THROW_EXCEPTION("Failed to add icon font to imgui");

		ImGuiContext* context{ ImGui::GetCurrentContext() };
		context->TempBuffer.resize(static_cast<int>(2_Mb));
	}
	
	if (!ImGui_ImplGlfw_InitForOpenGL(windowContext->GetHandle(), true))
		THROW_EXCEPTION("Failed to initialize glfw imgui backend");
	
	if (!ImGui_ImplOpenGL3_Init("#version 130"))
		THROW_EXCEPTION("Failed to initialize opengl3 imgui backend");
	
	if (!ImPlot::CreateContext())
		THROW_EXCEPTION("Failed to create implot context");
	
	ImGui::StyleColorsDark();
	ImGuiStyle& style{ ImGui::GetStyle() };
	auto& colors{ style.Colors };
	colors[ImGuiCol_Text]					= ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_TextDisabled]			= ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
	colors[ImGuiCol_WindowBg]				= ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	colors[ImGuiCol_ChildBg]				= ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
	colors[ImGuiCol_PopupBg]				= ImVec4(0.163f, 0.163f, 0.163f, 0.95f);
	colors[ImGuiCol_Border]					= ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
	colors[ImGuiCol_BorderShadow]			= ImVec4(0.0f, 0.0f, 0.0f, 0.0);
	colors[ImGuiCol_FrameBg]				= ImVec4(0.106f, 0.106f, 0.106f, 1.0f);
	colors[ImGuiCol_FrameBgHovered]			= ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	colors[ImGuiCol_FrameBgActive]			= ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
	colors[ImGuiCol_TitleBg]				= ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgActive]			= ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed]		= ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_MenuBarBg]				= ImVec4(0.121f, 0.121f, 0.121f, 1.000f);
	colors[ImGuiCol_ScrollbarBg]			= ImVec4(0.160f, 0.160f, 0.160f, 1.0f);
	colors[ImGuiCol_ScrollbarGrab]			= ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabHovered]	= ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabActive]	= ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_CheckMark]				= ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	colors[ImGuiCol_SliderGrab]				= ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SliderGrabActive]		= ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_Button]					= ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
	colors[ImGuiCol_ButtonHovered]			= ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_ButtonActive]			= ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
	colors[ImGuiCol_Header]					= ImVec4(0.218f, 0.218f, 0.218f, 1.000f);
	colors[ImGuiCol_HeaderHovered]			= ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_HeaderActive]			= ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_Separator]				= ImVec4(0.135f, 0.135f, 0.135f, 1.0f);
	colors[ImGuiCol_SeparatorHovered]		= ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SeparatorActive]		= ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_ResizeGrip]				= ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	colors[ImGuiCol_ResizeGripHovered]		= ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	colors[ImGuiCol_ResizeGripActive]		= ImVec4(1.000f, 1.0f, 1.000f, 1.000f);
	colors[ImGuiCol_Tab]					= ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabHovered]				= ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
	colors[ImGuiCol_TabActive]				= ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_TabUnfocused]			= ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	colors[ImGuiCol_TabUnfocusedActive]		= ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_PlotLines]				= ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_PlotLinesHovered]		= ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
	colors[ImGuiCol_PlotHistogram]			= ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	colors[ImGuiCol_PlotHistogramHovered]	= ImVec4(1.000f, 0.391f, 1.000f, 1.000f);
	colors[ImGuiCol_TextSelectedBg]			= ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_DragDropTarget]			= ImVec4{ 204.0f / 255.0f, 164.0f / 255.0f, 61.0f / 255.0f, 1.0f };
	colors[ImGuiCol_DockingPreview]			= ImVec4{ 204.0f / 255.0f, 164.0f / 255.0f, 61.0f / 255.0f, 1.0f };
	colors[ImGuiCol_DockingEmptyBg]			= ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
	colors[ImGuiCol_DockingPreview]			= ImVec4{ 204.0f / 255.0f, 164.0f / 255.0f, 61.0f / 255.0f, 1.0f };
	colors[ImGuiCol_NavHighlight]			= ImVec4{ 204.0f / 255.0f, 164.0f / 255.0f, 61.0f / 255.0f, 1.0f };
	colors[ImGuiCol_NavWindowingHighlight]	= ImVec4{ 204.0f / 255.0f, 164.0f / 255.0f, 61.0f / 255.0f, 1.0f };
	colors[ImGuiCol_NavWindowingDimBg]		= ImVec4{ 204.0f / 255.0f, 164.0f / 255.0f, 61.0f / 255.0f, 1.0f };
	colors[ImGuiCol_ModalWindowDimBg]		= ImVec4{ 204.0f / 255.0f, 164.0f / 255.0f, 61.0f / 255.0f, 0.03f };
	
	style.ChildRounding						= 4.0f;
	style.FrameBorderSize					= 0.0f;
	style.FrameRounding						= 2.0f;
	style.GrabMinSize						= 7.0f;
	style.PopupRounding						= 2.0f;
	style.ScrollbarRounding					= 0;
	style.ScrollbarSize						= 12.0f;
	style.TabBorderSize						= 0.0f;
	style.TabRounding						= 0.0f;
	style.WindowRounding					= 0.0f;
	style.IndentSpacing						= 11.0f;
	style.PopupBorderSize					= 1.0f;
	style.AntiAliasedLines					= true;
	style.AntiAliasedFill					= true;
	style.AntiAliasedLinesUseTex			= true;

	std::array<ImVec4, 28> extendedPlasmaColorMap
	{
		ImVec4{ 0.050383f,0.029803f,0.527975f,1.0f},
		ImVec4{ 0.134238f,0.0221291f,0.564088f,1.0f},
		ImVec4{ 0.196567f,0.0181499f,0.5917f,1.0f},
		ImVec4{ 0.2525f,0.0140617f,0.614596f,1.0f},
		ImVec4{ 0.305595f,0.00896577f,0.633494f,1.0f},
		ImVec4{ 0.357158f,0.00372639f,0.647995f,1.0f},
		ImVec4{ 0.407659f,0.000642161f,0.657244f,1.0f},
		ImVec4{ 0.45713f,0.00323055f,0.660291f,1.0f},
		ImVec4{ 0.505336f,0.0158921f,0.656373f,1.0f},
		ImVec4{ 0.551893f,0.0432713f,0.645218f,1.0f},
		ImVec4{ 0.596367f,0.0783064f,0.627252f,1.0f},
		ImVec4{ 0.638404f,0.114221f,0.603686f,1.0f},
		ImVec4{ 0.677813f,0.150562f,0.576205f,1.0f},
		ImVec4{ 0.714603f,0.187013f,0.546575f,1.0f},
		ImVec4{ 0.748938f,0.223424f,0.51624f,1.0f},
		ImVec4{ 0.781067f,0.25979f,0.486137f,1.0f},
		ImVec4{ 0.811234f,0.29621f,0.456705f,1.0f},
		ImVec4{ 0.839614f,0.332858f,0.428011f,1.0f},
		ImVec4{ 0.86628f,0.369954f,0.399907f,1.0f},
		ImVec4{ 0.891185f,0.407748f,0.372154f,1.0f},
		ImVec4{ 0.914179f,0.446495f,0.344516f,1.0f},
		ImVec4{ 0.935011f,0.486439f,0.316822f,1.0f},
		ImVec4{ 0.953361f,0.527795f,0.288992f,1.0f},
		ImVec4{ 0.968851f,0.570731f,0.261064f,1.0f},
		ImVec4{ 0.981089f,0.615359f,0.233237f,1.0f},
		ImVec4{ 0.989649f,0.661749f,0.206009f,1.0f},
		ImVec4{ 0.994066f,0.709929f,0.180477f,1.0f},
		ImVec4{ 0.993815f,0.759904f,0.158879f,1.0f}
	};

	ImPlot::AddColormap(
		GUI::g_ExtendedPlasmaColorMapID, 
		reinterpret_cast<ImVec4*>(extendedPlasmaColorMap.data()),
		static_cast<int>(extendedPlasmaColorMap.size()));
}

GUIRenderer::~GUIRenderer() noexcept
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();
}

void GUIRenderer::BeginFrame()
{
	/* Begin internal frame */
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	/* Setup dockspace */
	constexpr ImGuiWindowFlags dockSpaceWindowFlags
	{
		ImGuiWindowFlags_MenuBar				|
		ImGuiWindowFlags_NoDocking				|
		ImGuiWindowFlags_NoScrollbar			|
		ImGuiWindowFlags_NoTitleBar				|
		ImGuiWindowFlags_NoCollapse				|
		ImGuiWindowFlags_NoResize				|
		ImGuiWindowFlags_NoMove					|
		ImGuiWindowFlags_NoNavFocus				|
		ImGuiWindowFlags_NoBringToFrontOnFocus	|
		ImGuiWindowFlags_NoBackground
	};
	
	/* Set dockspace size to fullscreen */
	ImGuiViewport* const mainViewport{ ImGui::GetMainViewport() };
	ImGui::SetNextWindowPos(mainViewport->Pos);
	ImGui::SetNextWindowSize(mainViewport->Size);
	ImGui::SetNextWindowViewport(mainViewport->ID);
	
	constexpr std::string_view dockspaceID{ "Dockspace" };
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin(dockspaceID.data(), nullptr, dockSpaceWindowFlags);
	ImGui::PopStyleVar(3);
	
	constexpr ImGuiDockNodeFlags dockspaceFlags
	{
		ImGuiDockNodeFlags_NoCloseButton		|
		ImGuiDockNodeFlags_NoWindowMenuButton	|
		ImGuiDockNodeFlags_NoWindowMenuButton 
	};
	
	const ImGuiID ID{ ImGui::GetID(dockspaceID.data()) };
	if (!ImGui::DockBuilderGetNode(ID))
	{
		ImGui::DockBuilderRemoveNode(ID);	// Clear out existing layout
		ImGui::DockBuilderAddNode(ID);		// Add empty node
		ImGui::DockBuilderSetNodeSize(ID, { ImGui::GetIO().DisplaySize.x * ImGui::GetIO().DisplayFramebufferScale.x, ImGui::GetIO().DisplaySize.y * ImGui::GetIO().DisplayFramebufferScale.y });
		ImGui::DockBuilderFinish(ID);
	}
	
	const ImGuiID dockID{ ImGui::GetID(dockspaceID.data()) };
	ImGui::DockSpace(dockID, { 0.0f, 0.0f }, dockspaceFlags);
}

void GUIRenderer::EndFrame()
{
	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
}

void GUIRenderer::OnEvent(const Event& event)
{
	switch (event.GetEventType())
	{
		case EEventType::MouseScrolled:
		{
			const MouseScrolledEvent& mouseScrolledEvent{ EventCast<MouseScrolledEvent>(event) };
			const auto [scrollDeltaX, scrollDeltaY] { mouseScrolledEvent.GetScrollDelta() };
			ImGuiIO& io{ ImGui::GetIO() };

			io.AddMouseWheelEvent
			(
				static_cast<float>(scrollDeltaX),
				static_cast<float>(scrollDeltaY)
			);
		} break;

		case EEventType::MouseCursorMoved:
		{
			const MouseCursorMovedEvent& mouseScrolledEvent{ EventCast<MouseCursorMovedEvent>(event) };
			const auto [xPosition, yPosition] { mouseScrolledEvent.GetPosition() };
			ImGuiIO& io{ ImGui::GetIO() };

			io.AddMousePosEvent
			(
				xPosition,
				yPosition
			);
		} break;

		default: break;
	}
}