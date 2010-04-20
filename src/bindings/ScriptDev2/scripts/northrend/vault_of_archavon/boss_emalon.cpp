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
SDName: Boss_Emalon_The_Storm_Watcher
SD%Complete: 0%
SDComment:
SDCategory: Vault of Archavon
EndScriptData */

#include "precompiled.h"
#include "vault_of_archavon.h"

enum
{
    // Emalon spells
    SPELL_CHAIN_LIGHTNING_N                 = 64213,
    SPELL_CHAIN_LIGHTNING_H                 = 64215,
    SPELL_LIGHTNING_NOVA_N                  = 64216,
    SPELL_LIGHTNING_NOVA_H                  = 65279,
    SPELL_OVERCHARGE                        = 64379,        //This spell is used by Time Warder, and temporary by Emalon, because 64218 is bugged
    SPELL_BERSERK                           = 26662,

    // Tempest Minion spells
    SPELL_SHOCK                             = 64363,
    SPELL_OVERCHARGED_BLAST                 = 64219,
    SPELL_OVERCHARGED                       = 64217
};

struct MANGOS_DLL_DECL npc_tempest_minionAI : public ScriptedAI
{
    npc_tempest_minionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_fDefaultX = me->GetPositionX();
        m_fDefaultY = me->GetPositionY();
        m_fDefaultZ = me->GetPositionZ();
        m_fDefaultO = me->GetOrientation();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiEvadeCheckCooldown;
	MobEventTasks Tasks;

    uint32 m_uiShockTimer;
    uint32 m_uiRespawnTimer;
    uint32 m_uiOverchargedStacksCheckTimer;
    bool m_bDead;
    bool m_bTimeToDie;
    float m_fDefaultX;
    float m_fDefaultY;
    float m_fDefaultZ;
    float m_fDefaultO;

    void Init()
    {
        m_uiEvadeCheckCooldown = 2000;
        m_bDead = false;
        m_bTimeToDie = false;
        m_uiRespawnTimer = 4000;
        m_uiOverchargedStacksCheckTimer = 2000;

        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetStandState(UNIT_STAND_STATE_STAND);
        me->SetVisibility(VISIBILITY_ON);
    }

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_SHOCK,urand(8000,12000),8000,4000,TARGET_MAIN);
        Init();
    }

    void Aggro(Unit* pWho)
    {
        me->CallForHelp(80.0f);
    }

    void FakeDeath()
    {
        m_bDead = true;
        m_bTimeToDie = false;
        m_uiRespawnTimer = 4000;
        me->InterruptNonMeleeSpells(false);
        me->SetHealth(0);
        me->StopMoving();
        me->ClearComboPointHolders();
        me->RemoveAllAurasOnDeath();
        me->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        me->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->ClearAllReactives();
        me->SetUInt64Value(UNIT_FIELD_TARGET,0);
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveIdle();
        me->SetStandState(UNIT_STAND_STATE_DEAD);
        me->GetMap()->CreatureRelocation(me, m_fDefaultX, m_fDefaultY, m_fDefaultZ, m_fDefaultO);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (uiDamage < me->GetHealth())
            return;

        if (m_pInstance && (m_pInstance->GetData(TYPE_EMALON) != DONE))
        {
            uiDamage = 0;
            FakeDeath();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
		if (!CanDoSomething())
            return;

        if (m_uiEvadeCheckCooldown < uiDiff)
        {
            Creature* pEmalon = (Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_EMALON));
            if ((pEmalon && pEmalon->IsInEvadeMode()) || (me->GetDistance2d(-219.119f, -289.037f) > 80.0f))
            {
                EnterEvadeMode();
                return;
            }
            m_uiEvadeCheckCooldown = 2000;
        }
        else
            m_uiEvadeCheckCooldown -= uiDiff;

        if (m_bTimeToDie)
        {
            FakeDeath();
            return;
        }

        if (m_bDead)
        {
            if (m_uiRespawnTimer < uiDiff)
            {
                me->SetHealth(me->GetMaxHealth());
                me->SetVisibility(VISIBILITY_OFF);
                Init();
				Tasks.Speak(CHAT_TYPE_BOSS_EMOTE,0,"Un seide de la tempete vient defendre Emalon !");
                me->SetInCombatWithZone();
                DoResetThreat();
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    me->GetMotionMaster()->MoveChase(pTarget);

            }
            else
                m_uiRespawnTimer -= uiDiff;

            return;
        }

        if (m_uiOverchargedStacksCheckTimer < uiDiff)
        {
            m_uiOverchargedStacksCheckTimer = 2000;
            Aura* pAuraOvercharged = me->GetAura(SPELL_OVERCHARGED, 0);
            if(pAuraOvercharged && pAuraOvercharged->GetStackAmount() >= 10)
            {
                DoCast(me, SPELL_OVERCHARGED_BLAST);
                m_bTimeToDie = true;
                return;
            }
        }
        else
            m_uiOverchargedStacksCheckTimer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};


struct MANGOS_DLL_DECL boss_emalonAI : public ScriptedAI
{
    boss_emalonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint32 m_uiEvadeCheckCooldown;
	MobEventTasks Tasks;

    uint64 m_auiTempestMinionGUID[4];
    uint32 m_uiChainLightningCount;
    uint32 m_uiOverchargeTimer;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_BERSERK,360000,30000,0,TARGET_ME);
		if(m_bIsRegularMode)
		{
			Tasks.AddEvent(SPELL_LIGHTNING_NOVA_N,20000,45000,0,TARGET_ME);
			Tasks.AddEvent(SPELL_CHAIN_LIGHTNING_N,15000,10000,5000);
		}
		else
		{
			Tasks.AddEvent(SPELL_LIGHTNING_NOVA_H,20000,45000,0,TARGET_ME);
			Tasks.AddEvent(SPELL_CHAIN_LIGHTNING_H,15000,10000,5000);
		}
        m_uiEvadeCheckCooldown = 2000;
        memset(&m_auiTempestMinionGUID, 0, sizeof(m_auiTempestMinionGUID));
        m_uiChainLightningCount = 0;
        m_uiOverchargeTimer = 45000;

        if (m_pInstance)
        {
            m_auiTempestMinionGUID[0] = m_pInstance->GetData64(DATA_TEMPEST_MINION_1);
            m_auiTempestMinionGUID[1] = m_pInstance->GetData64(DATA_TEMPEST_MINION_2);
            m_auiTempestMinionGUID[2] = m_pInstance->GetData64(DATA_TEMPEST_MINION_3);
            m_auiTempestMinionGUID[3] = m_pInstance->GetData64(DATA_TEMPEST_MINION_4);
        }

        for (uint8 i=0; i<4; ++i)
        {
            Creature* pMinion = (Creature*)Unit::GetUnit((*me), m_auiTempestMinionGUID[i]);
            if (pMinion && pMinion->isDead())
                pMinion->Respawn();
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_EMALON, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
        {
            m_auiTempestMinionGUID[0] = m_pInstance->GetData64(DATA_TEMPEST_MINION_1);
            m_auiTempestMinionGUID[1] = m_pInstance->GetData64(DATA_TEMPEST_MINION_2);
            m_auiTempestMinionGUID[2] = m_pInstance->GetData64(DATA_TEMPEST_MINION_3);
            m_auiTempestMinionGUID[3] = m_pInstance->GetData64(DATA_TEMPEST_MINION_4);
			for (uint8 i=0; i<4; ++i)
			{
				if (Creature *pMinion = (Creature*)Unit::GetUnit((*me), m_auiTempestMinionGUID[i]))
					if(pMinion->isAlive())
						pMinion->AddThreat(me->getVictim());
			}
        }

        me->CallForHelp(80.0f);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_EMALON, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_EMALON, DONE);
        for (uint8 i=0; i<4; ++i)
        {
            Creature *pMinion = (Creature*)Unit::GetUnit((*me), m_auiTempestMinionGUID[i]);
            if (pMinion)
			{
				Tasks.Kill(pMinion);
				pMinion->SetRespawnDelay(7*RESPAWN_ONE_DAY*1000);
			}
        }
		GiveEmblemsToGroup((m_bIsRegularMode) ? CONQUETE : VAILLANCE ,3);
    }

	void DamageDeal(Unit* pDoneTo, uint32& uiDamage)
	{
		if(uiDamage > 16000 && pDoneTo != me->getVictim())
		{
			if(me->GetDistance2d(pDoneTo) < 5.0f)
				uiDamage = uiDamage;
			else if(me->GetDistance2d(pDoneTo) < 9.0f)
				uiDamage = uiDamage * 90 / 100;
			else if(me->GetDistance2d(pDoneTo) < 13.0f)
				uiDamage = uiDamage * 75 / 100;
			else if(me->GetDistance2d(pDoneTo) < 19.0f)
				uiDamage = uiDamage * 65 / 100;
			else if(me->GetDistance2d(pDoneTo) < 25.0f)
				uiDamage = uiDamage * 50 / 100;
			else if(me->GetDistance2d(pDoneTo) < 32.0f)
				uiDamage = uiDamage * 30 / 100;
			else if(me->GetDistance2d(pDoneTo) < 40.0f)
				uiDamage = uiDamage * 20 / 100;
			else if(me->GetDistance2d(pDoneTo) < 50.0f)
				uiDamage = uiDamage * 10 / 100;
			else
				uiDamage = 0;
		}
	}

    void UpdateAI(const uint32 uiDiff)
    {
        if (CanDoSomething())
            return;

        if (m_uiEvadeCheckCooldown < uiDiff)
        {
            if (me->GetDistance2d(-219.119f, -289.037f) > 80.0f)
                EnterEvadeMode();
            me->CallForHelp(80.0f);
            m_uiEvadeCheckCooldown = 2000;
        }
        else
            m_uiEvadeCheckCooldown -= uiDiff;

        if (m_uiOverchargeTimer < uiDiff)
        {
            Creature* pMinion = (Creature*)Unit::GetUnit((*me), m_auiTempestMinionGUID[rand()%4]);
            if(pMinion && pMinion->isAlive())
            {
				Tasks.Speak(CHAT_TYPE_BOSS_EMOTE,0,"Emalon surchage un seide de la tempete !");
                pMinion->SetHealth(pMinion->GetMaxHealth());
                pMinion->CastSpell(pMinion, SPELL_OVERCHARGE, false);
            }
            m_uiOverchargeTimer = 45000;
        }
        else
            m_uiOverchargeTimer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_tempest_warderAI : public ScriptedAI
{
    npc_tempest_warderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
	MobEventTasks Tasks;

    bool m_bOvercharged;
    uint32 m_uiOverchargedStacksCheckTimer;
    bool m_bTimeToDie;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_SHOCK,urand(8000,12000),8000,4000,TARGET_MAIN);
        m_bOvercharged = false;
        uint32 m_uiOverchargedStacksCheckTimer = 2000;
        m_bTimeToDie = false;
    }

    void Aggro(Unit* pWho) {}

    void UpdateAI(const uint32 uiDiff)
    {  
        if (CanDoSomething())
            return;

        if (m_bTimeToDie)
        {
			Tasks.Kill(me);
            return;
        }

        if (!m_bOvercharged && ((me->GetHealth()*100 / me->GetMaxHealth()) < 37))
        {
            DoCast(me, SPELL_OVERCHARGE);
            m_bOvercharged = true;
        }

        if (m_bOvercharged)
        {
            if (m_uiOverchargedStacksCheckTimer < uiDiff)
            {
                m_uiOverchargedStacksCheckTimer = 2000;
                Aura* pAuraOvercharged = me->GetAura(SPELL_OVERCHARGED, 0);
                if(pAuraOvercharged && pAuraOvercharged->GetStackAmount() >= 10)
                {
                    DoCast(me, SPELL_OVERCHARGED_BLAST);
                    m_bTimeToDie = true;
                    return;
                }
            }
            else
                m_uiOverchargedStacksCheckTimer -= uiDiff;
        }

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_emalonAI(Creature* pCreature)
{
    return new boss_emalonAI(pCreature);
}

CreatureAI* GetAI_npc_tempest_minionAI(Creature* pCreature)
{
    return new npc_tempest_minionAI(pCreature);
}

CreatureAI* GetAI_npc_tempest_warderAI(Creature* pCreature)
{
    return new npc_tempest_warderAI(pCreature);
}

void AddSC_boss_emalon()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_emalon";
    newscript->GetAI = &GetAI_boss_emalonAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tempest_minion";
    newscript->GetAI = &GetAI_npc_tempest_minionAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tempest_warder";
    newscript->GetAI = &GetAI_npc_tempest_warderAI;
    newscript->RegisterSelf();
}
