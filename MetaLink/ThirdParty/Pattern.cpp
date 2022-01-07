#include "../BaseIncludes.h"
#include "Pattern.h"



template <typename T>
constexpr auto relativeToAbsolute(uintptr_t address) noexcept
{
    return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}

std::pair<void*, std::size_t> getModuleInformation(const char* name) noexcept
{

    if (HMODULE handle = GetModuleHandleA(name)) {
        MODULEINFO moduleInfo;
        if (GetModuleInformation(GetCurrentProcess(), handle, &moduleInfo, sizeof(moduleInfo))) {
            return std::make_pair(moduleInfo.lpBaseOfDll, moduleInfo.SizeOfImage);
        }
    }
    return {};
}

[[nodiscard]] auto generateBadCharTable(std::string_view pattern) noexcept
{
    std::array<std::size_t, (std::numeric_limits<std::uint8_t>::max)() + 1> table;

    auto lastWildcard = pattern.rfind('?');
    if (lastWildcard == std::string_view::npos)
        lastWildcard = 0;

    const auto defaultShift = (std::max)(std::size_t(1), pattern.length() - 1 - lastWildcard);
    table.fill(defaultShift);

    for (auto i = lastWildcard; i < pattern.length() - 1; ++i)
        table[static_cast<std::uint8_t>(pattern[i])] = pattern.length() - 1 - i;

    return table;
}

static std::uintptr_t GetCallOffset(DWORD offset)
{
    auto call = *(std::uintptr_t*)((std::uintptr_t)offset + 1);
    return (std::uintptr_t)((std::uintptr_t)offset + call + 5);
}

