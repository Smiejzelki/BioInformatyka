project "ImPlot"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"

	targetdir ("binaries/bin/" .. OutputDirectory .. "/%{prj.name}")
	objdir ("binaries/bin-int/" .. OutputDirectory .. "/%{prj.name}")

	files
	{
		"implot.h",
		"implot.cpp",
		"implot_internal.h",
		"implot_demo.cpp",
		"implot_items.cpp",
	}

	includedirs
    {
        "%{wks.location}/BioInformatyka/thirdparty/imgui/",
	}