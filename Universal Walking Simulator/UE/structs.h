#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>
#include <locale>
#include <format>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <thread>

#include "other.h"
#include <regex>

using namespace std::chrono;

#define INL __forceinline

static inline void (*ToStringO)(struct FName*, class FString&);
static inline void* (*ProcessEventO)(void*, void*, void*);

std::string FN_Version = "0.0";
static double FnVerDouble = 0;
int Engine_Version;

static struct FChunkedFixedUObjectArray* ObjObjects;
static struct FFixedUObjectArray* OldObjects;

namespace FMemory
{
	void (*Free)(void* Original);
	void* (*Realloc)(void* Original, SIZE_T Count, uint32_t Alignment /* = DEFAULT_ALIGNMENT */);
}

struct Timer
{
	time_point<std::chrono::steady_clock> start, end;
	duration<float> dura;

	Timer()
	{
		start = high_resolution_clock::now();
	}

	~Timer()
	{
		end = high_resolution_clock::now();
		dura = end - start;

		float ns = dura.count() * 10000.0f;
		std::cout << ("Took ") << ns << ("ns \n");
	}

	// You would do "Timer* t = new Timer;" and then delete at the end of the function.
};

template <class ElementType>
class TArray // https://github.com/EpicGames/UnrealEngine/blob/4.21/Engine/Source/Runtime/Core/Public/Containers/Array.h#L305
{
	friend class FString;
protected:
	ElementType* Data = nullptr;
	int32_t ArrayNum = 0;
	int32_t ArrayMax = 0;
public:
	void Free()
	{
		if (FMemory::Free)
			FMemory::Free(Data);

		Data = nullptr;

		ArrayNum = 0;
		ArrayMax = 0;
	}

	INL auto Num() const { return ArrayNum; }

	INL ElementType& operator[](int Index) const { return Data[Index]; }

	INL ElementType& At(int Index) const
	{
		return Data[Index];
	}

	bool IsValid() const
	{
		return Data;
	}

	INL int32_t Slack() const
	{
		return ArrayMax - ArrayNum;
	}

	void Reserve(int Number, int Size = sizeof(ElementType))
	{
		if (!FMemory::Realloc)
		{
			MessageBoxA(0, ("How are you expecting to reserve with no Realloc?"), ("Project Reboot"), MB_ICONERROR);
			return;
		}

		// if (Number > ArrayMax)
		{
			// Data = (ElementType*)realloc(Data, Size * (ArrayNum + 1));
			// Data = Slack() >= Number ? Data : (ElementType*)FMemory::Realloc(Data, (ArrayMax = ArrayNum + Number) * Size, 0);
			// Data = (ElementType*)realloc(Data, sizeof(ElementType) * (ArrayNum + 1));
			Data = (ElementType*)FMemory::Realloc(Data, (ArrayMax = ArrayNum + Number) * Size, 0);
		}
	}

	int Add(const ElementType& New, int Size = sizeof(ElementType))
	{
		Reserve(1, Size);

		if (Data)
		{
			memcpy_s((ElementType*)(__int64(Data) + (ArrayNum * Size)), Size, (void*)&New, Size);
			// Data[ArrayNum] = New;
			++ArrayNum;
			++ArrayMax;
			return ArrayNum; // - 1;
		}

		std::cout << ("Invalid Data when adding!\n");

		/*

		if (Data)
		{
			Data = (ElementType*)realloc(Data, sizeof(ElementType) * (ArrayNum + 1));
			Data[ArrayNum] = New;
			++ArrayNum;
			return ArrayNum - 1;
		}

		*/

		return -1;
	};


	void RemoveAtSwapImpl(int Index, int Count = 1, bool bAllowShrinking = true)
	{
		if (Count)
		{
			// CheckInvariants();
			// checkSlow((Count >= 0) & (Index >= 0) & (Index + Count <= ArrayNum));

			// DestructItems(GetData() + Index, Count);

			// Replace the elements in the hole created by the removal with elements from the end of the array, so the range of indices used by the array is contiguous.
			const int NumElementsInHole = Count;
			const int NumElementsAfterHole = ArrayNum - (Index + Count);
			const int NumElementsToMoveIntoHole = min(NumElementsInHole, NumElementsAfterHole);
			if (NumElementsToMoveIntoHole)
			{
				memcpy(// FMemory::Memcpy(
					(uint8_t*)Data + (Index) * sizeof(ElementType),
					(uint8_t*)Data + (ArrayNum - NumElementsToMoveIntoHole) * sizeof(ElementType),
					NumElementsToMoveIntoHole * sizeof(ElementType)
				);
			}
			ArrayNum -= Count;

			if (bAllowShrinking)
			{
				// ResizeShrink();
			}
		}
	}

	inline void RemoveAtSwap(int Index)
	{
		RemoveAtSwapImpl(Index, 1, true);
	}

	inline bool RemoveAt(const int Index) // , int Size = sizeof(ElementType)) // NOT MINE
	{
		if (Index < ArrayNum)
		{
			if (Index != ArrayNum - 1)
			{
				// memcpy_s((ElementType*)(__int64(Data) + (Index * Size)), Size, (ElementType*)(__int64(Data) + ((ArrayNum - 1) * Size)), Size);
				Data[Index] = Data[ArrayNum - 1];
			}

			--ArrayNum;

			return true;
		}

		return false;
	};

	INL auto GetData() const { return Data; }
};

class FString // https://github.com/EpicGames/UnrealEngine/blob/4.21/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h#L59
{
public:
	TArray<TCHAR> Data;

	void Set(const wchar_t* NewStr) // by fischsalat
	{
		if (!NewStr || std::wcslen(NewStr) == 0) return;

		Data.ArrayMax = Data.ArrayNum = *NewStr ? (int)std::wcslen(NewStr) + 1 : 0;

		if (Data.ArrayNum)
			Data.Data = const_cast<wchar_t*>(NewStr);
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data.Data);
		std::string str(length, '\0');
		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data.Data, Data.Data + length, '?', &str[0]);

		return str;
	}

	INL void operator=(const std::string& str) { Set(std::wstring(str.begin(), str.end()).c_str()); }

	void FreeString()
	{
		Data.Free();
	}

	FString()
	{
		// const wchar_t* Lmaoo = new wchar_t[1000]();
		// Set(Lmaoo);
	}

	~FString()
	{
		// FreeString();
	}

	/*

	template <typename StrType>
	FORCEINLINE FString& operator+=(StrType&& Str)	{ return Append(Forward<StrType>(Str)); }

	FORCEINLINE FString& operator+=(ANSICHAR Char) { return AppendChar(Char); }
	FORCEINLINE FString& operator+=(WIDECHAR Char) { return AppendChar(Char); }
	FORCEINLINE FString& operator+=(UCS2CHAR Char) { return AppendChar(Char); }

	*/

	// Ill add setting and stuff soon, check out argon's FString if you need.
};

#define NAME_NO_NUMBER_INTERNAL 0
#define NAME_None 0

struct FNameEntryId(*FromValidEName)(EName Ename);

struct FNameEntryId
{
	FORCEINLINE static FNameEntryId FromEName(EName Ename)
	{
		return Ename == NAME_None ? FNameEntryId() : FromValidEName(Ename);
	}

	int32_t Value;
};

struct FName // https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/Core/Public/UObject/NameTypes.h#L403
{
	uint32_t ComparisonIndex;
	uint32_t Number = NAME_NO_NUMBER_INTERNAL;

	INL std::string ToString()
	{
		if (!this)
			return "";

		FString temp;

		ToStringO(this, temp);

		auto Str = temp.ToString();

		temp.FreeString();

		return Str;
	}

	INL std::wstring ToSFtring()
	{
		if (!this)
			return L"";

		FString temp;

		ToStringO(this, temp);

		auto Str = std::wstring(temp.Data.GetData());

		temp.FreeString();

		return Str;
	}

	bool operator==(const FName& other)
	{
		return (other.ComparisonIndex == this->ComparisonIndex);
	}

	FORCEINLINE FName(EName Ename) : FName(Ename, NAME_NO_NUMBER_INTERNAL) {}

	FORCEINLINE FName(EName Ename, int32_t InNumber)
		: ComparisonIndex(FNameEntryId::FromEName(Ename).Value)
		, Number(InNumber)
	{
	}

	FName() {}
	FName(int _ComparisonIndex) : ComparisonIndex(_ComparisonIndex) {}
};

template <typename Fn>
inline Fn GetVFunction(const void* instance, std::size_t index)
{
	auto vtable = *reinterpret_cast<const void***>(const_cast<void*>(instance));
	return reinterpret_cast<Fn>(vtable[index]);
}

struct light
{
	int finalSize = 0;
	std::vector<int> Offsets;
	void* addr = nullptr;
	int onOffset = 0;

	template <typename T> void set(const T& t) { // This function gets called for the last argument.
		auto OffsetToUse = onOffset == 0 ? 0 : Offsets.at(onOffset);
		*(T*)(__int64(addr) + OffsetToUse) = t;
		onOffset++;
	}

	template <typename First, typename... Rest> void set(const First& first, const Rest&... rest) {
		if (addr)
		{
			auto OffsetToUse = onOffset == 0 ? 0 : Offsets.at(onOffset);
			std::cout << std::format("Setting {} at offset: {}", first, OffsetToUse) // << " with currentRead as: " << currentRead << " Setting to: " << __int64(&*(First*)(__int64(addr) + (currentRead == 0 ? 0 : currentRead))) << '\n';
				* (First*)(__int64(addr) + OffsetToUse) = first;
			onOffset++;
			// std::cout << "\nPadding: " << (sizeof(First) > sizeOfLastType ? sizeof(First) - sizeOfLastType : sizeOfLastType - sizeof(First)) << "\n\n";
			// sizeOfLastType = sizeof(First);
			set(rest...); // recursive call using pack expansion 
		}
	}

	template <typename First, typename... Rest> void execute(const std::string& funcName, const First& first, const Rest&... rest) {
		addr = malloc(finalSize);

		set(first, rest...);

		return;
	}

	~light()
	{
		if (addr)
			free(addr);
	}
};

struct UObject;

struct UObject // https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/CoreUObject/Public/UObject/UObjectBase.h#L20
{
	void** VFTable;
	int32_t ObjectFlags;
	int32_t InternalIndex;
	UObject* ClassPrivate; // Keep it an object because the we will have to cast it to the correct type depending on the version.
	FName NamePrivate;
	UObject* OuterPrivate;

	INL std::string GetName() { return NamePrivate.ToString(); }
	INL std::wstring GetNFame() { return NamePrivate.ToSFtring(); }

	INL std::string GetFullName()
	{
		std::string temp;

		for (auto outer = OuterPrivate; outer; outer = outer->OuterPrivate)
			temp = std::format("{}.{}", outer->GetName(), temp);

		return std::format("{} {}{}", ClassPrivate->GetName(), temp, this->GetName());
	}

	INL std::string GetFullNameWOCP()
	{
		std::string temp;

		for (auto outer = OuterPrivate; outer; outer = outer->OuterPrivate)
			temp = std::format("{}.{}", outer->GetName(), temp);

		return std::format("{}{}", temp, this->GetName());
	}

	INL std::wstring GetFullNFame()
	{
		std::wstring temp;

		for (auto outer = OuterPrivate; outer; outer = outer->OuterPrivate)
			temp = outer->GetNFame() + L"." + temp;

		return ClassPrivate->GetNFame() + L" " + temp + this->GetNFame();
	}

	INL std::string GetFullNameT()
	{
		std::string temp;

		for (auto outer = OuterPrivate; outer; outer = outer->OuterPrivate)
			temp = std::format("{}.{}", outer->GetName(), temp);

		return temp + this->GetName();
	}

	bool IsA(UObject* cmp) const;

	INL struct UFunction* Function(const std::string& FuncName);

	INL auto ProcessEvent(UObject* Function, void* Params = nullptr)
	{
		return ProcessEventO(this, Function, Params);
	}

	INL void* ProcessEvent(const std::string& FuncName, void* Params = nullptr)
	{
		auto fn = this->Function(FuncName); // static?

		if (!fn)
		{
			std::cout << ("[ERROR] Unable to find ") << FuncName << '\n';
			return nullptr;
		}

		return ProcessEvent((UObject*)fn, Params);
	}

	UObject* CreateDefaultObject()
	{
		static auto Index = 0;

		if (Index == 0)
		{
			if (Engine_Version < 421)
				Index = 101;
			else if (Engine_Version >= 420 && FnVerDouble < 7.40)
				Index = 102;
			else if (FnVerDouble >= 7.40)
				Index = 103;
			else if (Engine_Version == 424)
				Index = 106; // got on 11.01
			else if (Engine_Version == 425)
				Index = 114;
			else
				std::cout << ("Unable to determine CreateDefaultObject Index!\n");
		}

		if (Index != 0)
		{
			if (Engine_Version < 424 || Engine_Version == 425)
				return GetVFunction<UObject* (*)(UObject*)>(this, Index)(this);
			else
			{
				// return GetVFunction<UObject* (*)(UObject*, __int64)>(this, Index)(this, 69);
			}
		}
		else
			std::cout << ("Unable to create default object because Index is 0!\n");
		return nullptr;
	}

	// TODO: add return types via a template
	// DO NOT USE, VERY VERY VERY unsafe
	template <typename strType = std::string, typename First, typename... Rest> void Exec(const strType& FunctionName, const First& first, const Rest&... rest) {
		static auto fn = this->Function(FunctionName);

		if (fn)
		{
			auto Params = light();
			Params.finalSize = fn->GetParmsSize();
			Params.Offsets = fn->GetAllParamOffsets();

			Params.execute(FunctionName, first, rest...);

			if (Params.addr)
				this->ProcessEvent(fn, Params.addr);
		}
	}

	// protected:
	template <typename MemberType = UObject*>
	INL MemberType* Member(const std::string& MemberName); // DONT USE FOR SCRIPTSTRUCTS

	template <typename MemberType>
	INL MemberType* CachedMember(const std::string& MemberName);

	// ONLY USE IF YOU KNOW WHAT UR DOING
	template <typename MemberType>
	INL MemberType* FastMember(const std::string& MemberName); // DONT USE FOR SCRIPTSTRUCTS
};



struct UFunction : UObject
{
	template <typename T>
	T* GetParam(const std::string& ParameterName, void* Params);

	std::vector<int> GetAllParamOffsets();

	unsigned short GetParmsSize();

	void* GetFunc();
};


struct FUObjectItem // https://github.com/EpicGames/UnrealEngine/blob/4.27/Engine/Source/Runtime/CoreUObject/Public/UObject/UObjectArray.h#L26
{
	UObject* Object;
	int32_t Flags;
	int32_t ClusterRootIndex;
	int32_t SerialNumber;
	// int pad_01;
};

struct FFixedUObjectArray
{
	FUObjectItem* Objects;
	int32_t MaxElements;
	int32_t NumElements;

	INL const int32_t Num() const { return NumElements; }

	INL const int32_t Capacity() const { return MaxElements; }

	INL bool IsValidIndex(int32_t Index) const { return Index < Num() && Index >= 0; }

	INL UObject* GetObjectById(int32_t Index) const
	{
		return Objects[Index].Object;
	}

	INL FUObjectItem* GetItemById(int32_t Index) const
	{
		return &Objects[Index];
	}
};

struct FChunkedFixedUObjectArray // https://github.com/EpicGames/UnrealEngine/blob/7acbae1c8d1736bb5a0da4f6ed21ccb237bc8851/Engine/Source/Runtime/CoreUObject/Public/UObject/UObjectArray.h#L321
{
	enum
	{
		NumElementsPerChunk = 64 * 1024,
	};

	FUObjectItem** Objects;
	FUObjectItem* PreAllocatedObjects;
	int32_t MaxElements;
	int32_t NumElements;
	int32_t MaxChunks;
	int32_t NumChunks;

	INL const int32_t Num() const { return NumElements; }

	INL const int32_t Capacity() const { return MaxElements; }

	INL UObject* GetObjectById(int32_t Index) const
	{
		if (Index > NumElements || Index < 0) return nullptr;

		const int32_t ChunkIndex = Index / NumElementsPerChunk;
		const int32_t WithinChunkIndex = Index % NumElementsPerChunk;

		if (ChunkIndex > NumChunks) return nullptr;
		FUObjectItem* Chunk = Objects[ChunkIndex];
		if (!Chunk) return nullptr;

		auto obj = (Chunk + WithinChunkIndex)->Object;

		return obj;
	}

	INL FUObjectItem* GetItemById(int32_t Index) const
	{
		if (Index > NumElements || Index < 0) return nullptr;

		const int32_t ChunkIndex = Index / NumElementsPerChunk;
		const int32_t WithinChunkIndex = Index % NumElementsPerChunk;

		if (ChunkIndex > NumChunks) return nullptr;
		FUObjectItem* Chunk = Objects[ChunkIndex];
		if (!Chunk) return nullptr;

		auto obj = (Chunk + WithinChunkIndex);

		return obj;
	}
};

static UObject* (*StaticFindObjectO)(
	UObject* Class,
	UObject* InOuter,
	const TCHAR* Name,
	bool ExactClass);

static UObject* (*StaticLoadObjectO)(
	UObject* Class, // UClass*
	UObject* InOuter,
	const TCHAR* Name,
	const TCHAR* Filename,
	uint32_t LoadFlags,
	UObject* Sandbox, // UPackageMap*
	bool bAllowObjectReconciliation,
	void* InSerializeContext // FUObjectSerializeContext
	);

// Class and name are required
template <typename T = UObject>
static T* StaticLoadObject(UObject* Class, UObject* Outer, const std::string& name, int LoadFlags = 0)
{
	if (!StaticLoadObjectO)
		return nullptr;

	auto Name = std::wstring(name.begin(), name.end()).c_str();
	return (T*)StaticLoadObjectO(Class, Outer, Name, nullptr, LoadFlags, nullptr, false, nullptr);
}

template <typename ReturnType = UObject>
static ReturnType* LoadObject(UObject* Class, UObject* Outer, const std::string& name)
{
	return StaticLoadObject<ReturnType>(Class, Outer, name.substr(name.find(" ") + 1));
}

template <typename ReturnType = UObject>
static ReturnType* StaticFindObject(const std::string& str)
{
	auto Name = std::wstring(str.begin(), str.end()).c_str();
	return (ReturnType*)StaticFindObjectO(nullptr, nullptr, Name, false);
}

template <typename ReturnType = UObject>
static ReturnType* GetByIndex(int Index)
{
	return (ReturnType*)(ObjObjects ? ObjObjects->GetObjectById(Index) : OldObjects->GetObjectById(Index));
}

template <typename ReturnType = UObject>
static ReturnType* FindObjectOld(const std::string& str, bool bIsEqual = false, bool bIsName = false)
{
	if (bIsName) bIsEqual = true;

	for (int32_t i = 0; i < (ObjObjects ? ObjObjects->Num() : OldObjects->Num()); i++)
	{
		auto Object = ObjObjects ? ObjObjects->GetObjectById(i) : OldObjects->GetObjectById(i);

		if (!Object) continue;

		auto ObjectName = bIsName ? Object->GetName() : Object->GetFullName();

		// cant we do like if ((bIsEqual) ? ObjectName == str : ObjectName.contains(str))
		if (bIsEqual)
		{
			if (ObjectName == str)
				return (ReturnType*)Object;
		}
		else
		{
			if (ObjectName.contains(str))
				return (ReturnType*)Object;
		}
	}

	return nullptr;
}

template <typename ReturnType = UObject>
static ReturnType* FindObject(const std::string& str, bool bIsEqual = false, bool bIsName = false, bool bDoNotUseStaticFindObject = false, bool bSkipIfSFOFails = true)
{
	if (StaticFindObjectO && !bDoNotUseStaticFindObject)
	{
		auto Object = StaticFindObject<ReturnType>(str.substr(str.find(" ") + 1));
		if (Object)
		{
			// std::cout << ("Found SFO!\n");
			return Object;
		}
		// std::cout << ("[WARNING] Failed to find object with SFO named: ") << str << " (if you're game doesn't crash soon, it means it's fine)\n";

		if (bSkipIfSFOFails)
			return nullptr;
	}

	return FindObjectOld<ReturnType>(str, bIsEqual, bIsName);
}

// Here comes the version changing and makes me want to die I need to find a better way to do this

struct UField : UObject
{
	UField* Next;
};

struct UFieldPadding : UObject
{
	UField* Next;
	void* pad_01;
	void* pad_02;
};

struct UProperty_UE : public UField // Default UProperty for UE, >4.20.
{
	int32_t ArrayDim;
	int32_t ElementSize;
	uint64_t PropertyFlags;
	uint16_t RepIndex;
	TEnumAsByte<ELifetimeCondition> BlueprintReplicationCondition;
	int32_t Offset_Internal;
	FName RepNotifyFunc;
	UProperty_UE* PropertyLinkNext;
	UProperty_UE* NextRef;
	UProperty_UE* DestructorLinkNext;
	UProperty_UE* PostConstructLinkNext;
};

struct UStruct_FT : public UField // >4.20
{
	UStruct_FT* SuperStruct;
	UField* ChildProperties; // Children
	int32_t PropertiesSize;
	int32_t MinAlignment;
	TArray<uint8_t> Script;
	UProperty_UE* PropertyLink;
	UProperty_UE* RefLink;
	UProperty_UE* DestructorLink;
	UProperty_UE* PostConstructLink;
	TArray<UObject*> ScriptObjectReferences;
};

struct UProperty_FTO : UField
{
	uint32_t ArrayDim; // 0x30
	uint32_t ElementSize; // 0x34
	uint64_t PropertyFlags; // 0x38
	char pad_40[4]; // 0x40
	uint32_t Offset_Internal; // 0x44
	char pad_48[0x70 - 0x48];
};

struct UStruct_FTO : public UField // 4.21 only
{
	UStruct_FTO* SuperStruct;
	UField* ChildProperties; // Children
	int32_t PropertiesSize;
	int32_t MinAlignment;
	UProperty_FTO* PropertyLink;
	UProperty_FTO* RefLink;
	UProperty_FTO* DestructorLink;
	UProperty_FTO* PostConstructLink;
	TArray<UObject*> ScriptObjectReferences;
};

struct UStruct_FTT : UField // 4.22-4.24
{
	void* Pad;
	void* Pad2;
	UStruct_FTT* SuperStruct; // 0x30
	UField* ChildProperties; // 0x38
	uint32_t PropertiesSize; // 0x40
	char pad_44[0x88 - 0x30 - 0x14];
};

struct FField
{
	void** VFT;
	void* ClassPrivate;
	void* Owner;
	void* pad;
	FField* Next;
	FName NamePrivate;
	EObjectFlags FlagsPrivate;

	std::string GetName()
	{
		return NamePrivate.ToString();
	}
};

struct FProperty : public FField
{
	int32_t	ArrayDim;
	int32_t	ElementSize;
	EPropertyFlags PropertyFlags;
	uint16_t RepIndex;
	TEnumAsByte<ELifetimeCondition> BlueprintReplicationCondition;
	int32_t	Offset_Internal;
	FName RepNotifyFunc;
	FProperty* PropertyLinkNext;
	FProperty* NextRef;
	FProperty* DestructorLinkNext;
	FProperty* PostConstructLinkNext;
};

class UStruct_CT : public UFieldPadding
{
public:
	UStruct_CT* SuperStruct;
	UFieldPadding* Children;
	FField* ChildProperties;
	int32_t PropertiesSize;
	int32_t MinAlignment;
	TArray<uint8_t> Script;
	FProperty* PropertyLink;
	FProperty* RefLink;
	FProperty* DestructorLink;
	FProperty* PostConstructLink;
	TArray<UObject*> ScriptAndPropertyObjectReferences;
};

struct UClass_FT : public UStruct_FT {}; // >4.20
struct UClass_FTO : public UStruct_FTO {}; // 4.21
struct UClass_FTT : public UStruct_FTT {}; // 4.22-4.24
struct UClass_CT : public UStruct_CT {}; // C2 to before C3

template <typename ClassType, typename PropertyType, typename ReturnValue = PropertyType>
auto GetMembers(UObject* Object)
{
	std::vector<ReturnValue*> Members;

	if (Object)
	{
		for (auto CurrentClass = (ClassType*)Object->ClassPrivate; CurrentClass; CurrentClass = (ClassType*)CurrentClass->SuperStruct)
		{
			auto Property = CurrentClass->ChildProperties;

			if (Property)
			{
				auto Next = Property->Next;

				if (Next)
				{
					while (Property)
					{
						Members.push_back((ReturnValue*)Property);

						Property = Property->Next;
					}
				}
			}

		}
	}

	return Members;
}

template <typename ClassType, typename PropertyType, typename ReturnValue = PropertyType>
auto GetMembersFProperty(UObject* Object, bool bOnlyMembers = false, bool bOnlyFunctions = false)
{
	std::vector<ReturnValue*> Members;

	if (Object)
	{
		for (auto CurrentClass = (ClassType*)Object->ClassPrivate; CurrentClass; CurrentClass = (ClassType*)CurrentClass->SuperStruct)
		{
			auto Property = CurrentClass->ChildProperties;
			auto Child = CurrentClass->Children;

			if ((!bOnlyFunctions && bOnlyMembers) || (!bOnlyFunctions && !bOnlyMembers)) // Only members
			{
				if (Property)
				{
					Members.push_back((ReturnValue*)Property);

					auto Next = Property->Next;

					if (Next)
					{
						while (Property)
						{
							Members.push_back((ReturnValue*)Property);

							Property = Property->Next;
						}
					}
				}
			}

			if ((!bOnlyMembers && bOnlyFunctions) || (!bOnlyMembers && !bOnlyFunctions)) // Only functions
			{
				if (Child)
				{
					Members.push_back((ReturnValue*)Child);

					auto Next = Child->Next;

					if (Next)
					{
						while (Child)
						{
							Members.push_back((ReturnValue*)Child);

							Child = decltype(Child)(Child->Next);
						}
					}
				}
			}

		}
	}

	return Members;
}

auto GetMembersAsObjects(UObject* Object, bool bOnlyMembers = false, bool bOnlyFunctions = false)
{
	std::vector<UObject*> Members;

	if (Engine_Version <= 420)
		Members = GetMembers<UClass_FT, UProperty_UE, UObject>(Object);

	else if (Engine_Version == 421) // && Engine_Version <= 424)
		Members = GetMembers<UClass_FTO, UProperty_FTO, UObject>(Object);

	else if (Engine_Version >= 422 && Engine_Version <= 424)
		Members = GetMembers<UClass_FTT, UProperty_FTO, UObject>(Object);

	else if (Engine_Version >= 425 && FnVerDouble < 20)
		Members = GetMembersFProperty<UClass_CT, FProperty, UObject>(Object, bOnlyMembers, bOnlyFunctions);

	else if (FnVerDouble >= 20)
		Members = GetMembersFProperty<UClass_CT, FProperty, UObject>(Object, bOnlyMembers, bOnlyFunctions);

	return Members;
}

std::vector<std::string> GetMemberNames(UObject* Object, bool bOnlyMembers = false, bool bOnlyFunctions = false)
{
	std::vector<std::string> Names;
	std::vector<UObject*> Members = GetMembersAsObjects(Object, bOnlyMembers, bOnlyFunctions);

	for (auto Member : Members)
	{
		if (Member)
			Names.push_back(Member->GetName());
	}

	return Names;
}

UFunction* FindFunction(const std::string& Name, UObject* Object) // might as well pass in object because what else u gon use a func for.
{
	for (auto Member : GetMembersAsObjects(Object, false, true))
	{
		if (Member && Member->GetName() == Name) // dont use IsA cuz slower
			return (UFunction*)Member;
	}

	return nullptr;
}

template <typename ClassType, typename PropertyType>
PropertyType* LoopMembersAndGetProperty(UObject* Object, const std::string& MemberName)
{
	// We loop through the whole class hierarchy to find the offset.

	// auto MemberFName = StringToName(MemberName);

	for (auto Member : GetMembers<ClassType, PropertyType>(Object))
	{
		if (Member)
		{
			if (Member->GetName() == MemberName)
				// if (Member->NamePrivate == MemberFName)
			{
				return ((PropertyType*)Member);
			}
		}
	}

	return 0;
}

template <typename ClassType, typename PropertyType>
int LoopMembersAndFindOffset(UObject* Object, const std::string& MemberName, int offset = 0)
{
	// We loop through the whole class hierarchy to find the offset.

	// auto MemberFName = StringToName(MemberName);
	PropertyType* Prop = LoopMembersAndGetProperty<ClassType, PropertyType>(Object, MemberName);
	if (Prop != nullptr) {
		if (offset)
			return *(int*)(__int64(Prop) + offset);
		else
			return Prop->Offset_Internal;
	}
	else {
		return -1;
	}
}

static void* GetProperty(UObject* Object, const std::string& MemberName)
{
	if (Object && !MemberName.contains((" ")))
	{
		if (Engine_Version <= 420)
			return LoopMembersAndGetProperty<UClass_FT, UProperty_UE>(Object, MemberName);

		else if (Engine_Version == 421) // && Engine_Version <= 424)
			return LoopMembersAndGetProperty<UClass_FTO, UProperty_FTO>(Object, MemberName);

		else if (Engine_Version >= 422 && Engine_Version <= 424)
			return LoopMembersAndGetProperty<UClass_FTT, UProperty_FTO>(Object, MemberName);

		else if (Engine_Version >= 425 && FnVerDouble < 20)
			return LoopMembersAndGetProperty<UClass_CT, FProperty>(Object, MemberName);

		else if (std::stod(FN_Version) >= 20)
			return LoopMembersAndGetProperty<UClass_CT, FProperty>(Object, MemberName);
	}
	else
	{
		std::cout << std::format(("Either invalid object or MemberName. MemberName {} Object {}"), MemberName, Object->GetFullName());
	}

	return nullptr;
}

static int GetOffset(UObject* Object, const std::string& MemberName)
{
	if (Object && !MemberName.contains((" ")))
	{
		if (Engine_Version <= 420)
			return LoopMembersAndFindOffset<UClass_FT, UProperty_UE>(Object, MemberName);

		else if (Engine_Version == 421) // && Engine_Version <= 424)
			return LoopMembersAndFindOffset<UClass_FTO, UProperty_FTO>(Object, MemberName);

		else if (Engine_Version >= 422 && Engine_Version <= 424)
			return LoopMembersAndFindOffset<UClass_FTT, UProperty_FTO>(Object, MemberName);

		else if (Engine_Version >= 425 && FnVerDouble < 20.00)
			return LoopMembersAndFindOffset<UClass_CT, FProperty>(Object, MemberName);

		else if (FnVerDouble >= 20) // s20 maybe
			return LoopMembersAndFindOffset<UClass_CT, FProperty>(Object, MemberName, 0x44);
	}
	else
	{
		// std::cout << std::format(("Either invalid object or MemberName. MemberName {} Object {}"), MemberName, __int64(Object));
	}

	return -1;
}

template <typename ClassType>
bool IsA_(const UObject* cmpto, UObject* cmp)
{
	if (!cmpto || !cmpto->ClassPrivate)
		return false;

	for (auto super = (ClassType*)cmpto->ClassPrivate; super; super = (ClassType*)super->SuperStruct)
	{
		if (super == cmp)
			return true;
	}

	return false;
}

bool UObject::IsA(UObject* cmp) const
{
	if (Engine_Version <= 420)
		return IsA_<UClass_FT>(this, cmp);

	else if (Engine_Version == 421) // && Engine_Version <= 424)
		return IsA_<UClass_FTO>(this, cmp);

	else if (Engine_Version >= 422 && Engine_Version < 425)
		return IsA_<UClass_FTT>(this, cmp);

	else if (Engine_Version >= 425)
		return IsA_<UClass_CT>(this, cmp);

	return false;
}

INL UFunction* UObject::Function(const std::string& FuncName)
{
	return FindFunction(FuncName, this);
}

FString(*GetEngineVersion)();

// TODO: There is this 1.9 function, 48 8D 05 D9 51 22 03. It has the CL and stuff. We may be able to determine the version using the CL.
// There is also a string for the engine version and fortnite version, I think it's for every version its like "engineversion=". I will look into it when I find time.

struct FActorSpawnParameters
{
	// char pad[0x40];
	FName Name;
	UObject* Template; // AActor*
	UObject* Owner; // AActor*
	UObject* Instigator; // APawn*
	UObject* OverrideLevel; // ULevel*
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride;
	uint16_t	bRemoteOwned : 1;
	uint16_t	bNoFail : 1;
	uint16_t	bDeferConstruction : 1;
	uint16_t	bAllowDuringConstructionScript : 1;
	EObjectFlags ObjectFlags;
};

static UObject* (*SpawnActorOTrans)(UObject* World, UObject* Class, FTransform* Transform, const FActorSpawnParameters& SpawnParameters);
static UObject* (*SpawnActorO)(UObject* World, UObject* Class, FVector* Position, FRotator* Rotation, const FActorSpawnParameters& SpawnParameters);

uint64_t ToStringAddr = 0;
uint64_t ProcessEventAddr = 0;
uint64_t ObjectsAddr = 0;
uint64_t FreeMemoryAddr = 0;
uint64_t SpawnActorAddr = 0;

static int ServerReplicateActorsOffset = 0x53; // UE4.20

bool Setup(/* void* ProcessEventHookAddr */)
{
	SpawnActorAddr = FindPattern(("40 53 56 57 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 0F 28 1D ? ? ? ? 0F 57 D2 48 8B B4 24 ? ? ? ? 0F 28 CB"));

	if (!SpawnActorAddr)
		SpawnActorAddr = FindPattern(("40 53 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 0F 28 1D ? ? ? ? 0F 57 D2 48 8B 9C 24 ? ? ? ? 0F 28 CB 0F 54 1D ? ? ? ? 0F 57"));

	if (!SpawnActorAddr)
		SpawnActorAddr = FindPattern("48 89 5C 24 ? 55 56 57 48 8B EC 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 F0 0F 28 05 ? ? ? ? 48 8B FA 0F 28 0D ? ? ? ? 48 8B D9 48 8B 75 40 0F 29 45 C0 0F 28 05 ? ? ? ? 0F 29 45 E0 0F 29 4D D0 4D 85 C0 74 12 F3 41 0F 10 50 ? F2 41 0F 10 18");

	if (!SpawnActorAddr)
	{
		std::cout << "[WARNING] Failed to find SpawnActor function!\n";
		// MessageBoxA(0, ("Failed to find SpawnActor function."), ("Universal Walking Simulator"), MB_OK);
		// return 0;
	}
	
	auto SpawnActorTransAddr = FindPattern("48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 A8 0F 29 78 98 44 0F 29 40 ? 44 0F 29 88 ? ? ? ? 44 0F 29 90 ? ? ? ? 44 0F 29 98 ? ? ? ? 44 0F 29 A0 ? ? ? ? 44 0F 29 A8 ? ? ? ? 44 0F 29 B0 ? ? ? ? 44 0F 29 B8 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 60 45 33 ED 48 89 4D 90 44 89 6D 80 48 8D 05 ? ? ? ? 44 38");

	SpawnActorOTrans = decltype(SpawnActorOTrans)(SpawnActorTransAddr);
	SpawnActorO = decltype(SpawnActorO)(SpawnActorAddr);

	bool bOldObjects = false;

	GetEngineVersion = decltype(GetEngineVersion)(FindPattern(("40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8B C8 41 B8 04 ? ? ? 48 8B D3")));

	std::string FullVersion;
	FString toFree;

	if (!GetEngineVersion)
	{
		auto VerStr = FindPattern(("2B 2B 46 6F 72 74 6E 69 74 65 2B 52 65 6C 65 61 73 65 2D ? ? ? ?"));

		if (!VerStr)
		{
			MessageBoxA(0, ("Failed to find fortnite version!"), ("Universal Walking Simulator"), MB_ICONERROR);
			return false;
		}

		FullVersion = decltype(FullVersion.c_str())(VerStr);
		Engine_Version = 500;
	}

	else
	{
		toFree = GetEngineVersion();
		FullVersion = toFree.ToString();
	}

	std::string FNVer = FullVersion;
	std::string EngineVer = FullVersion;

	if (!FullVersion.contains(("Live")) && !FullVersion.contains(("Next")) && !FullVersion.contains(("Cert")))
	{
		if (GetEngineVersion)
		{
			FNVer.erase(0, FNVer.find_last_of(("-"), FNVer.length() - 1) + 1);
			EngineVer.erase(EngineVer.find_first_of(("-"), FNVer.length() - 1), 40);

			if (EngineVer.find_first_of(".") != EngineVer.find_last_of(".")) // this is for 4.21.0 and itll remove the .0
				EngineVer.erase(EngineVer.find_last_of((".")), 2);

			Engine_Version = std::stod(EngineVer) * 100;
		}

		else
		{
			const std::regex base_regex(("-([0-9.]*)-"));
			std::cmatch base_match;

			std::regex_search(FullVersion.c_str(), base_match, base_regex);

			FNVer = base_match[1];
		}

		FN_Version = FNVer;

		if (FnVerDouble >= 16.00 && FnVerDouble < 18.40)
			Engine_Version = 427; // 4.26.1;
	}

	else
	{
		Engine_Version = 419;
		FN_Version = ("2.69");
	}

	if (Engine_Version >= 416 && Engine_Version <= 420)
	{
		ObjectsAddr = FindPattern(("48 8B 05 ? ? ? ? 48 8D 1C C8 81 4B ? ? ? ? ? 49 63 76 30"), false, 7, true);

		if (!ObjectsAddr)
			ObjectsAddr = FindPattern(("48 8B 05 ? ? ? ? 48 8D 14 C8 EB 03 49 8B D6 8B 42 08 C1 E8 1D A8 01 0F 85 ? ? ? ? F7 86 ? ? ? ? ? ? ? ?"), false, 7, true);

		if (Engine_Version == 420)
			ToStringAddr = FindPattern(("48 89 5C 24 ? 57 48 83 EC 40 83 79 04 00 48 8B DA 48 8B F9 75 23 E8 ? ? ? ? 48 85 C0 74 19 48 8B D3 48 8B C8 E8 ? ? ? ? 48"));
		else
		{
			ToStringAddr = FindPattern(("40 53 48 83 EC 40 83 79 04 00 48 8B DA 75 19 E8 ? ? ? ? 48 8B C8 48 8B D3 E8 ? ? ? ?"));

			if (!ToStringAddr) // This means that we are in season 1 (i think).
			{
				ToStringAddr = FindPattern(("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B DA 4C 8B F1 E8 ? ? ? ? 4C 8B C8 41 8B 06 99"));

				if (ToStringAddr)
					Engine_Version = 416;
			}
		}

		FreeMemoryAddr = FindPattern(("48 85 C9 74 1D 4C 8B 05 ? ? ? ? 4D 85 C0 0F 84 ? ? ? ? 49"));

		if (!FreeMemoryAddr)
			FreeMemoryAddr = FindPattern(("48 85 C9 74 2E 53 48 83 EC 20 48 8B D9 48 8B 0D ? ? ? ? 48 85 C9 75 0C E8 ? ? ? ? 48 8B 0D ? ? ? ? 48"));

		ProcessEventAddr = FindPattern(("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 48 63 41 0C 45 33 F6"));

		bOldObjects = true;
	}

	if (Engine_Version >= 421 && Engine_Version <= 424)
	{
		ToStringAddr = FindPattern(("48 89 5C 24 ? 57 48 83 EC 30 83 79 04 00 48 8B DA 48 8B F9"));
		ProcessEventAddr = FindPattern(("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? ? ? ? 45 33 F6"));

		if (!ToStringAddr)
			ToStringAddr = FindPattern(("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 30 8B 01 48 8B F1 44 8B 49 04 8B F8 C1 EF 10 48 8B DA 0F B7 C8 89 4D 24 89 7D 20 45 85 C9 75 50 44 38 0D ? ? ? ? 74 09 4C 8D 05 ? ? ? ?")); // s11
	}

	if (Engine_Version >= 425 && Engine_Version < 500)
	{
		ToStringAddr = FindPattern(("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 30 8B 01 48 8B F1 44 8B 49 04 8B F8 C1 EF 10 48 8B DA 0F B7 C8 89 4D 24 89 7D 20 45 85 C9"));
		ProcessEventAddr = FindPattern(("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6"));
	}

	FnVerDouble = std::stod(FN_Version);

	if (Engine_Version == 421 && FnVerDouble >= 5 && FnVerDouble < 6)
		ServerReplicateActorsOffset = 0x54;
	else if (Engine_Version == 421 || (Engine_Version == 422 || Engine_Version == 423))
		ServerReplicateActorsOffset = 0x56;
	if (FnVerDouble >= 7.40 && FnVerDouble < 8.40)
		ServerReplicateActorsOffset = 0x57;
	if (Engine_Version == 424)
		ServerReplicateActorsOffset = 0x5A;
	else if (Engine_Version >= 425 && FnVerDouble < 14)
		ServerReplicateActorsOffset = 0x5D;
	else if (std::floor(FnVerDouble) == 14)
		ServerReplicateActorsOffset = 0x5E;
	else if (Engine_Version >= 426 && FnVerDouble < 19)
		ServerReplicateActorsOffset = 0x5F;
	else if (std::floor(FnVerDouble) == 19)
		ServerReplicateActorsOffset = 0x66;
	else if (FnVerDouble >= 20.00)
		ServerReplicateActorsOffset = 0x67;

	if (FnVerDouble >= 11.00 && FnVerDouble <= 11.01)
		ServerReplicateActorsOffset = 0x57;

	if (FnVerDouble >= 5)
	{
		ObjectsAddr = FindPattern(("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1 EB 03 48 8B ? 81 48 08 ? ? ? 40 49"), false, 7, true);
		FreeMemoryAddr = FindPattern(("48 85 C9 74 2E 53 48 83 EC 20 48 8B D9"));
		bOldObjects = false;

		if (!ObjectsAddr)
			ObjectsAddr = FindPattern(("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1"), true, 3);

		if (!ObjectsAddr)
			ObjectsAddr = FindPattern(("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1"), true, 3); // stupid 5.41
	}

	if (FnVerDouble >= 16.00) // 4.26.1
	{
		FreeMemoryAddr = FindPattern(("48 85 C9 0F 84 ? ? ? ? 48 89 5C 24 ? 57 48 83 EC 20 48 8B 3D ? ? ? ? 48 8B D9 48"));

		if (FnVerDouble < 19.00)
		{
			ToStringAddr = FindPattern(("48 89 5C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B 19 48 8B F2 0F B7 FB 4C 8B F1 E8 ? ? ? ? 44 8B C3 8D 1C 3F 49 C1 E8 10 33 FF 4A 03 5C C0 ? 41 8B 46 04"));
			ProcessEventAddr = FindPattern(("40 55 53 56 57 41 54 41 56 41 57 48 81 EC"));

			if (!ToStringAddr)
				ToStringAddr = FindPattern(("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B 19 33 ED 0F B7 01 48 8B FA C1 EB 10 4C"));
		}
	}

	// if (Engine_Version >= 500)
	if (FnVerDouble >= 19.00)
	{
		ToStringAddr = FindPattern(("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B"));
		ProcessEventAddr = FindPattern(("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 45 33 ED"));

		if (!FreeMemoryAddr)
			FreeMemoryAddr = FindPattern(("48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 ? 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF"));

		// C3 S3

		if (!ToStringAddr) // 19.00
			ToStringAddr = FindPattern("48 89 5C 24 ? 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 83 79 04 00 48 8B DA 0F 85 ? ? ? ? 48 89 BC 24 ? ? ? ? E8 ? ? ? ? 48 8B F8 48 8D 54 24 ? 48 8B C8");

		if (!ProcessEventAddr) // 19.00
			ProcessEventAddr = FindPattern("40 55 53 56 57 41 54 41 56 41 57 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 4D 8B F8 48 8B FA 48 8B F1 48 85 C9 0F 84 ? ? ? ? F7 41 ? ? ? ? ? 0F");

		if (!ToStringAddr)
			ToStringAddr = FindPattern(("48 89 5C 24 ? 48 89 74 24 ? 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B 01 48 8B F2 8B"));

		if (!ProcessEventAddr)
			ProcessEventAddr = FindPattern(("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 45"));
	}

	if (!FreeMemoryAddr)
	{
		MessageBoxA(0, ("Failed to find FMemory::Free"), ("Universal Walking Simulator"), MB_OK);
		return false;
	}

	FMemory::Free = decltype(FMemory::Free)(FreeMemoryAddr);

	toFree.FreeString();

	if (!ToStringAddr)
	{
		MessageBoxA(0, ("Failed to find FName::ToString"), ("Universal Walking Simulator"), MB_OK);
		return false;
	}

	ToStringO = decltype(ToStringO)(ToStringAddr);

	if (!ProcessEventAddr)
	{
		MessageBoxA(0, ("Failed to find UObject::ProcessEvent"), ("Universal Walking Simulator"), MB_OK);
		return false;
	}

	ProcessEventO = decltype(ProcessEventO)(ProcessEventAddr);

	if (!ObjectsAddr)
	{
		MessageBoxA(0, ("Failed to find FUObjectArray::ObjObjects"), ("Universal Walking Simulator"), MB_OK);
		return false;
	}

	if (bOldObjects)
		OldObjects = decltype(OldObjects)(ObjectsAddr);
	else
		ObjObjects = decltype(ObjObjects)(ObjectsAddr);

	return true;
}

UObject* GetEngine()
{
	static auto Engine = FindObjectOld(("FortEngine_"));

	return Engine;
}

struct FURL
{
	FString                                     Protocol;                                                 // 0x0000(0x0010) (ZeroConstructor)
	FString                                     Host;                                                     // 0x0010(0x0010) (ZeroConstructor)
	int                                         Port;                                                     // 0x0020(0x0004) (ZeroConstructor, IsPlainOldData)
	int                                         Valid;                                                    // 0x0024(0x0004) (ZeroConstructor, IsPlainOldData)
	FString                                     Map;                                                      // 0x0028(0x0010) (ZeroConstructor)
	FString                                     RedirectUrl;                                              // 0x0038(0x0010) (ZeroConstructor)
	TArray<FString>                             Op;                                                       // 0x0048(0x0010) (ZeroConstructor)
	FString                                     Portal;                                                   // 0x0058(0x0010) (ZeroConstructor)
};

struct FLevelCollection
{
	unsigned char                                      UnknownData00[0x8];                                       // 0x0000(0x0008) MISSED OFFSET
	UObject* GameState;                                                // 0x0008(0x0008) (ZeroConstructor, IsPlainOldData)
	UObject* NetDriver;                                                // 0x0010(0x0008) (ZeroConstructor, IsPlainOldData)
	UObject* DemoNetDriver;                                            // 0x0018(0x0008) (ZeroConstructor, IsPlainOldData)
	UObject* PersistentLevel;                                          // 0x0020(0x0008) (ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData01[0x50];  // TSet<ULevel*> Levels;
};

struct FLevelCollectionNewer
{
	UObject* GameState;                                                // 0x0008(0x0008) (ZeroConstructor, IsPlainOldData)
	UObject* NetDriver;                                                // 0x0010(0x0008) (ZeroConstructor, IsPlainOldData)
	UObject* DemoNetDriver;                                            // 0x0018(0x0008) (ZeroConstructor, IsPlainOldData)
	UObject* PersistentLevel;                                          // 0x0020(0x0008) (ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData01[0x50];  // TSet<ULevel*> Levels;
};


template <typename ClassType, typename FieldType, typename Prop>
int FindOffsetStructAh(const std::string& ClassName, const std::string& MemberName, int offset = 0)
{
	auto Class = FindObject<ClassType>(ClassName, true);

	if (Class)
	{
		if (FieldType* Next = Class->ChildProperties)
		{
			auto PropName = Class->ChildProperties->GetName();

			while (Next)
			{
				if (PropName == MemberName)
				{
					if (!offset)
						return ((Prop*)Next)->Offset_Internal;
					else
						return *(int*)(__int64(Next) + offset);
				}
				else
				{
					Next = Next->Next;

					if (Next)
					{
						PropName = Next->GetName();
					}
				}
			}
		}
	}

	return 0;
}

int FindOffsetStruct(const std::string& ClassName, const std::string& MemberName)
{
	if (Engine_Version <= 420)
		return FindOffsetStructAh<UClass_FT, UField, UProperty_UE>(ClassName, MemberName);

	else if (Engine_Version == 421) // && Engine_Version <= 424)
		return FindOffsetStructAh<UClass_FTO, UField, UProperty_FTO>(ClassName, MemberName);

	else if (Engine_Version >= 422 && Engine_Version <= 424)
		return FindOffsetStructAh<UClass_FTT, UField, UProperty_FTO>(ClassName, MemberName);

	else if (Engine_Version >= 425 && FnVerDouble < 20.00)
		return FindOffsetStructAh<UClass_CT, FField, FProperty>(ClassName, MemberName);

	else if (FnVerDouble >= 20)
		return FindOffsetStructAh<UClass_CT, FField, FProperty>(ClassName, MemberName, 0x44);

	return 0;
}

int GetOffsetFromProp(void* Prop)
{
	if (!Prop)
		return -1;

	if (Engine_Version <= 420)
		return ((UProperty_UE*)Prop)->Offset_Internal;

	else if (Engine_Version >= 421 && Engine_Version <= 424)
		return ((UProperty_FTO*)Prop)->Offset_Internal;

	else if (Engine_Version >= 425 && FnVerDouble < 20.00)
		return ((FProperty*)Prop)->Offset_Internal;

	else if (FnVerDouble >= 20)
		return *(int*)(__int64(Prop) + 0x44);

	return -1;
}

template <typename StructType = UObject>
StructType* GetSuperStructOfClass(UObject* Class)
{
	if (!Class)
		return nullptr;

	if (Engine_Version <= 420)
		return ((UClass_FT*)Class)->SuperStruct;

	else if (Engine_Version == 421)
		return ((UClass_FTO*)Class)->SuperStruct;

	else if (Engine_Version >= 422 && Engine_Version <= 424)
		return ((UClass_FTT*)Class)->SuperStruct;

	else if (Engine_Version >= 425)
		return ((UClass_CT*)Class)->SuperStruct;

	return nullptr;
}

template <typename MemberType>
INL MemberType* UObject::FastMember(const std::string& MemberName)
{
	if (!StaticFindObjectO)
		return this->Member<MemberType>(MemberName);

	// credit android and ender

	static auto PropertyClass = FindObject("/Script/CoreUObject.Property");
	auto die = std::wstring(MemberName.begin(), MemberName.end()).c_str();

	for (auto CurrentClass = this->ClassPrivate; CurrentClass; CurrentClass = GetSuperStructOfClass(CurrentClass))
	{
		auto property = StaticFindObjectO ? StaticFindObjectO(CurrentClass, PropertyClass, die, false) : nullptr;

		if (property)
		{
			auto offset = GetOffsetFromProp(property); // *(uint32_t*)(__int64(Property) + 0x44);
			return (MemberType*)(__int64(property) + offset);
		}
	}

	std::cout << "Failed FastMember!\n";

	return this->Member<MemberType>(MemberName);
}


uint8_t GetFieldMask(void* Property)
{
	if (!Property)
		return -1;

	// 3 = sizeof(FieldSize) + sizeof(ByteOffset) + sizeof(ByteMask)

	if (Engine_Version <= 420)
		return *(uint8_t*)(__int64(Property) + (sizeof(UProperty_UE) + 3));
	else if (Engine_Version >= 421 && Engine_Version <= 424)
		return *(uint8_t*)(__int64(Property) + (sizeof(UProperty_FTO) + 3));
	else if (Engine_Version >= 425)
		return *(uint8_t*)(__int64(Property) + (sizeof(FProperty) + 3));

	return -1;
}

uint8_t GetBitIndex(void* Property, uint8_t FieldMask_ = -1)
{
	auto FieldMask = FieldMask_ == -1 ? GetFieldMask(Property) : FieldMask_;

	if (FieldMask == 0xFF)
		return FieldMask;

	if (FieldMask == 1)
		return 1;
	if (FieldMask == 2)
		return 2;
	if (FieldMask == 4)
		return 3;
	if (FieldMask == 8)
		return 4;
	if (FieldMask == 16)
		return 5;
	if (FieldMask == 32)
		return 6;
	if (FieldMask == 64)
		return 7;
	if (FieldMask == 128)
		return 8;

	return 0;
}

bool readd(uint8_t* Actual, int BitIndex) // broken
{
	if (BitIndex != 0xFF) // if it is 0xFF then its just a normal bool
	{
		return !(bool)(*Actual & BitIndex);
	}

	return *(bool*)Actual;
}

bool readBitfield(UObject* Object, const std::string& MemberName)
{
	auto Prop = GetProperty(Object, MemberName);

	auto offset = GetOffsetFromProp(Prop);

	if (offset == -1)
		return false;

	auto Actual = (__int64*)(__int64(Object) + offset);

	const auto FieldMask = GetFieldMask(Prop);
	const auto BitIndex = GetBitIndex(Prop, FieldMask);

	return readd((uint8_t*)Actual, BitIndex);
}

bool sett(uint8_t* Actual, int BitIndex, int FieldMask, bool val)
{
	if (BitIndex != 0xFF) // if it is 0xFF then its just a normal bool
	{
		uint8_t* Byte = (uint8_t*)Actual;

		// if (BitfieldVal <= 1)
		{
			if (((bool(1) << BitIndex) & *(bool*)(Actual)) != val)
			{
				*Byte = (*Byte & ~FieldMask) | (val ? FieldMask : 0);
			}
		}
		// else
		{
			// *Byte = ((bool(1) << BitIndex) & *(bool*)(Actual));
		}
	}
	else
	{
		*(bool*)Actual = val;
	}

	return false;
}

bool setBitfield(UObject* Object, const std::string& MemberName, bool val, bool bWithCache = false)
{
	auto Val = val ? 1 : 0;

	// credits fischsalat for most of thisd

	// auto Actual = bWithCache ? Object->CachedMember<uint8_t>(MemberName) : Object->Member<uint8_t>(MemberName);

	int offset = -1;
	void* Prop = nullptr;

	int FieldMask = -1;
	int BitIndex = -1;

	bool refindBitIndex = !bWithCache;
	bool refindMember = !bWithCache;

	static std::unordered_map<std::string, std::pair<int32_t, int32_t>> SavedBits; // Name (formatted in {Class}{Member}) and BitIndex and FieldMask
	static std::unordered_map<std::string, int32_t> SavedOffsets;

	auto CachedName = Object->ClassPrivate->GetName() + MemberName;

	if (bWithCache)
	{
		auto Bit = SavedBits.find(CachedName);

		if (Bit != SavedBits.end())
		{
			BitIndex = Bit->second.first;
			FieldMask = Bit->second.second;
		}
		else
			refindBitIndex = true;

		auto Offset = SavedOffsets.find(CachedName);

		if (Offset != SavedOffsets.end())
			offset = Offset->second;
		else
			refindMember = true;
	}
	
	if (refindBitIndex)
	{
		FieldMask = GetFieldMask(Prop);
		BitIndex = GetBitIndex(Prop, FieldMask);
	}

	if (refindMember)
	{
		Prop = GetProperty(Object, MemberName);
		offset = GetOffsetFromProp(Prop);
	}

	if (offset == -1 || BitIndex == -1 || FieldMask == -1 || !Prop)
		return false;

	if (bWithCache)
	{
		if (refindBitIndex)
			SavedBits.emplace(CachedName, std::make_pair(BitIndex, FieldMask));
		if (refindMember)
			SavedOffsets.emplace(CachedName, offset);
	}

	auto Actual = (__int64*)(__int64(Object) + offset);

	// std::cout << "FieldMask: " << std::to_string(FieldMask) << '\n';
	// std::cout << "Index: " << std::to_string(BitIndex) << '\n';

	sett((uint8_t*)Actual, BitIndex, FieldMask, val);

	return val;
}

template <typename MemberType>
INL MemberType* UObject::Member(const std::string& MemberName)
{
	// MemberName.erase(0, MemberName.find_last_of(".", MemberName.length() - 1) + 1); // This would be getting the short name of the member if you did like ObjectProperty /Script/stuff

	auto Prop = GetProperty(this, MemberName);

	auto offset = GetOffsetFromProp(Prop);

	if (offset == -1)
		return nullptr;

	auto Actual = (MemberType*)(__int64(this) + offset);

	return Actual;
}

template <typename MemberType>
INL MemberType* UObject::CachedMember(const std::string& MemberName)
{
	// MemberName.erase(0, MemberName.find_last_of(".", MemberName.length() - 1) + 1); // This would be getting the short name of the member if you did like ObjectProperty /Script/stuff

	static std::unordered_map<std::string, int32_t> SavedOffsets; // Name (formatted in {Class}{Member}) and Offset

	auto CachedName = ClassPrivate->GetName() + MemberName;
	auto Offset = SavedOffsets.find(CachedName);

	if (Offset != SavedOffsets.end())
	{
		int off = Offset->second;
		
		return (MemberType*)(__int64(this) + off);
	}

	auto Prop = GetProperty(this, MemberName);

	auto offset = GetOffsetFromProp(Prop);

	if (offset == -1)
		return nullptr;

	auto Actual = (MemberType*)(__int64(this) + offset);

	SavedOffsets.emplace(CachedName, offset);

	return Actual;
}

template<typename ElementType>
union TSparseArrayElementOrFreeListLink
{
	/** If the element is allocated, its value is stored here. */
	ElementType ElementData;

	struct
	{
		/** If the element isn't allocated, this is a link to the previous element in the array's free list. */
		int32_t PrevFreeIndex;

		/** If the element isn't allocated, this is a link to the next element in the array's free list. */
		int32_t NextFreeIndex;
	};
};

template <typename ElementType>
union TSparseArrayElementOrListLink
{
	TSparseArrayElementOrListLink(ElementType& InElement)
		: ElementData(InElement)
	{
	}
	TSparseArrayElementOrListLink(ElementType&& InElement)
		: ElementData(InElement)
	{
	}

	TSparseArrayElementOrListLink(int32_t InPrevFree, int32_t InNextFree)
		: PrevFreeIndex(InPrevFree)
		, NextFreeIndex(InNextFree)
	{
	}

	TSparseArrayElementOrListLink<ElementType> operator=(const TSparseArrayElementOrListLink<ElementType>& Other)
	{
		return TSparseArrayElementOrListLink(Other.NextFreeIndex, Other.PrevFreeIndex);
	}

	/** If the element is allocated, its value is stored here. */
	ElementType ElementData;

	struct
	{
		/** If the element isn't allocated, this is a link to the previous element in the array's free list. */
		int32_t PrevFreeIndex;

		/** If the element isn't allocated, this is a link to the next element in the array's free list. */
		int32_t NextFreeIndex;
	};
};

template <int32_t NumElements>
struct TInlineAllocator
{
	template <int32_t Size, int32_t Alignment>
	struct alignas(Alignment) TAlligendBytes
	{
		uint8_t Pad[Size];
	};

	template <typename ElementType>
	struct TTypeCompatibleBytes : public TAlligendBytes<sizeof(ElementType), alignof(ElementType)>
	{
	};

	template <typename ElementType>
	class ForElementType
	{
		friend class TBitArray;

	private:
		TTypeCompatibleBytes<ElementType> InlineData[NumElements];

		ElementType* SecondaryData;
	};
};;

class TBitArray
{
private:
	TInlineAllocator<4>::ForElementType<uint32_t> Data;
	int32_t NumBits;
	int32_t MaxBits;
};

template<typename InElementType>//, typename Allocator /*= FDefaultSparseArrayAllocator */>
class TSparseArray
{
public:
	/*
	using ElementType = InElementType;

	typedef TSparseArrayElementOrFreeListLink<
		TAlignedBytes<sizeof(ElementType), alignof(ElementType)>
	> FElementOrFreeListLink;

	typedef TArray<FElementOrFreeListLink> DataType;
	DataType Data;

	typedef TBitArray<typename Allocator::BitArrayAllocator> AllocationBitArrayType;
	AllocationBitArrayType AllocationFlags;

	int32_t FirstFreeIndex;

	int32_t NumFreeIndices;
	*/

	typedef TSparseArrayElementOrListLink<InElementType> FSparseArrayElement;

	TArray<FSparseArrayElement> Data;
	TBitArray AllocationFlags;
	int32_t FirstFreeIndex;
	int32_t NumFreeIndices;
};

class FSetElementId { int32_t Index; };

template<typename InElementType>//, bool bTypeLayout>
class TSetElementBase
{
public:
	typedef InElementType ElementType;

	/** The element's value. */
	ElementType Value;

	/** The id of the next element in the same hash bucket. */
	mutable FSetElementId HashNextId;

	/** The hash bucket that the element is currently linked to. */
	mutable int32_t HashIndex;
};

template <typename InElementType>
class TSetElement : public TSetElementBase<InElementType>//, THasTypeLayout<InElementType>::Value>
{

};

template<
	typename InElementType//, typename KeyFuncs /*= DefaultKeyFuncs<ElementType>*/ //, typename Allocator /*= FDefaultSetAllocator*/
>
class TSet
{
public:
	typedef TSetElement<InElementType> SetElementType;

	typedef TSparseArray<SetElementType/*, typename Allocator::SparseArrayAllocator*/>     ElementArrayType;
	// typedef typename Allocator::HashAllocator::template ForElementType<FSetElementId> HashType;
	typedef int32_t HashType;

	ElementArrayType Elements;

	mutable HashType Hash;
	mutable int32_t	 HashSize;
};

template <typename KeyType, typename ValueType>
class TPair // this is a very simplified version fo tpair when in reality its a ttuple and a ttuple has a base and stuff but this works
{
public:
	KeyType First;
	ValueType Second;

	TPair(KeyType Key, ValueType Value)
		: First(Key)
		, Second(Value)
	{
	}

	inline KeyType& Key()
	{
		return First;
	}
	inline const KeyType& Key() const
	{
		return First;
	}
	inline ValueType& Value()
	{
		return Second;
	}
	inline const ValueType& Value() const
	{
		return Second;
	}
};

template<typename KeyType, typename ValueType> // , typename SetAllocator, typename KeyFuncs>
struct TMap
{
	typedef TPair<KeyType, ValueType> ElementType;

	typedef TSet<ElementType/*, KeyFuncs, SetAllocator */> ElementSetType;

	/** A set of the key-value pairs in the map. */
	ElementSetType Pairs;
};

struct FFastArraySerializerItem
{
	int                                                ReplicationID;                                            // 0x0000(0x0004) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
	int                                                ReplicationKey;                                           // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
	int                                                MostRecentArrayReplicationKey;                            // 0x0008(0x0004) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
};

#define INDEX_NONE -1
#define IDK -1

struct UScriptStruct : UObject
{
	template <typename MemberType>
	INL MemberType* MemberStruct(const std::string& MemberName, int extraOffset = 0)
	{
		std::cout << "ClassPrivate: " << this->ClassPrivate << '\n';

		std::cout << "Name: " << this->GetName() << '\n';

		// auto off = FindOffsetStruct(this->GetFullName(), MemberName);
		// return (MemberType*)(__int64(this) + off);
		return nullptr;
	}
};

struct FFastArraySerializerSE // 264
{
	// TMap<int32_t, int32_t> ItemMap;
	char ItemMap[0x50];

	int32_t IDCounter;
	int32_t ArrayReplicationKey;

	char GuidReferencesMap[0x50];
	char GuidReferencesMap_StructDelta[0x50];

	int32_t CachedNumItems;
	int32_t CachedNumItemsToConsiderForWriting;
	EFastArraySerializerDeltaFlags DeltaFlags; // 256
	// structural padding here i guess 4
	int idkwhatthisis;

	// template<typename Type, typename SerializerType>
	struct TFastArraySerializeHelper
	{
		using SerializerType = FFastArraySerializerSE;
		using Type = __int64;

		/** Array element type struct. */
		UScriptStruct* Struct;

		/** Set of array elements we're serializing. */
		TArray<Type>& Items;

		/** The actual FFastArraySerializer struct we're serializing. */
		SerializerType& ArraySerializer;

		/** Cached DeltaSerialize params. */
		// FNetDeltaSerializeInfo& Parms;
	};

	void SetDeltaSerializationEnabled(const bool bEnabled)
	{
		// if (!EnumHasAnyFlags(DeltaFlags, EFastArraySerializerDeltaFlags::HasBeenSerialized))
		{
			static auto DeltaFlagsOffset = FindOffsetStruct("ScriptStruct /Script/Engine.FastArraySerializer", "DeltaFlags");
			auto TheseDeltaFlags = (EFastArraySerializerDeltaFlags*)(__int64(this) + DeltaFlagsOffset);

			if (bEnabled)
			{
				*TheseDeltaFlags = EFastArraySerializerDeltaFlags::HasDeltaBeenRequested; // |= EFastArraySerializerDeltaFlags::HasDeltaBeenRequested;
			}
			else
			{
				//*TheseDeltaFlags = !EFastArraySerializerDeltaFlags::HasDeltaBeenRequested;// &= ~EFastArraySerializerDeltaFlags::HasDeltaBeenRequested;
			}
		}
	}

	FFastArraySerializerSE()
		: IDCounter(0)
		, ArrayReplicationKey(0)
#if WITH_PUSH_MODEL
		, OwningObject(nullptr)
		, RepIndex(INDEX_NONE)
#endif // WITH_PUSH_MODEL
		, CachedNumItems(INDEX_NONE)
		, CachedNumItemsToConsiderForWriting(INDEX_NONE)
		, DeltaFlags(EFastArraySerializerDeltaFlags::None)
	{
		std::cout << "Constructor called!\n";
		SetDeltaSerializationEnabled(true);
	}

	void MarkItemDirty(FFastArraySerializerItem* Item)
	{
		if (Item->ReplicationID == IDK)
		{
			Item->ReplicationID = ++IDCounter;
			if (IDCounter == IDK)
				IDCounter++;
		}

		Item->ReplicationKey++;
		MarkArrayDirty();
	}

	void MarkAllItemsDirty() // This is my function, not ue.
	{

	}

	void MarkArrayDirty()
	{
		// ItemMap.Reset();		// This allows to clients to add predictive elements to arrays without affecting replication.

		/*

		ItemMap.Pairs.Elements.Data.Reset();
		ItemMap.Pairs.Elements.FirstFreeIndex = -1;
		ItemMap.Pairs.Elements.NumFreeIndices = 0;
		ItemMap.Pairs.Elements.AllocationFlags.Reset();

		*/

		IncrementArrayReplicationKey();

		// Invalidate the cached item counts so that they're recomputed during the next write
		CachedNumItems = IDK;
		CachedNumItemsToConsiderForWriting = IDK;
	}

	void IncrementArrayReplicationKey()
	{
		ArrayReplicationKey++;
		if (ArrayReplicationKey == IDK)
			ArrayReplicationKey++;
	}
};



struct FFastArraySerializerOL
{
	// TMap<int32_t, int32_t> ItemMap;
	char ItemMap[0x50];
	int32_t IDCounter;
	int32_t ArrayReplicationKey;

	char GuidReferencesMap[0x50];

	int32_t CachedNumItems;
	int32_t CachedNumItemsToConsiderForWriting;

	void MarkItemDirty(FFastArraySerializerItem* Item)
	{
		if (Item->ReplicationID == IDK)
		{
			Item->ReplicationID = ++IDCounter;
			if (IDCounter == IDK)
				IDCounter++;
		}

		Item->ReplicationKey++;
		MarkArrayDirty();
	}

	void MarkArrayDirty()
	{
		// ItemMap.Reset();		// This allows to clients to add predictive elements to arrays without affecting replication.
		IncrementArrayReplicationKey();

		// Invalidate the cached item counts so that they're recomputed during the next write
		CachedNumItems = IDK;
		CachedNumItemsToConsiderForWriting = IDK;
	}

	void IncrementArrayReplicationKey()
	{
		ArrayReplicationKey++;

		if (ArrayReplicationKey == IDK)
			ArrayReplicationKey++;
	}
};

void MarkArrayDirty(void* Array)
{
	if (FnVerDouble < 8.30)
		((FFastArraySerializerOL*)Array)->MarkArrayDirty();
	else
		((FFastArraySerializerSE*)Array)->MarkArrayDirty();
}

void MarkItemDirty(void* Array, FFastArraySerializerItem* Item)
{
	if (!Array)
		return;

	if (FnVerDouble < 8.30)
		((FFastArraySerializerOL*)Array)->MarkItemDirty(Item);
	else
		((FFastArraySerializerSE*)Array)->MarkItemDirty(Item);
}

int32_t GetSizeOfStruct(UObject* Struct)
{
	if (!Struct)
		return -1;

	if (Engine_Version <= 420)
		return ((UClass_FT*)Struct)->PropertiesSize;

	else if (Engine_Version == 421)
		return ((UClass_FTO*)Struct)->PropertiesSize;

	else if (Engine_Version >= 422 && Engine_Version <= 424)
		return ((UClass_FTT*)Struct)->PropertiesSize;

	else if (Engine_Version >= 425)
		return ((UClass_CT*)Struct)->PropertiesSize;

	return 0;
}

template <typename T>
T* Get(int offset, uintptr_t addr)
{
	return (T*)(__int64(addr) + offset);
}

// TODO: REMAKME

struct FWeakObjectPtr
{
public:
	inline bool SerialNumbersMatch(FUObjectItem* ObjectItem) const
	{
		return ObjectItem->SerialNumber == ObjectSerialNumber;
	}

	int32_t ObjectIndex;
	int32_t ObjectSerialNumber;
};

template<typename TObjectID>
class TPersistentObjectPtr
{
public:
	FWeakObjectPtr WeakPtr;
	int32_t TagAtLastTest;
	TObjectID ObjectID;
};

struct FSoftObjectPath
{
	FName AssetPathName;
	FString SubPathString;
};

class FSoftObjectPtr : public TPersistentObjectPtr<FSoftObjectPath>
{

};

class TSoftObjectPtr : public FSoftObjectPtr
{

};
class TSoftClassPtr : public FSoftObjectPtr
{

};

auto GetSerialNumber(UObject* Object)
{
	return (ObjObjects ? ObjObjects->GetItemById(Object->InternalIndex) : OldObjects->GetItemById(Object->InternalIndex))->SerialNumber;
}

template<class T = UObject, class TWeakObjectPtrBase = FWeakObjectPtr>
struct TWeakObjectPtr : public FWeakObjectPtr
{
public:
	inline T* Get() {
		return GetByIndex<T>(ObjectIndex);
	}

	TWeakObjectPtr(int32_t ObjectIndex)
	{
		this->ObjectIndex = ObjectIndex;
		this->ObjectSerialNumber = GetSerialNumber(GetByIndex<UObject>(ObjectIndex));
	}

	TWeakObjectPtr(UObject* Obj)
	{
		this->ObjectIndex = Obj->InternalIndex;
		this->ObjectSerialNumber = GetSerialNumber(GetByIndex<UObject>(Obj->InternalIndex));
	}

	TWeakObjectPtr()
	{
	}
};

namespace EAbilityGenericReplicatedEvent
{
	enum Type
	{
		/** A generic confirmation to commit the ability */
		GenericConfirm = 0,
		/** A generic cancellation event. Not necessarily a canellation of the ability or targeting. Could be used to cancel out of a channelling portion of ability. */
		GenericCancel,
		/** Additional input presses of the ability (Press X to activate ability, press X again while it is active to do other things within the GameplayAbility's logic) */
		InputPressed,
		/** Input release event of the ability */
		InputReleased,
		/** A generic event from the client */
		GenericSignalFromClient,
		/** A generic event from the server */
		GenericSignalFromServer,
		/** Custom events for game use */
		GameCustom1,
		GameCustom2,
		GameCustom3,
		GameCustom4,
		GameCustom5,
		GameCustom6,
		MAX
	};
}

template <class ObjectType>
class TSharedRef
{
public:
	ObjectType* Object;

	int SharedReferenceCount;
	int WeakReferenceCount;

	inline ObjectType* Get()
	{
		return Object;
	}
	inline ObjectType* Get() const
	{
		return Object;
	}
	inline ObjectType& operator*()
	{
		return *Object;
	}
	inline const ObjectType& operator*() const
	{
		return *Object;
	}
	inline ObjectType* operator->()
	{
		return Object;
	}
};

template <class ObjectType>
class TSharedPtrOld
{
public:
	ObjectType* Object;

	int WeakReferenceCount;

	inline ObjectType* Get()
	{
		return Object;
	}
	inline ObjectType* Get() const
	{
		return Object;
	}
	inline ObjectType& operator*()
	{
		return *Object;
	}
	inline const ObjectType& operator*() const
	{
		return *Object;
	}
	inline ObjectType* operator->()
	{
		return Object;
	}

	inline TSharedRef<ObjectType> ToSharedRef()
	{
		return TSharedRef<ObjectType>(Object);
	}
};

template <class ObjectType>
class TSharedPtr
{
public:
	ObjectType* Object;

	int SharedReferenceCount;
	int WeakReferenceCount;

	inline ObjectType* Get()
	{
		return Object;
	}
	inline ObjectType* Get() const
	{
		return Object;
	}
	inline ObjectType& operator*()
	{
		return *Object;
	}
	inline const ObjectType& operator*() const
	{
		return *Object;
	}
	inline ObjectType* operator->()
	{
		return Object;
	}

	inline TSharedRef<ObjectType> ToSharedRef()
	{
		return TSharedRef<ObjectType>(Object);
	}
};

struct FAbilityReplicatedData
{
	bool bTriggered;
	FVector VectorPayload;
	unsigned char Pad[24];
};

int GetEnumValue(UObject* Enum, const std::string& EnumMemberName)
{
	if (!Enum)
		return -1;

	auto Names = (TArray<TPair<FName, __int64>>*)(__int64(Enum) + sizeof(UField) + sizeof(FString));

	if (Names)
	{
		for (int i = 0; i < Names->Num(); i++)
		{
			auto& Pair = Names->At(i);
			auto& Name = Pair.Key();
			auto Value = Pair.Value();

			if (Name.ComparisonIndex && Name.ToString().contains(EnumMemberName))
				return Value;
		}
	}

	return -1;
}

template <typename T>
T* UFunction::GetParam(const std::string& ParameterName, void* Params)
{
	auto off = FindOffsetStruct(this->GetFullName(), ParameterName);
	return (T*)(__int64(Params) + off);
}

std::vector<int> UFunction::GetAllParamOffsets()
{
	{
		if (Engine_Version <= 420)
		{
			auto Members = GetMembers<UClass_FT, UProperty_UE>(this);
			std::vector<int> offs;

			for (auto Member : Members)
			{
				if (Member)
					offs.push_back(Member->Offset_Internal);
			}

			return offs;
		}

		else if (Engine_Version == 421)
		{
			auto Members = GetMembers<UClass_FTO, UProperty_FTO>(this);
			std::vector<int> offs;

			for (auto Member : Members)
			{
				if (Member)
					offs.push_back(Member->Offset_Internal);
			}

			return offs;
		}

		else if (Engine_Version >= 422 && Engine_Version <= 424)
		{
			auto Members = GetMembers<UClass_FTT, UProperty_FTO>(this);
			std::vector<int> offs;

			for (auto Member : Members)
			{
				if (Member)
					offs.push_back(Member->Offset_Internal);
			}

			return offs;
		}

		else if (Engine_Version >= 425 && Engine_Version < 500)
		{
			auto Members = GetMembers<UClass_CT, FProperty>(this);
			std::vector<int> offs;

			for (auto Member : Members)
			{
				if (Member)
					offs.push_back(Member->Offset_Internal);
			}

			return offs;
		}

		else if (FnVerDouble >= 19)
		{
			auto Members = GetMembers<UClass_CT, FProperty>(this);
			std::vector<int> offs;

			for (auto Member : Members)
			{
				if (Member)
					offs.push_back(Member->Offset_Internal);
			}

			return offs;
		}
	}
}

unsigned short UFunction::GetParmsSize()
{
	auto additionalUFunctionOff = sizeof(EFunctionFlags) + sizeof(unsigned char); // funcflags and numparms
	int sizeofUStruct = 0;

	if (Engine_Version <= 420)
		sizeofUStruct = sizeof(UClass_FT);

	else if (Engine_Version == 421) // && Engine_Version <= 424)
		sizeofUStruct = sizeof(UClass_FTO);

	else if (Engine_Version >= 422 && Engine_Version <= 424)
		sizeofUStruct = sizeof(UClass_FTT);

	else if (Engine_Version >= 425)
		sizeofUStruct = sizeof(UClass_CT);

	std::cout << "Off: " << (sizeofUStruct + additionalUFunctionOff) << '\n';

	return *(short*)(__int64(this) + (sizeofUStruct + additionalUFunctionOff));
}

template <typename Type, typename ArrayType>
Type* TArrayAt(TArray<ArrayType>* Array, int i, int Size = sizeof(Type), int ExtraOffset = 0)
{
	return (Type*)(__int64((__int64*)((__int64(Array->GetData()) + (static_cast<long long>(Size) * i)))) + ExtraOffset);
}

struct FScalableFloat
{
public:
	float                                        Value;                                             // 0x0(0x4)(Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	int idk;                    // Fixing Size After Last Property  [ Dumper-7 ]
	char Curve[0x10]; // struct FCurveTableRowHandle                  Curve;                                             // 0x8(0x10)(Edit, BlueprintVisible, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	void* idk2;                              // Fixing Size Of Struct [ Dumper-7 ]
};

struct FNetworkObjectInfo
{
	UObject* Actor; // AActor*

	TWeakObjectPtr<UObject> WeakActor; // AActor

	double NextUpdateTime;

	double LastNetReplicateTime;

	float OptimalNetUpdateDelta;

	float LastNetUpdateTime;

	uint32_t bPendingNetUpdate : 1;

	uint32_t bForceRelevantNextUpdate : 1;

	TSet<TWeakObjectPtr<UObject>> DormantConnections; // UNetConnection

	TSet<TWeakObjectPtr<UObject>> RecentlyDormantConnections; // UNetConnection
};

template<typename SharedPtrType>
class FNetworkObjectList
{
public:
	using FNetworkObjectSet = TSet<TSharedPtrOld<FNetworkObjectInfo>>;

	FNetworkObjectSet AllNetworkObjects;
	FNetworkObjectSet ActiveNetworkObjects;
	FNetworkObjectSet ObjectsDormantOnAllConnections;

	TMap<TWeakObjectPtr<UObject>, int32_t> NumDormantObjectsPerConnection;
};

struct FNeighboringWallInfo
{
	TWeakObjectPtr<UObject>             NeighboringActor;                                         // 0x0000(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	FBuildingSupportCellIndex                   NeighboringCellIdx;                                       // 0x0008(0x000C) (Edit, BlueprintVisible)
	EStructuralWallPosition                            WallPosition;                                             // 0x0014(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData00[0x3];                                       // 0x0015(0x0003) MISSED OFFSET
};

// ScriptStruct FortniteGame.NeighboringFloorInfo
// 0x0018
struct FNeighboringFloorInfo
{
	TWeakObjectPtr<UObject>             NeighboringActor;                                         // 0x0000(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	FBuildingSupportCellIndex                   NeighboringCellIdx;                                       // 0x0008(0x000C) (Edit, BlueprintVisible)
	EStructuralFloorPosition                           FloorPosition;                                            // 0x0014(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	unsigned char                                      UnknownData00[0x3];                                       // 0x0015(0x0003) MISSED OFFSET
};

// ScriptStruct FortniteGame.NeighborifngCenterCellInfo
// 0x0014
struct FNeighboringCenterCellInfo
{
	TWeakObjectPtr<class ABuildingSMActor>             NeighboringActor;                                         // 0x0000(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	FBuildingSupportCellIndex                   NeighboringCellIdx;                                       // 0x0008(0x000C) (Edit, BlueprintVisible)
};

struct FBuildingNeighboringActorInfo
{
	TArray<FNeighboringWallInfo>                NeighboringWallInfos;                                     // 0x0000(0x0010) (Edit, BlueprintVisible, ZeroConstructor)
	TArray<FNeighboringFloorInfo>               NeighboringFloorInfos;                                    // 0x0010(0x0010) (Edit, BlueprintVisible, ZeroConstructor)
	TArray<FNeighboringCenterCellInfo>          NeighboringCenterCellInfos;                               // 0x0020(0x0010) (Edit, BlueprintVisible, ZeroConstructor)
};

struct FKeyHandle
{
	uint32_t Index;
};

/* struct FKeyHandleMap
{
	TMap<FKeyHandle, int32_t> KeyHandlesToIndices;
	TArray<FKeyHandle> KeyHandles;
};

struct FIndexedCurve
{
	mutable FKeyHandleMap KeyHandlesToIndices;
};

struct FRealCurve
	: public FIndexedCurve
{
	float DefaultValue;

	TEnumAsByte<ERichCurveExtrapolation> PreInfinityExtrap;

	TEnumAsByte<ERichCurveExtrapolation> PostInfinityExtrap;
}; */

void* UFunction::GetFunc()
{
	struct fortnite : UStruct_FTT
	{
		uint32_t FunctionFlags;
		uint16_t RepOffset;

		// Variables in memory only.
		uint8_t NumParms;
		uint16_t ParmsSize;
		uint16_t ReturnValueOffset;
		/** Id of this RPC function call (must be FUNC_Net & (FUNC_NetService|FUNC_NetResponse)) */
		uint16_t RPCId;
		/** Id of the corresponding response call (must be FUNC_Net & FUNC_NetService) */
		uint16_t RPCResponseId;

		/** pointer to first local struct property in this UFunction that contains defaults */
		UProperty_UE* FirstPropertyToInit;

		void* Func;
	};

	return ((fortnite*)this)->Func;
}

struct FUniqueNetIdRepl
{
	unsigned char UnknownData00[0x1];
	unsigned char UnknownData01[0x17];
	TArray<unsigned char> ReplicationBytes;
};

struct FFortPickupLocationData
{
public:
	UObject* PickupTarget; // class AFortPawn* PickupTarget;                                      // 0x0(0x8)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	UObject* CombineTarget; // class AFortPickup* CombineTarget;                                     // 0x8(0x8)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	UObject* ItemOwner; // class AFortPawn* ItemOwner;                                         // 0x10(0x8)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	FVector                 LootInitialPosition;                               // 0x18(0xC)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	FVector                LootFinalPosition;                                 // 0x24(0xC)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                        FlyTime;                                           // 0x30(0x4)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	FVector            StartDirection;                                    // 0x34(0xC)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	FVector                 FinalTossRestLocation;                             // 0x40(0xC)(NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8_t TossState; // enum class EFortPickupTossState              TossState;                                         // 0x4C(0x1)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	bool                                         bPlayPickupSound;                                  // 0x4D(0x1)(ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	uint8_t                                        Pad_3687[0x2];                                     // Fixing Size After Last Property  [ Dumper-7 ]
	FGuid                                 PickupGuid;                                        // 0x50(0x10)(ZeroConstructor, IsPlainOldData, RepSkip, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPrivate)
};