add_requires("glfw", "glew", "libtorch", "nativefiledialog-extended", "stb", "ffmpeg")
add_requires("imgui v1.90.4-docking", {configs = {opengl3 = true, glfw = true}})
set_policy("build.ccache", false)
set_languages("c++23")

add_rules("mode.debug", "mode.release")

target("pbt", function ()
    set_kind("binary")
    add_headerfiles("src/**.h")
    add_packages("libtorch", "glew", "glfw", "imgui", "nativefiledialog-extended", "stb", "ffmpeg")
    add_files("src/**.cpp")

    if is_plat("windows") then
        add_ldflags("/subsystem:windows")
        add_ldflags("/INCREMENTAL:NO")
    elseif is_plat("mingw") then
        add_ldflags("-mwindows")
    end

    after_build(function (target)
        os.cp("./rsc", path.join(target:targetdir(), "./rsc"))
    end)
end)
