workspace "LI"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "LI/vendor/GLFW/include"
IncludeDir["Glad"] = "LI/vendor/Glad/include"
IncludeDir["imGui"] = "LI/vendor/imgui"
IncludeDir["imGuiBackends"] = "LI/vendor/imgui/backends"


include "LI/vendor/GLFW"
include "LI/vendor/Glad"
include "LI/vendor/imgui"

project "LI"
	location "LI"
	kind "SharedLib"
	language "C++"

	pchheader "pch.h"
	pchsource "LI/src/pch.cpp"


	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.imGui}",
		"%{IncludeDir.imGuiBackends}"
	}

	links
	{
		"GLFW",
		"Glad",
		"opengl32.lib",
		"ImGui"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"LI_PLATFORM_WINDOWS",
			"LI_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

        buildoptions
        {
            "/utf-8"  -- Add UTF-8 compilation option.(because of spdlog)
        }

		postbuildcommands
		{
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox") -- COPYFILE is suitable for copying a single file.
		}

	filter "configurations:Debug"
		defines "LI_DEBUG"
		buildoptions "/MDd"
		symbols "on"

	filter "configurations:Release"
		defines "LI_RELEASE"
		buildoptions "/MD"
		symbols "on"

	filter "configurations:Dist"
		defines "LI_DIST"
		buildoptions "/MD"
		symbols "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"LI/vendor/spdlog/include",
		"LI/src"
	}

	links
	{
		"LI"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"LI_PLATFORM_WINDOWS"
		}

        buildoptions
        {
            "/utf-8"  -- Add UTF-8 compilation option.(because of spdlog)
        }

	filter "configurations:Debug"
		defines "LI_DEBUG"
		buildoptions "/MDd"
		symbols "on"

	filter "configurations:Release"
		defines "LI_RELEASE"
		buildoptions "/MD"
		symbols "on"

	filter "configurations:Dist"
		defines "LI_DIST"
		buildoptions "/MD"
		symbols "on"