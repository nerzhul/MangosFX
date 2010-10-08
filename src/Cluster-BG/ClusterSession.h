#ifndef __CLUSTER_SESSION_H_
#define __CLUSTER_SESSION_H_

#include <Common.h>
#include <WorldPacket.h>
#include <cIncludes.h>
#include <ace/Atomic_Op.h>

class ClusterSession: public ACE_Based::Runnable
{
	public:
		ClusterSession();
		~ClusterSession();
		void SetParams(SocketTCP* sock, std::string str);
		void SetRPC(bool rpc) { isRPC = rpc; }
		void QueuePacket(WorldPacket* new_packet);
		void SendPacket(const Packet* pkt);
		void SendNullPacket();
		void SendBoolPacket(bool _bool);
		void SendUint32(uint32 value);

		void Update();

		// Handlers
		void Handle_Null(WorldPacket& pck) {}
		void Handle_ClientSide(WorldPacket &pck) {}
		void Handle_Unhandled(WorldPacket& pck) { error_log("Unhandled Packet opcode 0x%x recvd",pck.GetOpcode()); }

		void Handle_ClusterPing(WorldPacket &pck);

		// BG
		void Handle_GenerateBGId(WorldPacket &pck);
		void Handle_GetRewardPlayers(WorldPacket &pck);
		void Handle_BGGetOfflineTime(WorldPacket &pck);
		void Handle_BGRemovePlayerAtLeave(WorldPacket &pck);
		void Handle_IsInBG(WorldPacket &pck);
		void Handle_GetBgTeam(WorldPacket &pck);
		void Handle_Updt_Plr(WorldPacket &pck);
		void Handle_GetBGCommand(WorldPacket &pck);
		void Handle_BGGetPlayerNumberByTeam(WorldPacket &pck);
		void Handle_BGSetArenaTeam(WorldPacket &pck);
		void Handle_BGGetArenaTeam(WorldPacket &pck);

		void SendMonoPlayerPacket(uint64 guid, WorldPacket &pck);
		void SendMultiPlayerPacket(std::vector<uint64> GUIDs, WorldPacket &pck);
	protected:
		void run();
	private:
		bool CheckState(Socket::Status st);
		void HandlePacket(Packet* pck);
		void SendClusterIdentity();
		void SendPing();

		bool mustStop;
		bool isRPC;
		SocketTCP* m_sock;
		std::string m_addr;

		ACE_Based::LockedQueue<WorldPacket*, ACE_Thread_Mutex> _recvQueue;
};
#endif
