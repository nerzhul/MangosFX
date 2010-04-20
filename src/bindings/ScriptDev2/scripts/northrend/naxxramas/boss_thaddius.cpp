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
SDName: Boss_Thaddius
SDAuthor: ckegg
SD%Complete: 0
SDComment: Placeholder. Includes Feugen & Stalagg.
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "def_naxxramas.h"

//Stalagg
#define SAY_STAL_AGGRO          -1533023
#define SAY_STAL_SLAY           -1533024
#define SAY_STAL_DEATH          -1533025
#define SPELL_POWERSURGE           28134
#define H_SPELL_POWERSURGE         54529

//Feugen
#define SAY_FEUG_AGGRO          -1533026
#define SAY_FEUG_SLAY           -1533027
#define SAY_FEUG_DEATH          -1533028
#define SPELL_STATICFIELD          28135
#define H_SPELL_STATICFIELD        54528

//both
#define SPELL_WARSTOMP             28125
#define SPELL_MAGNETIC_PULL        28338 // 54517?
#define SPELL_MAGNETIC_PULL_VISUAL 28337

//Thaddus
#define SAY_AGGRO               -1533029
#define SAY_KILL1               -1533031
#define SAY_KILL2               -1533032
#define SAY_KILL3               -1533033
#define SAY_KILL4               -1533034
#define SAY_DEATH               -1533035
#define SAY_SCREAM1             -1533036
#define SAY_SCREAM2             -1533037
#define SAY_SCREAM3             -1533038
#define SAY_SCREAM4             -1533039

#define SPELL_BALL_LIGHTNING                28299

#define SPELL_POLARITY_SHIFT                28089
#define SPELL_CHARGE_POSITIVE_DMGBUFF       29659
#define SPELL_CHARGE_POSITIVE_NEARDMG       28059 // *
#define SPELL_CHARGE_NEGATIVE_DMGBUFF       29660
#define SPELL_CHARGE_NEGATIVE_NEARDMG       28084 // *

#define SPELL_CHAIN_LIGHTNING               28167
#define H_SPELL_CHAIN_LIGHTNING             54531

#define SPELL_BESERK                        26662

//generic
#define C_TESLA_COIL                        16218           //the coils (emotes "Tesla Coil overloads!")

static float Thaddius_Coords = 3509.149f;


class MANGOS_DLL_DECL BugAura : public Aura
{
    public:
        BugAura(SpellEntry *spell, uint32 eff, int32 *bp, Unit *target, Unit *caster) : Aura(spell, eff, bp, target, caster, NULL)
            {}
};

struct MANGOS_DLL_DECL mob_stalaggAI : public ScriptedAI
{
    mob_stalaggAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
    bool m_bIsDeath;
    bool m_bIsHold;

    float HomeX, HomeY, HomeZ;

    uint32 DeathCheck_Timer;
    uint32 Hold_Timer;

	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_WARSTOMP,8000,8000,2000,TARGET_MAIN);
		if(m_bIsHeroic)
			Tasks.AddEvent(H_SPELL_POWERSURGE,12000,10000,5000,TARGET_ME);
    	m_bIsDeath = false;
    	m_bIsHold = false;

        HomeX = 3450.45;
        HomeY = -2931.42;
        HomeZ = 312.091;

        DeathCheck_Timer = 1000;
        Hold_Timer = 3000;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_STAL_AGGRO, me);
    }

    void SetHold()
    {
        me->StopMoving();
        me->GetMotionMaster()->Clear(false);
        me->GetMotionMaster()->MoveIdle();
        Hold_Timer = 3000;
        m_bIsHold = true;
    }

    void AttackStart(Unit* pWho)
    {
        if (m_bIsHold)
            return;

        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho, 0.0f);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);
            DoStartMovement(pWho);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!CanDoSomething())
            return;

        if (m_bIsHold)
        {
            if (Hold_Timer < uiDiff)
            {
                me->AI()->AttackStart(me->getVictim());
                DoStartMovement(me->getVictim());
                m_bIsHold = false;
            }else Hold_Timer -= uiDiff;
        }

        if (DeathCheck_Timer < uiDiff)
        {
            if (m_pInstance)
                if (Creature* pFeugen = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_FEUGEN))))
                {
                    if (!pFeugen->isAlive() && !m_bIsDeath)
                    {
                        m_bIsDeath = true;
                        DeathCheck_Timer = 5000;
                    }
                    else if (!pFeugen->isAlive() && m_bIsDeath)
                    {
                        pFeugen->Respawn();
                        m_bIsDeath = false;
                        DeathCheck_Timer = 1000;
                    }
                    else
                        DeathCheck_Timer = 1000;
                }
        }
		else 
			DeathCheck_Timer -= uiDiff;

        Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }

    void KilledUnit(Unit *victim)
    {
        if(victim == me)
            return;
        DoScriptText(SAY_STAL_SLAY, me);

    }
    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_STAL_DEATH, me);
    }
};

struct MANGOS_DLL_DECL mob_feugenAI : public ScriptedAI
{
    mob_feugenAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		m_bIsHeroic = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
    bool m_bIsDeath;
    bool m_bIsHold;

    float HomeX, HomeY, HomeZ;

    uint32 DeathCheck_Timer;
    uint32 Hold_Timer;

	MobEventTasks Tasks;

    void Reset()
    {
		Tasks.SetObjects(this,me);
		Tasks.AddEvent(SPELL_WARSTOMP,urand(8000,10000),8000,2000,TARGET_ME);
		Tasks.AddEvent(m_bIsHeroic ? H_SPELL_STATICFIELD : SPELL_STATICFIELD,urand(10000,15000),10000,5000,TARGET_ME);
        m_bIsDeath = false;
        m_bIsHold = false;

        HomeX = 3508.14;
        HomeY = -2988.65;
        HomeZ = 312.092;

        DeathCheck_Timer = 1000;
        Hold_Timer = 3000;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_FEUG_AGGRO, me);
    }

    void SetHold()
    {
        me->StopMoving();
        me->GetMotionMaster()->Clear(false);
        me->GetMotionMaster()->MoveIdle();
        Hold_Timer = 3000;
        m_bIsHold = true;
    }

    void AttackStart(Unit* pWho)
    {
        if (m_bIsHold)
            return;

        if (me->Attack(pWho, true))
        {
            me->AddThreat(pWho, 0.0f);
            me->SetInCombatWith(pWho);
            pWho->SetInCombatWith(me);
            DoStartMovement(pWho);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!CanDoSomething())
            return;

        if (m_bIsHold)
        {
            if (Hold_Timer < uiDiff)
            {
                me->AI()->AttackStart(me->getVictim());
                DoStartMovement(me->getVictim());
                m_bIsHold = false;
            }else Hold_Timer -= uiDiff;
        }

        if (DeathCheck_Timer < uiDiff)
        {
            if (m_pInstance)
                if (Creature* pStalagg = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_STALAGG))))
                {
                    if (!pStalagg->isAlive() && !m_bIsDeath)
                    {
                        m_bIsDeath = true;
                        DeathCheck_Timer = 5000;
                    }
                    else if (!pStalagg->isAlive() && m_bIsDeath)
                    {
                        pStalagg->Respawn();
                        m_bIsDeath = false;
                        DeathCheck_Timer = 1000;
                    }
                    else
                        DeathCheck_Timer = 1000;
                }
        }else DeathCheck_Timer -= uiDiff;

		Tasks.UpdateEvent(uiDiff);

        DoMeleeAttackIfReady();
    }

    void KilledUnit(Unit *victim)
    {
        if(victim == me)
            return;
        DoScriptText(SAY_FEUG_SLAY, me);
    }
    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_FEUG_DEATH, me);
    }
};

struct MANGOS_DLL_DECL boss_thaddiusAI : public ScriptedAI
{
    boss_thaddiusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;
    bool m_bIsActiveCheck;
    bool m_bIsActived;
    bool m_bIsPolarityShift;
    bool m_bInMeleeRange;

    uint32 Active_Timer;
    uint32 ChainLightning_Timer;
    uint32 PolarityShift_Timer;
    uint32 BallLightning_Timer;
    uint32 Enrage_Timer;
    uint32 Scream_Timer;
    uint32 RangeCheck_Timer;
	uint32 Buff_Timer;
	uint16 countbuff[2];

    uint32 SwitchTarget_Timer;

    void Reset()
    {
        m_bIsActiveCheck = false;
        m_bIsActived = false;
        m_bIsPolarityShift = false;
        m_bInMeleeRange = false;

        Active_Timer = 1000;
        ChainLightning_Timer = 15000;
        PolarityShift_Timer = 30000;
        BallLightning_Timer = 1000;
        Enrage_Timer = 300000;
        Scream_Timer = 60000+rand()%30000;
        RangeCheck_Timer = 2000;
		Buff_Timer = 2000;
		countbuff[0] = countbuff[1] = 0;

        SwitchTarget_Timer = 20000;

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (m_pInstance)
        {
            if (Creature* pStalagg = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_STALAGG))))
                if (!pStalagg->isAlive())
                    pStalagg->Respawn();

            if (Creature* pFeugen = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_FEUGEN))))
                if (!pFeugen->isAlive())
                    pFeugen->Respawn();

            m_pInstance->SetData(TYPE_THADDIUS, NOT_STARTED);
        }
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_AGGRO, me);

        if (!who || me->getVictim())
            return;

        if (who->isTargetableForAttack() && who->isInAccessablePlaceFor(me) && me->IsHostileTo(who))
            AttackStart(who);

        if(m_pInstance)
            m_pInstance->SetData(TYPE_THADDIUS, IN_PROGRESS);
    }

    void AttackStart(Unit* who)
    {
        if (!m_bIsActived)
            return;

        if (!who || who == me)
            return;

        if (me->Attack(who, true))
        {
            me->SetInCombatWithZone();
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            DoStartMovement(who);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bIsActiveCheck)
        {
            if (!m_bIsActived && Active_Timer < uiDiff)
            {
                m_bIsActived = true;
                Active_Timer = 1000;
            }else Active_Timer -= uiDiff;
        }
        else
        {
            if (Active_Timer < uiDiff)
            {
                if(m_pInstance)
                {
                    bool m_bIsAlive = false;
                    Creature* pStalagg;
                    Creature* pFeugen;
                    if (pStalagg = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_STALAGG))))
                        if (pStalagg->isAlive())
                            m_bIsAlive = true;
                    if (pFeugen = ((Creature*)Unit::GetUnit((*me), m_pInstance->GetData64(DATA_FEUGEN))))
                        if (pFeugen->isAlive())
                            m_bIsAlive = true;

                    if (!m_bIsAlive)
                    {
                        m_bIsActiveCheck = true;
                        Active_Timer = 15000;
                    }
                    else
                    {
                        if (pStalagg->isInCombat() && pFeugen->isInCombat())
                        {
                            if (SwitchTarget_Timer < uiDiff)
                            {
                                Unit* pStalaggTarget;
                                Unit* pFeugenTarget;
                                float StalaggTargetThreat;
                                float FeugenTargetThreat;

                                // Get Stalagg's target threat
                                if (pStalagg && pStalagg->isAlive())
                                {
                                    if (pStalaggTarget = pStalagg->getVictim())
                                        StalaggTargetThreat = me->getThreatManager().getThreat(pStalaggTarget);
                                }
                                // Get Feugen's target threat
                                if (pFeugen && pFeugen->isAlive())
                                {
                                    if (pFeugenTarget = pFeugen->getVictim())
                                        FeugenTargetThreat = me->getThreatManager().getThreat(pFeugenTarget);
                                }

                                // Switch Feugen's target from Stalagg
                                if (pStalagg && pStalagg->isAlive())
                                {
                                    if (pFeugen && pFeugen->isAlive())
                                    {
                                        ((mob_stalaggAI*)pStalagg->AI())->SetHold();
                                        pStalagg->CastSpell(pFeugenTarget, 54517, true);
                                        ((Player*)pFeugenTarget)->TeleportTo(pFeugenTarget->GetMapId(), pStalagg->GetPositionX(), pStalagg->GetPositionY(), pStalagg->GetPositionZ(), 0, TELE_TO_NOT_LEAVE_COMBAT);
                                        pStalagg->AddThreat(pFeugenTarget, FeugenTargetThreat);
                                        //pStalagg->AI()->AttackStart(pFeugenTarget);
                                    }
                                }
                                // Switch Stalagg's target from Feugen
                                if (pFeugen && pFeugen->isAlive())
                                {
                                    if (pStalagg && pStalagg->isAlive())
                                    {
                                        ((mob_feugenAI*)pFeugen->AI())->SetHold();
                                        pFeugen->CastSpell(pStalaggTarget, 54517, true);
                                        ((Player*)pStalaggTarget)->TeleportTo(pStalaggTarget->GetMapId(), pFeugen->GetPositionX(), pFeugen->GetPositionY(), pFeugen->GetPositionZ(), 0, TELE_TO_NOT_LEAVE_COMBAT);
                                        pFeugen->AddThreat(pStalaggTarget, StalaggTargetThreat);
                                        //pFeugen->AI()->AttackStart(pStalaggTarget);
                                    }
                                }

                                SwitchTarget_Timer = 20000;
                            }else SwitchTarget_Timer -= uiDiff;
                        }
                        else if (pStalagg->isInCombat() || pFeugen->isInCombat())
                        {
                            if (m_pInstance)
                                m_pInstance->SetData(TYPE_THADDIUS, IN_PROGRESS);
                        }
                        else if (!pStalagg->isInCombat() && !pFeugen->isInCombat())
                        {
                            if (m_pInstance)
                                m_pInstance->SetData(TYPE_THADDIUS, NOT_STARTED);
                        }

                        Active_Timer = 1000;
                    }
                }
            }
			else 
				Active_Timer -= uiDiff;
        }

        if (!CanDoSomething())
            return;

        if (ChainLightning_Timer < uiDiff)
        {
            DoCastMe( m_bIsHeroic ? H_SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING);
            ChainLightning_Timer = 12000+rand()%5000;
        }
		else 
			ChainLightning_Timer -= uiDiff;

        if(m_bIsPolarityShift)
        {
            // workaround for POLARITY_SHIFT
            if (PolarityShift_Timer < uiDiff)
            {
                Map *map = me->GetMap();
                Map::PlayerList const &PlayerList = map->GetPlayers();

                if (PlayerList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (i->getSource()->isAlive() && i->getSource()->isTargetableForAttack())
                    {

						uint8 _rand;
						if(!i->getSource()->HasAura(SPELL_CHARGE_NEGATIVE_NEARDMG) &&
							!i->getSource()->HasAura(SPELL_CHARGE_POSITIVE_DMGBUFF) &&
							!i->getSource()->HasAura(SPELL_CHARGE_POSITIVE_NEARDMG) &&
							!i->getSource()->HasAura(SPELL_CHARGE_NEGATIVE_DMGBUFF))
							_rand = urand(0,1);
						else
							_rand = urand(0,2);
                        switch(_rand)
                        {
                            case 0:
                                i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_NEGATIVE_NEARDMG);
								i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_POSITIVE_NEARDMG);
								i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_POSITIVE_DMGBUFF);
								i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_NEGATIVE_DMGBUFF);
                                i->getSource()->CastSpell(i->getSource(), SPELL_CHARGE_POSITIVE_NEARDMG, true);
                                break;
                            case 1:
                                i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_POSITIVE_NEARDMG);
								i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_NEGATIVE_NEARDMG);
								i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_POSITIVE_DMGBUFF);
								i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_NEGATIVE_DMGBUFF);
                                i->getSource()->CastSpell(i->getSource(), SPELL_CHARGE_NEGATIVE_NEARDMG, true);
                                break;
							case 2:
								// do nothing
								break;
                        }
                    }


                m_bIsPolarityShift = false;
                PolarityShift_Timer = 27000;
            }else PolarityShift_Timer -= uiDiff;
        }
        else
        {
            if(PolarityShift_Timer < uiDiff)
            {
      	        DoCastMe( SPELL_POLARITY_SHIFT); // need core support
                m_bIsPolarityShift = true;
                PolarityShift_Timer = 3000;
            }
			else 
				PolarityShift_Timer -= uiDiff;
        }

		if(Buff_Timer <= uiDiff)
		{
			Map *map = me->GetMap();
            Map::PlayerList const &PlayerList = map->GetPlayers();

            if (PlayerList.isEmpty())
                return;

			countbuff[0] = countbuff[1] = 0;
            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
			{
				if (i->getSource()->isAlive() && i->getSource()->isTargetableForAttack())
				{
					if(i->getSource()->GetPositionX() < Thaddius_Coords && (i->getSource()->HasAura(SPELL_CHARGE_NEGATIVE_NEARDMG) || i->getSource()->HasAura(SPELL_CHARGE_NEGATIVE_DMGBUFF)))
						countbuff[0]++;
					else if (i->getSource()->GetPositionX() > Thaddius_Coords && (i->getSource()->HasAura(SPELL_CHARGE_POSITIVE_NEARDMG) || i->getSource()->HasAura(SPELL_CHARGE_POSITIVE_DMGBUFF)))
						countbuff[1]++;
				}
			}

			for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
			{
				if (i->getSource()->isAlive() && i->getSource()->isTargetableForAttack())
				{
					if(i->getSource()->HasAura(SPELL_CHARGE_POSITIVE_NEARDMG) || i->getSource()->HasAura(SPELL_CHARGE_POSITIVE_DMGBUFF))
					{
						i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_POSITIVE_DMGBUFF);
						i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_POSITIVE_NEARDMG);
						if(i->getSource()->GetPositionX() > Thaddius_Coords)
						{
							for(int k=0;k < countbuff[1];k++)
							{
								SpellEntry *spell = (SpellEntry *)GetSpellStore()->LookupEntry(SPELL_CHARGE_POSITIVE_DMGBUFF);
								for (int j=0; j<3; ++j)
								{
									i->getSource()->AddAura(new BugAura(spell, j, NULL, i->getSource(), i->getSource()));
								}
							}
						}
						else
						{
							SpellEntry *spell = (SpellEntry *)GetSpellStore()->LookupEntry(SPELL_CHARGE_POSITIVE_NEARDMG);
							for (int j=0; j<3; ++j)
							{
								i->getSource()->AddAura(new BugAura(spell, j, NULL, i->getSource(), i->getSource()));
							}

						}
					}
					else if(i->getSource()->HasAura(SPELL_CHARGE_NEGATIVE_NEARDMG) || i->getSource()->HasAura(SPELL_CHARGE_NEGATIVE_DMGBUFF))
					{
						i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_NEGATIVE_DMGBUFF);
						i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_NEGATIVE_NEARDMG);
						if(i->getSource()->GetPositionX() < Thaddius_Coords)
						{
							for(int k=0;k < countbuff[0];k++)
							{
								SpellEntry *spell = (SpellEntry *)GetSpellStore()->LookupEntry(SPELL_CHARGE_NEGATIVE_DMGBUFF);
								for (int j=0; j<3; ++j)
								{
									i->getSource()->AddAura(new BugAura(spell, j, NULL, i->getSource(), i->getSource()));
								}
							}
						}
						else
						{
							SpellEntry *spell = (SpellEntry *)GetSpellStore()->LookupEntry(SPELL_CHARGE_NEGATIVE_NEARDMG);
							for (int j=0; j<3; ++j)
							{
								i->getSource()->AddAura(new BugAura(spell, j, NULL, i->getSource(), i->getSource()));
							}
						}
					}
				}
			}

			Buff_Timer = 2000;
		}
		else
			Buff_Timer -= uiDiff;

        if (Enrage_Timer < uiDiff)
        {
            DoCastMe( SPELL_BESERK);
            Enrage_Timer = 300000;
        }else Enrage_Timer -= uiDiff;

        if (Scream_Timer < uiDiff)
        {
            switch(rand()%4)
            {
                case 0: DoScriptText(SAY_SCREAM1, me);break;
                case 1: DoScriptText(SAY_SCREAM2, me);break;
                case 2: DoScriptText(SAY_SCREAM3, me);break;
                case 3: DoScriptText(SAY_SCREAM4, me);break;
            }
            Scream_Timer = 60000+rand()%30000;
        }
		else 
			Scream_Timer -= uiDiff;

        if (RangeCheck_Timer < uiDiff)
        {
            m_bInMeleeRange = false;
            std::list<HostileReference *> t_list = me->getThreatManager().getThreatList();
            for(std::list<HostileReference *>::iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
            {
                Unit* pTarget = Unit::GetUnit(*me, (*itr)->getUnitGuid());

                //if in melee range
                if (pTarget && pTarget->IsWithinDistInMap(me, ATTACK_DISTANCE))
                {
                    m_bInMeleeRange = true;
                    break;
                }
            }

            if (!m_bInMeleeRange)
                DoCast(SelectUnit(SELECT_TARGET_TOPAGGRO,0), SPELL_BALL_LIGHTNING);

            RangeCheck_Timer = 2000;
        }else RangeCheck_Timer -= uiDiff;

        //if nobody is in melee range
        if (m_bInMeleeRange)
            DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, me);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_THADDIUS, DONE);

		Map *map = me->GetMap();
            Map::PlayerList const &PlayerList = map->GetPlayers();

        if (PlayerList.isEmpty())
            return;

		for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (i->getSource()->isAlive())
			{
				i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_NEGATIVE_DMGBUFF);
				i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_NEGATIVE_NEARDMG);
				i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_POSITIVE_DMGBUFF);
				i->getSource()->RemoveAurasDueToSpell(SPELL_CHARGE_POSITIVE_NEARDMG);
			}
    }

    void KilledUnit(Unit *victim)
    {
        if(victim == me)
            return;
        switch(rand()%4)
        {
            case 0: DoScriptText(SAY_KILL1, me);break;
            case 1: DoScriptText(SAY_KILL2, me);break;
            case 2: DoScriptText(SAY_KILL3, me);break;
            case 3: DoScriptText(SAY_KILL4, me);break;
        }
    }
};

CreatureAI* GetAI_boss_thaddius(Creature* pCreature)
{
    return new boss_thaddiusAI(pCreature);
}

CreatureAI* GetAI_mob_stalagg(Creature* pCreature)
{
    return new mob_stalaggAI(pCreature);
}

CreatureAI* GetAI_mob_feugen(Creature* pCreature)
{
    return new mob_feugenAI(pCreature);
}

void AddSC_boss_thaddius()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_thaddius";
    newscript->GetAI = &GetAI_boss_thaddius;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_stalagg";
    newscript->GetAI = &GetAI_mob_stalagg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_feugen";
    newscript->GetAI = &GetAI_mob_feugen;
    newscript->RegisterSelf();
}
