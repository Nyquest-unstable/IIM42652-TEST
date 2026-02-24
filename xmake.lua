add_rules("mode.debug", "mode.release")

-- 工程基础配置（补全，避免解析异常）
set_project("IIM42652-TEST")
set_version("1.0.0")
set_languages("c99")

-- ==============================================
-- 核心：定义指向 LubanCat SDK 自带工具链的配置
-- ==============================================
toolchain("rv1126_lubancat")
    set_kind("standalone")
    -- 1. 工具链 bin 目录（精准指向你提供的路径）
    set_bindir("/home/lubancat/LubanCat_SDK/prebuilts/gcc/linux-x86/arm/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin")
    -- 2. 交叉编译前缀（匹配工具链：arm-none-linux-gnueabihf-）
    set_cross("arm-none-linux-gnueabihf-")
    
    -- 3. RV1126 (Cortex-A7) 专属编译优化
    add_cflags("-march=armv7-a", "-mtune=cortex-a7", "-mfpu=neon-vfpv4", "-mfloat-abi=hard", "-fPIC")
    add_asflags("-march=armv7-a", "-mtune=cortex-a7", "-mfpu=neon-vfpv4", "-mfloat-abi=hard")
    
    -- 4. 静态编译选项（解决开发板 "not found" 问题）
    add_cflags("-static")
    add_ldflags("-static", "-static-libgcc", "-lm", "-lc")  -- 显式链接静态库


-- ==============================================
-- 目标配置：关联 RV1126 工具链
-- ==============================================
target("IIM42652")
    set_kind("binary")
    add_files("src/*.c", "public.mcu.iim42652/Ixm42xxx/*.c")
    add_includedirs("public.mcu.iim42652/Ixm42xxx", "src", "test")
    add_defines("ICM42652")
    
    -- 指定 RV1126 编译环境
    set_plat("linux")       -- RV1126 运行 Linux 系统
    set_arch("arm")         -- ARM32 架构
    set_toolchains("rv1126_lubancat")-- 关联上面定义的 poky 工具链

-- target("spi_detector")
--     set_kind("binary")
--     add_files("test/*.c", "public.mcu.iim42652/Ixm42xxx/*.c", "src/platform.c")
--     add_includedirs("public.mcu.iim42652/Ixm42xxx", "src", "test")
--     add_defines("ICM42652")
--
--     -- 同样关联 RV1126 poky 工具链
--     set_plat("linux")
--     set_arch("arm")
--     set_toolchains("rv1126")

-- ==============================================
-- 构建后脚本：复制 RV1126 产物到 out/rv1126 目录
-- ==============================================
after_build(function (target)
    local out_dir = path.join(os.projectdir(), "out", "rv1126")  -- 新增 rv1126 子目录，区分产物
    os.mkdir(out_dir)
    
    local exe_name = target:targetfile()
    if os.isfile(exe_name) then  -- 增加文件存在性检查，避免报错
        local exe_basename = path.filename(exe_name)
        local dest_path = path.join(out_dir, exe_basename)
        
        cprint("${yellow}Copying RV1126 executable to out folder...")
        os.cp(exe_name, dest_path)
        cprint("${green}RV1126 executable copied to: %s", dest_path)
        
        -- 可选：验证产物架构（输出 ARM32 则正确）
        local file_info = os.iorun("file " .. dest_path)
        cprint("${cyan}File architecture: %s", file_info:sub(1, 50))  -- 只显示前50个字符，避免刷屏
    else
        cprint("${red}Error: Executable file %s not found!", exe_name)
    end
end)
