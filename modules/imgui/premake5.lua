project "ImGui"
    kind "SharedLib"
    language "C++"
    
	targetdir ("../..bin/%{cfg.buildcfg}")
    objdir ("../../obj/%{cfg.buildcfg}")

    includedirs { "../glad/include" }

    pic "On"

	files
	{
        "imconfig.h",
        "imgui.h",
        "imgui.cpp",
        "imgui_draw.cpp",
        "imgui_internal.h",
        "imgui_widgets.cpp",
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h",
        "imgui_demo.cpp",
        "imgui_impl_glfw.h",
        "imgui_impl_glfw.cpp",
        "imgui_impl_opengl3.h",
        "imgui_impl_opengl3.cpp",
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"