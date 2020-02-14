workspace "Squirrel"
    configurations { "Debug", "Release" }

    flags { "FatalWarnings", "MultiProcessorCompile", "ShadowedVariables" }


    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        optimize "Speed"
        flags { "LinkTimeOptimization" }

include "modules/game"
include "modules/imgui"
include "modules/glad"

project "Squirrel"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
	architecture "x86_64"

    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"

    files { "src/**.h", "src/**.cpp" }
    
    includedirs { "/usr/include/GLFW", "/usr/include/GL", "modules/imgui", "modules/glad/include" }

    links { "dl", "GL", "glfw", "Glad", "ImGui" }
