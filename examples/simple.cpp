//
// Minimal nctui example — creates a label and an entry widget.
//
import nctui;
#include <memory>

int main() {
    nctui::application::init();

    auto d = std::make_shared<nctui::Container>(
        0, 0,
        nctui::application::cols(),
        nctui::application::lines());

    d->add(std::make_shared<nctui::Label>(10, 10, "Text"));
    d->add(std::make_shared<nctui::Entry>(16, 10, 20, "Edit me"));

    nctui::application::run(d);
    return 0;
}
