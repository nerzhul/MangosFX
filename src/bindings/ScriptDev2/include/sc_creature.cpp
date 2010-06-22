/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#include "precompiled.h"
#include "Item.h"
#include "Spell.h"
#include "WorldPacket.h"
#include "ObjectMgr.h"
#include "utf8cpp/utf8.h"

// Spell summary for ScriptedAI::SelectSpell
struct TSpellSummary
{
    uint8 Targets;                                          // set of enum SelectTarget
    uint8 Effects;                                          // set of enum SelectEffect
} *SpellSummary;

ScriptedAI::ScriptedAI(Creature* pCreature) : CreatureAI(pCreature),
    m_bCombatMovement(true),
    m_uiEvadeCheckCooldown(2500)
{}

bool ScriptedAI::IsVisible(Unit* pWho) const
{
    if (!pWho)
        return false;

    return me->IsWithinDist(pWho, VISIBLE_RANGE) && pWho->isVisibleForOrDetect(me, me, true);
}

void ScriptedAI::MoveInLineOfSight(Unit* pWho)
{
    if (!me->hasUnitState(UNIT_STAT_STUNNED) && pWho->isTargetableForAttack() &&
        me->IsHostileTo(pWho) && pWho->isInAccessablePlaceFor(me))
    {
        if (!me->canFly() && me->GetDistanceZ(pWho) > CREATURE_Z_ATTACK_RANGE)
            return;

        if (me->IsWithinDistInMap(pWho, me->GetAttackDistance(pWho)) && me->IsWithinLOSInMap(pWho))
        {
            if (!me->getVictim())
            {
                pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                AttackStart(pWho);
            }
            else if (me->GetMap()->IsDungeon())
            {
                pWho->SetInCombatWith(me);
                me->AddThreat(pWho, 0.0f);
            }
        }
    }
}

void ScriptedAI::AttackStart(Unit* pWho)
{
    if (!pWho)
        return;

    if (me->Attack(pWho, true))
    {
        me->AddThreat(pWho);
        me->SetInCombatWith(pWho);
        pWho->SetInCombatWith(me);

        if (IsCombatMovement())
            me->GetMotionMaster()->MoveChase(pWho);
    }
}

void ScriptedAI::EnterCombat(Unit* pEnemy)
{
    if (!pEnemy)
        return;

    Aggro(pEnemy);
}

void ScriptedAI::Aggro(Unit* pEnemy)
{
}

void ScriptedAI::UpdateAI(const uint32 diff)
{
    //Check if we have a current target
    if (!CanDoSomething())
        return;

    if (me->isAttackReady())
    {
        //If we are within range melee the target
        if (me->IsWithinDistInMap(me->getVictim(), ATTACK_DISTANCE))
        {
            me->AttackerStateUpdate(me->getVictim());
            me->resetAttackTimer();
        }
    }
}

void ScriptedAI::EnterEvadeMode()
{
    me->RemoveAllAuras();
    me->DeleteThreatList();
    me->CombatStop(true);
    me->LoadCreaturesAddon();

    if (me->isAlive())
        me->GetMotionMaster()->MoveTargetedHome();

    me->SetLootRecipient(NULL);

    Reset();
}

void ScriptedAI::JustRespawned()
{
    Reset();
}

void ScriptedAI::DoStartMovement(Unit* pVictim, float fDistance, float fAngle)
{
    if (pVictim)
        me->GetMotionMaster()->MoveChase(pVictim, fDistance, fAngle);
}

void ScriptedAI::DoStartNoMovement(Unit* pVictim)
{
    if (!pVictim)
        return;

    me->GetMotionMaster()->MoveIdle();
    me->StopMoving();
}

void ScriptedAI::DoMeleeAttackIfReady()
{
    //Make sure our attack is ready before checking distance
    if (me->isAttackReady())
    {
        //If we are within range melee the target
        if (me->IsWithinDistInMap(me->getVictim(), ATTACK_DISTANCE))
        {
            me->AttackerStateUpdate(me->getVictim());
            me->resetAttackTimer();
        }
    }
}

void ScriptedAI::DoStopAttack()
{
    if (me->getVictim())
        me->AttackStop();
}

void ScriptedAI::DoCast(Unit* pTarget, uint32 uiSpellId, bool bTriggered)
{
    if (!pTarget || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(pTarget, uiSpellId, bTriggered);
}

void ScriptedAI::DoCastMe(uint32 uiSpellId, bool bTriggered)
{
    if (!me->isAlive() || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(me, uiSpellId, bTriggered);
}

void ScriptedAI::DoCastVictim(uint32 uiSpellId, bool bTriggered)
{
    if (!me->getVictim() || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(me->getVictim(), uiSpellId, bTriggered);
}

Unit* ScriptedAI::DoCastRandom(uint32 uiSpellId, bool bTriggered, bool InFront)
{
    Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

    if (!pTarget || me->IsNonMeleeSpellCasted(false) || !pTarget->isAlive())
        return NULL;

    me->StopMoving();
	if(InFront)
		me->SetInFront(pTarget);
    me->CastSpell(pTarget, uiSpellId, bTriggered);
    return pTarget;
}

void ScriptedAI::DoCastHasMana(uint32 uiSpellId, bool bTriggered, bool InFront)
{
	Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);

    if (!pTarget || me->IsNonMeleeSpellCasted(false) || !pTarget->isAlive())
        return;

	if(pTarget->getPowerType() != POWER_MANA)
	{
		int search=0;
		do
		{
			pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
			search++;
		}
		while(pTarget && pTarget->getPowerType() != POWER_MANA && search < 30);
	}

	if(!pTarget)
		return;

	me->StopMoving();
	if(InFront)
		me->SetInFront(pTarget);
	me->CastSpell(pTarget, uiSpellId, bTriggered);
}

void ScriptedAI::DoCastPlayer(uint32 uiSpellId, bool bTriggered, bool InFront)
{
	Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
	if (!lPlayers.isEmpty())
	{
		uint32 _rand = urand(0,lPlayers.getSize());
		uint16 i=0;
		for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
		{
			if(i == _rand)
			{
				if(Player* pPlayer = itr->getSource())
				{
					if(InFront)
						me->SetInFront(pPlayer);
					me->CastSpell(pPlayer, uiSpellId, bTriggered);
				}
				return;
			}
			i++;
		}
	}
}
void ScriptedAI::DoCastLowHP(uint32 uiSpellId, bool bTriggered)
{
	Unit *pTarget = DoSelectLowestHpFriendly(60);

    if (!pTarget ||!pTarget->isAlive() || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(pTarget, uiSpellId, bTriggered);
}

void ScriptedAI::DoCastNear(uint32 uiSpellId, bool bTriggered)
{
	Unit *pTarget = NULL;
    float tmpDist = 100.0f;
    ThreatList::const_iterator i;
    for (i = me->getThreatManager().getThreatList().begin();i != me->getThreatManager().getThreatList().end(); ++i)
    {
        Unit* pUnit = NULL;
        pUnit = Unit::GetUnit((*me), (*i)->getUnitGuid());
        if (!pUnit)
            continue;
        if(pUnit->GetDistance2d(me) < tmpDist)
		{
			tmpDist = pUnit->GetDistance2d(me);
			pTarget = pUnit;
		}
    }

    if (!pTarget ||!pTarget->isAlive() || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(pTarget, uiSpellId, bTriggered);
}

void ScriptedAI::DoCastSpell(Unit* pTarget, SpellEntry const* pSpellInfo, bool bTriggered)
{
    if (!pTarget || me->IsNonMeleeSpellCasted(false))
        return;

    me->StopMoving();
    me->CastSpell(pTarget, pSpellInfo, bTriggered);
}

void ScriptedAI::DoPlaySoundToSet(WorldObject* pSource, uint32 uiSoundId)
{
    if (!pSource)
        return;

    if (!GetSoundEntriesStore()->LookupEntry(uiSoundId))
    {
        error_log("SD2: Invalid soundId %u used in DoPlaySoundToSet (Source: TypeId %u, GUID %u)", uiSoundId, pSource->GetTypeId(), pSource->GetGUIDLow());
        return;
    }

    pSource->PlayDirectSound(uiSoundId);
}

Creature* ScriptedAI::DoSpawnCreature(uint32 uiId, float fX, float fY, float fZ, float fAngle, uint32 uiType, uint32 uiDespawntime)
{
    return me->SummonCreature(uiId,me->GetPositionX()+fX, me->GetPositionY()+fY, me->GetPositionZ()+fZ, fAngle, (TempSummonType)uiType, uiDespawntime);
}

Unit* ScriptedAI::SelectUnit(SelectAggroTarget target, uint32 uiPosition)
{
    //ThreatList m_threatlist;
   ThreatList const& threatlist = me->getThreatManager().getThreatList();
   ThreatList::const_iterator itr = threatlist.begin();
   ThreatList::const_reverse_iterator ritr = threatlist.rbegin();

    if (uiPosition >= threatlist.size() || !threatlist.size())
        return NULL;

    switch (target)
    {
    case SELECT_TARGET_RANDOM:
    {
		Unit* target;
		uint16 occur_ = 0;
		bool right_target = true;
		do
		{
			occur_ ++;
			right_target = true;
	       	advance(itr, uiPosition +  (rand() % (threatlist.size() - uiPosition)));
			target = Unit::GetUnit((*me),(*itr)->getUnitGuid());
			if(!target || (target->GetTypeId() == TYPEID_UNIT && ((Creature*)target)->isTotem()))
			{
				itr = threatlist.begin();
				right_target = false;
			}
			if(occur_ > 300)
				right_target = true;
		}
		while(!right_target);
		return target;
        break;
    }
    case SELECT_TARGET_TOPAGGRO:
        advance(itr, uiPosition);
        return Unit::GetUnit((*me),(*itr)->getUnitGuid());
        break;

    case SELECT_TARGET_BOTTOMAGGRO:
        advance(ritr, uiPosition);
        return Unit::GetUnit((*me),(*ritr)->getUnitGuid());
        break;
    }

    return NULL;
}

SpellEntry const* ScriptedAI::SelectSpell(Unit* pTarget, int32 uiSchool, int32 uiMechanic, SelectTarget selectTargets, uint32 uiPowerCostMin, uint32 uiPowerCostMax, float fRangeMin, float fRangeMax, SelectEffect selectEffects)
{
    //No target so we can't cast
    if (!pTarget)
        return false;

    //Silenced so we can't cast
    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
        return false;

    //Using the extended script system we first create a list of viable spells
    SpellEntry const* apSpell[4];
    memset(apSpell, 0, sizeof(SpellEntry*)*4);

    uint32 uiSpellCount = 0;

    SpellEntry const* pTempSpell;
    SpellRangeEntry const* pTempRange;

    //Check if each spell is viable(set it to null if not)
    for (uint32 i = 0; i < 4; ++i)
    {
        pTempSpell = GetSpellStore()->LookupEntry(me->m_spells[i]);

        //This spell doesn't exist
        if (!pTempSpell)
            continue;

        // Targets and Effects checked first as most used restrictions
        //Check the spell targets if specified
        if (selectTargets && !(SpellSummary[me->m_spells[i]].Targets & (1 << (selectTargets-1))))
            continue;

        //Check the type of spell if we are looking for a specific spell type
        if (selectEffects && !(SpellSummary[me->m_spells[i]].Effects & (1 << (selectEffects-1))))
            continue;

        //Check for school if specified
        if (uiSchool >= 0 && pTempSpell->SchoolMask & uiSchool)
            continue;

        //Check for spell mechanic if specified
        if (uiMechanic >= 0 && pTempSpell->Mechanic != uiMechanic)
            continue;

        //Make sure that the spell uses the requested amount of power
        if (uiPowerCostMin &&  pTempSpell->manaCost < uiPowerCostMin)
            continue;

        if (uiPowerCostMax && pTempSpell->manaCost > uiPowerCostMax)
            continue;

        //Continue if we don't have the mana to actually cast this spell
        if (pTempSpell->manaCost > me->GetPower((Powers)pTempSpell->powerType))
            continue;

        //Get the Range
        pTempRange = GetSpellRangeStore()->LookupEntry(pTempSpell->rangeIndex);

        //Spell has invalid range store so we can't use it
        if (!pTempRange)
            continue;

        //Check if the spell meets our range requirements
        if (fRangeMin && pTempRange->maxRange < fRangeMin)
            continue;

        if (fRangeMax && pTempRange->maxRange > fRangeMax)
            continue;

        //Check if our target is in range
        if (me->IsWithinDistInMap(pTarget, pTempRange->minRange) || !me->IsWithinDistInMap(pTarget, pTempRange->maxRange))
            continue;

        //All good so lets add it to the spell list
        apSpell[uiSpellCount] = pTempSpell;
        ++uiSpellCount;
    }

    //We got our usable spells so now lets randomly pick one
    if (!uiSpellCount)
        return NULL;

    return apSpell[rand()%uiSpellCount];
}

bool ScriptedAI::CanCast(Unit* pTarget, SpellEntry const* pSpellEntry, bool bTriggered)
{
    //No target so we can't cast
    if (!pTarget || !pSpellEntry)
        return false;

    //Silenced so we can't cast
    if (!bTriggered && me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED))
        return false;

    //Check for power
    if (!bTriggered && me->GetPower((Powers)pSpellEntry->powerType) < pSpellEntry->manaCost)
        return false;

    SpellRangeEntry const* pTempRange = GetSpellRangeStore()->LookupEntry(pSpellEntry->rangeIndex);

    //Spell has invalid range store so we can't use it
    if (!pTempRange)
        return false;

    //Unit is out of range of this spell
    if (!me->IsInRange(pTarget, pTempRange->minRange, pTempRange->maxRange))
        return false;

    return true;
}

void FillSpellSummary()
{
    SpellSummary = new TSpellSummary[GetSpellStore()->GetNumRows()];

    SpellEntry const* pTempSpell;

    for (int i=0; i < GetSpellStore()->GetNumRows(); ++i)
    {
        SpellSummary[i].Effects = 0;
        SpellSummary[i].Targets = 0;

        pTempSpell = GetSpellStore()->LookupEntry(i);
        //This spell doesn't exist
        if (!pTempSpell)
            continue;

        for (int j=0; j<3; ++j)
        {
            //Spell targets self
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_SELF)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SELF-1);

            //Spell targets a single enemy
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_CHAIN_DAMAGE ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_CURRENT_ENEMY_COORDINATES)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_ENEMY-1);

            //Spell targets AoE at enemy
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_ENEMY_IN_AREA ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_ENEMY_IN_AREA_INSTANT ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_CASTER_COORDINATES ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_ENEMY_IN_AREA_CHANNELED)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_ENEMY-1);

            //Spell targets an enemy
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_CHAIN_DAMAGE ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_CURRENT_ENEMY_COORDINATES ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_ENEMY_IN_AREA ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_ENEMY_IN_AREA_INSTANT ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_CASTER_COORDINATES ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_ENEMY_IN_AREA_CHANNELED)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_ENEMY-1);

            //Spell targets a single friend(or self)
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_SELF ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_SINGLE_FRIEND ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_SINGLE_PARTY)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_SINGLE_FRIEND-1);

            //Spell targets aoe friends
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_PARTY_AROUND_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_AREAEFFECT_PARTY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_CASTER_COORDINATES)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_AOE_FRIEND-1);

            //Spell targets any friend(or self)
            if (pTempSpell->EffectImplicitTargetA[j] == TARGET_SELF ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_SINGLE_FRIEND ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_SINGLE_PARTY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_ALL_PARTY_AROUND_CASTER ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_AREAEFFECT_PARTY ||
                pTempSpell->EffectImplicitTargetA[j] == TARGET_CASTER_COORDINATES)
                SpellSummary[i].Targets |= 1 << (SELECT_TARGET_ANY_FRIEND-1);

            //Make sure that this spell includes a damage effect
            if (pTempSpell->Effect[j] == SPELL_EFFECT_SCHOOL_DAMAGE ||
                pTempSpell->Effect[j] == SPELL_EFFECT_INSTAKILL ||
                pTempSpell->Effect[j] == SPELL_EFFECT_ENVIRONMENTAL_DAMAGE ||
                pTempSpell->Effect[j] == SPELL_EFFECT_HEALTH_LEECH)
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_DAMAGE-1);

            //Make sure that this spell includes a healing effect (or an apply aura with a periodic heal)
            if (pTempSpell->Effect[j] == SPELL_EFFECT_HEAL ||
                pTempSpell->Effect[j] == SPELL_EFFECT_HEAL_MAX_HEALTH ||
                pTempSpell->Effect[j] == SPELL_EFFECT_HEAL_MECHANICAL ||
                (pTempSpell->Effect[j] == SPELL_EFFECT_APPLY_AURA  && pTempSpell->EffectApplyAuraName[j]== 8))
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_HEALING-1);

            //Make sure that this spell applies an aura
            if (pTempSpell->Effect[j] == SPELL_EFFECT_APPLY_AURA)
                SpellSummary[i].Effects |= 1 << (SELECT_EFFECT_AURA-1);
        }
    }
}

void ScriptedAI::DoResetThreat()
{
    if (!me->CanHaveThreatList() || me->getThreatManager().isThreatListEmpty())
    {
        //error_log("SD2: DoResetThreat called for creature that either cannot have threat list or has empty threat list (me entry = %d)", me->GetEntry());
        return;
    }

   ThreatList const& threatlist = me->getThreatManager().getThreatList();

    for(ThreatList::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
    {
        Unit* pUnit = Unit::GetUnit((*me), (*itr)->getUnitGuid());

        if (pUnit && me->getThreatManager().getThreat(pUnit))
            me->getThreatManager().modifyThreatPercent(pUnit, -100);
    }
}

void ScriptedAI::DoTeleportPlayer(Unit* pUnit, float fX, float fY, float fZ, float fO)
{
    if (!pUnit || pUnit->GetTypeId() != TYPEID_PLAYER)
    {
        if (pUnit)
            error_log("SD2: Creature %u (Entry: %u) Tried to teleport non-player unit (Type: %u GUID: %u) to x: %f y:%f z: %f o: %f. Aborted.", me->GetGUID(), me->GetEntry(), pUnit->GetTypeId(), pUnit->GetGUID(), fX, fY, fZ, fO);

        return;
    }

    ((Player*)pUnit)->TeleportTo(pUnit->GetMapId(), fX, fY, fZ, fO, TELE_TO_NOT_LEAVE_COMBAT);
}

Unit* ScriptedAI::DoSelectLowestHpFriendly(float fRange, uint32 uiMinHPDiff)
{
    CellPair p(MaNGOS::ComputeCellPair(me->GetPositionX(), me->GetPositionY()));
    Cell cell(p);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    Unit* pUnit = NULL;

    MaNGOS::MostHPMissingInRange u_check(me, fRange, uiMinHPDiff);
    MaNGOS::UnitLastSearcher<MaNGOS::MostHPMissingInRange> searcher(me, pUnit, u_check);

    /*
    typedef TYPELIST_4(GameObject, Creature*except pets*, DynamicObject, Corpse*Bones*) AllGridObjectTypes;
    This means that if we only search grid then we cannot possibly return pets or players so this is safe
    */
    TypeContainerVisitor<MaNGOS::UnitLastSearcher<MaNGOS::MostHPMissingInRange>, GridTypeMapContainer >  grid_unit_searcher(searcher);

    cell.Visit(p, grid_unit_searcher, *(me->GetMap()));

    return pUnit;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyCC(float fRange)
{
    CellPair p(MaNGOS::ComputeCellPair(me->GetPositionX(), me->GetPositionY()));
    Cell cell(p);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    std::list<Creature*> pList;

    MaNGOS::FriendlyCCedInRange u_check(me, fRange);
    MaNGOS::CreatureListSearcher<MaNGOS::FriendlyCCedInRange> searcher(me, pList, u_check);

    TypeContainerVisitor<MaNGOS::CreatureListSearcher<MaNGOS::FriendlyCCedInRange>, GridTypeMapContainer >  grid_creature_searcher(searcher);

    cell.Visit(p, grid_creature_searcher, *(me->GetMap()));

    return pList;
}

std::list<Creature*> ScriptedAI::DoFindFriendlyMissingBuff(float fRange, uint32 uiSpellId)
{
    CellPair p(MaNGOS::ComputeCellPair(me->GetPositionX(), me->GetPositionY()));
    Cell cell(p);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    std::list<Creature*> pList;

    MaNGOS::FriendlyMissingBuffInRange u_check(me, fRange, uiSpellId);
    MaNGOS::CreatureListSearcher<MaNGOS::FriendlyMissingBuffInRange> searcher(me, pList, u_check);

    TypeContainerVisitor<MaNGOS::CreatureListSearcher<MaNGOS::FriendlyMissingBuffInRange>, GridTypeMapContainer >  grid_creature_searcher(searcher);

    cell.Visit(p, grid_creature_searcher, *(me->GetMap()));

    return pList;
}

Player* ScriptedAI::GetPlayerAtMinimumRange(float fMinimumRange)
{
    Player* pPlayer = NULL;

    CellPair pair(MaNGOS::ComputeCellPair(me->GetPositionX(), me->GetPositionY()));
    Cell cell(pair);
    cell.data.Part.reserved = ALL_DISTRICT;
    cell.SetNoCreate();

    PlayerAtMinimumRangeAway check(me, fMinimumRange);
    MaNGOS::PlayerSearcher<PlayerAtMinimumRangeAway> searcher(me, pPlayer, check);
    TypeContainerVisitor<MaNGOS::PlayerSearcher<PlayerAtMinimumRangeAway>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *(me->GetMap()));

    return pPlayer;
}

void ScriptedAI::SetEquipmentSlots(bool bLoadDefault, int32 uiMainHand, int32 uiOffHand, int32 uiRanged)
{
    if (bLoadDefault)
    {
        me->LoadEquipment(me->GetCreatureInfo()->equipmentId,true);
        return;
    }

    if (uiMainHand >= 0)
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(uiMainHand));

    if (uiOffHand >= 0)
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, uint32(uiOffHand));

    if (uiRanged >= 0)
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, uint32(uiRanged));
}

void ScriptedAI::SetCombatMovement(bool bCombatMove)
{
    m_bCombatMovement = bCombatMove;
}

// Hacklike storage used for misc creatures that are expected to evade of outside of a certain area.
// It is assumed the information is found elswehere and can be handled by mangos. So far no luck finding such information/way to extract it.
enum
{
    NPC_BROODLORD   = 12017,
    NPC_VOID_REAVER = 19516,
    NPC_JAN_ALAI    = 23578,
    NPC_SARTHARION  = 28860
};

bool ScriptedAI::EnterEvadeIfOutOfCombatArea(const uint32 diff)
{
    if (m_uiEvadeCheckCooldown < diff)
        m_uiEvadeCheckCooldown = 2500;
    else
    {
        m_uiEvadeCheckCooldown -= diff;
        return false;
    }

    if (me->IsInEvadeMode() || !me->getVictim())
        return false;

    float fX = me->GetPositionX();
    float fY = me->GetPositionY();
    float fZ = me->GetPositionZ();

    switch(me->GetEntry())
    {
        case NPC_BROODLORD:                                 // broodlord (not move down stairs)
            if (fZ > 448.60f)
                return false;
            break;
        case NPC_VOID_REAVER:                               // void reaver (calculate from center of room)
            if (me->GetDistance2d(432.59f, 371.93f) < 105.0f)
                return false;
            break;
        case NPC_JAN_ALAI:                                  // jan'alai (calculate by Z)
            if (fZ > 12.0f)
                return false;
            break;
        case NPC_SARTHARION:                                // sartharion (calculate box)
            if (fX > 3218.86f && fX < 3275.69f && fY < 572.40f && fY > 484.68f)
                return false;
            break;
        default:
            error_log("SD2: EnterEvadeIfOutOfCombatArea used for creature entry %u, but does not have any definition.", me->GetEntry());
            return false;
    }

    EnterEvadeMode();
    return true;
}

void Scripted_NoMovementAI::AttackStart(Unit* pWho)
{
    if (!pWho)
        return;

    if (me->Attack(pWho, true))
    {
        me->AddThreat(pWho, 0.0f);
        me->SetInCombatWith(pWho);
        pWho->SetInCombatWith(me);

        DoStartNoMovement(pWho);
    }
}

void ScriptedAI::DoCompleteQuest(uint32 entry, Player* player)
{
    player->CompleteQuest(entry);
}

void ScriptedAI::Kill(Unit* toKill)
{
	if(!toKill)
		return;
		
	if(!toKill->isAlive())
		return;

	me->DealDamage(toKill, toKill->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
}

void ScriptedAI::GiveEmblemsToGroup(uint32 type, uint8 nb, bool group5)
{
	if(type == 0)
		return;

    Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
	if (!lPlayers.isEmpty())
		for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
			if (Player* pPlayer = itr->getSource())
				GiveEmblems(type,pPlayer,nb, group5);
}

void ScriptedAI::GiveEmblems(uint32 type, Player* pPlayer, uint8 nb, bool group5)
{
	ItemPosCountVec dest;
	uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, type, nb, false);
	if (msg == EQUIP_ERR_OK)
		if(Item* pItem = pPlayer->StoreNewItem(dest, type, nb, true))
			pPlayer->SendNewItem(pItem, nb, true, false);

	// Wintergrasp Aura
	if(group5 && pPlayer->HasAura(57940))
	{
		ItemPosCountVec dest;
	        uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 43228, 4, false);
	        if (msg == EQUIP_ERR_OK)
                	if(Item* pItem = pPlayer->StoreNewItem(dest, 43228, 4, true))
        	                pPlayer->SendNewItem(pItem, 4, true, false);
	}
	
}

void ScriptedAI::FreezeMob(bool freeze, Creature* tmpCr, bool OOC)
{
	if(!tmpCr)
		tmpCr = me;
		
	if(tmpCr->isAlive())
	{
		tmpCr->CastStop();
		tmpCr->AttackStop();
		if(freeze)
		{
			tmpCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
			tmpCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
			if(OOC)
				tmpCr->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_OOC_NOT_ATTACKABLE);
			tmpCr->CastSpell(tmpCr,66830,false);
		}
		else
		{
			tmpCr->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
			tmpCr->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
			tmpCr->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_OOC_NOT_ATTACKABLE);
			tmpCr->RemoveAurasDueToSpell(66830);
		}
	}
}

void ScriptedAI::SetAuraStack(uint32 spell, uint32 stacks, Unit* target, Unit* caster, uint8 module)
{
	if(module == 0)
	{
		if (!target->HasAura(spell))
		{
			if(stacks > 0)
			{
				for(int k=0;k<stacks;k++)
					for(int i=0;i<3;i++)
					{
						Aura* aur = Aura::CreateBugAura(GetSpellStore()->LookupEntry(spell),i,NULL,target,caster);
						target->AddAura(aur);
					}
			}
		}
		else
		{
			if (target->GetAura(spell, 0)->GetStackAmount() == 1 && stacks == -1 
				|| target->GetAura(spell, 0)->GetStackAmount() < stacks)
					target->RemoveAurasDueToSpell(spell);
			else
				target->GetAura(spell,0)->modStackAmount(stacks);
		}
	}
	else
	{
		if(GetSpellStore()->LookupEntry(spell))
		{
			target->RemoveAurasDueToSpell(spell);
			for(int k=0;k<stacks;k++)
				for(int i=0;i<3;i++)
				{
					Aura* aur = Aura::CreateBugAura(GetSpellStore()->LookupEntry(spell),i,NULL,target,caster);
					target->AddAura(aur);
				}
		}
	}
}

void ScriptedAI::Speak(uint8 type, uint32 soundid, std::string text, Creature* spkCr)
{
	if(!spkCr)
		spkCr = me;

	if(soundid > 0 && GetSoundEntriesStore()->LookupEntry(soundid))
		spkCr->PlayDirectSound(soundid);

	switch(type)
    {
        case CHAT_TYPE_SAY:
			spkCr->MonsterSay(text.c_str(), 0, spkCr ? spkCr->GetGUID() : 0);
			DoSpeakEmote(spkCr);
            break;
        case CHAT_TYPE_YELL:
            spkCr->MonsterYell(text.c_str(), 0, spkCr ? spkCr->GetGUID() : 0);
			DoSpeakEmote(spkCr);
            break;
        case CHAT_TYPE_TEXT_EMOTE:
            spkCr->MonsterTextEmote(text.c_str(), spkCr ? spkCr->GetGUID() : 0);
            break;
        case CHAT_TYPE_BOSS_EMOTE:
            spkCr->MonsterTextEmote(text.c_str(), spkCr ? spkCr->GetGUID() : 0, true);
            break;
	}
}

void ScriptedAI::SetFlying(bool fly, Creature* who)
{
	if(!who)
		who = me;

	if (fly)
    {
		who->SetReactState(REACT_PASSIVE);
        who->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
        who->AddMonsterMoveFlag(MonsterMovementFlags(MOVEFLAG_CAN_FLY + MOVEFLAG_FLYING));
    }
    else
    {
		who->SetReactState(REACT_AGGRESSIVE);
        who->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
        who->RemoveMonsterMoveFlag(MonsterMovementFlags(MOVEFLAG_CAN_FLY + MOVEFLAG_FLYING));
    }
}

void ScriptedAI::Relocate(float x, float y, float z, bool fly, float Time)
{
	me->GetMap()->CreatureRelocation(me,x,y,z,0.0f);
	me->SendMonsterMove(x,y,z, 0, (fly ? MONSTER_MOVE_FLY : MONSTER_MOVE_NONE), Time);
}

void ScriptedAI::AggroAllPlayers(float maxdist)
{
	Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
	if (!lPlayers.isEmpty())
	{
		for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
			if (Player* pPlayer = itr->getSource())
				if(pPlayer->isAlive() && pPlayer->GetDistance2d(me) < maxdist)
				{
					me->AddThreat(pPlayer,1.0f);
				}
	}
}
void LibDevFSAI::AddEvent(uint32 SpellId, uint32 Timer, uint32 NormTimer, uint32 Diff,
							 SpellCastTarget targ, uint8 phase, uint32 TextId, bool MaxPriority, uint16 Repeat, bool front)
{
	EventSh tmpEvent;
	tmpEvent.SpellId = SpellId;
	tmpEvent.phase = phase;
	tmpEvent.Timer = Timer;
	tmpEvent.targ = targ;
	tmpEvent.Diff = Diff;
	tmpEvent.Repeat = Repeat;
	tmpEvent.NormTimer = NormTimer;
	tmpEvent.TextId = TextId;
	tmpEvent.MaxPriority = MaxPriority;
	tmpEvent.RequireFront = front;
	EventShVect.push_back(tmpEvent);
	SavedEventSh.push_back(tmpEvent);
}

void LibDevFSAI::AddTextEvent(uint32 soundId, std::string text, uint32 Timer, uint32 NormTimer, uint8 type, uint8 phase)
{
	if(soundId < 1 || !GetSoundEntriesStore()->LookupEntry(soundId))
		return;

	TextSh tmpEvent;
	tmpEvent.SoundId = soundId;
	tmpEvent.Text = text;
	tmpEvent.type = type;
	tmpEvent.phase = phase;
	tmpEvent.Timer = Timer;
	tmpEvent.NormTimer = NormTimer;
	EventTextVect.push_back(tmpEvent);
	SavedEventTexts.push_back(tmpEvent);
}

void MobEventTasks::AddEvent(uint32 SpellId, uint32 Timer, uint32 NormTimer, uint32 Diff,
			SpellCastTarget targ, uint8 phase, uint32 TextId, bool MaxPriority, uint16 Repeat, bool front)
{
	EventSh tmpEvent;
	tmpEvent.SpellId = SpellId;
	tmpEvent.phase = phase;
	tmpEvent.Timer = Timer;
	tmpEvent.targ = targ;
	tmpEvent.Diff = Diff;
	tmpEvent.Repeat = Repeat;
	tmpEvent.NormTimer = NormTimer;
	tmpEvent.TextId = TextId;
	tmpEvent.MaxPriority = MaxPriority;
	tmpEvent.RequireFront = front;
	EventShVect.push_back(tmpEvent);
}
void LibDevFSAI::UpdateEvent(uint32 diff, uint32 phase)
{
	for(SpellEvents::iterator itr = EventShVect.begin(); itr!= EventShVect.end(); ++itr)
	{
		if((*itr).phase == phase)
		{
			if((*itr).Timer <= diff || (*itr).Timer > 45000000)
			{
				if((*itr).MaxPriority)
					me->CastStop();
				
				if((*itr).TextId != 0)
					DoScriptText((*itr).TextId,me);
				
				for(uint16 i=0;i<(*itr).Repeat;i++)
				{
					switch((*itr).targ)
					{
						case TARGET_MAIN:
							DoCastVictim((*itr).SpellId);
							break;
						case TARGET_RANDOM:
							DoCastRandom((*itr).SpellId,false,(*itr).RequireFront);
							break;
						case TARGET_ME:
							DoCastMe((*itr).SpellId);
							break;
						case TARGET_LOW_HP:
							DoCastLowHP((*itr).SpellId);
							break;
						case TARGET_NEAR:
							DoCastNear((*itr).SpellId);
							break;
						case TARGET_HAS_MANA:
							DoCastHasMana((*itr).SpellId);
							break;
						case PLAYER_RANDOM:
							DoCastPlayer((*itr).SpellId);
							break;
						case HEAL_MY_FRIEND:
							if(Unit* target = DoSelectLowestHpFriendly(40.0f,100))
								DoCast(target,(*itr).SpellId);
							break;
						case NO_TANK:
							if(me->getVictim() && me->getVictim()->GetDistance2d(me) > 10.0f)
								DoCastVictim((*itr).SpellId);
							break;
						case TARGET_OTHER:
							// todo : consultation d'un vecteur spécial...
						default:
							break;
					}
				}
				(*itr).Timer = (*itr).NormTimer + urand(0,(*itr).Diff);
			}
			else
				(*itr).Timer -= diff;	
		}
	}

	for(SummonEvents::iterator itr = EventSummonVect.begin(); itr!= EventSummonVect.end(); ++itr)
	{
		if((*itr).phase == phase)
		{
			if((*itr).TextId != 0)
				DoScriptText((*itr).TextId,me);

			if((*itr).Timer <= diff || (*itr).Timer > 45000000)
			{

				for(int i=0;i<(*itr).Repeat;i++)
				{
					CallCreature((*itr).entry,(*itr).despawnTime,(*itr).WhereS,(*itr).Compo);
				}
				(*itr).Timer = (*itr).NormTimer + urand(0,(*itr).diff);
			}
			else
				(*itr).Timer -= diff;	
		}
	}

	for(TextEvents::iterator itr = EventTextVect.begin(); itr!= EventTextVect.end(); ++itr)
	{
		if((*itr).phase == phase)
		{
			if((*itr).Timer <= diff || (*itr).Timer > 45000000)
			{
				Speak((*itr).type,(*itr).SoundId,(*itr).Text);
				(*itr).Timer = (*itr).NormTimer;
			}
			else
				(*itr).Timer -= diff;	
		}
	}

	if(ManualMoveEnable && phase == 0)
	{
		if(CheckDistanceTimer <= diff)
		{
			if(CanMove && me->getVictim() && me->getVictim()->GetDistance2d(me) > 8.0f)
				me->GetMotionMaster()->MoveChase(me->getVictim());
			else
			{
				me->GetMotionMaster()->MoveIdle();
				me->StopMoving();
			}

			if(CanMove)
				CheckDistanceTimer = 1500;
			else
				CheckDistanceTimer = 200;
		}
		else
			CheckDistanceTimer -= diff;
	}

	if(TimedDownEnable && phase == 0)
	{
		if(AchTimedDownTimer <= diff)
			TimedDownEnable = false;
		else
			AchTimedDownTimer -= diff;
	}
}

void LibDevFSAI::ResetTimers()
{
	EventShVect.clear();
	EventSummonVect.clear();
	SavedEventTexts.clear();
	for(SpellEvents::iterator itr = SavedEventSh.begin(); itr!= SavedEventSh.end(); ++itr)
		EventShVect.push_back(*itr);
		
	for(SummonEvents::iterator itr = SavedEventSummon.begin(); itr!= SavedEventSummon.end(); ++itr)
		EventSummonVect.push_back(*itr);

	for(TextEvents::iterator itr = SavedEventTexts.begin(); itr!= SavedEventTexts.end(); ++itr)
		EventTextVect.push_back(*itr);
}

void MobEventTasks::UpdateEvent(uint32 diff, uint32 phase)
{
	for(SpellEvents::iterator itr = EventShVect.begin(); itr!= EventShVect.end(); ++itr)
	{
		if((*itr).phase == phase)
		{
			if((*itr).Timer <= diff || (*itr).Timer > 45000000)
			{
				if((*itr).MaxPriority)
					thisCr->CastStop();
				
				if((*itr).TextId != 0)
					DoScriptText((*itr).TextId,thisCr);
				
				for(uint16 i=0;i<(*itr).Repeat;i++)
				{
					switch((*itr).targ)
					{
						case TARGET_MAIN:
							thisAI->DoCastVictim((*itr).SpellId);
							break;
						case TARGET_RANDOM:
							thisAI->DoCastRandom((*itr).SpellId,false,(*itr).RequireFront);
							break;
						case TARGET_ME:
							thisAI->DoCastMe((*itr).SpellId);
							break;
						case TARGET_LOW_HP:
							thisAI->DoCastLowHP((*itr).SpellId);
							break;
						case TARGET_NEAR:
							thisAI->DoCastNear((*itr).SpellId);
							break;
						case TARGET_HAS_MANA:
							thisAI->DoCastHasMana((*itr).SpellId);
							break;
						case PLAYER_RANDOM:
							thisAI->DoCastPlayer((*itr).SpellId);
							break;
						case HEAL_MY_FRIEND:
							if(Unit* target = thisAI->DoSelectLowestHpFriendly(30.0f,100))
								thisAI->DoCast(target,(*itr).SpellId);
							break;
						case NO_TANK:
							if(thisCr->getVictim() && thisCr->getVictim()->GetDistance2d(thisCr) > 10.0f)
								thisAI->DoCastVictim((*itr).SpellId);
							break;
						case TARGET_OTHER:
							// todo : consultation d'un vecteur spécial...
						default:
							break;
					}
				}
				(*itr).Timer = (*itr).NormTimer + urand(0,(*itr).Diff);
			}
			else
				(*itr).Timer -= diff;	
		}
	}

	for(SummonEvents::iterator itr = EventSummonVect.begin(); itr!= EventSummonVect.end(); ++itr)
	{
		if((*itr).phase == phase)
		{
			if((*itr).TextId != 0)
				DoScriptText((*itr).TextId,thisCr);

			if((*itr).Timer <= diff || (*itr).Timer > 45000000)
			{

				for(int i=0;i<(*itr).Repeat;i++)
				{
					CallCreature((*itr).entry,(*itr).despawnTime,(*itr).WhereS,(*itr).Compo);
				}
				(*itr).Timer = (*itr).NormTimer + urand(0,(*itr).diff);
			}
			else
				(*itr).Timer -= diff;	
		}
	}
}


Creature* LibDevFSAI::CallCreature(uint32 entry, uint32 Despawn, ZoneInvoc WhereZone, Comportement Compo,
									  float x, float y, float z, bool force)
{
	Creature* tmp = NULL;
	if((me->isAlive() && MyAdds.size() < MAX_ADDS) || force)
	{
		float randX = x ,randY = y ,randZ = (me) ? me->GetPositionZ() + 1 : 0;
		switch(WhereZone)
		{
			case ON_ME:
				randX = 0;
				randY = 0;
				break;
			case NEAR_7M:
				randX = urand(5,9);
				randY = urand(5,9);
				if(urand(0,1))
					randX = -randX;
				if(urand(0,1))
					randY = -randY;
				break;
			case NEAR_15M:
				randX = urand(13,17);
				randY = urand(13,17);
				if(urand(0,1))
					randX = -randX;
				if(urand(0,1))
					randY = -randY;
				break;
			case NEAR_30M:
				randX = urand(28,32);
				randY = urand(28,32);
				if(urand(0,1))
					randX = -randX;
				if(urand(0,1))
					randY = -randY;
				break;
			case NEAR_45M:
				randX = urand(10,45);
				randY = urand(10,45);
				if(urand(0,1))
					randX = -randX;
				if(urand(0,1))
					randY = -randY;
				break;
			case NEAR_60M:
				randX = urand(45,60);
				randY = urand(45,60);
				if(urand(0,1))
					randX = -randX;
				if(urand(0,1))
					randY = -randY;
				break;
			case PREC_COORDS:
				randX = x;
				randY = y;
				randZ = z;
				break;
			default:
				randX = 0;
				randY = 0;
				break;
		}

		if(WhereZone != PREC_COORDS)
			tmp = me->SummonCreature(entry,me->GetPositionX() + randX,me->GetPositionY() + randY, me->GetPositionZ() + 1.0f,me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, Despawn);
		else
			tmp = me->SummonCreature(entry,randX,randY,randZ,0.0f, TEMPSUMMON_TIMED_DESPAWN, Despawn);

		if(tmp && tmp->isAlive())
		{
			switch(Compo)
			{
				case AGGRESSIVE_MAIN:
					if(me->getVictim())
						tmp->AddThreat(me->getVictim(),1);
					break;
				case AGGRESSIVE_RANDOM:
				{
					Unit* tmptar;
					uint8 MAXLOOP = 0;
					do
					{
						tmptar = SelectUnit(SELECT_TARGET_RANDOM,0);
						MAXLOOP++;
					}
					while(tmptar && tmptar->GetDistance2d(me) < 40.0f && MAXLOOP < 50);
					if(tmptar)
						tmp->AddThreat(tmptar,1);
					break;
				}
				case VERY_AGGRESSIVE_MAIN:
					if(me->getVictim())
						tmp->AddThreat(me->getVictim(),1000.0);
					break;
				case VERY_AGGRESSIVE_RANDOM:
					if(Unit* tmptar = SelectUnit(SELECT_TARGET_RANDOM,0))
						tmp->AddThreat(tmptar,1000.0);
					break;
				case GO_TO_CREATOR:
					tmp->GetMotionMaster()->MovePoint(0,me->GetPositionX(),me->GetPositionY(),me->GetPositionZ());
					break;
				case NOTHING:
				default:
					break;
			}
	
			tmp->SetRespawnDelay(RESPAWN_ONE_DAY*1000);

			MyAdds.push_back(tmp->GetGUID());
		}
	}
	return tmp;
}
Creature* MobEventTasks::CallCreature(uint32 entry, uint32 Despawn, ZoneInvoc WhereZone, Comportement Compo,
								 float x, float y, float z, bool force)
{
	Creature* tmp = NULL;
	if((thisCr->isAlive() || force) && MyAdds.size() < MAX_ADDS)
	{
		float randX = x ,randY = y ,randZ = (thisCr) ? thisCr->GetPositionZ() + 1 : 0;
		switch(WhereZone)
		{
			case ON_ME:
				randX = 0;
				randY = 0;
				break;
			case NEAR_7M:
				randX = urand(5,9);
				randY = urand(5,9);
				if(urand(0,1))
					randX = -randX;
				if(urand(0,1))
					randY = -randY;
				break;
			case NEAR_15M:
				randX = urand(13,17);
				randY = urand(13,17);
				if(urand(0,1))
					randX = -randX;
				if(urand(0,1))
					randY = -randY;
				break;
			case NEAR_30M:
				randX = urand(28,32);
				randY = urand(28,32);
				if(urand(0,1))
					randX = -randX;
				if(urand(0,1))
					randY = -randY;
				break;
			case NEAR_45M:
				randX = urand(10,45);
				randY = urand(10,45);
				if(urand(0,1))
					randX = -randX;
				if(urand(0,1))
					randY = -randY;
				break;
			case NEAR_60M:
				randX = urand(45,60);
				randY = urand(45,60);
				if(urand(0,1))
					randX = -randX;
				if(urand(0,1))
					randY = -randY;
				break;
			case PREC_COORDS:
				randX = x;
				randY = y;
				randZ = z;
				break;
			default:
				randX = 0;
				randY = 0;
				break;
		}

		if(WhereZone != PREC_COORDS)
			tmp = thisCr->SummonCreature(entry,thisCr->GetPositionX() + randX,thisCr->GetPositionY() + randY, thisCr->GetPositionZ() + 1.0f,thisCr->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, Despawn);
		else
			tmp = thisCr->SummonCreature(entry,randX,randY,randZ,0.0f, TEMPSUMMON_TIMED_DESPAWN, Despawn);

		if(tmp && tmp->isAlive())
		{
			switch(Compo)
			{
				case AGGRESSIVE_MAIN:
					if(thisCr->getVictim())
						tmp->AddThreat(thisCr->getVictim(),1);
					break;
				case AGGRESSIVE_RANDOM:
				{
					Unit* tmptar;
					uint8 MAXLOOP = 0;
					do
					{
						tmptar = thisAI->SelectUnit(SELECT_TARGET_RANDOM,0);
						MAXLOOP++;
					}
					while(tmptar && tmptar->GetDistance2d(thisCr) < 40.0f && MAXLOOP < 50);
					if(tmptar)
						tmp->AddThreat(tmptar,1);
					break;
				}
				case VERY_AGGRESSIVE_MAIN:
					if(thisCr->getVictim())
						tmp->AddThreat(thisCr->getVictim(),1000.0);
					break;
				case VERY_AGGRESSIVE_RANDOM:
					if(Unit* tmptar = thisAI->SelectUnit(SELECT_TARGET_RANDOM,0))
						tmp->AddThreat(tmptar,1000.0);
					break;
				case GO_TO_CREATOR:
					tmp->GetMotionMaster()->MovePoint(0,thisCr->GetPositionX(),thisCr->GetPositionY(),thisCr->GetPositionZ());
					break;
				case NOTHING:
				default:
					break;
			}
	
			tmp->SetRespawnDelay(RESPAWN_ONE_DAY*1000);

			MyAdds.push_back(tmp->GetGUID());
		}
	}
	return tmp;
}

void LibDevFSAI::AddSummonEvent(uint32 entry, uint32 Timer, uint32 NormTimer, uint32 phase, uint32 Diff,
			uint32 nb_spawn, uint32 Despawn, ZoneInvoc WhereZone, 
			Comportement Compo, uint32 TextId)
{
	EventSummon tmpEvent;
	tmpEvent.Compo = Compo;
	tmpEvent.Repeat = nb_spawn;
	tmpEvent.despawnTime = Despawn;
	tmpEvent.diff = Diff;
	tmpEvent.entry = entry;
	tmpEvent.NormTimer = NormTimer;
	tmpEvent.Timer = Timer;
	tmpEvent.phase = phase;
	tmpEvent.WhereS = WhereZone;
	tmpEvent.TextId = TextId;
	EventSummonVect.push_back(tmpEvent);
	SavedEventSummon.push_back(tmpEvent);

}
void MobEventTasks::AddSummonEvent(uint32 entry, uint32 Timer, uint32 NormTimer, uint32 phase, uint32 Diff,
			uint32 nb_spawn, uint32 Despawn, ZoneInvoc WhereZone, 
			Comportement Compo, uint32 TextId)
{
	EventSummon tmpEvent;
	tmpEvent.Compo = Compo;
	tmpEvent.Repeat = nb_spawn;
	tmpEvent.despawnTime = Despawn;
	tmpEvent.diff = Diff;
	tmpEvent.entry = entry;
	tmpEvent.NormTimer = NormTimer;
	tmpEvent.Timer = Timer;
	tmpEvent.phase = phase;
	tmpEvent.WhereS = WhereZone;
	tmpEvent.TextId = TextId;
	EventSummonVect.push_back(tmpEvent);

}

void LibDevFSAI::CleanMyAdds()
{
	if(!MyAdds.empty())
	{
		for(std::vector<uint64>::iterator itr = MyAdds.begin(); itr < MyAdds.end(); ++itr)
		{
			if (Creature* cr = ((Creature*)Unit::GetUnit(*me,(*itr))))
				if(cr->isAlive())
					cr->ForcedDespawn(1000);
		}
		MyAdds.clear();
	}
}
void MobEventTasks::CleanMyAdds()
{
	if(!MyAdds.empty())
	{
		for(std::vector<uint64>::iterator itr = MyAdds.begin(); itr < MyAdds.end(); ++itr)
		{
			if (Creature* cr = ((Creature*)Unit::GetUnit(*thisCr,(*itr))))
				if(cr->isAlive())
					cr->ForcedDespawn(1000);
		}
		MyAdds.clear();
	}
}

void LibDevFSAI::GetNewTargetForMyAdds(Unit* target)
{
	if(!MyAdds.empty())
	{
		for(std::vector<uint64>::iterator itr = MyAdds.begin(); itr < MyAdds.end(); ++itr)
		{
			if (Creature* cr = ((Creature*)Unit::GetUnit(*me,(*itr))))
				if(cr->isAlive())
				{
					cr->DeleteThreatList();
					cr->AddThreat(target,150);
				}				
		}
	}
}

void MobEventTasks::GetNewTargetForMyAdds(Unit* target)
{
	if(!MyAdds.empty())
	{
		for(std::vector<uint64>::iterator itr = MyAdds.begin(); itr < MyAdds.end(); ++itr)
		{
			if (Creature* cr = ((Creature*)Unit::GetUnit(*thisCr,(*itr))))
				if(cr->isAlive())
				{
					cr->DeleteThreatList();
					cr->AddThreat(target,150);
				}				
		}
	}
}

void LibDevFSAI::AddPercentLife(Unit* u,uint8 percent)
{
	if(!u || !u->isAlive())
		return;

	u->SetHealth(u->GetHealth() + u->GetMaxHealth() * percent / 100);
}

void LibDevFSAI::DealDamage(Unit* target,uint32 damage)
{
	if(!target || !target->isAlive())
		return;
		
	me->DealDamage(target, damage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
}

void LibDevFSAI::DealPercentDamage(Unit* target,float percent)
{
	if(percent <= 0)
		return;
		
	uint32 dmg = target->GetMaxHealth() / 100 * percent;
	DealDamage(target,dmg);	
}

void LibDevFSAI::MakeInvisibleStalker()
{
	me->SetDisplayId(16925);
	me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
}

void LibDevFSAI::MakeHostileInvisibleStalker()
{
	MakeInvisibleStalker();
	me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
	me->setFaction(14);
}

void ScriptedAI::DoSpeakEmote(Unit* who)
{
	if(!who)
		who = me;

	if(who->isAlive())
		who->HandleEmoteCommand(13);
}

