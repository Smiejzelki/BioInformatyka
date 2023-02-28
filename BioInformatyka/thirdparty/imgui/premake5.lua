project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	targetdir ("binaries/bin/" .. OutputDirectory .. "/%{prj.name}")
	objdir ("binaries/bin-int/" .. OutputDirectory .. "/%{prj.name}")

	files
	{
		"imgui.h",
		"imgui.cpp",
		"imgui_demo.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_tables.cpp",
		"imgui_tables.cpp",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
        "backends/imgui_impl_opengl3_loader.h", 
        "backends/imgui_impl_opengl3.h", 
        "backends/imgui_impl_opengl3.cpp", 
	}

    includedirs
    {
        "%{wks.location}/BioInformatyka/thirdparty/imgui/",
        "%{wks.location}/BioInformatyka/thirdparty/imgui/backends/",

        "%{wks.location}/BioInformatyka/thirdparty/GLFW/include/",
    }

    filter "system:windows"
		systemversion "latest"

        files
        {
		    "backends/imgui_impl_glfw.h", 
		    "backends/imgui_impl_glfw.cpp", 
        }
