cmake_minimum_required(VERSION 3.5.1)
project(Coin)

set(CMAKE_CXX_STANDARD 14)

# ============ 平台宏定义 ============
add_definitions(
            -DLINUX
            -DWAYLAND
            -DBOOST_ASIO_HAS_STD_STRING_VIEW
            -DWL_EGL_PLATFORM
            -DPNG_NO_MMX_CODE
            -DNDEBUG
            -DKANZI_FEATURE_3D
            )

# ============ Kanzi 引擎宏定义 ============
add_definitions(
            -DKZ_FREETYPE_FONT_ENGINE_LINKED
            -DKZ_SUPPORT_GRAPHICS_CONTEXT_API_EGL=1
            -DKZ_SUPPORT_GRAPHICS_API_GLES=1
            -DKZ_LINKED_GLES_SYMBOLS=31
            -DKZ_DLOAD_GLES_SYMBOLS=31
            )
        
include_directories(/opt/Engine/include)
include_directories(/opt/Engine/libraries/common/boost/include)
link_directories(/opt/Engine/lib/linux_wayland_aarch64_clang15/ES3_Release)

include_directories(inc)
link_directories(lib)

add_executable(Coin src/coin.cpp)
target_link_options(Coin PRIVATE "-Wl,--dynamic-linker=/lib/hmld.so.elf")

# ============ 链接库 ============
target_link_libraries(Coin
            # Wayland 库
            wayland-client
            wayland-egl
            EGL
            GLESv2
            kzappfw
            # Kanzi 动态库（.so 文件）
            kzcore
            kzcoreui
            kzui
            kzfreetypeplugin
            )

# ============ 定义编译后自动复制 ============
add_custom_command(TARGET Coin POST_BUILD
    # 使用 CMake 的跨平台复制命令
    COMMAND ${CMAKE_COMMAND} -E copy
    # 源文件路径：自动获取生成的 Coin 可执行文件路径
    $<TARGET_FILE:Coin>
    # 目标目录：修改为你想要的目录 (例如项目根目录下的 bin 文件夹)
    ${CMAKE_SOURCE_DIR}/bin
    # 打印提示信息
    COMMENT "Copying Coin to bin directory..."
)