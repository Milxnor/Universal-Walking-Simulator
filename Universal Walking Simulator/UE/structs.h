// TODO: Some day someone will need to refactor this see of code

#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>
#include <locale>
#include <format>
#include <iostream>
#include <chrono>
#include <corecrt_math_defines.h>
#include <random>

#include <regex>
#include <unordered_set>

#define INL __forceinline

static inline void (*ToStringO)(struct FName*, class FString&);
static inline void* (*ProcessEventO)(void*, void*, void*);

static double FortniteVersion = 0;
static int EngineVersion = 0;

static struct FChunkedFixedUObjectArray* ObjObjects;
static struct FFixedUObjectArray* OldObjects;

static uint64_t FindPattern(std::string signatureStr, bool bRelative = false, uint32_t offset = 0, bool bIsVar = false)
{
    auto signature = signatureStr.c_str();
    auto base_address = (uint64_t)GetModuleHandleW(NULL);
    static auto patternToByte = [](const char* pattern)
    {
        auto bytes = std::vector<int>{};
        const auto start = const_cast<char*>(pattern);
        const auto end = const_cast<char*>(pattern) + strlen(pattern);

        for (auto current = start; current < end; ++current)
        {
            if (*current == '?')
            {
                ++current;
                if (*current == '?') ++current;
                bytes.push_back(-1);
            }
            else { bytes.push_back(strtoul(current, &current, 16)); }
        }
        return bytes;
    };

    const auto dosHeader = (PIMAGE_DOS_HEADER)base_address;
    const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base_address + dosHeader->e_lfanew);

    const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto patternBytes = patternToByte(signature);
    const auto scanBytes = reinterpret_cast<std::uint8_t*>(base_address);

    const auto s = patternBytes.size();
    const auto d = patternBytes.data();

    for (auto i = 0ul; i < sizeOfImage - s; ++i)
    {
        bool found = true;
        for (auto j = 0ul; j < s; ++j)
        {
            if (scanBytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            auto address = (uint64_t)&scanBytes[i];
            if (bIsVar)
                address = (address + offset + *(int*)(address + 3));
            if (bRelative && !bIsVar)
                address = ((address + offset + 4) + *(int*)(address + offset));
            return address;
        }
    }
    return NULL;
}

namespace FMemory
{
    void (*Free)(void* Original);
    void* (*Realloc)(void* Original, SIZE_T Count, uint32_t Alignment /* = DEFAULT_ALIGNMENT */);
}

template <class ElementType>
class TArray
    // https://github.com/EpicGames/UnrealEngine/blob/4.21/Engine/Source/Runtime/Core/Public/Containers/Array.h#L305
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
            Data[ArrayNum] = New;
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
                memcpy( // FMemory::Memcpy(
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

    inline bool RemoveAt(const int Index) // NOT MINE
    {
        if (Index < ArrayNum)
        {
            if (Index != ArrayNum - 1)
                Data[Index] = Data[ArrayNum - 1];

            --ArrayNum;

            return true;
        }
        return false;
    };

    INL auto GetData() const { return Data; }

    auto begin() const { return GetData(); }
    auto end() const { return GetData() + Num(); }
};

class FString
    // https://github.com/EpicGames/UnrealEngine/blob/4.21/Engine/Source/Runtime/Core/Public/Containers/UnrealString.h#L59
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

enum EName
{
    // pragma = (push_macro("TRUE")) pragma(push_macro("FALSE")) None = 0,
    ByteProperty = 1,
    IntProperty = 2,
    BoolProperty = 3,
    FloatProperty = 4,
    ObjectProperty = 5,
    NameProperty = 6,
    DelegateProperty = 7,
    DoubleProperty = 8,
    ArrayProperty = 9,
    StructProperty = 10,
    VectorProperty = 11,
    RotatorProperty = 12,
    StrProperty = 13,
    TextProperty = 14,
    InterfaceProperty = 15,
    MulticastDelegateProperty = 16,
    LazyObjectProperty = 18,
    SoftObjectProperty = 19,
    Int64Property = 20,
    Int32Property = 21,
    Int16Property = 22,
    Int8Property = 23,
    UInt64Property = 24,
    UInt32Property = 25,
    UInt16Property = 26,
    MapProperty = 28,
    SetProperty = 29,
    Core = 30,
    Engine = 31,
    Editor = 32,
    CoreUObject = 33,
    EnumProperty = 34,
    Cylinder = 50,
    BoxSphereBounds = 51,
    Sphere = 52,
    Box = 53,
    Vector2D = 54,
    IntRect = 55,
    IntPoint = 56,
    Vector4 = 57,
    Name = 58,
    Vector = 59,
    Rotator = 60,
    SHVector = 61,
    Color = 62,
    Plane = 63,
    Matrix = 64,
    LinearColor = 65,
    AdvanceFrame = 66,
    Pointer = 67,
    Double = 68,
    Quat = 69,
    Self = 70,
    Transform = 71,
    Vector3f = 72,
    Vector3d = 73,
    Plane4f = 74,
    Plane4d = 75,
    Matrix44f = 76,
    Matrix44d = 77,
    Quat4f = 78,
    Quat4d = 79,
    Transform3f = 80,
    Transform3d = 81,
    Box3f = 82,
    Box3d = 83,
    BoxSphereBounds3f = 84,
    BoxSphereBounds3d = 85,
    Vector4f = 86,
    Vector4d = 87,
    Rotator3f = 88,
    Rotator3d = 89,
    Vector2f = 90,
    Vector2d = 91,
    Box2D = 92,
    Box2f = 93,
    Box2d = 94,
    Object = 100,
    Camera = 101,
    Actor = 102,
    ObjectRedirector = 103,
    ObjectArchetype = 104,
    Class = 105,
    ScriptStruct = 106,
    Function = 107,
    Pawn = 108,
    State = 200,
    TRue = 201,
    FAlse = 202,
    Enum = 203,
    Default = 204,
    Skip = 205,
    Input = 206,
    Package = 207,
    Groups = 208,
    Interface = 209,
    Components = 210,
    Global = 211,
    Super = 212,
    Outer = 213,
    Map = 214,
    Role = 215,
    RemoteRole = 216,
    PersistentLevel = 217,
    TheWorld = 218,
    PackageMetaData = 219,
    InitialState = 220,
    Game = 221,
    SelectionColor = 222,
    UI = 223,
    ExecuteUbergraph = 224,
    DeviceID = 225,
    RootStat = 226,
    MoveActor = 227,
    All = 230,
    MeshEmitterVertexColor = 231,
    TextureOffsetParameter = 232,
    TextureScaleParameter = 233,
    ImpactVel = 234,
    SlideVel = 235,
    TextureOffset1Parameter = 236,
    MeshEmitterDynamicParameter = 237,
    ExpressionInput = 238,
    Untitled = 239,
    Timer = 240,
    Team = 241,
    Low = 242,
    High = 243,
    NetworkGUID = 244,
    GameThread = 245,
    RenderThread = 246,
    OtherChildren = 247,
    Location = 248,
    Rotation = 249,
    BSP = 250,
    EditorSettings = 251,
    AudioThread = 252,
    ID = 253,
    UserDefinedEnum = 254,
    Control = 255,
    Voice = 256,
    Zlib = 257,
    Gzip = 258,
    LZ4 = 259,
    Mobile = 260,
    Oodle = 261,
    DGram = 280,
    Stream = 281,
    GameNetDriver = 282,
    PendingNetDriver = 283,
    BeaconNetDriver = 284,
    FlushNetDormancy = 285,
    DemoNetDriver = 286,
    GameSession = 287,
    PartySession = 288,
    GamePort = 289,
    BeaconPort = 290,
    MeshPort = 291,
    MeshNetDriver = 292,
    LiveStreamVoice = 293,
    LiveStreamAnimation = 294,
    Linear = 300,
    Point = 301,
    Aniso = 302,
    LightMapResolution = 303,
    UnGrouped = 311,
    VoiceChat = 312,
    Playing = 320,
    Spectating = 322,
    Inactive = 325,
    PerfWarning = 350,
    Info = 351,
    Init = 352,
    Exit = 353,
    Cmd = 354,
    Warning = 355,
    Error = 356,
    FontCharacter = 400,
    InitChild2StartBone = 401,
    SoundCueLocalized = 402,
    SoundCue = 403,
    RawDistributionFloat = 404,
    RawDistributionVector = 405,
    InterpCurveFloat = 406,
    InterpCurveVector2D = 407,
    InterpCurveVector = 408,
    InterpCurveTwoVectors = 409,
    InterpCurveQuat = 410,
    AI = 450,
    NavMesh = 451,
    PerformanceCapture = 500,
    EditorLayout = 600,
    EditorKeyBindings = 601,
    GameUserSettings = 602,
    Filename = 700,
    Lerp = 701,
    Root = 702,
    // pragma = (pop_macro("TRUE")) pragma(pop_macro("FALSE")) # 18 "D:/DocSource/Engine/Source/Runtime/Core/Public/UObject/UnrealNames.h" 2 MaxHardcodedNameIndex,
};

struct FNameEntryId (*FromValidEName)(EName Ename);

struct FNameEntryId
{
    FORCEINLINE static FNameEntryId FromEName(EName Ename)
    {
        return Ename == NAME_None ? FNameEntryId() : FromValidEName(Ename);
    }

    int32_t Value;
};

struct FName
    // https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/Core/Public/UObject/NameTypes.h#L403
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

    FORCEINLINE FName(EName Ename) : FName(Ename, NAME_NO_NUMBER_INTERNAL)
    {
    }

    FORCEINLINE FName(EName Ename, int32_t InNumber)
        : ComparisonIndex(FNameEntryId::FromEName(Ename).Value)
          , Number(InNumber)
    {
    }

    FName()
    {
    }

    FName(int _ComparisonIndex) : ComparisonIndex(_ComparisonIndex)
    {
    }
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

    template <typename T>
    void set(const T& t)
    {
        // This function gets called for the last argument.
        auto OffsetToUse = onOffset == 0 ? 0 : Offsets.at(onOffset);
        *(T*)(__int64(addr) + OffsetToUse) = t;
        onOffset++;
    }

    template <typename First, typename... Rest>
    void set(const First& first, const Rest&... rest)
    {
        if (addr)
        {
            auto OffsetToUse = onOffset == 0 ? 0 : Offsets.at(onOffset);
            std::cout << std::format("Setting {} at offset: {}", first, OffsetToUse)
                // << " with currentRead as: " << currentRead << " Setting to: " << __int64(&*(First*)(__int64(addr) + (currentRead == 0 ? 0 : currentRead))) << '\n';
                * (First*)(__int64(addr) + OffsetToUse) = first;
            onOffset++;
            // std::cout << "\nPadding: " << (sizeof(First) > sizeOfLastType ? sizeof(First) - sizeOfLastType : sizeOfLastType - sizeof(First)) << "\n\n";
            // sizeOfLastType = sizeof(First);
            set(rest...); // recursive call using pack expansion 
        }
    }

    template <typename First, typename... Rest>
    void execute(const std::string& funcName, const First& first, const Rest&... rest)
    {
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

static UObject* (*GetDefaultObject)(UObject* theClass);

struct UObject
    // https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/CoreUObject/Public/UObject/UObjectBase.h#L20
{
    void** VFTable;
    int32_t ObjectFlags;
    int32_t InternalIndex;
    UObject* ClassPrivate;
    // Keep it an object because the we will have to cast it to the correct type depending on the version.
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
            if (EngineVersion < 421)
                Index = 101;
            else if (EngineVersion >= 420 && FortniteVersion < 7.40)
                Index = 102;
            else if (FortniteVersion >= 7.40)
                Index = 103;
            else if (EngineVersion == 424)
                Index = 106; // got on 11.01
            else if (EngineVersion == 425)
                Index = 114;
            else
                std::cout << ("Unable to determine CreateDefaultObject Index!\n");
        }

        if (Index != 0)
        {
            if (EngineVersion < 424 || EngineVersion == 425)
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
    template <typename strType = std::string, typename First, typename... Rest>
    void Exec(const strType& FunctionName, const First& first, const Rest&... rest)
    {
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
    template <typename MemberType>
    INL MemberType* Member(const std::string& MemberName, int BitFieldVal = 0); // DONT USE FOR SCRIPTSTRUCTS

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
};


struct FUObjectItem
    // https://github.com/EpicGames/UnrealEngine/blob/4.27/Engine/Source/Runtime/CoreUObject/Public/UObject/UObjectArray.h#L26
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

struct FChunkedFixedUObjectArray
    // https://github.com/EpicGames/UnrealEngine/blob/7acbae1c8d1736bb5a0da4f6ed21ccb237bc8851/Engine/Source/Runtime/CoreUObject/Public/UObject/UObjectArray.h#L321
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
static UObject* StaticLoadObject(UObject* Class, UObject* Outer, const std::string& name)
{
    if (!StaticLoadObjectO)
        return nullptr;

    auto Name = std::wstring(name.begin(), name.end()).c_str();
    return StaticLoadObjectO(Class, Outer, Name, nullptr, 0, nullptr, false, nullptr);
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
static ReturnType* FindObject(const std::string& str, bool bIsEqual = false, bool bIsName = false,
                              bool bDoNotUseStaticFindObject = false, bool bSkipIfSFOFails = true)
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

template <class TEnum>
struct TEnumAsByte
    // https://github.com/EpicGames/UnrealEngine/blob/4.21/Engine/Source/Runtime/Core/Public/Containers/EnumAsByte.h#L18
{
    uint8_t Value;

    TEnumAsByte(TEnum _value)
        : Value((uint8_t)_value)
    {
    }

    TEnumAsByte() : Value(0)
    {
    }

    TEnum Get()
    {
        return (TEnum)Value;
    }
};

enum ELifetimeCondition
{
    COND_None = 0,
    COND_InitialOnly = 1,
    COND_OwnerOnly = 2,
    COND_SkipOwner = 3,
    COND_SimulatedOnly = 4,
    COND_AutonomousOnly = 5,
    COND_SimulatedOrPhysics = 6,
    COND_InitialOrOwner = 7,
    COND_Custom = 8,
    COND_ReplayOrOwner = 9,
    COND_ReplayOnly = 10,
    COND_SimulatedOnlyNoReplay = 11,
    COND_SimulatedOrPhysicsNoReplay = 12,
    COND_SkipReplay = 13,
    COND_Never = 15,
    COND_Max = 16
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

enum EObjectFlags
{
    RF_NoFlags = 0x00000000,
    RF_Public = 0x00000001,
    RF_Standalone = 0x00000002,
    RF_MarkAsNative = 0x00000004,
    RF_Transactional = 0x00000008,
    RF_ClassDefaultObject = 0x00000010,
    RF_ArchetypeObject = 0x00000020,
    RF_Transient = 0x00000040,
    RF_MarkAsRootSet = 0x00000080,
    RF_TagGarbageTemp = 0x00000100,
    RF_NeedInitialization = 0x00000200,
    RF_NeedLoad = 0x00000400,
    RF_KeepForCooker = 0x00000800,
    RF_NeedPostLoad = 0x00001000,
    RF_NeedPostLoadSubobjects = 0x00002000,
    RF_NewerVersionExists = 0x00004000,
    RF_BeginDestroyed = 0x00008000,
    RF_FinishDestroyed = 0x00010000,
    RF_BeingRegenerated = 0x00020000,
    RF_DefaultSubObject = 0x00040000,
    RF_WasLoaded = 0x00080000,
    RF_TextExportTransient = 0x00100000,
    RF_LoadCompleted = 0x00200000,
    RF_InheritableComponentTemplate = 0x00400000,
    RF_DuplicateTransient = 0x00800000,
    RF_StrongRefOnFrame = 0x01000000,
    RF_NonPIEDuplicateTransient = 0x02000000,
    RF_Dynamic = 0x04000000,
    RF_WillBeLoaded = 0x08000000,
    RF_HasExternalPackage = 0x10000000,
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

enum EPropertyFlags : unsigned __int64
{
    CPF_None = 0,

    CPF_Edit = 0x0000000000000001,
    ///< Property is user-settable in the editor.
    CPF_ConstParm = 0x0000000000000002,
    ///< This is a constant function parameter
    CPF_BlueprintVisible = 0x0000000000000004,
    ///< This property can be read by blueprint code
    CPF_ExportObject = 0x0000000000000008,
    ///< Object can be exported with actor.
    CPF_BlueprintReadOnly = 0x0000000000000010,
    ///< This property cannot be modified by blueprint code
    CPF_Net = 0x0000000000000020,
    ///< Property is relevant to network replication.
    CPF_EditFixedSize = 0x0000000000000040,
    ///< Indicates that elements of an array can be modified, but its size cannot be changed.
    CPF_Parm = 0x0000000000000080,
    ///< Function/When call parameter.
    CPF_OutParm = 0x0000000000000100,
    ///< Value is copied out after function call.
    CPF_ZeroConstructor = 0x0000000000000200,
    ///< memset is fine for construction
    CPF_ReturnParm = 0x0000000000000400,
    ///< Return value.
    CPF_DisableEditOnTemplate = 0x0000000000000800,
    ///< Disable editing of this property on an archetype/sub-blueprint
    //CPF_      						= 0x0000000000001000,	///< 
    CPF_Transient = 0x0000000000002000,
    ///< Property is transient: shouldn't be saved or loaded, except for Blueprint CDOs.
    CPF_Config = 0x0000000000004000,
    ///< Property should be loaded/saved as permanent profile.
    //CPF_								= 0x0000000000008000,	///< 
    CPF_DisableEditOnInstance = 0x0000000000010000,
    ///< Disable editing on an instance of this class
    CPF_EditConst = 0x0000000000020000,
    ///< Property is uneditable in the editor.
    CPF_GlobalConfig = 0x0000000000040000,
    ///< Load config from base class, not subclass.
    CPF_InstancedReference = 0x0000000000080000,
    ///< Property is a component references.
    //CPF_								= 0x0000000000100000,	///<
    CPF_DuplicateTransient = 0x0000000000200000,
    ///< Property should always be reset to the default value during any type of duplication (copy/paste, binary duplication, etc.)
    //CPF_								= 0x0000000000400000,	///< 
    //CPF_    							= 0x0000000000800000,	///< 
    CPF_SaveGame = 0x0000000001000000,
    ///< Property should be serialized for save games, this is only checked for game-specific archives with ArIsSaveGame
    CPF_NoClear = 0x0000000002000000,
    ///< Hide clear (and browse) button.
    //CPF_  							= 0x0000000004000000,	///<
    CPF_ReferenceParm = 0x0000000008000000,
    ///< Value is passed by reference; CPF_OutParam and CPF_Param should also be set.
    CPF_BlueprintAssignable = 0x0000000010000000,
    ///< MC Delegates only.  Property should be exposed for assigning in blueprint code
    CPF_Deprecated = 0x0000000020000000,
    ///< Property is deprecated.  Read it from an archive, but don't save it.
    CPF_IsPlainOldData = 0x0000000040000000,
    ///< If this is set, then the property can be memcopied instead of CopyCompleteValue / CopySingleValue
    CPF_RepSkip = 0x0000000080000000,
    ///< Not replicated. For non replicated properties in replicated structs 
    CPF_RepNotify = 0x0000000100000000,
    ///< Notify actors when a property is replicated
    CPF_Interp = 0x0000000200000000,
    ///< interpolatable property for use with matinee
    CPF_NonTransactional = 0x0000000400000000,
    ///< Property isn't transacted
    CPF_EditorOnly = 0x0000000800000000,
    ///< Property should only be loaded in the editor
    CPF_NoDestructor = 0x0000001000000000,
    ///< No destructor
    //CPF_								= 0x0000002000000000,	///<
    CPF_AutoWeak = 0x0000004000000000,
    ///< Only used for weak pointers, means the export type is autoweak
    CPF_ContainsInstancedReference = 0x0000008000000000,
    ///< Property contains component references.
    CPF_AssetRegistrySearchable = 0x0000010000000000,
    ///< asset instances will add properties with this flag to the asset registry automatically
    CPF_SimpleDisplay = 0x0000020000000000,
    ///< The property is visible by default in the editor details view
    CPF_AdvancedDisplay = 0x0000040000000000,
    ///< The property is advanced and not visible by default in the editor details view
    CPF_Protected = 0x0000080000000000,
    ///< property is protected from the perspective of script
    CPF_BlueprintCallable = 0x0000100000000000,
    ///< MC Delegates only.  Property should be exposed for calling in blueprint code
    CPF_BlueprintAuthorityOnly = 0x0000200000000000,
    ///< MC Delegates only.  This delegate accepts (only in blueprint) only events with BlueprintAuthorityOnly.
    CPF_TextExportTransient = 0x0000400000000000,
    ///< Property shouldn't be exported to text format (e.g. copy/paste)
    CPF_NonPIEDuplicateTransient = 0x0000800000000000,
    ///< Property should only be copied in PIE
    CPF_ExposeOnSpawn = 0x0001000000000000,
    ///< Property is exposed on spawn
    CPF_PersistentInstance = 0x0002000000000000,
    ///< A object referenced by the property is duplicated like a component. (Each actor should have an own instance.)
    CPF_UObjectWrapper = 0x0004000000000000,
    ///< Property was parsed as a wrapper class like TSubclassOf<T>, FScriptInterface etc., rather than a USomething*
    CPF_HasGetValueTypeHash = 0x0008000000000000,
    ///< This property can generate a meaningful hash value.
    CPF_NativeAccessSpecifierPublic = 0x0010000000000000,
    ///< Public native access specifier
    CPF_NativeAccessSpecifierProtected = 0x0020000000000000,
    ///< Protected native access specifier
    CPF_NativeAccessSpecifierPrivate = 0x0040000000000000,
    ///< Private native access specifier
    CPF_SkipSerialization = 0x0080000000000000,
    ///< Property shouldn't be serialized, can still be exported to text
};

struct FProperty : public FField
{
    int32_t ArrayDim;
    int32_t ElementSize;
    EPropertyFlags PropertyFlags;
    uint16_t RepIndex;
    TEnumAsByte<ELifetimeCondition> BlueprintReplicationCondition;
    int32_t Offset_Internal;
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

struct UClass_FT : public UStruct_FT
{
}; // >4.20
struct UClass_FTO : public UStruct_FTO
{
}; // 4.21
struct UClass_FTT : public UStruct_FTT
{
}; // 4.22-4.24
struct UClass_CT : public UStruct_CT
{
}; // C2 to before C3

template <typename ClassType, typename PropertyType, typename ReturnValue = PropertyType>
auto GetMembers(UObject* Object)
{
    std::vector<ReturnValue*> Members;

    if (Object)
    {
        for (auto CurrentClass = (ClassType*)Object->ClassPrivate; CurrentClass; CurrentClass = (ClassType*)CurrentClass
             ->SuperStruct)
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
        for (auto CurrentClass = (ClassType*)Object->ClassPrivate; CurrentClass; CurrentClass = (ClassType*)CurrentClass
             ->SuperStruct)
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

    if (EngineVersion <= 420)
        Members = GetMembers<UClass_FT, UProperty_UE, UObject>(Object);

    else if (EngineVersion == 421) // && Engine_Version <= 424)
        Members = GetMembers<UClass_FTO, UProperty_FTO, UObject>(Object);

    else if (EngineVersion >= 422 && EngineVersion <= 424)
        Members = GetMembers<UClass_FTT, UProperty_FTO, UObject>(Object);

    else if (EngineVersion >= 425 && FortniteVersion < 20)
        Members = GetMembersFProperty<UClass_CT, FProperty, UObject>(Object, bOnlyMembers, bOnlyFunctions);

    else if (FortniteVersion >= 20)
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

UFunction* FindFunction(const std::string& Name, UObject* Object)
// might as well pass in object because what else u gon use a func for.
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
    if (Prop != nullptr)
    {
        if (offset)
            return *(int*)(__int64(Prop) + offset);
        else
            return Prop->Offset_Internal;
    }
    else
    {
        return 0;
    }
}

static void* GetProperty(UObject* Object, const std::string& MemberName)
{
    if (Object && !MemberName.contains((" ")))
    {
        if (EngineVersion <= 420)
            return LoopMembersAndGetProperty<UClass_FT, UProperty_UE>(Object, MemberName);

        else if (EngineVersion == 421) // && Engine_Version <= 424)
            return LoopMembersAndGetProperty<UClass_FTO, UProperty_FTO>(Object, MemberName);

        else if (EngineVersion >= 422 && EngineVersion <= 424)
            return LoopMembersAndGetProperty<UClass_FTT, UProperty_FTO>(Object, MemberName);

        else if (EngineVersion >= 425 && FortniteVersion < 20)
            return LoopMembersAndGetProperty<UClass_CT, FProperty>(Object, MemberName);

        else if (FortniteVersion >= 20)
            return LoopMembersAndGetProperty<UClass_CT, FProperty>(Object, MemberName);
    }
    else
    {
        std::cout << std::format(("Either invalid object or MemberName. MemberName {} Object {}"), MemberName,
                                 Object->GetFullName());
    }

    return nullptr;
}

static int GetOffset(UObject* Object, const std::string& MemberName)
{
    if (Object && !MemberName.contains((" ")))
    {
        if (EngineVersion <= 420)
            return LoopMembersAndFindOffset<UClass_FT, UProperty_UE>(Object, MemberName);

        else if (EngineVersion == 421) // && Engine_Version <= 424)
            return LoopMembersAndFindOffset<UClass_FTO, UProperty_FTO>(Object, MemberName);

        else if (EngineVersion >= 422 && EngineVersion <= 424)
            return LoopMembersAndFindOffset<UClass_FTT, UProperty_FTO>(Object, MemberName);

        else if (EngineVersion >= 425 && FortniteVersion < 20.00)
            return LoopMembersAndFindOffset<UClass_CT, FProperty>(Object, MemberName);

        else if (FortniteVersion >= 20) // s20 maybe
            return LoopMembersAndFindOffset<UClass_CT, FProperty>(Object, MemberName, 0x44);
    }
    else
    {
        // std::cout << std::format(("Either invalid object or MemberName. MemberName {} Object {}"), MemberName, __int64(Object));
    }

    return 0;
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
    if (EngineVersion <= 420)
        return IsA_<UClass_FT>(this, cmp);

    else if (EngineVersion == 421) // && Engine_Version <= 424)
        return IsA_<UClass_FTO>(this, cmp);

    else if (EngineVersion >= 422 && EngineVersion < 425)
        return IsA_<UClass_FTT>(this, cmp);

    else if (EngineVersion >= 425)
        return IsA_<UClass_CT>(this, cmp);

    return false;
}

INL UFunction* UObject::Function(const std::string& FuncName)
{
    return FindFunction(FuncName, this);
}

FString (*GetEngineVersion)();

// TODO: There is this 1.9 function, 48 8D 05 D9 51 22 03. It has the CL and stuff. We may be able to determine the version using the CL.
// There is also a string for the engine version and fortnite version, I think it's for every version its like "engineversion=". I will look into it when I find time.

enum ESpawnActorCollisionHandlingMethod
{
    Undefined,
    AlwaysSpawn,
    AdjustIfPossibleButAlwaysSpawn,
    AdjustIfPossibleButDontSpawnIfColliding,
    DontSpawnIfColliding,
};

struct FActorSpawnParameters
{
    // char pad[0x40];
    FName Name;
    UObject* Template; // AActor*
    UObject* Owner; // AActor*
    UObject* Instigator; // APawn*
    UObject* OverrideLevel; // ULevel*
    ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride;
    uint16_t bRemoteOwned : 1;
    uint16_t bNoFail : 1;
    uint16_t bDeferConstruction : 1;
    uint16_t bAllowDuringConstructionScript : 1;
    EObjectFlags ObjectFlags;
};

struct FVector
{
    float X;
    float Y;
    float Z;

    FVector() : X(0), Y(0), Z(0)
    {
    }

    FVector(float x, float y, float z) : X(x), Y(y), Z(z)
    {
    }

    FVector operator+(const FVector& A)
    {
        return FVector{this->X + A.X, this->Y + A.Y, this->Z + A.Z};
    }

    FVector operator-(const FVector& A)
    {
        return FVector{this->X - A.X, this->Y - A.Y, this->Z - A.Z};
    }

    FVector operator*(const float A)
    {
        return FVector{this->X * A, this->Y * A, this->Z * A};
    }

    bool operator==(const FVector& A)
    {
        return X == A.X && Y == A.Y && Z == A.Z;
    }

    void operator+=(const FVector& A)
    {
        *this = *this + A;
    }

    void operator-=(const FVector& A)
    {
        *this = *this - A;
    }
};


#define FASTASIN_HALF_PI (1.5707963050f)
/**
* Computes the ASin of a scalar value.
*
* @param Value  input angle
* @return ASin of Value
*/
static FORCEINLINE float FastAsin(float Value)
{
    // Clamp input to [-1,1].
    bool nonnegative = (Value >= 0.0f);
    float x = fabsf(Value);
    float omx = 1.0f - x;
    if (omx < 0.0f)
    {
        omx = 0.0f;
    }
    float root = sqrtf(omx);
    // 7-degree minimax approximation
    float result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x -
        0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + FASTASIN_HALF_PI;
    result *= root; // acos(|x|)
    // acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
    return (nonnegative ? FASTASIN_HALF_PI - result : result - FASTASIN_HALF_PI);
}
#undef FASTASIN_HALF_PI

float UE_Atan2(float Y, float X)
{
    //return atan2f(Y,X);
    // atan2f occasionally returns NaN with perfectly valid input (possibly due to a compiler or library bug).
    // We are replacing it with a minimax approximation with a max relative error of 7.15255737e-007 compared to the C library function.
    // On PC this has been measured to be 2x faster than the std C version.

    const float absX = fabsf(X);
    const float absY = fabsf(Y);
    const bool yAbsBigger = (absY > absX);
    float t0 = yAbsBigger ? absY : absX; // Max(absY, absX)
    float t1 = yAbsBigger ? absX : absY; // Min(absX, absY)

    if (t0 == 0.f)
        return 0.f;

    float t3 = t1 / t0;
    float t4 = t3 * t3;

    static const float c[7] = {
        +7.2128853633444123e-03f,
        -3.5059680836411644e-02f,
        +8.1675882859940430e-02f,
        -1.3374657325451267e-01f,
        +1.9856563505717162e-01f,
        -3.3324998579202170e-01f,
        +1.0f
    };

    t0 = c[0];
    t0 = t0 * t4 + c[1];
    t0 = t0 * t4 + c[2];
    t0 = t0 * t4 + c[3];
    t0 = t0 * t4 + c[4];
    t0 = t0 * t4 + c[5];
    t0 = t0 * t4 + c[6];
    t3 = t0 * t3;

    t3 = yAbsBigger ? (0.5f * M_PI) - t3 : t3;
    t3 = (X < 0.0f) ? M_PI - t3 : t3;
    t3 = (Y < 0.0f) ? -t3 : t3;

    return t3;
}

struct FQuat;

float UE_Fmod(float X, float Y)
{
    const float AbsY = fabs(Y);
    if (AbsY <= 1.e-8f)
    {
        // FmodReportError(X, Y);
        return 0.0;
    }

    // Convert to double for better precision, since intermediate rounding can lose enough precision to skew the result.
    const double DX = double(X);
    const double DY = double(Y);

    const double Div = (DX / DY);
    const double IntPortion = DY * trunc(Div);
    const double Result = DX - IntPortion;
    // Convert back to float. This is safe because the result will by definition not exceed the X input.
    return float(Result);
}

struct FRotator
{
    float Pitch;
    float Yaw;
    float Roll;

    FQuat Quaternion() const;

    static __forceinline float ClampAxis(float Angle)
    {
        // returns Angle in the range (-360,360)
        Angle = UE_Fmod(Angle, 360.f);

        if (Angle < 0.f)
        {
            // shift to [0,360) range
            Angle += 360.f;
        }

        return Angle;
    }

    static __forceinline float NormalizeAxis(float Angle)
    {
        // returns Angle in the range [0,360)
        Angle = ClampAxis(Angle);

        if (Angle > 180.f)
        {
            // shift to (-180,180]
            Angle -= 360.f;
        }

        return Angle;
    }

    bool operator==(const FRotator& A)
    {
        return Yaw == A.Yaw && Pitch == A.Pitch && Roll == A.Roll;
    }
};


// alignas(16)
struct FQuat
{
    float W;
    float X;
    float Y;
    float Z;

    FRotator Rotator() const
    {
        // DiagnosticCheckNaN();
        const float SingularityTest = Z * X - W * Y;
        const float YawY = 2.f * (W * Z + X * Y);
        const float YawX = (1.f - 2.f * ((Y * Y) + (Z * Z)));

        // reference 
        // http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
        // http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

        // this value was found from experience, the above websites recommend different values
        // but that isn't the case for us, so I went through different testing, and finally found the case 
        // where both of world lives happily.

        const float SINGULARITY_THRESHOLD = 0.4999995f;
        const float RAD_TO_DEG = (180.f) / M_PI;
        FRotator RotatorFromQuat = FRotator();

        if (SingularityTest < -SINGULARITY_THRESHOLD)
        {
            RotatorFromQuat.Pitch = -90.f;
            RotatorFromQuat.Yaw = UE_Atan2(YawY, YawX) * RAD_TO_DEG;
            RotatorFromQuat.Roll = FRotator::NormalizeAxis(-RotatorFromQuat.Yaw - (2.f * UE_Atan2(X, W) * RAD_TO_DEG));
        }
        else if (SingularityTest > SINGULARITY_THRESHOLD)
        {
            RotatorFromQuat.Pitch = 90.f;
            RotatorFromQuat.Yaw = UE_Atan2(YawY, YawX) * RAD_TO_DEG;
            RotatorFromQuat.Roll = FRotator::NormalizeAxis(RotatorFromQuat.Yaw - (2.f * UE_Atan2(X, W) * RAD_TO_DEG));
        }
        else
        {
            RotatorFromQuat.Pitch = FastAsin(2.f * (SingularityTest)) * RAD_TO_DEG;
            RotatorFromQuat.Yaw = UE_Atan2(YawY, YawX) * RAD_TO_DEG;
            RotatorFromQuat.Roll = UE_Atan2(-2.f * (W * X + Y * Z), (1.f - 2.f * ((X * X) + (Y * Y)))) * RAD_TO_DEG;
        }

        return RotatorFromQuat;
    }
};

struct FTransform
    // https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/Core/Public/Math/TransformNonVectorized.h#L28
{
    FQuat Rotation;
    FVector Translation;
    char pad_1C[0x4]; // Padding never changes
    FVector Scale3D;
    char pad_2C[0x4];
};

static UObject* (*SpawnActorOTrans)(UObject* World, UObject* Class, FTransform* Transform,
                                    const FActorSpawnParameters& SpawnParameters);
static UObject* (*SpawnActorO)(UObject* World, UObject* Class, FVector* Position, FRotator* Rotation,
                               const FActorSpawnParameters& SpawnParameters);

uint64_t ToStringAddr = 0;
uint64_t ProcessEventAddr = 0;
uint64_t ObjectsAddr = 0;
uint64_t FreeMemoryAddr = 0;

static int ServerReplicateActorsOffset = 0x53; // UE4.20

bool Setup(/* void* ProcessEventHookAddr */)
{
    auto SpawnActorAddr = FindPattern(
        ("40 53 56 57 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 0F 28 1D ? ? ? ? 0F 57 D2 48 8B B4 24 ? ? ? ? 0F 28 CB"));

    if (!SpawnActorAddr)
        SpawnActorAddr = FindPattern(
            ("40 53 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 0F 28 1D ? ? ? ? 0F 57 D2 48 8B 9C 24 ? ? ? ? 0F 28 CB 0F 54 1D ? ? ? ? 0F 57"));

    if (!SpawnActorAddr)
        SpawnActorAddr = FindPattern(
            "48 89 5C 24 ? 55 56 57 48 8B EC 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 F0 0F 28 05 ? ? ? ? 48 8B FA 0F 28 0D ? ? ? ? 48 8B D9 48 8B 75 40 0F 29 45 C0 0F 28 05 ? ? ? ? 0F 29 45 E0 0F 29 4D D0 4D 85 C0 74 12 F3 41 0F 10 50 ? F2 41 0F 10 18");

    if (!SpawnActorAddr)
    {
        std::cout << "[WARNING] Failed to find SpawnActor function!\n";
        // MessageBoxA(0, ("Failed to find SpawnActor function."), ("Universal Walking Simulator"), MB_OK);
        // return 0;
    }

    auto SpawnActorTransAddr = FindPattern(
        "48 8B C4 55 53 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 A8 0F 29 78 98 44 0F 29 40 ? 44 0F 29 88 ? ? ? ? 44 0F 29 90 ? ? ? ? 44 0F 29 98 ? ? ? ? 44 0F 29 A0 ? ? ? ? 44 0F 29 A8 ? ? ? ? 44 0F 29 B0 ? ? ? ? 44 0F 29 B8 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 60 45 33 ED 48 89 4D 90 44 89 6D 80 48 8D 05 ? ? ? ? 44 38");

    SpawnActorOTrans = decltype(SpawnActorOTrans)(SpawnActorTransAddr);
    SpawnActorO = decltype(SpawnActorO)(SpawnActorAddr);

    bool bOldObjects = false;

    GetEngineVersion = decltype(GetEngineVersion)(
        FindPattern(("40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8B C8 41 B8 04 ? ? ? 48 8B D3")));

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
        EngineVersion = 500;
    }

    else
    {
        toFree = GetEngineVersion();
        FullVersion = toFree.ToString();
    }

    std::string FNVer = FullVersion;
    std::string EngineVer = FullVersion;

    std::string FN_Version;

    if (!FullVersion.contains(("Live")) && !FullVersion.contains(("Next")) && !FullVersion.contains(("Cert")))
    {
        if (GetEngineVersion)
        {
            FNVer.erase(0, FNVer.find_last_of(("-"), FNVer.length() - 1) + 1);
            EngineVer.erase(EngineVer.find_first_of(("-"), FNVer.length() - 1), 40);

            if (EngineVer.find_first_of(".") != EngineVer.find_last_of("."))
                // this is for 4.21.0 and itll remove the .0
                EngineVer.erase(EngineVer.find_last_of((".")), 2);

            EngineVersion = std::stod(EngineVer) * 100;
        }

        else
        {
            const std::regex base_regex(("-([0-9.]*)-"));
            std::cmatch base_match;

            std::regex_search(FullVersion.c_str(), base_match, base_regex);

            FNVer = base_match[1];
        }

        FN_Version = FNVer;

        if (FortniteVersion >= 16.00 && FortniteVersion < 18.40)
            EngineVersion = 427; // 4.26.1;
    }

    else
    {
        EngineVersion = 419;
        FN_Version = ("2.69");
    }

    if (EngineVersion >= 416 && EngineVersion <= 420)
    {
        ObjectsAddr = FindPattern(("48 8B 05 ? ? ? ? 48 8D 1C C8 81 4B ? ? ? ? ? 49 63 76 30"), false, 7, true);

        if (!ObjectsAddr)
            ObjectsAddr = FindPattern(
                ("48 8B 05 ? ? ? ? 48 8D 14 C8 EB 03 49 8B D6 8B 42 08 C1 E8 1D A8 01 0F 85 ? ? ? ? F7 86 ? ? ? ? ? ? ? ?"),
                false, 7, true);

        if (EngineVersion == 420)
            ToStringAddr = FindPattern(
                ("48 89 5C 24 ? 57 48 83 EC 40 83 79 04 00 48 8B DA 48 8B F9 75 23 E8 ? ? ? ? 48 85 C0 74 19 48 8B D3 48 8B C8 E8 ? ? ? ? 48"));
        else
        {
            ToStringAddr = FindPattern(
                ("40 53 48 83 EC 40 83 79 04 00 48 8B DA 75 19 E8 ? ? ? ? 48 8B C8 48 8B D3 E8 ? ? ? ?"));

            if (!ToStringAddr) // This means that we are in season 1 (i think).
            {
                ToStringAddr = FindPattern(
                    ("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 48 8B DA 4C 8B F1 E8 ? ? ? ? 4C 8B C8 41 8B 06 99"));

                if (ToStringAddr)
                    EngineVersion = 416;
            }
        }

        FreeMemoryAddr = FindPattern(("48 85 C9 74 1D 4C 8B 05 ? ? ? ? 4D 85 C0 0F 84 ? ? ? ? 49"));

        if (!FreeMemoryAddr)
            FreeMemoryAddr = FindPattern(
                ("48 85 C9 74 2E 53 48 83 EC 20 48 8B D9 48 8B 0D ? ? ? ? 48 85 C9 75 0C E8 ? ? ? ? 48 8B 0D ? ? ? ? 48"));

        ProcessEventAddr = FindPattern(
            ("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 48 63 41 0C 45 33 F6"));

        bOldObjects = true;
    }

    if (EngineVersion >= 421 && EngineVersion <= 424)
    {
        ToStringAddr = FindPattern(("48 89 5C 24 ? 57 48 83 EC 30 83 79 04 00 48 8B DA 48 8B F9"));
        ProcessEventAddr = FindPattern(
            ("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? ? ? ? 45 33 F6"));

        if (!ToStringAddr)
            ToStringAddr = FindPattern(
                ("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 30 8B 01 48 8B F1 44 8B 49 04 8B F8 C1 EF 10 48 8B DA 0F B7 C8 89 4D 24 89 7D 20 45 85 C9 75 50 44 38 0D ? ? ? ? 74 09 4C 8D 05 ? ? ? ?"));
        // s11
    }

    if (EngineVersion >= 425 && EngineVersion < 500)
    {
        ToStringAddr = FindPattern(
            ("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 30 8B 01 48 8B F1 44 8B 49 04 8B F8 C1 EF 10 48 8B DA 0F B7 C8 89 4D 24 89 7D 20 45 85 C9"));
        ProcessEventAddr = FindPattern(
            ("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 8B 41 0C 45 33 F6"));
    }

    FortniteVersion = std::stod(FN_Version);

    if (EngineVersion == 421 && FortniteVersion >= 5 && FortniteVersion < 6)
        ServerReplicateActorsOffset = 0x54;
    else if (EngineVersion == 421 || (EngineVersion == 422 || EngineVersion == 423))
        ServerReplicateActorsOffset = 0x56;
    if (FortniteVersion >= 7.40 && FortniteVersion < 8.40)
        ServerReplicateActorsOffset = 0x57;
    if (EngineVersion == 424)
        ServerReplicateActorsOffset = 0x5A;
    else if (EngineVersion >= 425 && FortniteVersion < 14)
        ServerReplicateActorsOffset = 0x5D;
    else if (std::floor(FortniteVersion) == 14)
        ServerReplicateActorsOffset = 0x5E;
    else if (EngineVersion >= 426 && FortniteVersion < 19)
        ServerReplicateActorsOffset = 0x5F;
    else if (std::floor(FortniteVersion) == 19)
        ServerReplicateActorsOffset = 0x66;
    else if (FortniteVersion >= 20.00)
        ServerReplicateActorsOffset = 0x67;

    if (FortniteVersion >= 5)
    {
        ObjectsAddr = FindPattern(("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1 EB 03 48 8B ? 81 48 08 ? ? ? 40 49"),
                                  false, 7, true);
        FreeMemoryAddr = FindPattern(("48 85 C9 74 2E 53 48 83 EC 20 48 8B D9"));
        bOldObjects = false;

        if (!ObjectsAddr)
            ObjectsAddr = FindPattern(("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1"), true, 3);

        if (!ObjectsAddr)
            ObjectsAddr = FindPattern(("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1"), true, 3); // stupid 5.41
    }

    if (FortniteVersion >= 16.00) // 4.26.1
    {
        FreeMemoryAddr = FindPattern(
            ("48 85 C9 0F 84 ? ? ? ? 48 89 5C 24 ? 57 48 83 EC 20 48 8B 3D ? ? ? ? 48 8B D9 48"));

        if (FortniteVersion < 19.00)
        {
            ToStringAddr = FindPattern(
                ("48 89 5C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B 19 48 8B F2 0F B7 FB 4C 8B F1 E8 ? ? ? ? 44 8B C3 8D 1C 3F 49 C1 E8 10 33 FF 4A 03 5C C0 ? 41 8B 46 04"));
            ProcessEventAddr = FindPattern(("40 55 53 56 57 41 54 41 56 41 57 48 81 EC"));

            if (!ToStringAddr)
                ToStringAddr = FindPattern(
                    ("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B 19 33 ED 0F B7 01 48 8B FA C1 EB 10 4C"));
        }
    }

    // if (Engine_Version >= 500)
    if (FortniteVersion >= 19.00)
    {
        ToStringAddr = FindPattern(
            ("48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B"));
        ProcessEventAddr = FindPattern(
            ("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 45 33 ED"));

        if (!FreeMemoryAddr)
            FreeMemoryAddr = FindPattern(
                ("48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 ? 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF"));

        // C3 S3

        if (!ToStringAddr) // 19.00
            ToStringAddr = FindPattern(
                "48 89 5C 24 ? 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 83 79 04 00 48 8B DA 0F 85 ? ? ? ? 48 89 BC 24 ? ? ? ? E8 ? ? ? ? 48 8B F8 48 8D 54 24 ? 48 8B C8");

        if (!ProcessEventAddr) // 19.00
            ProcessEventAddr = FindPattern(
                "40 55 53 56 57 41 54 41 56 41 57 48 81 EC ? ? ? ? 48 8D AC 24 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 4D 8B F8 48 8B FA 48 8B F1 48 85 C9 0F 84 ? ? ? ? F7 41 ? ? ? ? ? 0F");

        if (!ToStringAddr)
            ToStringAddr = FindPattern(
                ("48 89 5C 24 ? 48 89 74 24 ? 57 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 8B 01 48 8B F2 8B"));

        if (!ProcessEventAddr)
            ProcessEventAddr = FindPattern(
                ("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 45"));
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
    FString Protocol; // 0x0000(0x0010) (ZeroConstructor)
    FString Host; // 0x0010(0x0010) (ZeroConstructor)
    int Port; // 0x0020(0x0004) (ZeroConstructor, IsPlainOldData)
    int Valid; // 0x0024(0x0004) (ZeroConstructor, IsPlainOldData)
    FString Map; // 0x0028(0x0010) (ZeroConstructor)
    FString RedirectUrl; // 0x0038(0x0010) (ZeroConstructor)
    TArray<FString> Op; // 0x0048(0x0010) (ZeroConstructor)
    FString Portal; // 0x0058(0x0010) (ZeroConstructor)
};

struct FLevelCollection
{
    unsigned char UnknownData00[0x8]; // 0x0000(0x0008) MISSED OFFSET
    UObject* GameState; // 0x0008(0x0008) (ZeroConstructor, IsPlainOldData)
    UObject* NetDriver; // 0x0010(0x0008) (ZeroConstructor, IsPlainOldData)
    UObject* DemoNetDriver; // 0x0018(0x0008) (ZeroConstructor, IsPlainOldData)
    UObject* PersistentLevel; // 0x0020(0x0008) (ZeroConstructor, IsPlainOldData)
    unsigned char UnknownData01[0x50]; // TSet<ULevel*> Levels;
};

struct FLevelCollectionNewer
{
    UObject* GameState; // 0x0008(0x0008) (ZeroConstructor, IsPlainOldData)
    UObject* NetDriver; // 0x0010(0x0008) (ZeroConstructor, IsPlainOldData)
    UObject* DemoNetDriver; // 0x0018(0x0008) (ZeroConstructor, IsPlainOldData)
    UObject* PersistentLevel; // 0x0020(0x0008) (ZeroConstructor, IsPlainOldData)
    unsigned char UnknownData01[0x50]; // TSet<ULevel*> Levels;
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
    if (EngineVersion <= 420)
        return FindOffsetStructAh<UClass_FT, UField, UProperty_UE>(ClassName, MemberName);

    else if (EngineVersion == 421) // && Engine_Version <= 424)
        return FindOffsetStructAh<UClass_FTO, UField, UProperty_FTO>(ClassName, MemberName);

    else if (EngineVersion >= 422 && EngineVersion <= 424)
        return FindOffsetStructAh<UClass_FTT, UField, UProperty_FTO>(ClassName, MemberName);

    else if (EngineVersion >= 425 && FortniteVersion < 20.00)
        return FindOffsetStructAh<UClass_CT, FField, FProperty>(ClassName, MemberName);

    else if (FortniteVersion >= 20)
        return FindOffsetStructAh<UClass_CT, FField, FProperty>(ClassName, MemberName, 0x44);

    return 0;
}

int GetOffsetFromProp(void* Prop)
{
    if (!Prop)
        return -1;

    if (EngineVersion <= 420)
        return ((UProperty_UE*)Prop)->Offset_Internal;

    else if (EngineVersion >= 421 && EngineVersion <= 424)
        return ((UProperty_FTO*)Prop)->Offset_Internal;

    else if (EngineVersion >= 425 && FortniteVersion < 20.00)
        return ((FProperty*)Prop)->Offset_Internal;

    else if (FortniteVersion >= 20)
        return *(int*)(__int64(Prop) + 0x44);

    return -1;
}

template <typename StructType = UObject>
StructType* GetSuperStructOfClass(UObject* Class)
{
    if (!Class)
        return nullptr;

    if (EngineVersion <= 420)
        return ((UClass_FT*)Class)->SuperStruct;

    else if (EngineVersion == 421)
        return ((UClass_FTO*)Class)->SuperStruct;

    else if (EngineVersion >= 422 && EngineVersion <= 424)
        return ((UClass_FTT*)Class)->SuperStruct;

    else if (EngineVersion >= 425)
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
    uint8_t FieldMask = *(uint8_t*)(__int64(Property) + (sizeof(FProperty) + 3));
    return FieldMask;
}

uint8_t GetBitIndex(void* Property)
{
    auto FieldMask = GetFieldMask(Property);

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
}

template <typename MemberType>
INL MemberType* UObject::Member(const std::string& MemberName, int BitfieldVal)
{
    // MemberName.erase(0, MemberName.find_last_of(".", MemberName.length() - 1) + 1); // This would be getting the short name of the member if you did like ObjectProperty /Script/stuff

    auto Prop = GetProperty(this, MemberName);

    auto offset = GetOffsetFromProp(Prop);

    if (offset == -1)
        return nullptr;

    auto Actual = (MemberType*)(__int64(this) + offset);

    // if (!bIsStruct)

    // CREDITS FISCHSALAT FOR BITFIELD

    /* if (std::is_same<MemberType, bool>())
    {
        const auto FieldMask = GetFieldMask(Prop);
        const auto BitIndex = GetBitIndex(Prop);
        if (BitIndex != 0xFF) // if it is 0xFF then its just a normal bool
        {
            uint8_t* Byte = (uint8_t*)Actual;

            if (BitfieldVal <= 1)
            {
                if (((bool(1) << BitIndex) & *(bool*)(Actual)) != (bool)BitfieldVal)
                {
                    *Byte = (*Byte & ~FieldMask) | (BitfieldVal == 1 ? FieldMask : 0);
                    return (MemberType*)&BitfieldVal;
                }
            }
            else
            {
                *Byte = ((bool(1) << BitIndex) & *(bool*)(Actual));
            }

            return (MemberType*)&BitfieldVal;
        }
    } */

    return Actual;
}

template <typename ElementType>
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

template <typename InElementType> //, typename Allocator /*= FDefaultSparseArrayAllocator */>
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

class FSetElementId
{
    int32_t Index;
};

template <typename InElementType> //, bool bTypeLayout>
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
class TSetElement : public TSetElementBase<InElementType> //, THasTypeLayout<InElementType>::Value>
{
};

template <
    typename InElementType
    //, typename KeyFuncs /*= DefaultKeyFuncs<ElementType>*/ //, typename Allocator /*= FDefaultSetAllocator*/
>
class TSet
{
public:
    typedef TSetElement<InElementType> SetElementType;

    typedef TSparseArray<SetElementType/*, typename Allocator::SparseArrayAllocator*/> ElementArrayType;
    // typedef typename Allocator::HashAllocator::template ForElementType<FSetElementId> HashType;
    typedef int32_t HashType;

    ElementArrayType Elements;

    mutable HashType Hash;
    mutable int32_t HashSize;
};

template <typename KeyType, typename ValueType>
class TPair
    // this is a very simplified version fo tpair when in reality its a ttuple and a ttuple has a base and stuff but this works
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

template <typename KeyType, typename ValueType> // , typename SetAllocator, typename KeyFuncs>
struct TMap
{
    typedef TPair<KeyType, ValueType> ElementType;

    typedef TSet<ElementType/*, KeyFuncs, SetAllocator */> ElementSetType;

    /** A set of the key-value pairs in the map. */
    ElementSetType Pairs;
};

struct FFastArraySerializerItem
{
    int ReplicationID;
    // 0x0000(0x0004) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    int ReplicationKey;
    // 0x0004(0x0004) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
    int MostRecentArrayReplicationKey;
    // 0x0008(0x0004) (ZeroConstructor, IsPlainOldData, RepSkip, RepNotify, Interp, NonTransactional, EditorOnly, NoDestructor, AutoWeak, ContainsInstancedReference, AssetRegistrySearchable, SimpleDisplay, AdvancedDisplay, Protected, BlueprintCallable, BlueprintAuthorityOnly, TextExportTransient, NonPIEDuplicateTransient, ExposeOnSpawn, PersistentInstance, UObjectWrapper, HasGetValueTypeHash, NativeAccessSpecifierPublic, NativeAccessSpecifierProtected, NativeAccessSpecifierPrivate)
};

#define INDEX_NONE -1
#define IDK -1

enum class EFastArraySerializerDeltaFlags : uint8_t
{
    None,
    //! No flags.
    HasBeenSerialized = 1 << 0,
    //! Set when serialization at least once (i.e., this struct has been written or read).
    HasDeltaBeenRequested = 1 << 1,
    //! Set if users requested Delta Serialization for this struct.
    IsUsingDeltaSerialization = 1 << 2,
    //! This will remain unset until we've serialized at least once.
       //! At that point, this will be set if delta serialization was requested and
       //! we support it.
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

    void SetDeltaSerializationEnabled(const bool bEnabled)
    {
        // if (!EnumHasAnyFlags(DeltaFlags, EFastArraySerializerDeltaFlags::HasBeenSerialized))
        {
            static auto DeltaFlagsOffset = FindOffsetStruct("ScriptStruct /Script/Engine.FastArraySerializer",
                                                            "DeltaFlags");
            auto TheseDeltaFlags = (EFastArraySerializerDeltaFlags*)(__int64(this) + DeltaFlagsOffset);

            if (bEnabled)
            {
                *TheseDeltaFlags = EFastArraySerializerDeltaFlags::HasDeltaBeenRequested;
                // |= EFastArraySerializerDeltaFlags::HasDeltaBeenRequested;
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
    if (EngineVersion <= 422)
        ((FFastArraySerializerOL*)Array)->MarkArrayDirty();
    else
        ((FFastArraySerializerSE*)Array)->MarkArrayDirty();
}

void MarkItemDirty(void* Array, FFastArraySerializerItem* Item)
{
    if (EngineVersion <= 422)
        ((FFastArraySerializerOL*)Array)->MarkItemDirty(Item);
    else
        ((FFastArraySerializerSE*)Array)->MarkItemDirty(Item);
}

int32_t GetSizeOfStruct(UObject* Struct)
{
    if (!Struct)
        return -1;

    if (EngineVersion <= 420)
        return ((UClass_FT*)Struct)->PropertiesSize;

    else if (EngineVersion == 421)
        return ((UClass_FTO*)Struct)->PropertiesSize;

    else if (EngineVersion >= 422 && EngineVersion <= 424)
        return ((UClass_FTT*)Struct)->PropertiesSize;

    else if (EngineVersion >= 425)
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

template <class T, class TWeakObjectPtrBase = FWeakObjectPtr>
struct TWeakObjectPtr : public FWeakObjectPtr
{
public:
    inline T* Get()
    {
        return GetByIndex<T>(ObjectIndex);
    }
};

template <typename TObjectID>
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
    return (ObjObjects
                ? ObjObjects->GetItemById(Object->InternalIndex)
                : OldObjects->GetItemById(Object->InternalIndex))->SerialNumber;
}

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
    auto Names = (TArray<TPair<FName, __int64>>*)(__int64(Enum) + sizeof(UField) + sizeof(FString));

    for (int i = 0; i < Names->Num(); i++)
    {
        auto& Pair = Names->At(i);
        auto& Name = Pair.Key();
        auto Value = Pair.Value();

        if (Name.ToString().contains(EnumMemberName))
            return Value;
    }

    return 0;
}

template <typename T>
T* UFunction::GetParam(const std::string& ParameterName, void* Params)
{
    auto off = FindOffsetStruct(this->GetFullName(), ParameterName);
    return (T*)(__int64(Params) + off);
}

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

std::vector<int> UFunction::GetAllParamOffsets()
{
    {
        if (EngineVersion <= 420)
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

        else if (EngineVersion == 421)
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

        else if (EngineVersion >= 422 && EngineVersion <= 424)
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

        else if (EngineVersion >= 425 && EngineVersion < 500)
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

        else if (FortniteVersion >= 19)
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

enum EFunctionFlags
{
    FUNC_None = 0x00000000,
    FUNC_Final = 0x00000001,
    FUNC_RequiredAPI = 0x00000002,
    FUNC_BlueprintAuthorityOnly = 0x00000004,
    FUNC_BlueprintCosmetic = 0x00000008,
    FUNC_Net = 0x00000040,
    FUNC_NetReliable = 0x00000080,
    FUNC_NetRequest = 0x00000100,
    FUNC_Exec = 0x00000200,
    FUNC_Native = 0x00000400,
    FUNC_Event = 0x00000800,
    FUNC_NetResponse = 0x00001000,
    FUNC_Static = 0x00002000,
    FUNC_NetMulticast = 0x00004000,
    FUNC_UbergraphFunction = 0x00008000,
    FUNC_MulticastDelegate = 0x00010000,
    FUNC_Public = 0x00020000,
    FUNC_Private = 0x00040000,
    FUNC_Protected = 0x00080000,
    FUNC_Delegate = 0x00100000,
    FUNC_NetServer = 0x00200000,
    FUNC_HasOutParms = 0x00400000,
    FUNC_HasDefaults = 0x00800000,
    FUNC_NetClient = 0x01000000,
    FUNC_DLLImport = 0x02000000,
    FUNC_BlueprintCallable = 0x04000000,
    FUNC_BlueprintEvent = 0x08000000,
    FUNC_BlueprintPure = 0x10000000,
    FUNC_EditorOnly = 0x20000000,
    FUNC_Const = 0x40000000,
    FUNC_NetValidate = 0x80000000,
    FUNC_AllFlags = 0xFFFFFFFF,
};

unsigned short UFunction::GetParmsSize()
{
    auto additionalUFunctionOff = sizeof(EFunctionFlags) + sizeof(unsigned char); // funcflags and numparms
    int sizeofUStruct = 0;

    if (EngineVersion <= 420)
        sizeofUStruct = sizeof(UClass_FT);

    else if (EngineVersion == 421) // && Engine_Version <= 424)
        sizeofUStruct = sizeof(UClass_FTO);

    else if (EngineVersion >= 422 && EngineVersion <= 424)
        sizeofUStruct = sizeof(UClass_FTT);

    else if (EngineVersion >= 425)
        sizeofUStruct = sizeof(UClass_CT);

    std::cout << "Off: " << (sizeofUStruct + additionalUFunctionOff) << '\n';

    return *(short*)(__int64(this) + (sizeofUStruct + additionalUFunctionOff));
}

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

class FNetworkObjectList
{
public:
    using FNetworkObjectSet = TSet<TSharedPtr<FNetworkObjectInfo>>;

    FNetworkObjectSet AllNetworkObjects;
    FNetworkObjectSet ActiveNetworkObjects;
    FNetworkObjectSet ObjectsDormantOnAllConnections;

    TMap<TWeakObjectPtr<UObject>, int32_t> NumDormantObjectsPerConnection;
};

enum class EDeathCause : uint8_t
{
    OutsideSafeZone = 0,
    FallDamage = 1,
    Pistol = 2,
    Shotgun = 3,
    Rifle = 4,
    SMG = 5,
    Sniper = 6,
    SniperNoScope = 7,
    Melee = 8,
    InfinityBlade = 9,
    Grenade = 10,
    C4 = 11,
    GrenadeLauncher = 12,
    RocketLauncher = 13,
    Minigun = 14,
    Bow = 15,
    Trap = 16,
    DBNOTimeout = 17,
    Banhammer = 18,
    RemovedFromGame = 19,
    MassiveMelee = 20,
    MassiveDiveBomb = 21,
    MassiveRanged = 22,
    Vehicle = 23,
    LMG = 24,
    GasGrenade = 25,
    InstantEnvironmental = 26,
    Turret = 27,
    Cube = 28,
    Balloon = 29,
    StormSurge = 30,
    BasicFiend = 31,
    EliteFiend = 32,
    RangedFiend = 33,
    BasicBrute = 34,
    EliteBrute = 35,
    MegaBrute = 36,
    LoggedOut = 37,
    TeamSwitchSuicide = 38,
    WonMatch = 39,
    Unspecified = 40,
    EDeathCause_MAX = 41
};

static FORCEINLINE void SinCos(float* ScalarSin, float* ScalarCos, float Value)
{
    // Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
    float quotient = (0.31830988618f * 0.5f) * Value;
    if (Value >= 0.0f)
    {
        quotient = (float)((int)(quotient + 0.5f));
    }
    else
    {
        quotient = (float)((int)(quotient - 0.5f));
    }
    float y = Value - (2.0f * M_PI) * quotient;

    // Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
    float sign;
    if (y > 1.57079632679f)
    {
        y = M_PI - y;
        sign = -1.0f;
    }
    else if (y < -1.57079632679f)
    {
        y = -M_PI - y;
        sign = -1.0f;
    }
    else
    {
        sign = +1.0f;
    }

    float y2 = y * y;

    // 11-degree minimax approximation
    *ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 -
        0.16666667f) * y2 + 1.0f) * y;

    // 10-degree minimax approximation
    float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 +
        1.0f;
    *ScalarCos = sign * p;
}

struct FText
{
    char UnknownData[0x18];
};

enum class EFortRarityC2 : uint8_t // C2
{
    Common = 0,
    Uncommon = 1,
    Rare = 2,
    Epic = 3,
    Legendary = 4,
    Mythic = 5,
    Transcendent = 6,
    Unattainable = 7,
    NumRarityValues = 8,
    EFortRarity_MAX = 9,
};

enum class EFortSafeZoneState : uint8_t
{
    None = 0,
    Starting = 1,
    Holding = 2,
    Shrinking = 3,
    EFortSafeZoneState_MAX = 4,
};

FQuat FRotator::Quaternion() const
{
    const float DEG_TO_RAD = M_PI / (180.f);
    const float RADS_DIVIDED_BY_2 = DEG_TO_RAD / 2.f;
    float SP, SY, SR;
    float CP, CY, CR;

    const float PitchNoWinding = fmod(Pitch, 360.0f);
    const float YawNoWinding = fmod(Yaw, 360.0f);
    const float RollNoWinding = fmod(Roll, 360.0f);

    SinCos(&SP, &CP, PitchNoWinding * RADS_DIVIDED_BY_2);
    SinCos(&SY, &CY, YawNoWinding * RADS_DIVIDED_BY_2);
    SinCos(&SR, &CR, RollNoWinding * RADS_DIVIDED_BY_2);

    FQuat RotationQuat;
    RotationQuat.X = CR * SP * SY - SR * CP * CY;
    RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
    RotationQuat.Z = CR * CP * SY - SR * SP * CY;
    RotationQuat.W = CR * CP * CY + SR * SP * SY;

    return RotationQuat;
}

enum class EFortResourceType : uint8_t
{
    Wood = 0,
    Stone = 1,
    Metal = 2,
    Permanite = 3,
    None = 4,
    EFortResourceType_MAX = 5
};

enum class ETInteractionType : uint8_t
{
    IT_NoInteraction = 0,
    IT_Simple = 1,
    IT_LongPress = 2,
    IT_BuildingEdit = 3,
    IT_BuildingImprovement = 4,
    IT_TrapPlacement = 5,
    IT_MAX = 6
};

enum class EFortCustomBodyType : uint8_t
{
    Small = 0,
    Medium = 1,
    MediumAndSmall = 2,
    Large = 3,
    LargeAndSmall = 4,
    LargeAndMedium = 5,
    All = 6,
    Deprecated = 7,
    EFortCustomBodyType_MAX = 8
};

// Enum FortniteGame.EFortCustomGender
enum class EFortCustomGender : uint8_t
{
    Invalid = 0,
    Male = 1,
    Female = 2,
    Both = 3,
    EFortCustomGender_MAX = 4
};

enum class EFortGliderType : uint8_t
{
    Glider = 0,
    Umbrella = 1,
    EFortGliderType_MAX = 2,
};

enum EClassFlags
{
    CLASS_None = 0x00000000u,
    CLASS_Abstract = 0x00000001u,
    CLASS_DefaultConfig = 0x00000002u,
    CLASS_Config = 0x00000004u,
    CLASS_Transient = 0x00000008u,
    CLASS_Parsed = 0x00000010u,
    CLASS_MatchedSerializers = 0x00000020u,
    CLASS_ProjectUserConfig = 0x00000040u,
    CLASS_Native = 0x00000080u,
    CLASS_NoExport = 0x00000100u,
    CLASS_NotPlaceable = 0x00000200u,
    CLASS_PerObjectConfig = 0x00000400u,
    CLASS_ReplicationDataIsSetUp = 0x00000800u,
    CLASS_EditInlineNew = 0x00001000u,
    CLASS_CollapseCategories = 0x00002000u,
    CLASS_Interface = 0x00004000u,
    CLASS_CustomConstructor = 0x00008000u,
    CLASS_Const = 0x00010000u,
    CLASS_LayoutChanging = 0x00020000u,
    CLASS_CompiledFromBlueprint = 0x00040000u,
    CLASS_MinimalAPI = 0x00080000u,
    CLASS_RequiredAPI = 0x00100000u,
    CLASS_DefaultToInstanced = 0x00200000u,
    CLASS_TokenStreamAssembled = 0x00400000u,
    CLASS_HasInstancedReference = 0x00800000u,
    CLASS_Hidden = 0x01000000u,
    CLASS_Deprecated = 0x02000000u,
    CLASS_HideDropDown = 0x04000000u,
    CLASS_GlobalUserConfig = 0x08000000u,
    CLASS_Intrinsic = 0x10000000u,
    CLASS_Constructed = 0x20000000u,
    CLASS_ConfigDoNotCheckDefaults = 0x40000000u,
    CLASS_NewerVersionExists = 0x80000000u,
};

enum EClassCastFlags
{
    CASTCLASS_None = 0x0000000000000000,
    CASTCLASS_UField = 0x0000000000000001,
    CASTCLASS_FInt8Property = 0x0000000000000002,
    CASTCLASS_UEnum = 0x0000000000000004,
    CASTCLASS_UStruct = 0x0000000000000008,
    CASTCLASS_UScriptStruct = 0x0000000000000010,
    CASTCLASS_UClass = 0x0000000000000020,
    CASTCLASS_FByteProperty = 0x0000000000000040,
    CASTCLASS_FIntProperty = 0x0000000000000080,
    CASTCLASS_FFloatProperty = 0x0000000000000100,
    CASTCLASS_FUInt64Property = 0x0000000000000200,
    CASTCLASS_FClassProperty = 0x0000000000000400,
    CASTCLASS_FUInt32Property = 0x0000000000000800,
    CASTCLASS_FInterfaceProperty = 0x0000000000001000,
    CASTCLASS_FNameProperty = 0x0000000000002000,
    CASTCLASS_FStrProperty = 0x0000000000004000,
    CASTCLASS_FProperty = 0x0000000000008000,
    CASTCLASS_FObjectProperty = 0x0000000000010000,
    CASTCLASS_FBoolProperty = 0x0000000000020000,
    CASTCLASS_FUInt16Property = 0x0000000000040000,
    CASTCLASS_UFunction = 0x0000000000080000,
    CASTCLASS_FStructProperty = 0x0000000000100000,
    CASTCLASS_FArrayProperty = 0x0000000000200000,
    CASTCLASS_FInt64Property = 0x0000000000400000,
    CASTCLASS_FDelegateProperty = 0x0000000000800000,
    CASTCLASS_FNumericProperty = 0x0000000001000000,
    CASTCLASS_FMulticastDelegateProperty = 0x0000000002000000,
    CASTCLASS_FObjectPropertyBase = 0x0000000004000000,
    CASTCLASS_FWeakObjectProperty = 0x0000000008000000,
    CASTCLASS_FLazyObjectProperty = 0x0000000010000000,
    CASTCLASS_FSoftObjectProperty = 0x0000000020000000,
    CASTCLASS_FTextProperty = 0x0000000040000000,
    CASTCLASS_FInt16Property = 0x0000000080000000,
    CASTCLASS_FDoubleProperty = 0x0000000100000000,
    CASTCLASS_FSoftClassProperty = 0x0000000200000000,
    CASTCLASS_UPackage = 0x0000000400000000,
    CASTCLASS_ULevel = 0x0000000800000000,
    CASTCLASS_AActor = 0x0000001000000000,
    CASTCLASS_APlayerController = 0x0000002000000000,
    CASTCLASS_APawn = 0x0000004000000000,
    CASTCLASS_USceneComponent = 0x0000008000000000,
    CASTCLASS_UPrimitiveComponent = 0x0000010000000000,
    CASTCLASS_USkinnedMeshComponent = 0x0000020000000000,
    CASTCLASS_USkeletalMeshComponent = 0x0000040000000000,
    CASTCLASS_UBlueprint = 0x0000080000000000,
    CASTCLASS_UDelegateFunction = 0x0000100000000000,
    CASTCLASS_UStaticMeshComponent = 0x0000200000000000,
    CASTCLASS_FMapProperty = 0x0000400000000000,
    CASTCLASS_FSetProperty = 0x0000800000000000,
    CASTCLASS_FEnumProperty = 0x0001000000000000,
    CASTCLASS_USparseDelegateFunction = 0x0002000000000000,
    CASTCLASS_FMulticastInlineDelegateProperty = 0x0004000000000000,
    CASTCLASS_FMulticastSparseDelegateProperty = 0x0008000000000000,
    CASTCLASS_FFieldPathProperty = 0x0010000000000000,
};

enum class ENetDormancy : uint8_t
{
    DORM_Never = 0,
    DORM_Awake = 1,
    DORM_DormantAll = 2,
    DORM_DormantPartial = 3,
    DORM_Initial = 4,
    DORM_MAX = 5
};

enum class ETeamMemberState : uint8_t
{
    None = 0,
    FIRST_CHAT_MESSAGE = 1,
    NeedAmmoHeavy = 2,
    NeedAmmoLight = 3,
    NeedAmmoMedium = 4,
    NeedAmmoShells = 5,
    NeedAmmoRocket = 6,
    ChatBubble = 7,
    EnemySpotted = 8,
    NeedBandages = 9,
    NeedMaterials = 10,
    NeedShields = 11,
    NeedWeapon = 12,
    LAST_CHAT_MESSAGE = 13,
    MAX = 14
};

enum EChannelType
{
    CHTYPE_None = 0,
    // Invalid type.
    CHTYPE_Control = 1,
    // Connection control.
    CHTYPE_Actor = 2,
    // Actor-update channel.

    // @todo: Remove and reassign number to CHTYPE_Voice (breaks net compatibility)
    CHTYPE_File = 3,
    // Binary file transfer.

    CHTYPE_Voice = 4,
    // VoIP data channel
    CHTYPE_MAX = 8,
    // Maximum.
};

enum class ENetRole : uint8_t
{
    ROLE_None = 0,
    ROLE_SimulatedProxy = 1,
    ROLE_AutonomousProxy = 2,
    ROLE_Authority = 3,
    ROLE_MAX = 4
};

// #define PATTERN_TESTING // Never use this unless you know what this is for

template <typename T>
void CheckPattern(const std::string& Name, uint64_t Pattern, T** Delegate)
{
    if (!Pattern)
    {
        MessageBoxA(0, ("Failed to find: " + Name).c_str(), ("Universal Walking Simulator"), MB_ICONERROR);
#ifndef PATTERN_TESTING
        FreeLibraryAndExitThread(GetModuleHandle(0), 0);
#endif
    }

    else
    {
        if (Delegate)
            *Delegate = (T*)(Pattern);
    }
}

enum class EAthenaGamePhase : uint8_t
{
    None = 0,
    Setup = 1,
    Warmup = 2,
    Aircraft = 3,
    SafeZones = 4,
    EndGame = 5,
    Count = 6,
    EAthenaGamePhase_MAX = 7
};

enum ENetMode
{
    NM_Standalone,
    NM_DedicatedServer,
    NM_ListenServer,
    NM_Client,
    NM_MAX,
};

enum class EDynamicFoundationType : uint8_t
{
    Static = 0,
    StartEnabled_Stationary = 1,
    StartEnabled_Dynamic = 2,
    StartDisabled = 3,
    EDynamicFoundationType_MAX = 4
};

// the nam e changes for EGBuildingFoundtaainon Type the varibles but no size

enum class EBuildingFoundationType : uint8_t
{
    BFT = 0,
    BFT01 = 1,
    BFT02 = 2,
    BFT_None = 3,
    BFT_MAX = 4
};

enum class EFortCustomPartType : uint8_t
{
    Head = 0,
    Body = 1,
    Hat = 2,
    Backpack = 3,
    Charm = 4,
    Face = 5,
    NumTypes = 6,
    EFortCustomPartType_MAX = 7
};

auto RandomIntInRange(int min, int max)
{
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    static std::uniform_int_distribution<> distr(min, max); // define the range

    return distr(gen);
}

namespace EClientLoginState
{
    enum Type
    {
        Invalid = 0,
        // This must be a client (which doesn't use this state) or uninitialized.
        LoggingIn = 1,
        // The client is currently logging in.
        Welcomed = 2,
        // Told client to load map and will respond with SendJoin
        ReceivedJoin = 3,
        // NMT_Join received and a player controller has been created
        CleanedUp = 4 // Cleanup has been called at least once, the connection is considered abandoned/terminated/gone
    };
}

struct FGuid
{
    unsigned int A;
    unsigned int B;
    unsigned int C;
    unsigned int D;

    bool operator==(const FGuid& other)
    {
        return A == other.A && B == other.B && C == other.C && D == other.D;
    }
};

enum ETravelType
{
    TRAVEL_Absolute,
    TRAVEL_Partial,
    TRAVEL_Relative,
    TRAVEL_MAX
};

enum class EFortQuickBars : uint8_t
    // This isn't always correct due to them adding Creative Quickbars but for our usage it's fine.
{
    Primary = 0,
    Secondary = 1,
    Max_None = 2,
    EFortQuickBars_MAX = 3
};

enum class EFortPickupSourceTypeFlag : uint8_t
{
    Other = 0,
    Player = 1,
    Destruction = 2,
    Container = 3,
    AI = 4,
    Tossed = 5,
    FloorLoot = 6,
    EFortPickupSourceTypeFlag_MAX = 7
};

enum class EFriendlyFireType : uint8_t
{
    Off = 0,
    On = 1,
    EFriendlyFireType_MAX = 2
};

enum class EFortPickupSpawnSource : uint8_t
{
    Unset = 0,
    PlayerElimination = 1,
    Chest = 2,
    SupplyDrop = 3,
    AmmoBox = 4,
    EFortPickupSpawnSource_MAX = 5
};

enum class EGameplayAbilityReplicationPolicy : uint8_t
{
    ReplicateNo = 0,
    ReplicateYes = 1,
    EGameplayAbilityReplicationPolicy_MAX = 2
};

enum class EFortBuildingType : uint8_t
{
    Wall = 0,
    Floor = 1,
    Corner = 2,
    Deco = 3,
    Prop = 4,
    Stairs = 5,
    Roof = 6,
    Pillar = 7,
    SpawnedItem = 8,
    Container = 9,
    Trap = 10,
    GenericCenterCellActor = 11,
    None = 12,
    EFortBuildingType_MAX = 13
};

enum class EChannelCreateFlags : uint32_t
{
    None = (1 << 0),
    OpenedLocally = (1 << 1)
};

struct FBuildingGridActorFilter
{
    bool bIncludeWalls; // 0x0000(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    bool bIncludeFloors; // 0x0001(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    bool bIncludeFloorInTop; // 0x0002(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    bool bIncludeCenterCell; // 0x0003(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
};

struct FBuildingSupportCellIndex
{
    int X; // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    int Y; // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    int Z; // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)

    bool operator==(const FBuildingSupportCellIndex& A)
    {
        return X == A.X && Y == A.Y && Z == A.Z;
    }
};

enum class EStructuralFloorPosition : uint8_t
{
    Top = 0,
    Bottom = 1,
    EStructuralFloorPosition_MAX = 2
};


// Enum FortniteGame.EStructuralWallPosition
enum class EStructuralWallPosition : uint8_t
{
    Left = 0,
    Right = 1,
    Front = 2,
    Back = 3,
    EStructuralWallPosition_MAX = 4
};

struct FGameplayAbilitySpecHandle
{
    int Handle;
    static inline std::unordered_set<int> Handles;

    void GenerateNewHandle()
    {
        auto newHandle = rand();

        while (Handles.find(newHandle) != Handles.end())
        {
            newHandle = rand();
        }

        Handle = newHandle;
        Handles.emplace(Handle);
    }
};

struct FPredictionKey
{
    int16_t Current;
    int16_t Base;
    unsigned char UnknownData00[0x4];
    UObject* PredictiveConnection;
    bool bIsStale;
    bool bIsServerInitiated;
    unsigned char UnknownData01[0x6];
};

struct FPredictionKeyFTS // 4.26
{
    UObject* PredictiveConnection;
    int16_t Current;
    int16_t Base;
    bool bIsStale;
    bool bIsServerInitiated;
    unsigned char UnknownData00[0x2];
};

struct FGameplayEffectContextHandle
{
    unsigned char UnknownData00[0x18];
};

struct FGameplayTag
{
    FName TagName;
};

struct FOnGameplayEffectTagCountChanged
{
    // I did math to figure this out...
    char pad[0x18];
};

struct FGameplayTagContainer
{
    TArray<FGameplayTag> GameplayTags;
    TArray<FGameplayTag> ParentTags;

    std::string ToStringSimple(bool bQuoted) const
    {
        std::string RetString;
        for (int i = 0; i < GameplayTags.Num(); ++i)
        {
            if (bQuoted)
            {
                RetString += ("\"");
            }
            RetString += GameplayTags[i].TagName.ToString();
            if (bQuoted)
            {
                RetString += ("\"");
            }

            if (i < GameplayTags.Num() - 1)
            {
                RetString += (", ");
            }
        }
        return RetString;
    }
};

struct FGameplayAbilityTargetDataHandleOL
{
    unsigned char UnknownData00[0x20];
};

struct FServerAbilityRPCBatchOL
{
    FGameplayAbilitySpecHandle AbilitySpecHandle;
    unsigned char UnknownData00[0x4];
    FPredictionKey PredictionKey;
    FGameplayAbilityTargetDataHandleOL TargetData;
    bool InputPressed;
    bool Ended;
    bool Started;
    unsigned char UnknownData01[0x5];
};

struct FGameplayEventDataOL
{
    FGameplayTag EventTag;
    UObject* Instigator;
    UObject* Target;
    UObject* OptionalObject;
    UObject* OptionalObject2;
    FGameplayEffectContextHandle ContextHandle;
    FGameplayTagContainer InstigatorTags;
    FGameplayTagContainer TargetTags;
    float EventMagnitude;
    unsigned char UnknownData00[0x4];
    FGameplayAbilityTargetDataHandleOL TargetData;
};

struct FGameplayAbilityTargetDataHandleSE
{
    unsigned char UnknownData00[0x28];
};

struct FServerAbilityRPCBatchSE
{
    FGameplayAbilitySpecHandle AbilitySpecHandle;
    unsigned char UnknownData00[0x4];
    FPredictionKey PredictionKey;
    FGameplayAbilityTargetDataHandleSE TargetData;
    bool InputPressed;
    bool Ended;
    bool Started;
    unsigned char UnknownData01[0x5];
};

struct FServerAbilityRPCBatchNewer
{
    FGameplayAbilitySpecHandle AbilitySpecHandle;
    unsigned char UnknownData00[0x4];
    char PredictionKey[0x18];
    FGameplayAbilityTargetDataHandleSE TargetData;
    bool InputPressed;
    bool Ended;
    bool Started;
    unsigned char UnknownData01[0x5];
};

struct FGameplayEventDataSE
{
    FGameplayTag EventTag;
    UObject* Instigator;
    UObject* Target;
    UObject* OptionalObject;
    UObject* OptionalObject2;
    FGameplayEffectContextHandle ContextHandle;
    FGameplayTagContainer InstigatorTags;
    FGameplayTagContainer TargetTags;
    float EventMagnitude;
    unsigned char UnknownData00[0x4];
    FGameplayAbilityTargetDataHandleSE TargetData;
};

enum class EGameplayAbilityActivationMode : uint8_t
{
    Authority = 0,
    NonAuthority = 1,
    Predicting = 2,
    Confirmed = 3,
    Rejected = 4,
    EGameplayAbilityActivationMode_MAX = 5
};

struct FGameplayAbilityActivationInfo
{
    TEnumAsByte<EGameplayAbilityActivationMode> ActivationMode;
    unsigned char bCanBeEndedByOtherInstance : 1;
    unsigned char UnknownData00[0x6];
    FPredictionKey PredictionKeyWhenActivated;
};

struct FServerAbilityRPCBatchFTS
{
    FGameplayAbilitySpecHandle AbilitySpecHandle;
    unsigned char UnknownData00[0x4];
    FPredictionKeyFTS PredictionKey;
    FGameplayAbilityTargetDataHandleSE TargetData;
    bool InputPressed;
    bool Ended;
    bool Started;
    unsigned char UnknownData01[0x5];
};

struct FGameplayAbilityActivationInfoFTS
{
    TEnumAsByte<EGameplayAbilityActivationMode> ActivationMode;
    unsigned char bCanBeEndedByOtherInstance : 1;
    unsigned char UnknownData00[0x6];
    FPredictionKeyFTS PredictionKeyWhenActivated;
};

struct FActiveGameplayEffectHandle
{
    int Handle;
    bool bPassedFiltersAndWasExecuted;
    unsigned char UnknownData00[0x3];
};

template <typename ActivationInfoType>
struct FGameplayAbilitySpec : public FFastArraySerializerItem
{
    FGameplayAbilitySpecHandle Handle;
    UObject* Ability;
    int Level;
    int InputID;
    UObject* SourceObject;
    unsigned char ActiveCount;
    unsigned char InputPressed : 1;
    unsigned char RemoveAfterActivation : 1;
    unsigned char PendingRemove : 1;
    unsigned char UnknownData00[0x6];
    ActivationInfoType ActivationInfo;
    TArray<UObject*> NonReplicatedInstances;
    TArray<UObject*> ReplicatedInstances;
    FActiveGameplayEffectHandle GameplayEffectHandle;
    unsigned char UnknownData01[0x50];
};

struct FGameplayAbilitySpecNewer : FFastArraySerializerItem
{
    FGameplayAbilitySpecHandle Handle;
    UObject* Ability;
    int32_t Level;
    int32_t InputID;
    UObject* SourceObject;
    char ActiveCount;
    char InputPressed : 1;
    char RemoveAfterActivation : 1;
    char PendingRemove : 1;
    char bActivateOnce : 1;
    char UnknownData_29_4 : 4;
    char UnknownData_2A[0x6];
    FGameplayAbilityActivationInfoFTS ActivationInfo;
    FGameplayTagContainer DynamicAbilityTags;
    TArray<UObject*> NonReplicatedInstances;
    TArray<UObject*> ReplicatedInstances;
    FActiveGameplayEffectHandle GameplayEffectHandle;
    char UnknownData_90[0x50];
    void* dababy;
};

struct FGameplayAbilitySpecContainerSE : public FFastArraySerializerSE
{
    TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfo>> Items;
    UObject* Owner;
};

struct FGameplayAbilitySpecContainerFTS : public FFastArraySerializerSE
{
    TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfoFTS>> Items;
    UObject* Owner;
};

struct FGameplayAbilitySpecContainerNewer : public FFastArraySerializerSE
{
    TArray<FGameplayAbilitySpecNewer> Items;
    UObject* Owner;
};

struct FGameplayAbilitySpecContainerOL : public FFastArraySerializerOL
{
    TArray<FGameplayAbilitySpec<FGameplayAbilityActivationInfo>> Items;
    UObject* Owner;
};

enum class EGameplayAbilityInstancingPolicy : uint8_t
{
    NonInstanced = 0,
    InstancedPerActor = 1,
    InstancedPerExecution = 2,
    EGameplayAbilityInstancingPolicy_MAX = 3
};

struct FGameplayAbilitySpecHandleAndPredictionKey
{
    FGameplayAbilitySpecHandle AbilityHandle;

    int32_t PredictionKeyAtCreation;
};

struct FNeighboringWallInfo
{
    TWeakObjectPtr<UObject> NeighboringActor;
    // 0x0000(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    FBuildingSupportCellIndex NeighboringCellIdx; // 0x0008(0x000C) (Edit, BlueprintVisible)
    EStructuralWallPosition WallPosition; // 0x0014(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    unsigned char UnknownData00[0x3]; // 0x0015(0x0003) MISSED OFFSET
};

// ScriptStruct FortniteGame.NeighboringFloorInfo
// 0x0018
struct FNeighboringFloorInfo
{
    TWeakObjectPtr<UObject> NeighboringActor;
    // 0x0000(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    FBuildingSupportCellIndex NeighboringCellIdx; // 0x0008(0x000C) (Edit, BlueprintVisible)
    EStructuralFloorPosition FloorPosition; // 0x0014(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    unsigned char UnknownData00[0x3]; // 0x0015(0x0003) MISSED OFFSET
};

// ScriptStruct FortniteGame.NeighborifngCenterCellInfo
// 0x0014
struct FNeighboringCenterCellInfo
{
    TWeakObjectPtr<class ABuildingSMActor> NeighboringActor;
    // 0x0000(0x0008) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
    FBuildingSupportCellIndex NeighboringCellIdx; // 0x0008(0x000C) (Edit, BlueprintVisible)
};

struct FBuildingNeighboringActorInfo
{
    TArray<FNeighboringWallInfo> NeighboringWallInfos; // 0x0000(0x0010) (Edit, BlueprintVisible, ZeroConstructor)
    TArray<FNeighboringFloorInfo> NeighboringFloorInfos; // 0x0010(0x0010) (Edit, BlueprintVisible, ZeroConstructor)
    TArray<FNeighboringCenterCellInfo> NeighboringCenterCellInfos;
    // 0x0020(0x0010) (Edit, BlueprintVisible, ZeroConstructor)
};

struct Bitfield_242
{
    unsigned char bHidden : 1; // 0x0084(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, Net)
    unsigned char bNetTemporary : 1; // 0x0084(0x0001)
    unsigned char bNetStartup : 1; // 0x0084(0x0001)
    unsigned char bOnlyRelevantToOwner : 1;
    // 0x0084(0x0001) (Edit, BlueprintVisible, BlueprintReadOnly, DisableEditOnInstance)
    unsigned char bAlwaysRelevant : 1; // 0x0084(0x0001) (Edit, BlueprintVisible, DisableEditOnInstance)
    unsigned char bReplicateMovement : 1; // 0x0084(0x0001) (Edit, Net, DisableEditOnInstance)
    unsigned char bTearOff : 1; // 0x0084(0x0001) (Net)
    unsigned char bExchangedRoles : 1; // 0x0084(0x0001) (Transient)
};

struct Bitfield2_242
{
    unsigned char AutoDestroyWhenFinished : 1; // 0x013C(0x0001) (BlueprintVisible)
    unsigned char bCanBeDamaged : 1; // 0x013C(0x0001) (Edit, BlueprintVisible, Net, SaveGame)
    unsigned char bActorIsBeingDestroyed : 1; // 0x013C(0x0001) (Transient, DuplicateTransient)
    unsigned char bCollideWhenPlacing : 1; // 0x013C(0x0001)
    unsigned char bFindCameraComponentWhenViewTarget : 1; // 0x013C(0x0001) (Edit, BlueprintVisible)
    unsigned char bRelevantForNetworkReplays : 1; // 0x013C(0x0001)
    unsigned char bGenerateOverlapEventsDuringLevelStreaming : 1; // 0x013C(0x0001) (Edit, BlueprintVisible)
    unsigned char bCanBeInCluster : 1;
};

struct FixMovementParameters
{
    UObject* Object;
};

struct TravelParameters
{
    FString URL;
    ETravelType TravelType;
    bool bSeamless;
    FGuid MapPackageGuid;
};

struct HealthParameters
{
    float NewHealthVal;
};

struct SkyDiveParameters
{
    float HeightAboveGround;
};

struct ApplyGameplayEffectParams
{
    UObject* GameplayEffect;
    float Level;
    FGameplayEffectContextHandle EffectContext;
    FActiveGameplayEffectHandle Return;
};

struct UAbilitySystemComponentServerTryActivateAbilityParams
{
    FGameplayAbilitySpecHandle AbilityToActivate;
    bool InputPressed;
    FPredictionKey PredictionKey;
};

struct UAbilitySystemComponentServerAbilityRpcBatchParams
{
    long long BatchInfo;
};

struct UAbilitySystemComponentServerTryActivateAbilityWithEventDataParams
{
    FGameplayAbilitySpecHandle AbilityToActivate;
    bool InputPressed;
    FPredictionKey PredictionKey;
    long long TriggerEventData;
};

struct FCreateBuildingActorData
{
    uint32_t BuildingClassHandle;
    FVector BuildLoc;
    FRotator BuildRot;
    bool Mirrored;
    unsigned char UnknownData00[0x3];
    float SyncKey;
    unsigned char UnknownData01[0x4];
    char Pad[0x10];
};

struct NewBuildingParameters
{
    FCreateBuildingActorData CreateBuildingData;
};

struct FBuildingClassData
{
    UObject* BuildingClass;
    int PreviousBuildingLevel;
    int UpgradeLevel;
};

struct NewBuildingParametersOld
{
    FBuildingClassData BuildingClassData;
    FVector BuildLoc;
    FRotator BuildRot;
    bool Mirrored;
    float SyncKey; // does this exist below 7.4
};

struct BeginEditingParameters
{
    UObject* BuildingActor;
};

struct EditingParameters
{
    UObject* BuildingActorToEdit;
    UObject* NewBuildingClass;
    int RotationIterations;
    bool Mirrored;
};

struct IsDestroyedBitField
{
    unsigned char SurpressHealthBar : 1;
    unsigned char CreateVerboseHealthLogs : 1;
    unsigned char IsIndestructibleForTargetSelection : 1;
    unsigned char Destroyed : 1;
    unsigned char PersistToWorld : 1;
    unsigned char RefreshFullSaveDataBeforeZoneSave : 1;
    unsigned char BeingDragged : 1;
    unsigned char RotateInPlaceGame : 1;
};

struct EndEditingParameters
{
    UObject* BuildingActorToStopEditing;
};

struct ReportDamagedResourceBuildingParameters
{
    UObject* BuildingSmActor;
    TEnumAsByte<EFortResourceType> PotentialResourceType;
    int PotentialResourceCount;
    bool Destroyed;
    bool JustHitWeakspot;
};

struct CurveTableRowHandle
{
    UObject* CurveTable;
    FName RowName;
};

struct InstigatorParameters {
    FGameplayTagContainer InTags;
    UObject* InstigatorController; // AController*
    bool Ret;
};

struct EquipWeaponParametersOld{
    UObject* Def;
    FGuid Guid;
    UObject* Wep;
};

struct EquipWeaponParameters {
    UObject* Def;
    FGuid Guid;
    FGuid TrackerGuid;
    UObject* Wep;
};

struct CreateTempItemParameters{
    int Count;
    int Level;
    UObject* Instance;
};

struct AddItemInternalParameters
{
    FGuid Item;
    EFortQuickBars Quickbar;
    int Slot;
};

struct ServerAttemptInventoryDropParameters
{
    FGuid ItemGuid; // (Parm, ZeroConstructor, IsPlainOldData)
    int Count; // (Parm, ZeroConstructor, IsPlainOldData)
};

struct ServerHandlePickupParameters
{
    UObject* Pickup;
    float InFlyTime;
    FVector InStartDirection;
    bool bPlayPickupSound;
};

struct ServerHandlePickupWithSwapParameters
{
    UObject* Pickup;
    FGuid Swap;
    float InFlyTime;
    FVector InStartDirection;
    bool PlayPickupSound;
};

struct ClientDropParameters
{
    FGuid ItemGuid;
    int Count;
};

struct ClientPickupParameters
{
    UObject* Pickup;
    float InFlyTime;
    FVector InStartDirection;
    bool PlayPickupSound;
};

enum class ItemType
{
    None,
    Weapon,
    Consumable,
    Ammo
};

struct DefinitionInRow // 50 bytes
{
    UObject* Definition = nullptr;
    float Weight;
    int DropCount = 1;
    std::string RowName;
    ItemType Type = ItemType::None;
};

struct FixFloorLootCrash {
    uint8_t DoDelayedUpdateCullDistanceVolumes : 1;
    uint8_t IsRunningConstructionScript : 1;
    uint8_t ShouldSimulatePhysics : 1;
    uint8_t DropDetail : 1;
    uint8_t AggressiveLOD : 1;
    uint8_t IsDefaultLevel : 1;
    uint8_t RequestedBlockOnAsyncLoading : 1;
    uint8_t ActorsInitialized : 1;
};

struct GetAmmoWorldItemDefinitionParameters
{
    UObject* AmmoDefinition;
};
