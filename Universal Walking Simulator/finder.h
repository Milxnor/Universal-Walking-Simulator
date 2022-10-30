#include <memcury.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <format>

std::string ToByteStr(uintptr_t Addr)
{
	return std::to_string(*(uint8_t*)Addr);
}

int HexToDec(std::string hexValue)
{
	int len = hexValue.size();
	// Initializing base value to 1, i.e 16 ^ 0
	int base = 1;
	int dec_value = 0;
	// extracting characters as digits from last
	// character
	for (int i = len - 1; i >= 0; i--) {
		if (hexValue[i] >= '0' && hexValue[i] <= '9') {
			dec_value += (int(hexValue[i]) - 48) * base;


			// incrementing base by power
			base = base * 16;
		}
		// if character lies in 'A' - 'F' , converting
		// it to integral 10 - 15 by subtracting 55
		// from ASCII value
		else if (hexValue[i] >= 'A' && hexValue[i] <= 'F') {
			dec_value += (int(hexValue[i]) - 55) * base;


			// incrementing base by power
			base = base * 16;
		}
	}
	return dec_value;
}

// TODO: Make a function that finds all references of a function and then we can keep adding bytes to the pattern until there is 1 reference and that is the address.

bool IsBestSignature(const std::string& Pattern, uintptr_t Address)
{
	return false;
}

std::string GetBytes(uintptr_t Address, int count = 10) { // generates a sig
	std::string Bytes;

	for (int i = 0; i < count; i++) {
		auto byte = *(uint8_t*)(Address + i) & 0xff;
		auto Byte = (byte == 0) ? "? " : std::format("{:x} ", byte);

		if (Byte.length() == 2 && byte != 0) // 2 because of the space
			Byte = "0" + Byte;

		Bytes += Byte;
	}

	std::transform(Bytes.begin(), Bytes.end(), Bytes.begin(), ::toupper);

	return Bytes;
}

uintptr_t FindBytes(Memcury::Scanner& Scanner, std::vector<uint8_t>& Bytes, int Count = 255, int SkipBytes = 0, bool bGoUp = false, int Skip = 0, const bool bPrint = false)
{
	for (int i = 0 + SkipBytes; i < Count + SkipBytes; i++) // we should subtract from skip if goup
	{
		auto CurrentByte = *(Memcury::ASM::MNEMONIC*)(bGoUp ? Scanner.Get() - i : Scanner.Get() + i);

		if (bPrint)
			std::cout << "CurrentByte: " << std::hex << (int)CurrentByte << '\n';

		if (CurrentByte == Bytes[0])
		{
			bool Found = true;
			for (int j = 1; j < Bytes.size(); j++)
			{
				if (*(Memcury::ASM::MNEMONIC*)(bGoUp ? Scanner.Get() - i + j : Scanner.Get() + i + j) != Bytes[j])
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

				return bGoUp ? Scanner.Get() - i : Scanner.Get() + i;
			}
		}

		// std::cout << std::format("CurrentByte: 0x{:x}\n", (uint8_t)CurrentByte);
	}

	return -1;// Scanner.Get();
}

namespace Finder
{
	namespace Functions
	{
		std::string GetBeaconNotifyControlMessage(int length = 35, uintptr_t* Addr = nullptr)
		{
			auto Ref = Memcury::Scanner::FindStringRef(L"%s[%s] Host received: %s");

			if (Ref.Get())
			{
				std::vector<uint8_t> BytesToFind = { 0x4C, 0x8B, 0xDC };

				auto Beginning = FindBytes(Ref, BytesToFind, 600, 10, true);

				if (Beginning)
				{
					if (Addr)
						*Addr = Beginning;

					return GetBytes(Beginning, length);
				}
				else
					std::cout << "Unable to find Beginning of NCM_Beacon";
			}

			return "";
		}

		std::string GetTickFlush(int length = 35, uintptr_t* Addr = nullptr)
		{
			auto Ref = Memcury::Scanner::FindStringRef(L"STAT_NetTickFlush");

			if (Ref.Get())
			{
				std::vector<uint8_t> BytesToFind = { 0x4C, 0x8B, 0xDC };

				auto Beginning = FindBytes(Ref, BytesToFind, 600, 0, true);

				if (Beginning)
				{
					if (Addr)
						*Addr = Beginning;

					return GetBytes(Beginning, length);
				}
				else
					std::cout << "Unable to find Beginning of TickFlush";
			}

			return "";
		}

		std::string GetStaticLoadObject(int length = 35, uintptr_t* Addr = nullptr)
		{
			auto Ref = Memcury::Scanner::FindStringRef(L"Failed to find object '{ClassName} {OuterName}.{ObjectName}'"); // it doesnt like the closer pattern

			if (Ref.Get())
			{
				std::vector<uint8_t> BytesToFind = { 0x40, 0x55 };

				auto Start = FindBytes(Ref, BytesToFind, 3000, 10, true);

				if (Start)
				{
					if (Addr)
						*Addr = Start;

					return GetBytes(Start, length);
				}
				else
					std::cout << "Couldn't find start!\n";
			}
			else
				std::cout << "Unable to find StaticLoadObject string!\n";

			return "";
		}
	}
}

int GetVirtualIdx(UFunction* Func)
{

}