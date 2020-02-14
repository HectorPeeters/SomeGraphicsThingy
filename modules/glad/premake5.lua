project "Glad"
    kind "SharedLib"
    language "C"
    staticruntime "on"
    
    targetdir ("../..bin/%{cfg.buildcfg}")
    objdir ("../../obj/%{cfg.buildcfg}")

    pic "On"

    files
    {
        "include/glad/glad.h",
        "include/KHR/khrplatform.h",
        "src/glad.c"
    }

    includedirs
    {
        "include"
    }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"