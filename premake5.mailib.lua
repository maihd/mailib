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
    links = function (config)
        config = config or {}
        if config.NonNative then
           return           
        end

        filter { "action:vs*"}
        do
            links {
                     
            } 
        end
    end,

    files = function (directory, config)
        config = config or {}
        if config.NonNative then
            return
        end

        filedirs {
            path.join(directory, "Include/**"),
            
            path.join(directory, "Source/Memory"),
            path.join(directory, "Source/Misc"),
        }

        filter { "action:vs*" }
        do
            filedirs {
                path.join(directory, "Source/Windows/Threading"),
            }
        end
    end,

    includedirs = function (directory)
        includedirs { path.join(directory, "Include") }
    end
}