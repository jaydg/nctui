# nctui - C++20 Terminal UI Framework

This is an Text User Interface (TUI) framework which provides a rich set of widgets for building sophisticated text-based user interfaces in the terminal. This framework leverages [notcurses](https://github.com/dankamongmen/notcurses) for rendering.

It is a C++20 port of a Terminal UI framework originally created for the Mono project as part of MonoCurses by Miguel de Icaza (see [his original blog post](https://tirania.org/blog/archive/2007/Apr-16.html)). Later it was - still using ncurses - [ported to D](https://github.com/jaydg/ctui).

## Overview

**nctui** is a C++20 module-based framework. It offers:

- **Module-based architecture** using C++20's module system
- **Comprehensive widget library** including buttons, labels, text entries, dialogs, menus, and more
- **Event-driven input handling** with keyboard and mouse support
- **Main loop integration** for application lifecycle management
- **Container-based layout** system for building complex UIs

## Features

### Widgets

| Widget        | Description                               |
|---------------|-------------------------------------------|
| `Button`      | Clickable button with customizable text   |
| `Label`       | Static text display                       |
| `TrimLabel`   | Label with automatic text trimming        |
| `Checkbox`    | Toggleable checkbox                       |
| `Entry`       | Single-line text input                    |
| `Combobox`    | Multi-select that shows selection         |
| `TextView`    | Multi-line text display and editing       |
| `ListView`    | Scrollable list of items                  |
| `ProgressBar` | Visual progress indicator                 |
| `Frame`       | Container with border and title           |
| `Dialog`      | Modal dialog windows                      |
| `MessageBox`  | Pre-configured message dialogs            |
| `MenuBar`     | Drop-down menu system                     |
| `RadioGroup`  | Group of mutually exclusive radio buttons |
| `Container`   | Base class for custom widget containers   |

### Core Components

- **Application** - Manages the notcurses context and top-level containers
- **MainLoop** - Event loop for processing input and rendering
- **Keys** - Key handling and accelerator support
- **Globals** - Global state and constants
- **Utils** - Utility functions and helpers

## Requirements

- C++20 compliant compiler (GCC 11+, Clang 14+, MSVC 19.29+)
- [notcurses](https://github.com/dankamongmen/notcurses) (via notcurses++ C++ bindings)
- [xmake](https://xmake.io/) build system

## Quick Start

### Building

The project uses [xmake](https://xmake.io/) as its build system:

```bash
# Install dependencies (notcurses and notcurses++)
# On Ubuntu/Debian:
sudo apt-get install libnotcurses-dev notcurses++-dev

# Build the library and examples
xmake

# Run a specific example
xmake run simple_example
xmake run mainloop_example
xmake run menu_example
xmake run tui_example
```

### Examples

See the examples directory for several partial examples and a complete example.

## Usage

### Using nctui in Your Project

Add nctui as a dependency in your xmake.lua:

```lua
add_requires("notcurses++")

target("my_app")
    set_kind("binary")
    add_deps("nctui")
    add_packages("notcurses++")
    add_files("src/*.cpp")
```

### Module Imports

nctui uses C++20's module system. Import the main module to access all functionality:

```cpp
import nctui;
```

## Input Handling

The framework supports three-phase key processing:

1. **Hot Keys** (Global accelerators) - Alt-key combinations processed first
2. **Normal Keys** - Sent to the focused widget
3. **Cold Keys** (Cold accelerators) - e.g., Enter on default buttons

Special keys:
- **Tab/Shift-Tab** - Cycle focus between widgets
- **Ctrl-C** - Stop the current top-level container's run loop
- **Alt-X** - Common hot key pattern for menu accelerators

## Widget Focus and Navigation

Widgets can receive focus and respond to keyboard input. The framework automatically manages focus cycling with Tab and Shift-Tab. Use the `canFocus` property to control which widgets are focusable.

## Dialogs and Message Boxes

The framework provides convenience functions for common dialogs:

```cpp
// Simple message box
nctui::messageBox("Title", "Message text");

// Yes/No dialog
bool result = nctui::messageBox("Question", "Continue?",
                              nctui::MessageBoxButtons::YesNo);
```

## Menu System

The `MenuBar` widget provides drop-down menu functionality:

```cpp
auto menubar = std::make_shared<nctui::MenuBar>();

auto fileMenu = std::make_shared<nctui::Menu>("File");
fileMenu->addItem("Open", []() { /* handle open */ });
fileMenu->addItem("Save", []() { /* handle save */ });
fileMenu->addSeparator();
fileMenu->addItem("Quit", []() { /* handle quit */ });

menubar->addMenu(fileMenu);
container->add(menubar);
```

## License

MIT License - see project root for full text.

## Contributing

Contributions are welcome! Please ensure:

- Code follows the existing style and conventions
- C++20 features are used appropriately
- Module interfaces are clean and well-organized
