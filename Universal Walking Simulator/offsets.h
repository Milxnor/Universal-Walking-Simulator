#include <UE/structs.h>

namespace Offsets // ObjectName_MemberName
{
	short Controller_Pawn;
}

void InitializeOffsets()
{
	Offsets::Controller_Pawn = FindOffsetStruct("Controller", "Pawn");
}