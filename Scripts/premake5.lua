-- Variables -----------------------------------------------------------------------
SOLUTION_NAME           = "DiveGames"
ENGINE_NAME             = "Engine"
EDITOR_NAME             = "Editor"
TESTS_NAME              = "Tests"
ENGINE_DIR              = "../" .. ENGINE_NAME
EDITOR_DIR              = "../" .. EDITOR_NAME
TESTS_DIR               = "../" .. TESTS_NAME
ENGINE_SRC_DIR          = ENGINE_DIR .. "/Src"
EDITOR_SRC_DIR          = EDITOR_DIR .. "/Src"
TESTS_SRC_DIR           = TESTS_DIR .. "/Src"
ENGINE_EXTERNAL_DIR     = ENGINE_DIR .. "/External"
EDITOR_EXTERNAL_DIR     = EDITOR_DIR .. "/External"
TESTS_EXTERNAL_DIR      = TESTS_DIR .. "/External" 
TARGET_DIR_DEBUG        = "../bin/debug"
TARGET_DIR_RELEASE      = "../bin/release"
INTERMEDIATE_DIR        = "../bin/int"
DEBUG_FORMAT            = "c7"

-- Solution ------------------------------------------------------------------------
solution (SOLUTION_NAME)
    location ".."
	systemversion "latest"
	cppdialect "C++17"
	language "C++"
	platforms "x64"
    configurations { "Release", "Debug" }
    
    -- Fliter
	filter { "platforms:x64" }
		system "Windows"
		architecture "x64"
		
	-- Debug
	filter "configurations:Debug"
		defines { "DEBUG" }
		flags { "MultiProcessorCompile", "LinkTimeOptimization" }
		symbols "On"			
		
	-- Release	
	filter "configurations:Release"
		defines { "NDEBUG" }
		flags { "MultiProcessorCompile" }
		symbols "Off"	
		optimize "Full"


-- Engine --------------------------------------------------------------------------
project (ENGINE_NAME)
    location (ENGINE_DIR)
    objdir (INTERMEDIATE_DIR)
    kind "StaticLib"
    staticruntime "On"

    -- PCH
    pchheader "DivePch.h"
    pchsource (ENGINE_SRC_DIR .. "/Core/DivePch.cpp")

    -- Files
    files 
    { 
        ENGINE_SRC_DIR .. "/**.h",
        ENGINE_SRC_DIR .. "/**.cpp"
    }

    -- Includes
    includedirs 
    { 
        (ENGINE_EXTERNAL_DIR)
    }
    
    -- Libraries
    libdirs (ENGINE_EXTERNAL_DIR .. "/lib")

    -- Debug"
    filter "configurations:Debug"
        targetdir (TARGET_DIR_DEBUG)
        debugdir (TARGET_DIR_DEBUG)
        debugformat (DEBUG_FORMAT)
        links("spdlogd")
            
    -- Release
    filter "configurations:Release"
        targetdir (TARGET_DIR_RELEASE)
        debugdir (TARGET_DIR_RELEASE)

-- Editor --------------------------------------------------------------------------
project (EDITOR_NAME)
    location (EDITOR_DIR)
    links {ENGINE_NAME}
    dependson {ENGINE_NAME}
    objdir (INTERMEDIATE_DIR)
    kind "WindowedApp"
    staticruntime "On"

    -- Files
    files 
    { 
        EDITOR_SRC_DIR .. "/**.h",
        EDITOR_SRC_DIR .. "/**.cpp"
    }

    -- Includes
    includedirs 
    { 
        ENGINE_SRC_DIR,
        ENGINE_EXTERNAL_DIR
    }
    
    -- Libraries
    -- libdirs

    -- Debug"
    filter "configurations:Debug"
        targetdir (TARGET_DIR_DEBUG)
        debugdir (TARGET_DIR_DEBUG)
        debugformat (DEBUG_FORMAT)
            
    -- Release
    filter "configurations:Release"
        targetdir (TARGET_DIR_RELEASE)
        debugdir (TARGET_DIR_RELEASE)

-- Tests ---------------------------------------------------------------------------
project (TESTS_NAME)
    location (TESTS_DIR)
    links {ENGINE_NAME}
    dependson {ENGINE_NAME}
    objdir (INTERMEDIATE_DIR)
    kind "ConsoleApp"
    staticruntime "On"
    
    -- Files
    files 
    { 
        TESTS_SRC_DIR .. "/**.h",
        TESTS_SRC_DIR .. "/**.cpp"
    }

    -- Includes
    includedirs 
    { 
        ENGINE_SRC_DIR,
        ENGINE_EXTERNAL_DIR,
        TESTS_EXTERNAL_DIR
    }
    
    -- Libraries
    libdirs { TESTS_EXTERNAL_DIR .. "/lib" }
   
    -- Debug"
    filter "configurations:Debug"
        targetdir (TARGET_DIR_DEBUG)
        debugdir (TARGET_DIR_DEBUG)
        debugformat (DEBUG_FORMAT)
        links { "gtestd" }
            
    -- Release
    filter "configurations:Release"
        targetdir (TARGET_DIR_RELEASE)
        debugdir (TARGET_DIR_RELEASE)
