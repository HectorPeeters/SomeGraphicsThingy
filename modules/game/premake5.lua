project "Game"
    kind "SharedLib"
    language "C++"
    targetdir "../../bin/%{cfg.buildcfg}"
    objdir "../../obj/%{cfg.buildcfg}"

    files { "src/**.h", "src/**.cpp" }

    pic "On"

    includedirs { "../imgui", "../glad/include", "usr/include/glm", "usr/include/zlib.h" }

    links { "z" }
    
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"