-- xmake.lua
set_xmakever("3.0.0")

set_project("ctui_project")
set_version("1.0.0")
set_languages("c++20")

add_requires(
    "notcurses++",
    {
        system = true,
        configs = { pkg_config = true }
    }
)

-------------------------------------------------------------------------------
-- nctui shared library
-------------------------------------------------------------------------------
target("nctui")
    set_kind("shared")
    add_packages("notcurses++")

    -- {public = true} macht die Modul-Schnittstellen für abhängige Targets sichtbar.
    add_files("src/*.mpp", {public = true})
    add_files("src/widgets/*.mpp", {public = true})

    add_includedirs("src", {public = true})

-------------------------------------------------------------------------------
-- examples
-------------------------------------------------------------------------------
target("simple_example")
    set_kind("binary")
    add_deps("nctui")
    add_packages("notcurses++")
    add_files("examples/simple.cpp")

target("mainloop_example")
    set_kind("binary")
    add_deps("nctui")
    add_packages("notcurses++")
    add_files("examples/mainloop.cpp")

target("menu_example")
    set_kind("binary")
    add_deps("nctui")
    add_packages("notcurses++")
    add_files("examples/menu.cpp")

target("tui_example")
    set_kind("binary")
    add_deps("nctui")
    add_packages("notcurses++")
    add_files("examples/tui.cpp")
