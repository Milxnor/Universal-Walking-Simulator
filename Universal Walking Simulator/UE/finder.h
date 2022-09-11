#pragma once

#include <memcury.h>
#include <algorithm>
#include <string>
#include <format>

namespace Finder
{
    inline auto GetBytes(const uintptr_t Address, const int Count = 10)
    {
        std::string Bytes;

        for (int i = 0; i < Count; i++)
        {
            auto byte = *reinterpret_cast<uint8_t*>(Address + i) & 0xff;
            auto Byte = byte == 0 ? "? " : std::format("{:x} ", byte);
            
            if (Byte.length() == 2 && byte != 0)
            {
                Byte = "0" + Byte;
            }
           

            Bytes += Byte;
        }

        std::ranges::transform(Bytes, Bytes.begin(), toupper);

        return Bytes;
    }

    inline auto FindBytes(Memcury::Scanner& Scanner, const std::vector<uint8_t>& Bytes, const int Count = 255, const int SkipBytes = 0,
                          const bool BGoUp = false, int Skip = 0)
    {
        for (int i = 0 + SkipBytes; i < Count + SkipBytes; i++)
        {
            if (const auto CurrentByte = *reinterpret_cast<Memcury::ASM::MNEMONIC*>(BGoUp ? Scanner.Get() - i : Scanner.Get() + i); CurrentByte == Bytes[0])
            {
                bool Found = true;
                for (int j = 1; j < Bytes.size(); j++)
                {
                    if (*reinterpret_cast<Memcury::ASM::MNEMONIC*>(BGoUp ? Scanner.Get() - i + j : Scanner.Get() + i + j) != Bytes[j])
                    {
                        Found = false;
                        break;
                    }
                }
                if (Found)
                {
                    if (Skip > 0)
                    {
                        Skip--;
                        continue;
                    }

                    return BGoUp ? Scanner.Get() - i : Scanner.Get() + i;
                }
            }
        }

        return -1; 
    }

    inline auto GetBeaconNotifyControlMessage(const int Length = 35, uintptr_t* Address = nullptr)
    {
        if (auto Ref = Memcury::Scanner::FindStringRef(L"%s[%s] Host received: %s"); Ref.Get())
        {
            const std::vector<uint8_t> BytesToFind = {0x4C, 0x8B, 0xDC};
            if (const auto Beginning = FindBytes(Ref, BytesToFind, 600, 10, true))
            {
                if (Address)
                {
                    *Address = Beginning; 
                }

                return GetBytes(Beginning, Length);
            }
        }

        return "";
    }

    inline auto GetTickFlush(const int Length = 35, uintptr_t* Address = nullptr)
    {
        if (auto Ref = Memcury::Scanner::FindStringRef(L"STAT_NetTickFlush"); Ref.Get())
        {
            const std::vector<uint8_t> BytesToFind = {0x4C, 0x8B, 0xDC};
            if (const auto Beginning = FindBytes(Ref, BytesToFind, 600, 0, true))
            {
                if (Address)
                    *Address = Beginning;

                return GetBytes(Beginning, Length);
            }
        }

        return "";
    }

    inline auto GetStaticLoadObject(const int Length = 35, uintptr_t* Address = nullptr)
    {
        if (auto Ref = Memcury::Scanner::FindStringRef(L"Failed to find object '{ClassName} {OuterName}.{ObjectName}'"); Ref.Get())
        {
            const std::vector<uint8_t> BytesToFind = {0x40, 0x55};

            if (const auto Start = FindBytes(Ref, BytesToFind, 3000, 10, true))
            {
                if (Address)
                    *Address = Start;

                return GetBytes(Start, Length);
            }
        }

        return "";
    }
}
