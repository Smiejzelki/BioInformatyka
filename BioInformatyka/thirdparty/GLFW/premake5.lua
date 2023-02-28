project "GLFW"
	kind "StaticLib"
	language "C"
	warnings "off"

	targetdir ("binaries/bin/" .. OutputDirectory .. "/%{prj.name}")
	objdir ("binaries/bin-int/" .. OutputDirectory .. "/%{prj.name}")

	files
	{
		"include/GLFW/glfw3.h",
		"include/GLFW/glfw3native.h",
		"src/context.c",
		"src/init.c",
		"src/input.c",
		"src/monitor.c",

		"src/null_init.c",
		"src/null_joystick.c",
		"src/null_monitor.c",
		"src/null_window.c",
		"src/vulkan.c",
		"src/platform.c",
		"src/window.c",
	}

	filter "system:windows"
		systemversion "latest"

		files
		{
			"src/win32_init.c",
			"src/win32_joystick.c",
			"src/win32_module.c",
			"src/win32_monitor.c",
			"src/win32_time.c",
			"src/win32_thread.c",
			"src/win32_window.c",
			"src/wgl_context.c",
			"src/egl_context.c",
			"src/osmesa_context.c",
		}

		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

		links
		{
			"Dwmapi.lib",
		}