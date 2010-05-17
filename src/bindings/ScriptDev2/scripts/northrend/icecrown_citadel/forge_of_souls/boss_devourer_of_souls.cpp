/* LibDevFS by Frost Sapphire Studios */

#include "precompiled.h"
#include "forge_of_souls.h"

enum spells
{
	SPELL_MIRRORED_SOUL			=	69051,
	SPELL_PHANTOM_BLAST			=	68982,
	SPELL_UNLEASHED_SOULS		=	68939,
	SPELL_WAILING_SOULS_START	=	68912,
	SPELL_WELL_OF_SOULS			=	68820,
	SPELL_WAILING_SOULS_BEAM    =	68875,
	SPELL_WAILING_SOULS         =	68873
};


enum eEnum
{
    ACHIEV_THREE_FACED          = 4523,
    DISPLAY_ANGER               = 30148,
    DISPLAY_SORROW              = 30149,
    DISPLAY_DESIRE              = 30150,
};

struct
{
    uint32 entry[2];
    float movePosition[4];
} outroPositions[] =
{
    { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE, { 5590.47, 2427.79, 705.935, 0.802851 } },
    { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE, { 5593.59, 2428.34, 705.935, 0.977384 } },
    { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE, { 5600.81, 2429.31, 705.935, 0.890118 } },
    { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE, { 5600.81, 2421.12, 705.935, 0.890118 } },
    { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE, { 5601.43, 2426.53, 705.935, 0.890118 } },
    { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE, { 5601.55, 2418.36, 705.935, 1.15192 } },
    { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE, { 5598, 2429.14, 705.935, 1.0472 } },
    { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE, { 5594.04, 2424.87, 705.935, 1.15192 } },
    { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE, { 5597.89, 2421.54, 705.935, 0.610865 } },
    { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE, { 5598.57, 2434.62, 705.935, 1.13446 } },
    { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE, { 5585.46, 2417.99, 705.935, 1.06465 } },
    { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE, { 5605.81, 2428.42, 705.935, 0.820305 } },
    { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE, { 5591.61, 2412.66, 705.935, 0.925025 } },
    { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE, { 5593.9, 2410.64, 705.935, 0.872665 } },
    { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE, { 5586.76, 2416.73, 705.935, 0.942478 } },
    { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_3_HORDE, { 5592.23, 2419.14, 705.935, 0.855211 } },
    { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_3_HORDE, { 5594.61, 2416.87, 705.935, 0.907571 } },
    { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_3_HORDE, { 5589.77, 2421.03, 705.935, 0.855211 } },

    { NPC_KORELN, NPC_LORALEN, { 5602.58, 2435.95, 705.935, 0.959931 } },
    { NPC_ELANDRA, NPC_KALIRA, { 5606.13, 2433.16, 705.935, 0.785398 } },
    { NPC_JAINA_PART2, NPC_SYLVANAS_PART2, { 5606.12, 2436.6, 705.935, 0.890118 } },

    { 0, 0, { 0, 0, 0, 0 } }
};

struct MANGOS_DLL_DECL boss_devourer_of_soulsAI : public LibDevFSAI
{
    boss_devourer_of_soulsAI(Creature *pCreature) : LibDevFSAI(pCreature)
    {
        InitInstance();
		AddEventOnTank(SPELL_PHANTOM_BLAST,5000,5000,1000);
		AddEvent(SPELL_WELL_OF_SOULS,30000,20000);
    }

	uint64 uiMirroredSoulTarget;
	bool bThreeFaceAchievement;

	uint32 Mirror_Timer;
	uint32 Unleashed_Timer;
	uint32 Face_Timer;
	uint32 Wailing_Timer;
	uint32 WailingTick_Timer;
	// wailing soul event
    float beamAngle;
    float beamAngleDiff;
    int8 wailingSoulTick;


    void Reset()
    {
		ResetTimers();
		me->SetDisplayId(DISPLAY_ANGER);
        me->SetReactState(REACT_AGGRESSIVE);
		if (pInstance)
            pInstance->SetData(DATA_DEVOURER_EVENT, NOT_STARTED);
		Mirror_Timer = 8000;
		Unleashed_Timer = 20000;
		Face_Timer = DAY*100;
		Wailing_Timer = urand(60000,70000);
		WailingTick_Timer = DAY*100;

    }

	void DamageTaken(Unit * /*pDoneBy*/, uint32 &uiDamage)
    {
        if (uiMirroredSoulTarget && me->HasAura(SPELL_MIRRORED_SOUL))
        {
            if (Unit *pPlayer = Unit::GetUnit(*me,uiMirroredSoulTarget))
            {
                if (Aura *pAura = pPlayer->GetAura(SPELL_MIRRORED_SOUL,0))
                {
                    int32 mirrorDamage = (uiDamage * 45)/100;
                    me->CastCustomSpell(pPlayer, 69034, &mirrorDamage, 0, 0, true);
//                    me->DealDamage(pPlayer, (uiDamage * 45)/100, 0, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW);
                }
                else
                    uiMirroredSoulTarget = 0;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!CanDoSomething())
            return;
	
		if(Mirror_Timer <= diff)
		{

			if (Unit *pTarget = GetRandomUnit())
            {
                uiMirroredSoulTarget = pTarget->GetGUID();
                DoCast(pTarget, SPELL_MIRRORED_SOUL);
//                DoScriptText(EMOTE_MIRRORED_SOUL, me);
            }
			Mirror_Timer = urand(15000,30000);
		}
		else
			Mirror_Timer -= diff;

		if(Unleashed_Timer <= diff)
		{
            DoCastRandom(SPELL_UNLEASHED_SOULS);
            me->SetDisplayId(DISPLAY_SORROW);
//            DoScriptText(RAND(SAY_FACE_ANGER_UNLEASH_SOUL,SAY_FACE_SORROW_UNLEASH_SOUL,SAY_FACE_DESIRE_UNLEASH_SOUL), me);
//            DoScriptText(EMOTE_UNLEASH_SOUL, me);
			Unleashed_Timer = 30000;
			Face_Timer = 5000;
		}
		else
			Unleashed_Timer -= diff;

		if(Face_Timer <= diff)
		{
			me->SetDisplayId(DISPLAY_ANGER);
			Face_Timer = DAY*100;
		}
		else
			Face_Timer -= diff;

		if(Wailing_Timer <= diff)
		{
			me->SetDisplayId(DISPLAY_DESIRE);
//            DoScriptText(RAND(SAY_FACE_ANGER_WAILING_SOUL,SAY_FACE_DESIRE_WAILING_SOUL), me);
//            DoScriptText(EMOTE_WAILING_SOUL, me);
            DoCastMe(SPELL_WAILING_SOULS_START);

            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                me->SetOrientation(me->GetAngle(pTarget));
                DoCastMe(SPELL_WAILING_SOULS_BEAM);
            }

            beamAngle = me->GetOrientation();

            beamAngleDiff = M_PI/30.0f; // PI/2 in 15 sec = PI/30 per tick
            if (urand(0,1))
                beamAngleDiff = -beamAngleDiff;

            me->InterruptNonMeleeSpells(false);
            me->SetReactState(REACT_PASSIVE);

            //Remove any target
            me->SetUInt64Value(UNIT_FIELD_TARGET, 0);

            me->GetMotionMaster()->Clear();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

            wailingSoulTick = 15;
			WailingTick_Timer = 3000;
			Wailing_Timer = DAY*100;
		}
		else
			Wailing_Timer -= diff;

		if(WailingTick_Timer <= diff)
		{
			beamAngle += beamAngleDiff;
            me->SetOrientation(beamAngle);
            me->StopMoving();

            DoCastMe(SPELL_WAILING_SOULS);

            if (--wailingSoulTick)
                WailingTick_Timer = 1000;
            else
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetDisplayId(DISPLAY_ANGER);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->GetMotionMaster()->MoveChase(me->getVictim());
                Wailing_Timer = urand(60000,70000);
				WailingTick_Timer = DAY*100;
            }
		}
		else
			WailingTick_Timer -= diff;

		UpdateEvent(diff);

        DoMeleeAttackIfReady();
    }

	void SpellHitTarget(Unit* /*pTarget*/, const SpellEntry *pSpell)
    {
        if (pSpell->Id == SPELL_PHANTOM_BLAST)
            bThreeFaceAchievement = false;
    }


    void JustDied(Unit* killer)
    {
		if (pInstance)
        {
            pInstance->SetData(DATA_DEVOURER_EVENT, DONE);


            /*if (bThreeFaceAchievement && m_difficulty)
                pInstance->DoCompleteAchievement(ACHIEV_THREE_FACED);*/

            int32 entryIndex;
			if (killer->GetTypeId() == TYPEID_PLAYER && ((Player*)killer)->GetBGTeam() == BG_TEAM_ALLIANCE)
                entryIndex = 0;
            else
                entryIndex = 1;

            /*for (int8 i = 0; outroPositions[i].entry[entryIndex] != 0; ++i)
            {
                if (Creature *pSummon = me->SummonCreature(outroPositions[i].entry[entryIndex], spawnPoint, TEMPSUMMON_DEAD_DESPAWN))
                {
                    pSummon->GetMotionMaster()->MovePoint(0, outroPositions[i].movePosition);

                    if (pSummon->GetEntry() == NPC_JAINA_PART2)
                        DoScriptText(SAY_JAINA_OUTRO, pSummon);
                    else if (pSummon->GetEntry() == NPC_SYLVANAS_PART2)
                        DoScriptText(SAY_SYLVANAS_OUTRO, pSummon);
                }
            }*/
        }
       GiveEmblemsToGroup(m_difficulty ? TRIOMPHE : 0,1,true);
    }
};

CreatureAI* GetAI_boss_devourer_of_souls(Creature* pCreature)
{
    return new boss_devourer_of_soulsAI (pCreature);
}

void AddSC_boss_devourer_of_souls()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_devourer_of_souls";
    newscript->GetAI = &GetAI_boss_devourer_of_souls;
    newscript->RegisterSelf();

}
