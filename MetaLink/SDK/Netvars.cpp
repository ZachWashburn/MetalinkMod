#include <cctype>
#include <cstdint>
#include <string>
#include <unordered_map>
#include "Netvars.h"
//#include "../ThirdParty/Hash.h"
#include "../BaseIncludes.h"
#include "IBaseClientDLL.h"
#include "Interfaces.h"
#include "Recv.h"
#include "ClientClass.h"




static std::unordered_map<uint32_t, recvProxy> proxies;

Netvars::Netvars() noexcept
{
    for (auto clientClass = g_pInterfaces->m_pClient->GetAllClasses(); clientClass; clientClass = clientClass->next)
        walkTable(false, clientClass->networkName, clientClass->recvTable);
}

void Netvars::restore() noexcept
{
    for (auto clientClass = g_pInterfaces->m_pClient->GetAllClasses(); clientClass; clientClass = clientClass->next)
        walkTable(true, clientClass->networkName, clientClass->recvTable);

    proxies.clear();
    offsets.clear();
}

void Netvars::walkTable(bool unload, const char* networkName, RecvTable* recvTable, const std::size_t offset) noexcept
{
    for (int i = 0; i < recvTable->propCount; ++i) {
        auto& prop = recvTable->props[i];

        if (isdigit(prop.name[0]))
            continue;

        if (fnv::hashRuntime(prop.name) == fnv::hash("baseclass"))
            continue;

        if (prop.type == 6
            && prop.dataTable
            && prop.dataTable->netTableName[0] == 'D')
            walkTable(unload, networkName, prop.dataTable, prop.offset + offset);

        const auto hash{ fnv::hashRuntime((networkName + std::string{ "->" } + prop.name).c_str()) };

        constexpr auto getHook{ [](uint32_t hash) noexcept -> recvProxy {
             switch (hash) {
             default:
                 return nullptr;
             }
        } };

        if (!unload) {
            offsets[hash] = uint16_t(offset + prop.offset);

            constexpr auto hookProperty{ [](uint32_t hash, recvProxy& originalProxy, recvProxy proxy) noexcept {
                if (originalProxy != proxy) {
                    proxies[hash] = originalProxy;
                    originalProxy = proxy;
                }
            } };

            if (auto hook{ getHook(hash) })
                hookProperty(hash, prop.proxy, hook);
        }
        else {
            constexpr auto unhookProperty{ [](recvProxy& proxy, uint32_t hash) noexcept {
                proxy = proxies[hash];
            } };

            if (auto hook{ getHook(hash) })
                unhookProperty(prop.proxy, hash);
        }
    }
}
