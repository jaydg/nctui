//
// Demonstrates the MenuBar widget with File and Help menus, and a
// read-only TextView that shows the project README.
//
import nctui;

#include <algorithm>
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
                int dw = std::min(nctui::application::cols() - 4, 80);
                int dh = std::min(nctui::application::lines() - 2, 28);

                auto dlg = std::make_shared<nctui::Dialog>(dw, dh, "nctui Documentation");

                auto browser = std::make_shared<nctui::DocBrowser>(
                    0, 0, dw - 4, dh - 6, "docs");
                browser->open("index.adoc");
                dlg->add(browser);

                auto btn = std::make_shared<nctui::Button>("Close");
                btn->clicked = [dlg]() { dlg->running = false; };
                dlg->addButton(btn);

                nctui::application::run(dlg);
            })
        });

    auto mb = std::make_shared<nctui::MenuBar>(
        std::vector<std::shared_ptr<nctui::MenuBarItem>>{ mFile, mHelp });
    top->add(mb);

    // Frame below the menu bar (y=1, height reduced by 1 to leave room).
    auto frame = std::make_shared<nctui::Frame>(
        0, 1,
        nctui::application::cols(),
        nctui::application::lines() - 1,
        "README.md");
    frame->fill = nctui::Fill::Horizontal | nctui::Fill::Vertical;
    top->add(frame);

    // Read-only TextView fills the frame interior.
    // Position (0, 0) is relative to the frame's inner area; the library
    // (placeOnScreen / doSizeChanged) handles absolute placement and resize.
    auto tv = std::make_shared<nctui::TextView>(
        *nctui::application::std_plane,
        0, 0,
        nctui::application::cols() - 2,
        nctui::application::lines() - 3);
    tv->fill = nctui::Fill::Horizontal | nctui::Fill::Vertical;
    tv->isReadOnly = true;
    tv->loadFile("README.md");
    frame->add(tv);

    nctui::application::run(top);
    return 0;
}
