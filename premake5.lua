workspace "Notris"
   configurations { "Debug", "Release" }
   architecture "x86_64"

   flags
	{
		"MultiProcessorCompile"
	}

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"


    


project "Notris"
    kind "WindowedApp"
    language "c++"
    cppdialect "C++latest"

    files 
    {
      "src/**.cpp",
      "src/**.hpp",
    }

    includedirs 
    {
      "src",
      "vendor/SDL2/include",
      "vendor/SDL2_ttf/include",
      "vendor/SDL2_mixer/include",
      "vendor/glm/glm"
    }

    libdirs {
      "vendor/SDL2/lib/x64",
      "vendor/SDL2_ttf/lib/x64",
      "vendor/SDL2_mixer/lib/x64"

    }

    links {
      "SDL2",
      "SDL2_ttf",
      "SDL2_mixer"
    }