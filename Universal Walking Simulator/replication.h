#include <patterns.h>
#include <Net/funcs.h>

int32_t ServerReplicateActors(UObject* NetDriver)
{
#if defined(T_F) || defined(S_F) || defined(S_T)
	// Supports replicationgraph

	auto ReplicationDriver = NetDriver->Member<UObject*>(_("ReplicationDriver"));

	if (ReplicationDriver && *ReplicationDriver)
	{
		if (RepGraph_ServerReplicateActors)
		{
			RepGraph_ServerReplicateActors(*ReplicationDriver);
			return 50; // pretty sure replicationdriver ticks like 50 connections max
		}
	}

	return 0;
#endif


}