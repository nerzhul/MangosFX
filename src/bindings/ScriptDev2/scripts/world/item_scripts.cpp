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
SDName: Item_Scripts
SD%Complete: 100
SDComment: Items for a range of different items. See content below (in script)
SDCategory: Items
EndScriptData */

/* ContentData
item_arcane_charges                 Prevent use if player is not flying (cannot cast while on ground)
item_draenei_fishing_net(i23654)    Hacklike implements chance to spawn item or creature
item_nether_wraith_beacon(i31742)   Summons creatures for quest Becoming a Spellfire Tailor (q10832)
item_flying_machine(i34060,i34061)  Engineering crafted flying machines
item_gor_dreks_ointment(i30175)     Protecting Our Own(q10488)
EndContentData */

#include "precompiled.h"
#include "Spell.h"

/*#####
# item_arcane_charges
#####*/

enum
{
    SPELL_ARCANE_CHARGES    = 45072
};

bool ItemUse_item_arcane_charges(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{
    if (pPlayer->isInFlight())
        return false;

    pPlayer->SendEquipError(EQUIP_ERR_NONE, pItem, NULL);

    if (const SpellEntry* pSpellInfo = GetSpellStore()->LookupEntry(SPELL_ARCANE_CHARGES))
        Spell::SendCastResult(pPlayer, pSpellInfo, 1, SPELL_FAILED_NOT_ON_GROUND);

    return true;
}

/*#####
# item_draenei_fishing_net
#####*/

//This is just a hack and should be removed from here.
//Creature/Item are in fact created before spell are sucessfully casted, without any checks at all to ensure proper/expected behavior.
bool ItemUse_item_draenei_fishing_net(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{
    //if (targets.getGOTarget() && targets.getGOTarget()->GetTypeId() == TYPEID_GAMEOBJECT &&
    //targets.getGOTarget()->GetGOInfo()->type == GAMEOBJECT_TYPE_SPELL_FOCUS && targets.getGOTarget()->GetEntry() == 181616)
    //{
    if (pPlayer->GetQuestStatus(9452) == QUEST_STATUS_INCOMPLETE)
    {
        if (!urand(0, 2))
        {
            Creature *Murloc = pPlayer->SummonCreature(17102,pPlayer->GetPositionX() ,pPlayer->GetPositionY()+20, pPlayer->GetPositionZ(), 0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,10000);
            if (Murloc)
                Murloc->AI()->AttackStart(pPlayer);
        }
        else
        {
            ItemPosCountVec dest;
            uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 23614, 1);
            if (msg == EQUIP_ERR_OK)
            {
                Item* item = pPlayer->StoreNewItem(dest,23614,true);
                if (item)
                    pPlayer->SendNewItem(item,1,false,true);
            }else
            pPlayer->SendEquipError(msg,NULL,NULL);
        }
    }
    //}
    return false;
}

/*#####
# item_nether_wraith_beacon
#####*/

bool ItemUse_item_nether_wraith_beacon(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{
    if (pPlayer->GetQuestStatus(10832) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->SummonCreature(22408,pPlayer->GetPositionX() ,pPlayer->GetPositionY()+20, pPlayer->GetPositionZ(), 0,TEMPSUMMON_TIMED_DESPAWN,180000);
        pPlayer->SummonCreature(22408,pPlayer->GetPositionX() ,pPlayer->GetPositionY()-20, pPlayer->GetPositionZ(), 0,TEMPSUMMON_TIMED_DESPAWN,180000);
    }
    return false;
}

/*#####
# item_flying_machine
#####*/

bool ItemUse_item_flying_machine(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{
    uint32 itemId = pItem->GetEntry();

    if (itemId == 34060)
        if (pPlayer->GetBaseSkillValue(SKILL_RIDING) >= 225)
            return false;

    if (itemId == 34061)
        if (pPlayer->GetBaseSkillValue(SKILL_RIDING) == 300)
            return false;

    debug_log("SD2: Player attempt to use item %u, but did not meet riding requirement",itemId);
    pPlayer->SendEquipError(EQUIP_ERR_CANT_EQUIP_SKILL, pItem, NULL);
    return true;
}

/*#####
# item_gor_dreks_ointment
#####*/

enum
{
    NPC_TH_DIRE_WOLF        = 20748,
    SPELL_GORDREKS_OINTMENT = 32578
};

bool ItemUse_item_gor_dreks_ointment(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{
    if (pTargets.getUnitTarget() && pTargets.getUnitTarget()->GetTypeId() == TYPEID_UNIT && pTargets.getUnitTarget()->HasAura(SPELL_GORDREKS_OINTMENT))
    {
        pPlayer->SendEquipError(EQUIP_ERR_NONE, pItem, NULL);

        if (const SpellEntry* pSpellInfo = GetSpellStore()->LookupEntry(SPELL_GORDREKS_OINTMENT))
            Spell::SendCastResult(pPlayer, pSpellInfo, 1, SPELL_FAILED_TARGET_AURASTATE);

        return true;
    }

    return false;
}

bool ItemUse_item_Capteur_Tellurique(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{
	if(pPlayer->GetAreaId() == 4157)
	{
		if(pPlayer->GetPositionZ()  < 185.0f)
		{ 
			pPlayer->TeleportTo(pPlayer->GetMapId(),3329.524f,2543.395f,197.317f,4.950f);
			pPlayer->KilledMonsterCredit(27853,0);
		}
		else
			pPlayer->TeleportTo(pPlayer->GetMapId(),3414.0f,2363.01f,37.911f,3.102f);
	}
	else
	{
		Spell::SendCastResult(pPlayer, GetSpellStore()->LookupEntry(47097), 1, SPELL_FAILED_INCORRECT_AREA);
		return false;
	}
	return true;
}

bool ItemUse_item_Tillinghasts_Plague_Canister(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{
    if(pTargets.getUnitTarget()->GetEntry() == 23777)
	{
		pPlayer->CastSpell(pTargets.getUnitTarget(),43157,true);
        pPlayer->DealDamage(pTargets.getUnitTarget(),pTargets.getUnitTarget()->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
	}
    else
	{
        Spell::SendCastResult(pPlayer, GetSpellStore()->LookupEntry(43157), 1, SPELL_FAILED_BAD_TARGETS);
		return false;
	}
	return true;
}

bool ItemUse_item_ArcaneBinder(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{
	if(pTargets.getUnitTarget()->GetEntry() == 23777)
	{
		// TODO : script de capture du mob
	}
	else
	{
		Spell::SendCastResult(pPlayer, GetSpellStore()->LookupEntry(43157), 1, SPELL_FAILED_BAD_TARGETS);
		return false;
	}
	return true;
}


bool ItemUse_item_RaeloraszSpear(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{
	if(pTargets.getUnitTarget()->GetEntry() == 26127)
	{
		// TODO : script de capture du dragon
	}
	else
	{
		Spell::SendCastResult(pPlayer, GetSpellStore()->LookupEntry(43157), 1, SPELL_FAILED_BAD_TARGETS);
		return false;
	}
	return true;
}

bool ItemUse_item_alystrosAggro(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{
	if(pTargets.getUnitTarget()->GetEntry() == 27249)
	{
		pTargets.getUnitTarget()->AddThreat(pPlayer,20.0f);
	}
	else
	{
		Spell::SendCastResult(pPlayer, GetSpellStore()->LookupEntry(49566), 1, SPELL_FAILED_BAD_TARGETS);
		return false;
	}
	return true;
}

class MANGOS_DLL_DECL BugAura : public Aura
{
    public:
        BugAura(SpellEntry *spell, uint32 eff, int32 *bp, Unit *target, Unit *caster) : Aura(spell, eff, bp, target, caster, NULL)
            {}
};

bool ItemUse_item_zuldrakrat(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{
	if(pTargets.getUnitTarget()->GetEntry() == 28145)
	{
		//pPlayer->CastSpell(pTargets.getUnitTarget(),50894,false);
		Aura* aur;
		SpellEntry *spell = (SpellEntry *)GetSpellStore()->LookupEntry(50894);
		for (int i=0; i<MAX_EFFECT_INDEX; ++i)
        {
			SpellEffectEntry const* effectI = spell->GetSpellEffect(SpellEffectIndex(i));
            if (!effectI || !effectI->Effect)
                continue;
			aur = new BugAura(spell, i, NULL, pTargets.getUnitTarget(), pTargets.getUnitTarget());
            pTargets.getUnitTarget()->AddAura(aur);
			pTargets.getUnitTarget()->AddThreat(pPlayer);
        }

		if(pTargets.getUnitTarget()->GetAura(50894,0)->GetStackAmount() == 5)
		{
			pTargets.getUnitTarget()->RemoveAurasDueToSpell(50894);
			((Creature*)pTargets.getUnitTarget())->UpdateEntry(28203);
			pTargets.getUnitTarget()->CastSpell(pPlayer,50919,false);
		}
	}
	else
	{
		Spell::SendCastResult(pPlayer, GetSpellStore()->LookupEntry(50894), 1, SPELL_FAILED_BAD_TARGETS);
		return false;
	}
	return true;
}

void AddSC_item_scripts()
{
    Script *newscript;

	newscript = new Script;
    newscript->Name = "item_zuldrakrat";
    newscript->pItemUse = &ItemUse_item_zuldrakrat;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_arcane_charges";
    newscript->pItemUse = &ItemUse_item_arcane_charges;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_draenei_fishing_net";
    newscript->pItemUse = &ItemUse_item_draenei_fishing_net;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_nether_wraith_beacon";
    newscript->pItemUse = &ItemUse_item_nether_wraith_beacon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_flying_machine";
    newscript->pItemUse = &ItemUse_item_flying_machine;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_gor_dreks_ointment";
    newscript->pItemUse = &ItemUse_item_gor_dreks_ointment;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "item_capteur_tellurique";
    newscript->pItemUse = &ItemUse_item_Capteur_Tellurique;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "item_peste_Tillinghast";
    newscript->pItemUse = &ItemUse_item_Tillinghasts_Plague_Canister;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "item_arcanebinder";
    newscript->pItemUse = &ItemUse_item_ArcaneBinder;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "item_raelostraszspear";
    newscript->pItemUse = &ItemUse_item_RaeloraszSpear;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "item_alystros";
    newscript->pItemUse = &ItemUse_item_alystrosAggro;
    newscript->RegisterSelf();
}

