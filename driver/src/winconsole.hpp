/*
 * winconsole.hpp by midrare <midrare9@gmail.com>
 * Allows console output to stdout and stderr in GUI mode Windows apps.
 * On other platforms, does nothing.
 *
 * This file, winconsole.hpp, is released under The Unlicense
 * Last modification: 2024/11/07
 */

/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <https://unlicense.org>
 */

#ifndef WINCON_3865628525_HPP
#define WINCON_3865628525_HPP
// clang-format off
// NOLINTBEGIN

#include <cassert>
#include <cstdio>
#include <stdexcept>

#if (defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64))
#include <consoleapi.h>
#endif


class WinConsole {
public:
    inline WinConsole(WinConsole&& old) noexcept
        : attached(old.attached), stdin_(old.stdin_), stdout_(old.stdout_),
          stderr_(old.stderr_) {
        old.stdin_ = nullptr;
        old.stdout_ = nullptr;
        old.stderr_ = nullptr;
        old.attached = false;
    }

    inline WinConsole(const WinConsole&) = delete;
    inline ~WinConsole() { this->detach_console(); }

    auto operator=(const WinConsole&) = delete;
    auto operator=(WinConsole&) -> WinConsole& = delete;
    auto operator=(WinConsole&&) -> WinConsole& = delete;

    // static "constructor" to make it more obvious this is RAII
    [[nodiscard]] [[maybe_unused]] inline static auto attach() -> WinConsole {
        return {};
    }

private:
    bool attached = false;

    FILE* stdin_ = nullptr;
    FILE* stdout_ = nullptr;
    FILE* stderr_ = nullptr;

    [[nodiscard]] [[maybe_unused]] inline WinConsole() { this->attach_console(); }

    inline void attach_console() {
        if (this->attached) {
            throw std::runtime_error("console is already attached");
        }

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
        // https://stackoverflow.com/a/26087606
        this->attached =
            static_cast<bool>(AttachConsole(ATTACH_PARENT_PROCESS));
        if (this->attached) {
            assert(this->stdin_ == nullptr);
            assert(this->stdout_ == nullptr);
            assert(this->stderr_ == nullptr);

            _wfreopen_s(&this->stdin_, L"CONIN$", L"r", stdin);
            _wfreopen_s(&this->stdout_, L"CONOUT$", L"w", stdout);
            _wfreopen_s(&this->stderr_, L"CONOUT$", L"w", stderr);
        }
#endif
    }

    inline void detach_console() {
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
        if (this->stdin_ != nullptr) {
            fflush(this->stdin_);
            fclose(this->stdin_);
            this->stdin_ = nullptr;
        }

        if (this->stdout_ != nullptr) {
            fflush(this->stdout_);
            fclose(this->stdout_);
            this->stdout_ = nullptr;
        }

        if (this->stderr_ != nullptr) {
            fflush(this->stderr_);
            fclose(this->stderr_);
            this->stderr_ = nullptr;
        }

        if (static_cast<bool>(this->attached)) {
            FreeConsole();
            this->attached = false;
        }
#endif
    }
};

/*
 * Example usage in main.cpp:
 *
 * #if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
 * #include <windows.h>  // windows.h must precede all other windows headers
 *
 * #include "winconsole.hpp"
 *
 * _Use_decl_annotations_ INT WINAPI WinMain(
 *     [[maybe_unused]] HINSTANCE hInstance,
 *     [[maybe_unused]] HINSTANCE hPrevInstance,
 *     [[maybe_unused]] PSTR lpCmdLine,
 *     [[maybe_unused]] INT nCmdShow) {
 *     [[maybe_unused]] auto console = WinConsole::attach();
 *     return main(__argc, __argv);
 * }
 * #endif
 */

// NOLINTEND
// clang-format on
#endif // WINCON_3865628525_HPP
