#pragma once
#include <vector>
#include <sstream>
#include <algorithm>


template <typename T>
constexpr auto relativeToAbsolute(uintptr_t address) noexcept;
std::pair<void*, std::size_t> getModuleInformation(const char* name) noexcept;
[[nodiscard]] auto generateBadCharTable(std::string_view pattern) noexcept;

