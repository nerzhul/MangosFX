#include "precompiled.h"
#include "ObjectMgr.h"

#define		NPC_PEON		25270
#define		NPC_SPIDER		24566
#define		NPC_COCOON		25284
#define		SPELL_POTION	33934

struct MANGOS_DLL_DECL cocoon_warsongAI : public ScriptedAI
{
	CreatureInfo const* cInfo;
	cocoon_warsongAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

	void Reset()
    {
    }

	void Aggro(Unit* pWho)
    {
		me->AddMonsterMoveFlag(MONSTER_MOVE_NONE);
	}

	void DamageTaken(Unit *done_by, uint32 &damage)	{}

	void JustDied(Unit* pKiller)
	{
		uint8 i = (rand()%2);
		if(i == 0)
		{
			me->SummonCreature(NPC_PEON,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),0.0f,TEMPSUMMON_TIMED_DESPAWN,120000);
			if(pKiller->GetTypeId() == TYPEID_PLAYER)
				((Player*)pKiller)->KilledMonsterCredit(NPC_PEON,0);
		}
		else
			me->SummonCreature(NPC_SPIDER,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ()+ 1,0.0f,TEMPSUMMON_TIMED_DESPAWN,120000);

		
	}

	void UpdateAI(const uint32 diff) {}
};


CreatureAI* GetAI_cocoon_warsong(Creature* pCreature)
{
    return new cocoon_warsongAI(pCreature);
}

void AddSC_cocoon_warsong()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "cocoon_warsong";
    newscript->GetAI = &GetAI_cocoon_warsong;
    newscript->RegisterSelf();
}