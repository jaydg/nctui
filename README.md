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
| `Docbrowser`  | AsciiDoc document browser                 |
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

The `examples/` directory contains several examples demonstrating various features of nctui:

| Example | Description | Widgets Used |
|---------|-------------|--------------|
| **simple.cpp** | Minimal nctui application showing basic widget usage | `Container`, `Label`, `Entry` |
| **mainloop.cpp** | Demonstrates the `MainLoop` class with timer-based events | `MainLoop` (no UI widgets) |
| **menu.cpp** | Shows a menu bar with drop-down menus and a read-only text viewer | `MenuBar`, `MenuBarItem`, `MenuItem`, `Frame`, `TextView`, `Dialog`, `DocBrowser`, `Button` |
| **tui.cpp** | Full torrent-manager-style TUI with resizable multi-pane layout | `Container`, `Frame`, `Button`, `ListView`, `Label`, `TrimLabel`, `ProgressBar`, `Entry`, `RadioGroup`, `ComboBox`, `Dialog`, plus custom `LogWidget` |

![Menu example](/menu.png "nctui: Menu, text viewer and documentation browser example")

#### Example Highlights

- **simple.cpp**: Perfect starting point. Creates a container with a label and an editable entry field.
- **mainloop.cpp**: Demonstrates timer management with `addTimeout()` for scheduled callbacks.
- **menu.cpp**: Features a complete menu system with File (Exit) and Help (About, Index) menus. The Index menu opens a `DocBrowser` dialog to view AsciiDoc documentation, and this README.md is displayed in a read-only `TextView` in the main window.
- **tui.cpp**: A comprehensive demo showcasing advanced features including:
  - Multi-pane layout with draggable resizable frames
  - Status panel with `ProgressBar` and multiple `Label` widgets
  - Options dialog with `Entry` fields, `RadioGroup`, and `ComboBox` for theme selection
  - Live theme switching with preview
  - Input validation for dialog fields
  - Custom `LogWidget` demonstrating how to create custom widgets
  - Timer-based status updates and logging
  - Add dialog for file selection with validation

![TUI example](/tui.png "nctui: Torrent Manager example")

For detailed usage information, module imports, input handling, focus management, dialogs, and menu system documentation, please refer to the [AsciiDoc documentation](https://github.com/jaydg/nctui/tree/main/docs) in the `docs/` folder.

## Documentation

The `docs/` directory contains comprehensive AsciiDoc documentation covering:

- **Usage**: Project setup, module imports, and requirements
- **Widgets**: Complete widget reference with examples
- **Input Handling**: Key processing phases and hot keys
- **Focus Management**: Widget focus and navigation
- **Dialogs**: Message boxes and dialog widgets
- **Menu System**: Menu bar and drop-down menus
- **Themes**: Color themes and styling

## License

MIT License - see project root for full text.

## Contributing

Contributions are welcome! Please ensure:

- Code follows the existing style and conventions
- C++20 features are used appropriately
- Module interfaces are clean and well-organized
