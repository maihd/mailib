local mailib = require("mailib.premake5")

local ROOT_DIR = path.getabsolute(".")
local BUILD_DIR = path.join(ROOT_DIR, "projects")

local function filedirs(dirs)
    for _, directory in ipairs(dirs) do
        files {
            path.join(directory, "*.h"),
            path.join(directory, "*.c"),
            path.join(directory, "*.cpp"),
        }
    end
end

workspace "mailib"
do
    language "C++"
    location (path.join(BUILD_DIR, _ACTION))

    configurations { "Debug", "Release" }
    platforms { "x32", "x64" }

    mailib.cflags();

    filter {}
end

project "mailib_unit_tests"
do
    kind "ConsoleApp"

    -- Add MaiLib files
    mailib.files(ROOT_DIR)
    mailib.links(ROOT_DIR)
    mailib.includedirs(ROOT_DIR)

    filedirs {
        path.join(ROOT_DIR, "tests"),
        path.join(ROOT_DIR, "tests/**")
    }

    filter {}
end