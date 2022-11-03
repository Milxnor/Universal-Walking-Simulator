#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#define _USE_MATH_DEFINES

#include <math.h>
#include <random>
#include <fstream>

static FORCEINLINE bool IsNaN(float A)
{
	return ((*(uint32_t*)&A) & 0x7FFFFFFF) > 0x7F800000;
}

static FORCEINLINE bool IsFinite(float A)
{
	return ((*(uint32_t*)&A) & 0x7F800000) != 0x7F800000;
}

struct FVector
{
	float X;
	float Y;
	float Z;

	FVector() : X(0), Y(0), Z(0) {}
	FVector(float x, float y, float z) : X(x), Y(y), Z(z) {}
	FVector(int x, int y, int z) : X(x), Y(y), Z(z) {}

	FVector operator+(const FVector& A)
	{
		return FVector{ this->X + A.X, this->Y + A.Y, this->Z + A.Z };
	}

	FVector operator-(const FVector& A)
	{
		return FVector{ this->X - A.X, this->Y - A.Y, this->Z - A.Z };
	}

	FVector operator*(const float A)
	{
		return FVector{ this->X * A, this->Y * A, this->Z * A };
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

	std::string Describe()
	{
		return std::format("{} {} {}", std::to_string(X), std::to_string(Y), std::to_string(Z));
	}

	FORCEINLINE bool ContainsNaN() const
	{
		return (!IsFinite(X) ||
			!IsFinite(Y) ||
			!IsFinite(Z));
	}
};


enum class EFortEmotePlayMode : uint8_t
{
	CheckIfOwned = 0,
	ForcePlay = 1,
	EFortEmotePlayMode_MAX = 2,
};







enum class EServerStatus : uint8_t
{
	Up = 0,
	Down = 1,
	Loading = 2,
	Restarting = 3
};

enum class EMontagePlayReturnType : uint8_t
{
	MontageLength = 0,
	Duration = 1,
	EMontagePlayReturnType_MAX = 2
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

static FORCEINLINE void SinCos(float* ScalarSin, float* ScalarCos, float  Value)
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
	*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

	// 10-degree minimax approximation
	float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
	*ScalarCos = sign * p;
}

template <class  T>
static auto DegreesToRadians(T const& DegVal) -> decltype(DegVal* (M_PI / 180.f))
{
	return DegVal * (M_PI / 180.f);
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

enum class EAthenaGamePhaseStep : uint8_t
{
	None = 0,
	Setup = 1,
	Warmup = 2,
	GetReady = 3,
	BusLocked = 4,
	BusFlying = 5,
	StormForming = 6,
	StormHolding = 7,
	StormShrinking = 8,
	Countdown = 9,
	FinalCountdown = 10,
	EndGame = 11,
	Count = 12,
	EAthenaGamePhaseStep_MAX = 13,
};

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

struct FAircraftFlightInfo
{
public:
	FVector                FlightStartLocation;                               // 0x0(0xC)(Edit, BlueprintVisible, BlueprintReadOnly, NoDestructor, NativeAccessSpecifierPublic)
	FRotator                              FlightStartRotation;                               // 0xC(0xC)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, NativeAccessSpecifierPublic)
	float                                        FlightSpeed;                                       // 0x18(0x4)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                        TimeTillFlightEnd;                                 // 0x1C(0x4)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                        TimeTillDropStart;                                 // 0x20(0x4)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
	float                                        TimeTillDropEnd;                                   // 0x24(0x4)(Edit, BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData, NoDestructor, HasGetValueTypeHash, NativeAccessSpecifierPublic)
};

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

enum ERichCurveExtrapolation
{
	RCCE_Cycle,
	RCCE_CycleWithOffset,
	RCCE_Oscillate,
	RCCE_Linear,
	RCCE_Constant,
	RCCE_None,
};

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

enum ERichCurveInterpMode
{
	/** Use linear interpolation between values. */
	RCIM_Linear,
	/** Use a constant value. Represents stepped values. */
	RCIM_Constant,
	/** Cubic interpolation. See TangentMode for different cubic interpolation options. */
	RCIM_Cubic,
	/** No interpolation. */
	RCIM_None
};

enum class EFortAbilityTargetDataPolicy : uint8_t
{
	ReplicateToServer = 0,
	SimulateOnServer = 1,
	EFortAbilityTargetDataPolicy_MAX = 2
};

void WriteToFile(const std::string& Text, const std::string& FileName = "DUMP.txt")
{
	std::ofstream stream(FileName, std::ios::out | std::ios::app);

	stream << Text << '\n';

	stream.close();
}

enum class EFortGameplayAbilityMontageSectionToPlay : uint8_t
{
	FirstSection = 0,
	RandomSection = 1,
	TestedRandomSection = 2,
	EFortGameplayAbilityMontageSectionToPlay_MAX = 3
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
	float result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + FASTASIN_HALF_PI;
	result *= root;  // acos(|x|)
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

struct FTransform // https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/Core/Public/Math/TransformNonVectorized.h#L28
{
	FQuat Rotation;
	FVector Translation;
	char pad_1C[0x4]; // Padding never changes
	FVector Scale3D = FVector{ 1, 1, 1 };
	char pad_2C[0x4];

	/* bool ContainsNaN() const
	{
		return (Translation.ContainsNaN() || Rotation.ContainsNaN() || Scale3D.ContainsNaN());
	} */
};

enum ESpawnActorCollisionHandlingMethod
{
	Undefined,
	AlwaysSpawn,
	AdjustIfPossibleButAlwaysSpawn,
	AdjustIfPossibleButDontSpawnIfColliding,
	DontSpawnIfColliding,
};

template<class TEnum>
struct TEnumAsByte // https://github.com/EpicGames/UnrealEngine/blob/4.21/Engine/Source/Runtime/Core/Public/Containers/EnumAsByte.h#L18
{
	uint8_t Value;

	TEnumAsByte(TEnum _value)
		: Value((uint8_t)_value)
	{
	}

	TEnumAsByte() : Value(0) {}

	TEnum Get()
	{
		return (TEnum)Value;
	}
};

struct bitfield
{
	uint8_t idk1;
	uint8_t idk2;
	uint8_t idk3;
	uint8_t idk4;
	uint8_t idk5;
	uint8_t idk6;
	uint8_t idk7;
	uint8_t idk8;

};

enum class EVehicleType
{
	Biplane,
	Cannon,
	Baller,
	Mech,
	Turret,
	Boat,
	Helicopter,
	Unknown
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

#pragma once
enum EPropertyFlags : unsigned __int64
{
	CPF_None = 0,

	CPF_Edit = 0x0000000000000001,	///< Property is user-settable in the editor.
	CPF_ConstParm = 0x0000000000000002,	///< This is a constant function parameter
	CPF_BlueprintVisible = 0x0000000000000004,	///< This property can be read by blueprint code
	CPF_ExportObject = 0x0000000000000008,	///< Object can be exported with actor.
	CPF_BlueprintReadOnly = 0x0000000000000010,	///< This property cannot be modified by blueprint code
	CPF_Net = 0x0000000000000020,	///< Property is relevant to network replication.
	CPF_EditFixedSize = 0x0000000000000040,	///< Indicates that elements of an array can be modified, but its size cannot be changed.
	CPF_Parm = 0x0000000000000080,	///< Function/When call parameter.
	CPF_OutParm = 0x0000000000000100,	///< Value is copied out after function call.
	CPF_ZeroConstructor = 0x0000000000000200,	///< memset is fine for construction
	CPF_ReturnParm = 0x0000000000000400,	///< Return value.
	CPF_DisableEditOnTemplate = 0x0000000000000800,	///< Disable editing of this property on an archetype/sub-blueprint
	//CPF_      						= 0x0000000000001000,	///< 
	CPF_Transient = 0x0000000000002000,	///< Property is transient: shouldn't be saved or loaded, except for Blueprint CDOs.
	CPF_Config = 0x0000000000004000,	///< Property should be loaded/saved as permanent profile.
	//CPF_								= 0x0000000000008000,	///< 
	CPF_DisableEditOnInstance = 0x0000000000010000,	///< Disable editing on an instance of this class
	CPF_EditConst = 0x0000000000020000,	///< Property is uneditable in the editor.
	CPF_GlobalConfig = 0x0000000000040000,	///< Load config from base class, not subclass.
	CPF_InstancedReference = 0x0000000000080000,	///< Property is a component references.
	//CPF_								= 0x0000000000100000,	///<
	CPF_DuplicateTransient = 0x0000000000200000,	///< Property should always be reset to the default value during any type of duplication (copy/paste, binary duplication, etc.)
	//CPF_								= 0x0000000000400000,	///< 
	//CPF_    							= 0x0000000000800000,	///< 
	CPF_SaveGame = 0x0000000001000000,	///< Property should be serialized for save games, this is only checked for game-specific archives with ArIsSaveGame
	CPF_NoClear = 0x0000000002000000,	///< Hide clear (and browse) button.
	//CPF_  							= 0x0000000004000000,	///<
	CPF_ReferenceParm = 0x0000000008000000,	///< Value is passed by reference; CPF_OutParam and CPF_Param should also be set.
	CPF_BlueprintAssignable = 0x0000000010000000,	///< MC Delegates only.  Property should be exposed for assigning in blueprint code
	CPF_Deprecated = 0x0000000020000000,	///< Property is deprecated.  Read it from an archive, but don't save it.
	CPF_IsPlainOldData = 0x0000000040000000,	///< If this is set, then the property can be memcopied instead of CopyCompleteValue / CopySingleValue
	CPF_RepSkip = 0x0000000080000000,	///< Not replicated. For non replicated properties in replicated structs 
	CPF_RepNotify = 0x0000000100000000,	///< Notify actors when a property is replicated
	CPF_Interp = 0x0000000200000000,	///< interpolatable property for use with matinee
	CPF_NonTransactional = 0x0000000400000000,	///< Property isn't transacted
	CPF_EditorOnly = 0x0000000800000000,	///< Property should only be loaded in the editor
	CPF_NoDestructor = 0x0000001000000000,	///< No destructor
	//CPF_								= 0x0000002000000000,	///<
	CPF_AutoWeak = 0x0000004000000000,	///< Only used for weak pointers, means the export type is autoweak
	CPF_ContainsInstancedReference = 0x0000008000000000,	///< Property contains component references.
	CPF_AssetRegistrySearchable = 0x0000010000000000,	///< asset instances will add properties with this flag to the asset registry automatically
	CPF_SimpleDisplay = 0x0000020000000000,	///< The property is visible by default in the editor details view
	CPF_AdvancedDisplay = 0x0000040000000000,	///< The property is advanced and not visible by default in the editor details view
	CPF_Protected = 0x0000080000000000,	///< property is protected from the perspective of script
	CPF_BlueprintCallable = 0x0000100000000000,	///< MC Delegates only.  Property should be exposed for calling in blueprint code
	CPF_BlueprintAuthorityOnly = 0x0000200000000000,	///< MC Delegates only.  This delegate accepts (only in blueprint) only events with BlueprintAuthorityOnly.
	CPF_TextExportTransient = 0x0000400000000000,	///< Property shouldn't be exported to text format (e.g. copy/paste)
	CPF_NonPIEDuplicateTransient = 0x0000800000000000,	///< Property should only be copied in PIE
	CPF_ExposeOnSpawn = 0x0001000000000000,	///< Property is exposed on spawn
	CPF_PersistentInstance = 0x0002000000000000,	///< A object referenced by the property is duplicated like a component. (Each actor should have an own instance.)
	CPF_UObjectWrapper = 0x0004000000000000,	///< Property was parsed as a wrapper class like TSubclassOf<T>, FScriptInterface etc., rather than a USomething*
	CPF_HasGetValueTypeHash = 0x0008000000000000,	///< This property can generate a meaningful hash value.
	CPF_NativeAccessSpecifierPublic = 0x0010000000000000,	///< Public native access specifier
	CPF_NativeAccessSpecifierProtected = 0x0020000000000000,	///< Protected native access specifier
	CPF_NativeAccessSpecifierPrivate = 0x0040000000000000,	///< Private native access specifier
	CPF_SkipSerialization = 0x0080000000000000,	///< Property shouldn't be serialized, can still be exported to text
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

enum class EFortGliderType : uint8_t
{
	Glider = 0,
	Umbrella = 1,
	EFortGliderType_MAX = 2,
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
	CHTYPE_None = 0,  // Invalid type.
	CHTYPE_Control = 1,  // Connection control.
	CHTYPE_Actor = 2,  // Actor-update channel.

	// @todo: Remove and reassign number to CHTYPE_Voice (breaks net compatibility)
	CHTYPE_File = 3,  // Binary file transfer.

	CHTYPE_Voice = 4,  // VoIP data channel
	CHTYPE_MAX = 8,  // Maximum.
};

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

using EFortTeam = uint8_t;

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
		Invalid = 0,		// This must be a client (which doesn't use this state) or uninitialized.
		LoggingIn = 1,		// The client is currently logging in.
		Welcomed = 2,		// Told client to load map and will respond with SendJoin
		ReceivedJoin = 3,		// NMT_Join received and a player controller has been created
		CleanedUp = 4			// Cleanup has been called at least once, the connection is considered abandoned/terminated/gone
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

enum class EFortQuickBars : uint8_t // This isn't always correct due to them adding Creative Quickbars but for our usage it's fine.
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

enum class EFortStructuralGridQueryResults : uint8_t
{
	CanAdd = 0,
	ExistingActor = 1,
	Obstructed = 2,
	NoStructuralSupport = 3,
	InvalidActor = 4,
	ReachedLimit = 5,
	NoEditPermission = 6,
	PatternNotPermittedByLayoutRequirement = 7,
	ResourceTypeNotPermittedByLayoutRequirement = 8,
	BuildingAtRequirementsDisabled = 9,
	BuildingOtherThanRequirementsDisabled = 10,
	EFortStructuralGridQueryResults_MAX = 11
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

enum class EFastArraySerializerDeltaFlags : uint8_t
{
	None,								//! No flags.
	HasBeenSerialized = 1 << 0,			//! Set when serialization at least once (i.e., this struct has been written or read).
	HasDeltaBeenRequested = 1 << 1,		//! Set if users requested Delta Serialization for this struct.
	IsUsingDeltaSerialization = 1 << 2,	//! This will remain unset until we've serialized at least once.
										//! At that point, this will be set if delta serialization was requested and
										//! we support it.
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
	bool                                               bIncludeWalls;                                            // 0x0000(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	bool                                               bIncludeFloors;                                           // 0x0001(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	bool                                               bIncludeFloorInTop;                                       // 0x0002(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	bool                                               bIncludeCenterCell;                                       // 0x0003(0x0001) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
};

struct FBuildingSupportCellIndex
{
	int                                                X;                                                        // 0x0000(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	int                                                Y;                                                        // 0x0004(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)
	int                                                Z;                                                        // 0x0008(0x0004) (Edit, BlueprintVisible, ZeroConstructor, IsPlainOldData)

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

struct FTimespan
{
	__int64 Ticks;
};

template <typename T>
static T* GetFromOffset(void* Instance, int Offset)
{
	return (T*)(__int64(Instance) + Offset);
}