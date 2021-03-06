/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Arathi Highlands
SD%Complete: 100
SDComment: Quest support: 665
SDCategory: Arathi Highlands
EndScriptData */

/* ContentData
npc_professor_phizzlethorpe
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_professor_phizzlethorpe
######*/

enum
{
    SAY_PROGRESS_1          = -1000264,
    SAY_PROGRESS_2          = -1000265,
    SAY_PROGRESS_3          = -1000266,
    EMOTE_PROGRESS_4        = -1000267,
    SAY_AGGRO               = -1000268,
    SAY_PROGRESS_5          = -1000269,
    SAY_PROGRESS_6          = -1000270,
    SAY_PROGRESS_7          = -1000271,
    EMOTE_PROGRESS_8        = -1000272,
    SAY_PROGRESS_9          = -1000273,

    QUEST_SUNKEN_TREASURE   = 665,
    ENTRY_VENGEFUL_SURGE    = 2776
};

struct MANGOS_DLL_DECL npc_professor_phizzlethorpeAI : public npc_escortAI
{
    npc_professor_phizzlethorpeAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch(uiPointId)
        {
            case 4: DoScriptText(SAY_PROGRESS_2, me, pPlayer); break;
            case 5: DoScriptText(SAY_PROGRESS_3, me, pPlayer); break;
            case 8: DoScriptText(EMOTE_PROGRESS_4, me); break;
            case 9:
                me->SummonCreature(ENTRY_VENGEFUL_SURGE, -2056.41, -2144.01, 20.59, 5.70, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);
                me->SummonCreature(ENTRY_VENGEFUL_SURGE, -2050.17, -2140.02, 19.54, 5.17, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);
                break;
            case 10: DoScriptText(SAY_PROGRESS_5, me, pPlayer); break;
            case 11:
                DoScriptText(SAY_PROGRESS_6, me, pPlayer);
                SetRun();
                break;
            case 19: DoScriptText(SAY_PROGRESS_7, me, pPlayer); break;
            case 20:
                DoScriptText(EMOTE_PROGRESS_8, me);
                DoScriptText(SAY_PROGRESS_9, me, pPlayer);
                pPlayer->GroupEventHappens(QUEST_SUNKEN_TREASURE, me);
                break;
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(me);
    }
};

bool QuestAccept_npc_professor_phizzlethorpe(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SUNKEN_TREASURE)
    {
        pCreature->setFaction(FACTION_ESCORT_N_NEUTRAL_PASSIVE);
        DoScriptText(SAY_PROGRESS_1, pCreature, pPlayer);

        if (npc_professor_phizzlethorpeAI* pEscortAI = dynamic_cast<npc_professor_phizzlethorpeAI*>(pCreature->AI()))
            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest, true);
    }
    return true;
}

CreatureAI* GetAI_npc_professor_phizzlethorpe(Creature* pCreature)
{
    return new npc_professor_phizzlethorpeAI(pCreature);
}

void AddSC_arathi_highlands()
{
    Script * newscript;

    newscript = new Script;
    newscript->Name = "npc_professor_phizzlethorpe";
    newscript->GetAI = &GetAI_npc_professor_phizzlethorpe;
    newscript->pQuestAccept = &QuestAccept_npc_professor_phizzlethorpe;
    newscript->RegisterSelf();
}
