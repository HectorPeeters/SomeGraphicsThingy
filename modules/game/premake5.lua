project "Game"
    kind "SharedLib"
    language "C++"
    targetdir "../../bin/%{cfg.buildcfg}"
    objdir "../../obj/%{cfg.buildcfg}"

    pchheader "squirrel_pch.h"

    files { "include/**.h", "src/**.cpp" }

    pic "On"

    includedirs { "include/", "../imgui", "../glad/include", "usr/include/glm" }

    -- links { "z" }
    
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"