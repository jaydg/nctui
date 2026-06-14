//
// Demonstrates the MenuBar widget with File and Help menus.
//
import nctui;

#include <memory>
#include <vector>
#include <string>

int main() {
    nctui::application::init();

    auto top = std::make_shared<nctui::Container>(
        0, 0,
        nctui::application::cols(),
        nctui::application::lines());

    // File menu — Exit item closes the run loop.
    auto mFile = std::make_shared<nctui::MenuBarItem>(
        "_File",
        std::vector<std::shared_ptr<nctui::MenuItem>>{
            std::make_shared<nctui::MenuItem>("E_xit", "", [&]() {
                top->running = false;
            })
        });

    // Help menu — About and Index items display information dialogs.
    auto mHelp = std::make_shared<nctui::MenuBarItem>(
        "_Help",
        std::vector<std::shared_ptr<nctui::MenuItem>>{
            std::make_shared<nctui::MenuItem>("_About", "", []() {
                nctui::application::info(
                    "About nctui",
                    "© Copyright 2018 Joachim de Groot");
            }),
            std::make_shared<nctui::MenuItem>("_Index", "", []() {
                nctui::application::info("Help", "Nothing to see here.");
            })
        });

    auto mb = std::make_shared<nctui::MenuBar>(
        std::vector<std::shared_ptr<nctui::MenuBarItem>>{ mFile, mHelp });
    top->add(mb);

    // Frame below the menu bar (y=1, height reduced by 1 to leave room).
    // The D original passes null as the title; we use an empty string.
    auto frame = std::make_shared<nctui::Frame>(
        0, 1,
        nctui::application::cols(),
        nctui::application::lines() - 1,
        "");
    top->add(frame);

    nctui::application::run(top);
    return 0;
}
