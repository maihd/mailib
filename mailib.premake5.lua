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
            path.join(directory, "include/**"),

            path.join(directory, "src/memory"),
            path.join(directory, "src/misc"),
            path.join(directory, "src/text"),
        }

        if not config.NoThreading then
            filedirs {
                path.join(directory, "src/threading"),
            }

            filter { "action:vs*" }
            do
                filedirs {
                    path.join(directory, "src/threading/windows"),
                }
            end

            filter {}
        end

        if not config.NoFileSystem then
            filedirs {
                path.join(directory, "src/fs"),
            }

            filter { "action:vs*" }
            do
                filedirs {
                    path.join(directory, "src/fs/windows"),
                }
            end

            filter {}
        end
    end,

    includedirs = function (directory)
        includedirs { path.join(directory, "include") }
    end
}