local MaiLib = require("premake5.mailib")

local ROOT_DIR = path.getabsolute(".")
local BUILD_DIR = path.join(ROOT_DIR, "Projects")

local function filedirs(dirs)
    for _, directory in ipairs(dirs) do
        files {
            path.join(directory, "*.h"),
            path.join(directory, "*.c"),
            path.join(directory, "*.cpp"),
        }
    end
end

workspace "MaiLib"
do
    language "C++"
    location (path.join(BUILD_DIR, _ACTION:upper()))

    configurations { "Debug", "Release" }
    platforms { "x32", "x64" }

    MaiLib.cflags();

    filter {}
end

project "MaiLib_UnitTests"
do
    kind "ConsoleApp"

    -- Add MaiLib files
    MaiLib.files(ROOT_DIR)
    MaiLib.links(ROOT_DIR)
    MaiLib.includedirs(ROOT_DIR)

    filedirs {
        path.join(ROOT_DIR, "Tests"),
        path.join(ROOT_DIR, "Tests/**")
    }

    filter {}
end