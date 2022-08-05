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

