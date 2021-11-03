--[[DOCS
  1. USE '_G.PREMAKE_INCLUDE_PATH'    (_G is the thing that contains global vars),     we do   include(_G.PREMAKE_INCLUDE_PATH .. "ModuleName.lua")
]]

include(_G.PREMAKE_INCLUDE_PATH .. "/amMACROS.lua")
-- DETECT if THIS FILE is _MAIN_SCRIPT
_fxedPath = curFilePath()
print(_fxedPath)


-- WE simply use    %{wks.location}   for project Locations
if (_fxedPath == _MAIN_SCRIPT) then
  workspace "amVK"
    configurations { "Debug", "Release" }
    platforms { "Win64",   "Linux64" }
    system  { "windows",   "linux" }
    architecture "x86_64"               -- ARM & Silicon-M1 for amVK????
    location("PremakeBuild")

    filter "configurations:Debug"
      defines { "DEBUG" }

    filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

    filter "system:windows"
      cppdialect "C++17"
      staticruntime "On"
      systemversion "latest"
end

    

    

-- Currently in Refactoring Stage....
project "amVK-0.1b"
  kind "StaticLib"                    -- [TODO] Shared Option???
  language "C++"
  location "%{wks.location}/lib-amVK"
  files {
    "amVK.cpp",
    "amVK_WI.cpp",
    "amVK_Device.cpp",
    "amVK_Pipeline.cpp",
    "amVK_ImgMemBuf.cpp"
  }

  -- OS SPECIFIC amVK settings
  filter "system:windows"
    defines { "amVK_BUILD_WIN32" }

  -- amVK Options   [using premake FILTERS, doesn't really give much criteria of acceptance or usability.... so we made our own Module on Options]
  newoption {
    trigger = "amVK_AMD_VMA_SUPPORT",
    value = "BOOLEAN",
    description = "Vulkan Memory Allocator.... amVK_DeviceMK2 will have some VMA-Related stuffs, as VMA is Device Specific",
    default = ON,
    allowed = {
      { " true/Y/1/ON",    "TRUE" },
      { "false/N/0/OFF",   "FALSE" }
    }
  }
  -- amVK Options Modifications
  filter { "options:amVK_AMD_VMA_SUPPORT=true or amVK_AMD_VMA_SUPPORT=Y or amVK_AMD_VMA_SUPPORT=1 or amVK_AMD_VMA_SUPPORT=ON" }
    defines { "amVK_AMD_VMA_SUP" }

  -- GET GLSLC [TEST-STUFF]
  local ENV_VK_SDK_PATH = os.getenv("VK_SDK_PATH")
  glslc_Path = nil
  if ENV_VK_SDK_PATH ~= nil then
    glslc_Path = os.pathsearch("glslc.exe", ENV_VK_SDK_PATH .. "/Bin", ENV_VK_SDK_PATH .. "/bin")
  end
  glslc_Path = glslc_Path .. "/glslc.exe"
  print("[amVK] Tried finding GLSLC: " .. glslc_Path)

  -- See if 'glslc' works
  local glslc_verProc = io.popen("glslc --version")
  local glslc_verInfo = glslc_verProc:read("*a") 
  glslc_verProc:close()
  if string.find(glslc_verInfo, "shaderc") and string.find(glslc_verInfo, "glslang") then
    print("[amVK] using \u{0027}glslc\u{0027}.... cz we can")
    glslc_Path = "glslc"
  end

  -- Compile Test with GLSLC
  shaderOne = "./shaders/one"
  if glslc_Path ~= nil then
    local executeCMD_One = glslc_Path .. " \u{0022}" .. shaderOne .. ".vert\u{0022} -o \u{0022}" .. shaderOne .. ".vert.spv\u{0022}"
    local executeCMD_Two = glslc_Path .. " \u{0022}" .. shaderOne .. ".frag\u{0022} -o \u{0022}" .. shaderOne .. ".frag.spv\u{0022}"
    print("[amVK] ".. executeCMD_One)
    print("[amVK] ".. executeCMD_Two)
    os.execute(executeCMD_One)
    os.execute(executeCMD_Two)
  end