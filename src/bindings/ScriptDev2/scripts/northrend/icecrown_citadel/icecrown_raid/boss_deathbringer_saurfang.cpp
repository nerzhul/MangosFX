#include "precompiled.h"
#include "icecrown_citadel.h"

enum
{
        //common
        SPELL_BERSERK                           = 47008,
        //yells
        //summons
        NPC_BLOOD_BEASTS                        = 38508,
        //Abilities
        SPELL_BLOOD_LINK                        = 72178,
        SPELL_BLOOD_POWER                       = 72371,
        SPELL_MARK                              = 72293,
        SPELL_FRENZY                            = 72737,
        SPELL_BOILING_BLOOD                     = 72385,
        SPELL_BLOOD_NOVA                        = 72380,
        SPELL_RUNE_OF_BLOOD                     = 72408,
        SPELL_CALL_BLOOD_BEASTS                 = 72173,
        SPELL_SCENT_OF_BLOOD                    = 72769, // Only in heroic
        SPELL_RESISTANT_SKIN                    = 72723,

        SPELL_BEAST_1                           = 72176,
        SPELL_BEAST_2                           = 72723,
        SPELL_BEAST_3                           = 21150,

};

struct MANGOS_DLL_DECL boss_saurfangAI : public LibDevFSAI
{
    boss_saurfangAI(Creature* pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEnrageTimer(480000);
		me->setPowerType(POWER_ENERGY);
		switch(m_difficulty)
		{
		case RAID_DIFFICULTY_10MAN_NORMAL:
		case RAID_DIFFICULTY_10MAN_HEROIC:
			AddNear15mSummonEvent(NPC_BLOOD_BEASTS,35000,35000,0,0,2);
			break;
		case RAID_DIFFICULTY_25MAN_NORMAL:
		case RAID_DIFFICULTY_25MAN_HEROIC:
			AddNear15mSummonEvent(NPC_BLOOD_BEASTS,35000,35000,0,0,5);
			break;
		}
    }

    void Reset()
    {
		ResetTimers();
		CleanMyAdds();
    }

    void Aggro(Unit* pWho)
    {
        if (pInstance)
            pInstance->SetData(TYPE_SAURFANG, IN_PROGRESS);
    }

	void KilledUnit(Unit* who)
	{
	}

    void JustDied(Unit* pKiller)
    {
        if (pInstance)
            pInstance->SetData(TYPE_SAURFANG, DONE);

		switch(m_difficulty)
		{
			case RAID_DIFFICULTY_10MAN_NORMAL:
				GiveEmblemsToGroup(TRIOMPHE,3);
				break;
			case RAID_DIFFICULTY_25MAN_NORMAL:
				GiveEmblemsToGroup(GIVRE,3);
				break;
			case RAID_DIFFICULTY_10MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,2);
				GiveEmblemsToGroup(TRIOMPHE,2);
				break;
			case RAID_DIFFICULTY_25MAN_HEROIC:
				GiveEmblemsToGroup(GIVRE,4);
				break;
		}
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_SAURFANG, FAIL);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;

		if(CheckPercentLife(30) && !me->HasAura(SPELL_FRENZY))
		{
			me->CastStop();
			DoCastMe(SPELL_FRENZY);
		}

		UpdateEvent(diff);
    }
};

CreatureAI* GetAI_boss_saurfang(Creature* pCreature)
{
    return new boss_saurfangAI(pCreature);
}

void AddSC_ICC_Saurfang()
{
	Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_saurfang";
    NewScript->GetAI = &GetAI_boss_saurfang;
    NewScript->RegisterSelf();
}