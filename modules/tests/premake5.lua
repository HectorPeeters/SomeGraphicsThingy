project "Tests"
    kind "ConsoleApp"
    language "C++"
    targetdir "../../bin/%{cfg.buildcfg}"
    objdir "../../obj/%{cfg.buildcfg}"

    files { "include/**.h", "src/**.cpp" }

    pic "On"

    includedirs { "include/", "../game/include" }
    
    links { "Game", "GL", "glfw", "Glad", "ImGui" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"