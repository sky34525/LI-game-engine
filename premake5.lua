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
IncludeDir["glm"] = "LI/vendor/glm"

startproject "Sandbox"

group "Dependencies"
	include "LI/vendor/GLFW"
	include "LI/vendor/Glad"
	include "LI/vendor/imgui"

group ""

project "LI"
	location "LI"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	pchheader "pch.h"
	pchsource "LI/src/pch.cpp"


	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.imGui}",
		"%{IncludeDir.glm}"
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
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "LI_RELEASE"
		runtime "Release"
		symbols "on"

	filter "configurations:Dist"
		defines "LI_DIST"
		runtime "Release"
		symbols "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

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
		"LI/src",
		"%{IncludeDir.glm}"
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
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "LI_RELEASE"
		runtime "Release"
		symbols "on"

	filter "configurations:Dist"
		defines "LI_DIST"
		runtime "Release"
		symbols "on"