#include <atomic>
#include <cstdint>
#include <iostream>
#include <optional>
#include <thread>
#include <vector>

#include <windows.h>
#include <winuser.h>

#include <atlbase.h>
#include <comdef.h>
#include <hidapi.h>
#include <objbase.h>

#include <scope_guard.hpp>

#define VENDOR_ID 0x2341
#define PRODUCT_ID 0x8036


void scroll_mouse_xy(int dx, int dy) {
    if (GetDpiAwarenessContextForProcess(NULL) == DPI_AWARENESS_CONTEXT_UNAWARE) {
        throw std::runtime_error("Must call SetProcessDPIAware() beforehand!");
    }

    INPUT input[2] = {};
    std::size_t count = 0;

    if (dx != 0) {
        input[count].type = INPUT_MOUSE;
        input[count].mi.dwFlags = MOUSEEVENTF_HWHEEL;
        input[count].mi.mouseData = dx;
        count++;
    }

    if (dy != 0) {
        input[count].type = INPUT_MOUSE;
        input[count].mi.dwFlags = MOUSEEVENTF_WHEEL;
        input[count].mi.mouseData = dy;
        count++;
    }

    if (count > 0) {
        SendInput(count, static_cast<LPINPUT>(input), sizeof(INPUT));
    }
}


auto find_devices() -> std::vector<hid_device*> {
    auto* devices = hid_enumerate(VENDOR_ID, PRODUCT_ID);
    auto _devices = sg::make_scope_guard(
    [&devices]() {
        hid_free_enumeration(devices);
        devices = nullptr;
    });

    std::vector<hid_device*> result;

    auto* current = devices;
    while (current) {
        if (current->usage_page == 0xffff) {
            auto* device = hid_open_path(current->path);
            assert(device);
            result.push_back(device);
        }
        current = current->next;
    }

    return result;
}


void read_device(hid_device* device) {
    assert(device);

    hid_set_nonblocking(device, 1);
    std::vector<uint8_t> data(255);

    while (true) {
        int bytes_read = hid_read(device, data.data(), data.size());
        if (bytes_read > 0 && data[0] == 0x20) {
            if (bytes_read != 5) {
                std::cerr << "Invalid packet length\n";
                continue;
            }

            // FIXME bad cast if host system is big endian
            int x = static_cast<int16_t>(data[1] | (data[2] << 8));
            int y = static_cast<int16_t>(data[3] | (data[4] << 8));

            std::cout << "(" << x << ", " << y << ")\n";
            scroll_mouse_xy(x, y);
        }
    }
}


class MouseHook {
public:
    static auto instance() -> MouseHook& {
        static MouseHook instance;
        return instance;
    }

    void start() {
        if (hook_id == NULL) {
            std::vector<wchar_t> buffer(MAX_PATH);
            auto length = GetModuleFileNameW(NULL, buffer.data(), buffer.size());
            while (length >= buffer.size() - 1) {
                buffer.resize(buffer.size() * 2);
                length = GetModuleFileNameW(NULL, buffer.data(), buffer.size());
            }

            module_name = buffer.data();
            hook_id = SetWindowsHookEx(
                WH_MOUSE_LL,
                callback,
                GetModuleHandleW(buffer.data()),
                0
            );
        }
    }

    void stop() {
        if (hook_id != NULL) {
            UnhookWindowsHookEx(hook_id);
            hook_id = NULL;
        }

        module_name.clear();
    }

private:
    std::atomic<HHOOK> hook_id = NULL;
    std::wstring module_name;

    MouseHook() = default;

    static auto callback(int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT CALLBACK {
        std::cout << "HookCallback called\n";

        auto& this_ = instance();

        if (nCode == HC_ACTION) {
            auto* event = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

            std::cout << "Mouse Position: (" << event->pt.x << ", " << event->pt.y << ")\n";

            if (wParam == WM_MOUSEWHEEL || wParam == WM_MOUSEHWHEEL) {
                std::cout << "Wheel\n";
                return 0x01;
            }
        }

        return CallNextHookEx(this_.hook_id, nCode, wParam, lParam);
    }
};


auto main(int argc, char** argv) -> int {
    // necessary for stepless scrolling
    SetProcessDPIAware();

    auto& driver = MouseHook::instance();
    driver.start();
    auto _driver = sg::make_scope_guard([&driver] {
        driver.stop();
    });


    MSG msg;
    memset(&msg, 0x00, sizeof(msg));

    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    return 0;
}


#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#include "winconsole.hpp"
_Use_decl_annotations_ INT WINAPI WinMain(
    [[maybe_unused]] HINSTANCE hInstance,
    [[maybe_unused]] HINSTANCE hPrevInstance,
    [[maybe_unused]] PSTR lpCmdLine,
    [[maybe_unused]] INT nCmdShow) {
    [[maybe_unused]] auto console = WinConsole::attach();
    return main(__argc, __argv);
}
#endif

