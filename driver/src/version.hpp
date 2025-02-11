#pragma once

// clang-format off
// NOLINTBEGIN

#include <string>

namespace program_info {
[[nodiscard]] [[maybe_unused]] auto name() -> const std::string&;
[[nodiscard]] [[maybe_unused]] auto file_path_component()
        -> const std::string&;
[[nodiscard]] [[maybe_unused]] auto version() -> const std::string&;
[[nodiscard]] [[maybe_unused]] auto version_major() -> int;
[[nodiscard]] [[maybe_unused]] auto version_minor() -> int;
[[nodiscard]] [[maybe_unused]] auto version_patch() -> int;
[[nodiscard]] [[maybe_unused]] auto version_tweak() -> int;
} // namespace program_info

// NOLINTEND
// clang-format on
