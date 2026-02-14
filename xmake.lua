add_rules("mode.debug", "mode.release")

target("IIM42652")
    set_kind("binary")
    add_files("src/*.c", "public.mcu.iim42652/Ixm42xxx/*.c")  -- 添加所有C源文件
    add_includedirs("public.mcu.iim42652/Ixm42xxx", "src", "test")  -- 添加头文件路径
    add_defines("ICM42652")  -- 定义芯片型号

target("spi_detector")
    set_kind("binary")
    add_files("test/*.c", "public.mcu.iim42652/Ixm42xxx/*.c", "src/platform.c")  -- 添加测试源文件、驱动库和平台实现
    add_includedirs("public.mcu.iim42652/Ixm42xxx", "src", "test")  -- 添加头文件路径
    add_defines("ICM42652")  -- 定义芯片型号

-- 构建后脚本：创建out文件夹并将可执行文件复制到那里
after_build(function (target)
    local out_dir = path.join(os.projectdir(), "out")
    os.mkdir(out_dir)
    
    local exe_name = target:targetfile()
    local exe_basename = path.filename(exe_name)
    local dest_path = path.join(out_dir, exe_basename)
    
    cprint("${yellow}Copying executable to out folder...")
    os.cp(exe_name, dest_path)
    cprint("${green}Executable copied to: %s", dest_path)
end)

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--