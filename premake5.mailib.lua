local function filedirs(directory)

end

return {
    links = function (config)
        config = config or {}
        if config.NonNative then
           return           
        end

        filters { "action:vs*"}
        do
            links {
                     
            } 
        end
    end,

    files = function (directory, config)
        if config.NonNative then
            return
        end

        files {
            path.join(directory, "src/Memory/GeneralAllocation.c")
        }

        filters { "actions:vs*" }
        do
            filedirs {
                path.join(directory, "src/Windows/Threading"),
            }
        end
    end,

    includedirs = function (directory)
        includedirs { path.join(directory, "include") }
    end
}