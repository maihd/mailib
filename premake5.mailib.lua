local function filedirs(dirs)
    for _, directory in ipairs(dirs) do
        files {
            path.join(directory, "*.h"),
            path.join(directory, "*.c"),
            path.join(directory, "*.cpp"),
        }
    end
end

return {
    cflags = function ()
        flags {
            "NoPCH",
            "NoRuntimeChecks",
            "ShadowedVariables",
            "LinkTimeOptimization",
    
            --"FatalWarnings",
            --"FatalLinkWarnings",
            "FatalCompileWarnings",
        }
    
        cppdialect "C++11"
        staticruntime "On"
        omitframepointer "On"
    
        rtti "Off"
        exceptionhandling "Off"
    end,

    links = function (config)
        config = config or {}
        if not config.NoThreading then
            filter { "action:vs*"}
            do
                links {
                        
                } 
            end         
        end
    end,

    files = function (directory, config)
        config = config or {}

        filedirs {
            path.join(directory, "Include/**"),

            path.join(directory, "Source/Memory"),
            path.join(directory, "Source/Misc"),
            path.join(directory, "Source/Text"),
        }

        if not config.NoThreading then
            filedirs {
                path.join(directory, "Source/Threading"),
            }

            filter { "action:vs*" }
            do
                filedirs {
                    path.join(directory, "Source/Threading/Windows"),
                }
            end

            filter {}
        end

        if not config.NoFileSystem then
            filedirs {
                path.join(directory, "Source/FileSystem"),
            }

            filter { "action:vs*" }
            do
                filedirs {
                    path.join(directory, "Source/FileSystem/Windows"),
                }
            end

            filter {}
        end
    end,

    includedirs = function (directory)
        includedirs { path.join(directory, "Include") }
    end
}