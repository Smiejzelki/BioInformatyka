-- Workspace 
OutputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

workspace("BioInformatyka")
	architecture "x64"
	platforms "x64"
	startproject "Wizualizator"
	targetdir (OutputDirectory)

	configurations 
	{
		"Debug",
		"Release",
		"Distribution",
	}

	flags
	{
		"MultiProcessorCompile",	-- Kompilacja wielordzeniowa
		"FatalCompileWarnings",		-- Traktuj ostrzezenia kompilatora jako bledy 
		"FatalLinkWarnings",		-- Traktuj ostrzezenia bibliotekarza jako bledy
	}

	exceptionhandling "On"			-- Zezwol na wyjatki w kodzie (try / catch)
	
	filter "configurations:Debug"
		defines 
		{
			"BIO_DEBUG"
		}

		optimize "Off"
		symbols "On"
		runtime "Debug"
		staticruntime "On"

	filter "configurations:Release"
		defines 
		{
			"NDEBUG",
			"BIO_RELEASE",
		}

		optimize "On"
		symbols "Off"
		runtime "Release"
		staticruntime "On"

	filter "configurations:Distribution"
		defines 
		{
			"NDEBUG",
		}

		optimize "Speed"				-- Faworyzuj predkosc
		optimize "Full"					-- Pelna optymalizacja 
		symbols "off"
		runtime "Release"
		staticruntime "On"
		flags "LinkTimeOptimization"	-- Zezwol na optymalizacje calego programu w czasie linkera

	filter "system:windows"
		defines "BIO_PLATFORM_WINDOWS"
		systemversion "latest"
		entrypoint "wWinMainCRTStartup"

	filter "system:linux"
		defines "BIO_PLATFORM_LINUX"

group "Thirdparty"
include "BioInformatyka/thirdparty/GLFW"
include "BioInformatyka/thirdparty/glad"
include "BioInformatyka/thirdparty/imgui"
include "BioInformatyka/thirdparty/implot"
group ""

project("BioInformatyka")
	location "BioInformatyka"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"
	warnings "Extra"				-- Ustawia poziom ostrzezen na "extra"

	local ProjectOutputDirectory = "binaries/bin/" .. (OutputDirectory) .. "/%{prj.name}";
	local ProjectIntermediateOutputDirectory = "binaries/bin-int/" .. (OutputDirectory) .. "/%{prj.name}";

	targetdir (ProjectOutputDirectory)
	objdir (ProjectIntermediateOutputDirectory)

	files 
	{ 
		"%{prj.name}/include/*.hpp", 

		"%{prj.name}/src/*.cpp",
		-------------------- 
		"%{prj.name}/platform/Windows/*.cpp",
		--------------------
		"%{prj.name}/platform/Linux/*.cpp",
	}

	includedirs
	{
		"%{prj.name}/include/",
		"%{prj.name}/thirdparty/GLFW/include/",
		"%{prj.name}/thirdparty/glad/include/", 
		"%{prj.name}/thirdparty/imgui",
		"%{prj.name}/thirdparty/implot",
		"%{prj.name}/thirdparty/freetype-2.12.1/include",
	}

	links
	{
		"GLFW",
		"glad",
		"imgui",
		"implot",
		"freetype.lib",
	}

project("Wizualizator")
	location "Wizualizator"
	language "C++"
	cppdialect "C++20"
	kind "ConsoleApp"
	warnings "Extra"				-- Ustawia poziom ostrzezen na "extra"

	local ProjectOutputDirectory = "binaries/bin/" .. (OutputDirectory) .. "/%{prj.name}";
	local ProjectIntermediateOutputDirectory = "binaries/bin-int/" .. (OutputDirectory) .. "/%{prj.name}";

	targetdir (ProjectOutputDirectory)
	objdir (ProjectIntermediateOutputDirectory)

	files 
	{ 
		"%{prj.name}/include/*.hpp",
		"%{prj.name}/include/Panels/*.hpp",

		"%{prj.name}/src/*.cpp",
		"%{prj.name}/src/Panels/*.cpp",
	}

	includedirs
	{
		"%{prj.name}/include",
		"%{prj.name}/include/framework",
		"%{wks.location}/BioInformatyka/include",

		"%{wks.location}/BioInformatyka/thirdparty/imgui",
		"%{wks.location}/BioInformatyka/thirdparty/implot",
	}

	links
	{
		"BioInformatyka",

		"imgui",
		"implot",
	}

project("Testy")
	location "Testy"
	language "C++"
	cppdialect "C++20"
	kind "ConsoleApp"
	warnings "Extra"				-- Ustawia poziom ostrzezen na "extra"

	local ProjectOutputDirectory = "binaries/bin/" .. (OutputDirectory) .. "/%{prj.name}";
	local ProjectIntermediateOutputDirectory = "binaries/bin-int/" .. (OutputDirectory) .. "/%{prj.name}";

	targetdir (ProjectOutputDirectory)
	objdir (ProjectIntermediateOutputDirectory)

	files 
	{ 
		"%{prj.name}/include/*.hpp",
		"%{prj.name}/src/*.cpp",
	}

	includedirs
	{
		"%{wks.location}/Wizualizator/include/framework",
	}