workspace "Cashew"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    project "CashewEngine"
        location "CashewEngine"
        kind "SharedLib"
        language "C++"

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files
        {
            "%{prj.name}/src/*.h",
            "%{prj.name}/src/**.cpp"
        }

        includedirs
        {
            "%{prj.name}/TPD/spdlog/include"
        }

        filter "system:windows"
            cppdialect "C++17"
            staticruntime "On"
            systemversion "latest"

            defines
            {
                "CASHEW_PLATFORM_WINDOWS",
                "CASHEW_BUILD_DLL"
            }

            postbuildcommands
            {
                ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Application")
            }
        
        filter "configurations:Debug"
            defines "CASHEW_DEBUG"
            symbols "On"
        
        filter "configurations:Release"
            optimize "On"

    
    project "Application"
        location "Application"
        kind "WindowedApp"
        language "C++"

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files
        {
            "%{prj.name}/src/*.h",
            "%{prj.name}/src/**.cpp"
        }

        includedirs
        {
            "CashewEngine/TPD/spdlog/include",
            "CashewEngine/src"
        }

        links
        {
            "CashewEngine"
        }

        filter "system:windows"
            cppdialect "C++17"
            staticruntime "On"
            systemversion "latest"

            defines 
            {
                "CASHEW_PLATFORM_WINDOWS"
            }

        filter "configurations:Debug"
            defines "CASHEW_DEBUG"
            symbols "On"
        
        filter "configurations:Release"
            optimize "On"
