//
// Demonstrates the nctui MainLoop independently of the UI layer,
// using only timers.
//
import nctui;

#include <iostream>
#include <chrono>
#include <format>

using namespace std::chrono_literals;

/// Prints a timestamped message to stdout.
static void stamp(std::string_view txt) {
    auto now = std::chrono::system_clock::now();
    std::cout << std::format("{:%Y-%m-%d %H:%M:%S} - {}\n", now, txt);
}

int main() {
    nctui::MainLoop ml;

    stamp("Start");

    // Fires every second, repeats indefinitely (returns true).
    ml.addTimeout(1s, [&]() {
        stamp("second");
        return true;
    });

    // Fires every 3 seconds, removes itself after the third invocation.
    int i = 0;
    ml.addTimeout(3s, [&]() {
        stamp("three");
        return ++i < 3;
    });

    // Fires after 15 seconds, stops the main loop and removes itself.
    ml.addTimeout(15s, [&]() {
        stamp("That's all, folks!");
        ml.stop();
        return false;
    });

    ml.run();
    return 0;
}
