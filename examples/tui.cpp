//
// Full TUI demo: torrent-manager-style interface with menus, list views,
// a log widget, a progress bar, and an options dialog.
//
import nctui;

#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <deque>
#include <filesystem>
#include <chrono>
#include <format>
#include <iostream>
#include <cstdlib>
#include <ncpp/Plane.hh>

using namespace std::chrono_literals;
namespace fs = std::filesystem;

// -----------------------------------------------------------------------
// Forward declarations
// -----------------------------------------------------------------------
static void optionsDialog();
static void addDialog();
static void layoutDialogs(
    std::shared_ptr<nctui::Frame> ftorrents,
    std::shared_ptr<nctui::Frame> fstatus,
    std::shared_ptr<nctui::Frame> fdetails,
    std::shared_ptr<nctui::Frame> fprogress);

// -----------------------------------------------------------------------
// TorrentDetailsList — IListProvider implementation
// -----------------------------------------------------------------------
class TorrentDetailsList : public nctui::IListProvider {
public:
    nctui::ListView* view = nullptr;

    void setListView(nctui::ListView* target) override { view = target; }

    std::size_t length() override { return 5; }

    bool isMarked(std::size_t) override { return false; }

    /// Renders "<index> This is item <index>", padded or truncated to width.
    void render(ncpp::Plane* plane, int y, int x, int w,
                std::size_t index) override
    {
        if (!plane) return;
        std::string s = std::format("{0} This is item {0}", index);
        if (static_cast<int>(s.length()) > w)
            s.resize(w);
        else
            s.append(w - s.length(), ' ');
        plane->putstr(y, x, s.c_str());
    }

    bool processKey(char32_t) override { return false; }
    void selectedChanged() override {}
};

// -----------------------------------------------------------------------
// LogWidget — scrolling text log
// -----------------------------------------------------------------------
class LogWidget : public nctui::Widget {
private:
    std::deque<std::string> messages;
    static constexpr std::size_t MAX_MESSAGES = 80;

public:
    LogWidget(int x, int y) : nctui::Widget(x, y, 0, 0) {
        fill = nctui::Fill::Horizontal | nctui::Fill::Vertical;
        addText("Started");
    }

    /// Appends a message, dropping the oldest entry when the buffer is full.
    void addText(const std::string& s) {
        messages.push_back(s);
        if (messages.size() > MAX_MESSAGES)
            messages.pop_front();
    }

    void redraw() override {
        ncpp::Plane* plane = nctui::application::std_plane;
        if (!plane) return;

        plane->set_channels(colorNormal());

        // Render from the most recent message upward, filling the widget height.
        int l = height - 1;
        for (auto it = messages.rbegin(); it != messages.rend() && l >= 0; ++it, --l) {
            std::string line = *it;
            if (static_cast<int>(line.length()) < width)
                line.append(width - line.length(), ' ');
            else if (static_cast<int>(line.length()) > width)
                line.resize(width);
            plane->putstr(y + l, x, line.c_str());
        }

        // Clear any remaining rows above the messages.
        std::string blank(width, ' ');
        for (; l >= 0; --l)
            plane->putstr(y + l, x, blank.c_str());
    }
};

// -----------------------------------------------------------------------
// Status panel — static label/progress-bar pointers
// -----------------------------------------------------------------------
static std::shared_ptr<nctui::ProgressBar> status_progress;
static std::shared_ptr<nctui::Label>       status_state;
static std::shared_ptr<nctui::Label>       status_peers;
static std::shared_ptr<nctui::Label>       status_up;
static std::shared_ptr<nctui::Label>       status_up_speed;
static std::shared_ptr<nctui::Label>       status_down;
static std::shared_ptr<nctui::Label>       status_down_speed;

static std::shared_ptr<nctui::Frame> setupStatus() {
    auto fstatus = std::make_shared<nctui::Frame>("Status");
    const std::string init = "<init>";
    int y = 0;
    const int x = 13;

    fstatus->add(status_progress = std::make_shared<nctui::ProgressBar>(x, y, 24));
    fstatus->add(std::make_shared<nctui::Label>(1, y++, "Progress:"));

    fstatus->add(status_state = std::make_shared<nctui::Label>(x, y, init));
    fstatus->add(std::make_shared<nctui::Label>(1, y++, "State:"));

    fstatus->add(status_peers = std::make_shared<nctui::Label>(x, y, init));
    fstatus->add(std::make_shared<nctui::Label>(1, y++, "Peers:"));
    ++y;

    fstatus->add(std::make_shared<nctui::Label>(1, y++, "Upload:"));
    fstatus->add(std::make_shared<nctui::Label>(16, y, "KB   Speed: "));
    fstatus->add(status_up       = std::make_shared<nctui::Label>(1, y, init));
    fstatus->add(status_up_speed = std::make_shared<nctui::Label>(28, y, init));
    ++y;

    fstatus->add(std::make_shared<nctui::Label>(1, y++, "Download:"));
    fstatus->add(std::make_shared<nctui::Label>(16, y, "KB   Speed: "));
    fstatus->add(status_down       = std::make_shared<nctui::Label>(1, y, init));
    fstatus->add(status_down_speed = std::make_shared<nctui::Label>(28, y, init));

    return fstatus;
}

/// Updates the status panel with mock values.
static void updateStatus(int iteration) {
    auto now = std::chrono::system_clock::now();
    std::string ct = std::format("{:%H:%M:%S}", now);

    status_progress->fraction(static_cast<float>(iteration % 100) / 100.0f);
    status_state->text(ct);
    status_peers->text(ct);
    status_up->text("1000");
    status_up_speed->text("Lots");
}

// -----------------------------------------------------------------------
// Four-quadrant layout — called on startup and on every resize.
// -----------------------------------------------------------------------
static void layoutDialogs(
    std::shared_ptr<nctui::Frame> ftorrents,
    std::shared_ptr<nctui::Frame> fstatus,
    std::shared_ptr<nctui::Frame> fdetails,
    std::shared_ptr<nctui::Frame> fprogress)
{
    const int cols = nctui::application::cols();
    const int midx = cols / 2;
    const int midy = nctui::application::lines() / 2;

    // Torrents: top-left, width = cols - 40
    ftorrents->x = 0;      ftorrents->y = 0;
    ftorrents->width = cols - 40;
    ftorrents->height = midy;

    // Status: top-right, always 40 columns wide
    fstatus->x = cols - 40; fstatus->y = 0;
    fstatus->width  = 40;
    fstatus->height = midy;

    // Details: bottom-left
    fdetails->x = 0;        fdetails->y = midy;
    fdetails->width  = midx;
    fdetails->height = midy;

    // Progress/messages: bottom-right (takes the odd column if cols is odd)
    fprogress->x = midx;    fprogress->y = midy;
    fprogress->width  = midx + cols % 2;
    fprogress->height = midy;
}

// -----------------------------------------------------------------------
// Options dialog
// -----------------------------------------------------------------------
static void optionsDialog() {
    static int s_theme_selection = 0;

    auto d = std::make_shared<nctui::Dialog>(62, 22, "Options");

    d->add(std::make_shared<nctui::Label>(1,  1, "  Download Directory:"));
    d->add(std::make_shared<nctui::Label>(1,  3, "         Listen Port:"));
    d->add(std::make_shared<nctui::Label>(1,  5, "  Upload Speed Limit:"));
    d->add(std::make_shared<nctui::Label>(35, 5, "kB/s"));
    d->add(std::make_shared<nctui::Label>(1,  7, "Download Speed Limit:"));
    d->add(std::make_shared<nctui::Label>(35, 7, "kB/s"));
    d->add(std::make_shared<nctui::Label>(1,  9, "        Stealth Mode:"));
    d->add(std::make_shared<nctui::Label>(1, 13, "         Color Theme:"));

    auto download_dir   = std::make_shared<nctui::Entry>(24, 1, 30, "~/Download");
    auto listen_port    = std::make_shared<nctui::Entry>(24, 3,  6, "34");
    auto upload_limit   = std::make_shared<nctui::Entry>(24, 5, 10, "1024");
    auto download_limit = std::make_shared<nctui::Entry>(24, 7, 10, "1024");

    auto stealth_mode = std::make_shared<nctui::RadioGroup>(
        24, 9,
        std::vector<std::string>{"Off", "Partial", "Full"});

    // Theme combo box — value is the theme file path, empty = built-in default.
    auto theme_model = std::make_shared<nctui::ComboBoxModel<std::string>>();
    theme_model->add("",                                  "Default (built-in)");
    theme_model->add("lib/themes/everforest-dark.theme",  "Everforest Dark");
    theme_model->add("lib/themes/everforest-light.theme", "Everforest Light");
    theme_model->add("lib/themes/gruvbox-dark.theme",     "Gruvbox Dark");
    theme_model->add("lib/themes/gruvbox-light.theme",    "Gruvbox Light");
    theme_model->add("lib/themes/tokyonight-night.theme", "Tokyo Night");
    theme_model->add("lib/themes/tokyonight-day.theme",   "Tokyo Day");
    theme_model->add("lib/themes/mellow-dark.theme",      "Mellow Dark");
    theme_model->add("lib/themes/mellow-light.theme",     "Mellow Light");
    theme_model->add("lib/themes/flexoki-dark.theme",     "Flexoki Dark");
    theme_model->add("lib/themes/flexoki-light.theme",    "Flexoki Light");
    theme_model->add("lib/themes/token-dark.theme",       "Token Dark");
    theme_model->add("lib/themes/token-light.theme",      "Token Light");

    theme_model->selected(s_theme_selection);

    auto theme_combo = std::make_shared<nctui::ComboBox<std::string>>(
        24, 13, 30, theme_model);

    // Snapshot the theme on entry so Cancel can restore it.
    nctui::Theme saved_theme = nctui::theme::current();

    theme_combo->onChanged = [&]() {
        const std::string& path = theme_model->value();
        if (path.empty())
            nctui::theme::apply(nctui::theme::defaultTheme());
        else
            nctui::theme::apply(nctui::theme::load(path));
    };

    d->add(download_dir);
    d->add(listen_port);
    d->add(upload_limit);
    d->add(download_limit);
    d->add(stealth_mode);
    d->add(theme_combo);

    bool ok = false;

    auto bok = std::make_shared<nctui::Button>("Ok", /*is_default=*/true);
    bok->clicked = [&ok, &d_running = d->running]() {
        ok = true;
        d_running = false;
    };
    d->addButton(bok);

    auto bcancel = std::make_shared<nctui::Button>("Cancel");
    bcancel->clicked = [&saved_theme, &d_running = d->running]() {
        nctui::theme::apply(saved_theme);
        d_running = false;
    };
    d->addButton(bcancel);

    nctui::application::run(d);

    if (ok)
        s_theme_selection = theme_model->selected();

    // Validation runs after the dialog closes.
    if (ok) {
        try {
            int port = std::stoi(listen_port->text());
            if (port < 0 || port > 65535) throw std::out_of_range("port");
        } catch (...) {
            nctui::application::error(
                "Error",
                std::format("The value `{}' is not a valid port number",
                            listen_port->text()));
            return;
        }

        // Expand leading ~ to $HOME.
        std::string path = download_dir->text();
        if (!path.empty() && path[0] == '~') {
            const char* home = std::getenv("HOME");
            if (home) path = std::string(home) + path.substr(1);
        }

        if (!fs::exists(path) || !fs::is_directory(path)) {
            nctui::application::error(
                "Error",
                std::format("The directory\n{}\ndoes not exist", path));
        }
    }
}

// -----------------------------------------------------------------------
// Add dialog — add a torrent by file path.
// -----------------------------------------------------------------------
static void addDialog() {
    int dlg_cols = static_cast<int>(nctui::application::cols() * 0.7);
    auto d = std::make_shared<nctui::Dialog>(dlg_cols, 8, "Add");

    d->add(std::make_shared<nctui::Label>(1, 0, "Torrent file:"));

    // Pre-fill with the current working directory
    std::string cwd = fs::current_path().string();
    auto e = std::make_shared<nctui::Entry>(1, 1, dlg_cols - 6, cwd);
    d->add(e);

    std::string name;

    auto bok = std::make_shared<nctui::Button>("Ok", /*is_default=*/true);
    bok->clicked = [&name, &e, &d_running = d->running]() {
        name = e->text();
        d_running = false;
    };
    d->addButton(bok);

    auto bcancel = std::make_shared<nctui::Button>("Cancel");
    bcancel->clicked = [&d_running = d->running]() { d_running = false; };
    d->addButton(bcancel);

    nctui::application::run(d);

    if (!name.empty()) {
        if (!fs::exists(name) || !fs::is_regular_file(name)) {
            nctui::application::error(
                "Missing File",
                std::format("Torrent file:\n{}\ndoes not exist", name));
        }
    }
}

// -----------------------------------------------------------------------
// main
// -----------------------------------------------------------------------
int main() {
    nctui::application::init();

    auto top = std::make_shared<nctui::Container>(
        0, 0,
        nctui::application::cols(),
        nctui::application::lines());

    // Main torrent list frame (top-left quadrant).
    auto frame = std::make_shared<nctui::Frame>(
        0, 0,
        nctui::application::cols(),
        nctui::application::lines(),
        "List");
    top->add(frame);

    auto badd = std::make_shared<nctui::Button>(1, 1, "Add");
    badd->clicked = []() { addDialog(); };
    frame->add(badd);

    auto boptions = std::make_shared<nctui::Button>(9, 1, "Options");
    frame->add(boptions);
    // boptions->clicked is wired up below, after all frames are declared.

    auto bquit = std::make_shared<nctui::Button>(21, 1, "Quit");
    bquit->clicked = [&top_running = top->running]() { top_running = false; };
    frame->add(bquit);

    auto list = std::make_shared<nctui::ListView>(
        1, 5, 0, 0,
        std::make_shared<TorrentDetailsList>());
    list->fill = nctui::Fill::Horizontal | nctui::Fill::Vertical;
    frame->add(list);

    // Messages / log (bottom-right quadrant).
    auto fprogress  = std::make_shared<nctui::Frame>("Messages");
    auto log_widget = std::make_shared<LogWidget>(0, 0);
    fprogress->add(log_widget);
    top->add(fprogress);

    // Details (bottom-left quadrant).
    auto fdetails = std::make_shared<nctui::Frame>("Details");
    fdetails->add(std::make_shared<nctui::Label>(1, 1, "Files for: "));
    auto torrent_name = std::make_shared<nctui::TrimLabel>(12, 1, 10, "");
    torrent_name->fill = nctui::Fill::Horizontal;
    fdetails->add(torrent_name);

    auto list_details = std::make_shared<nctui::ListView>(
        1, 3, 0, 0,
        std::make_shared<TorrentDetailsList>());
    list_details->fill = nctui::Fill::Horizontal | nctui::Fill::Vertical;
    fdetails->add(list_details);
    top->add(fdetails);

    // Status panel (top-right quadrant).
    auto fstatus = setupStatus();
    top->add(fstatus);

    // Wire up Options button now that all frames are in scope.
    boptions->clicked = []() { optionsDialog(); };

    // Timer: update status and log every second.
    int it = 0;
    nctui::application::mainLoop->addTimeout(1s, [&]() {
        updateStatus(it);
        log_widget->addText(std::format("Iteration {}", it));
        ++it;
        nctui::application::refresh();
        return true;
    });

    // Initial layout, then hook sizeChanged for future resizes.
    layoutDialogs(frame, fstatus, fdetails, fprogress);
    top->onSizeChanged = [&]() {
        layoutDialogs(frame, fstatus, fdetails, fprogress);
    };

    updateStatus(it);

    nctui::application::run(top);
    return 0;
}
