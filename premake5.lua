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

        pchheader "Cashewpch.h"
        pchsource "CashewEngine/src/Cashewpch.cpp"

        files
        {
            "%{prj.name}/src/*.h",
            "%{prj.name}/src/**.cpp",
        }

        includedirs
        {
            "%{prj.name}/src",
            "%{prj.name}/TPD/spdlog/include"
        }

        links
        {
            "d3d12", "dxgi", "d3dcompiler", "dxguid"
        }

        filter "system:windows"
            cppdialect "C++20"
            staticruntime "On"
            systemversion "latest"

            defines
            {
                "CASHEW_PLATFORM_WINDOWS",
                "CASHEW_BUILD_DLL"
            }
            
            prebuildcommands
            {
                 "call \"%{prj.location}/../compile_shaders.bat\""
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
            cppdialect "C++20"
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
