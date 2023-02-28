project "glad"
	kind "StaticLib"
	language "C"

	targetdir ("binaries/bin/" .. OutputDirectory .. "/%{prj.name}")
	objdir ("binaries/bin-int/" .. OutputDirectory .. "/%{prj.name}")

	files
	{
		"src/*.c",
		"include/*.h",
	}

    includedirs
    {
		"include/"
    }