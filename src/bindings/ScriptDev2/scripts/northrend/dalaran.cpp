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
SDName: Dalaran
SD%Complete: 100
SDComment:
SDCategory: Dalaran
EndScriptData */

/* ContentData
npc_zirdomi
EndContentData */

#include "precompiled.h"

/*######
## npc_zidormi
######*/

enum
{
    SPELL_TELEPORT_COT          = 46343,
    GOSSIP_TEXTID_ZIDORMI1      = 14066
};

bool GossipHello_npc_zidormi(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->getLevel() >= 65)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take me to the Caverns of Time.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ZIDORMI1, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_zidormi(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        pPlayer->CastSpell(pPlayer,SPELL_TELEPORT_COT,false);

    return true;
}

void SendDefaultMenu_dalaran(Player* pPlayer, Creature* pCreature, uint32 action)
{
	switch (action)
	{
		
		case GOSSIP_ACTION_INFO_DEF + 1: 
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Eastern Sewer Entrance", 2, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Western Sewer Entrance", 2, GOSSIP_ACTION_INFO_DEF + 2);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Well Entrance", 2, GOSSIP_ACTION_INFO_DEF + 3);
			pPlayer->SEND_GOSSIP_MENU(13976, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 2:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Alliance Quarter", 3, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Horde Quarter", 3, GOSSIP_ACTION_INFO_DEF + 2);
			pPlayer->SEND_GOSSIP_MENU(14010, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 3:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Northern Bank", 4, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Southern Bank", 4, GOSSIP_ACTION_INFO_DEF + 2);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Schields", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->SEND_GOSSIP_MENU(14007, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 4: 
			pPlayer->SEND_POI(5892.12, 618.577, 7, 6, 0, "Dalaran Barber");
			pPlayer->SEND_GOSSIP_MENU(14003, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 5: 
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Alliance Quarter", 3, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Horde Quarter", 3, GOSSIP_ACTION_INFO_DEF + 2);
			pPlayer->SEND_GOSSIP_MENU(13977, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 6: 
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Alliance Quarter", 3, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Horde Quarter", 3, GOSSIP_ACTION_INFO_DEF + 2);
			pPlayer->SEND_GOSSIP_MENU(13977, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 7: 
			pPlayer->SEND_POI(5813.37, 453.403, 7, 6, 0, "Dalaran Flight Master");
			pPlayer->SEND_GOSSIP_MENU(14004, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 8: 
			pPlayer->SEND_POI(5767.96, 627.193, 7, 6, 0, "Dalaran Visitor Center");
			pPlayer->SEND_GOSSIP_MENU(14015, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 9: 
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Alliance Inn", 5, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Horde Inn", 5, GOSSIP_ACTION_INFO_DEF + 2);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Schields", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->SEND_POI(5859.86, 634.46, 7, 6, 0, "Dalaran Inn");
			pPlayer->SEND_GOSSIP_MENU(14002, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 10: 
			pPlayer->SEND_POI(5934.13, 575.821, 7, 6, 0, "Dalaran Locksmith");
			pPlayer->SEND_GOSSIP_MENU(14251, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 11:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Inn", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Bank", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Krasus\' Landing", 6, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->SEND_GOSSIP_MENU(14008, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 12:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Alliance Quarter", 3, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Horde Quarter", 3, GOSSIP_ACTION_INFO_DEF + 2);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Violet Citadel", 7, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Violet Hold", 7, GOSSIP_ACTION_INFO_DEF + 2);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Schields", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Toys", 7, GOSSIP_ACTION_INFO_DEF + 3);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Krasus\' Landing", 7, GOSSIP_ACTION_INFO_DEF + 4);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Antonidas Memorial", 7, GOSSIP_ACTION_INFO_DEF + 5);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Runeweaver Square", 7, GOSSIP_ACTION_INFO_DEF + 6);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Eventide", 7, GOSSIP_ACTION_INFO_DEF + 7);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Cemetery", 7, GOSSIP_ACTION_INFO_DEF + 8);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Lexique de puissance", 7, GOSSIP_ACTION_INFO_DEF + 9);
			pPlayer->SEND_GOSSIP_MENU(13975, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 13: 
			pPlayer->SEND_POI(5859.68, 557.571, 7, 6, 0, "Dalaran Stable Master");
			pPlayer->SEND_GOSSIP_MENU(14001, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 14:
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Class Trainer", 8, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Cold Wather Flying Trainer", 8, GOSSIP_ACTION_INFO_DEF + 2);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Portal Trainer", 8, GOSSIP_ACTION_INFO_DEF + 3);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Profession Trainer", 8, GOSSIP_ACTION_INFO_DEF + 4);
			pPlayer->SEND_GOSSIP_MENU(14000, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 15: 
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Armor", 9, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Clothing", 9, GOSSIP_ACTION_INFO_DEF + 2);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Emblem Gear", 9, GOSSIP_ACTION_INFO_DEF + 3);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Flowers", 9, GOSSIP_ACTION_INFO_DEF + 4);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Fruit", 9, GOSSIP_ACTION_INFO_DEF + 5);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "General Goods", 9, GOSSIP_ACTION_INFO_DEF + 6);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Jewelry", 9, GOSSIP_ACTION_INFO_DEF + 7);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Pet Supplies & Exotic Mounts", 9, GOSSIP_ACTION_INFO_DEF + 8);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Pie, Pastry & Cakes", 9, GOSSIP_ACTION_INFO_DEF + 9);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Reagents & Magical Goods", 9, GOSSIP_ACTION_INFO_DEF + 10);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Trade District", 9, GOSSIP_ACTION_INFO_DEF + 11);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Trade Supply", 9, GOSSIP_ACTION_INFO_DEF + 12);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Trinkets, Relics && Off-hand Items", 9, GOSSIP_ACTION_INFO_DEF + 13);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Weapons", 9, GOSSIP_ACTION_INFO_DEF + 14);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Wine & Cheese", 9, GOSSIP_ACTION_INFO_DEF + 15);
			pPlayer->SEND_GOSSIP_MENU(14117, pCreature->GetGUID());
			break;
		}

}

void SendArenaMenu_dalaran(Player* pPlayer, Creature* pCreature, uint32 action)
{
	switch(action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1: pPlayer->SEND_POI(5805.33, 556.04, 7, 6, 0, "Dalaran Eastern Sewer Entrance");
		pPlayer->SEND_GOSSIP_MENU(13961, pCreature->GetGUID());
		break;
	case GOSSIP_ACTION_INFO_DEF + 2: pPlayer->SEND_POI(5816.37, 758.51, 7, 6, 0, "Dalaran Western Sewer Entrance");
		pPlayer->SEND_GOSSIP_MENU(13960, pCreature->GetGUID());
		break;
	case GOSSIP_ACTION_INFO_DEF + 3: pPlayer->SEND_POI(5885.96, 647.26, 7, 6, 0, "Dalaran Well");
		pPlayer->SEND_GOSSIP_MENU(13969, pCreature->GetGUID());
		break;
	}
}
void SendQuarterMenu_dalaran(Player* pPlayer, Creature* pCreature, uint32 action)
{
	switch(action)
	{
		case GOSSIP_ACTION_INFO_DEF + 1: pPlayer->SEND_POI(5762.29, 712.98, 7, 6, 0, "Dalaran Silver Enclave");
		pPlayer->SEND_GOSSIP_MENU(13973, pCreature->GetGUID());
		break;
		case GOSSIP_ACTION_INFO_DEF + 2: pPlayer->SEND_POI(5857.64, 593.74, 7, 6, 0, "Dalaran Sunreaver's Sanctuary");
		pPlayer->SEND_GOSSIP_MENU(13973, pCreature->GetGUID());
		break;
	}
}


void SendBankMenu_dalaran(Player* pPlayer, Creature* pCreature, uint32 action)
{
	switch(action)
	{

		case GOSSIP_ACTION_INFO_DEF + 1: pPlayer->SEND_POI(5926.78, 646.52, 7, 6, 0, "Dalaran Northern Bank");
		pPlayer->SEND_GOSSIP_MENU(14005, pCreature->GetGUID());
		break;
		case GOSSIP_ACTION_INFO_DEF + 2: pPlayer->SEND_POI(5679.18, 666.92, 7, 6, 0, "Dalaran Northern Bank");
		pPlayer->SEND_GOSSIP_MENU(14006, pCreature->GetGUID());
		break;
	}

}
void SendInnMenu_dalaran(Player* pPlayer, Creature* pCreature, uint32 action)
{
	switch(action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1: pPlayer->SEND_POI(5735.24, 676.15, 7, 6, 0, "Dalaran Alliance Inn");
		pPlayer->SEND_GOSSIP_MENU(13992, pCreature->GetGUID());
		break;
	case GOSSIP_ACTION_INFO_DEF + 2: pPlayer->SEND_POI(5885.10, 527.30, 7, 6, 0, "Dalaran Horde Inn");
		pPlayer->SEND_GOSSIP_MENU(13993, pCreature->GetGUID());
		break;
	}

}
void SendMailMenu_dalaran(Player* pPlayer, Creature* pCreature, uint32 action)
{
	switch(action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1:pPlayer->SEND_POI(5831.95, 507.86, 7, 6, 0, "Krasus\' Landing");
		pPlayer->SEND_GOSSIP_MENU(14009, pCreature->GetGUID());
		break;
	}
}
void SendPoiMenu_dalaran(Player* pPlayer, Creature* pCreature, uint32 action)
{
	switch(action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1: pPlayer->SEND_POI(5790.33, 737.05, 7, 6, 0, "Dalaran Violet Citadel");
			pPlayer->SEND_GOSSIP_MENU(13971, pCreature->GetGUID());
			break;
	case GOSSIP_ACTION_INFO_DEF + 2: pPlayer->SEND_POI(5692.27, 500.17, 7, 6, 0, "Dalaran Violet Hold");
			pPlayer->SEND_GOSSIP_MENU(13970, pCreature->GetGUID());
			break;
	case GOSSIP_ACTION_INFO_DEF + 3: pPlayer->SEND_POI(5897.56, 733.24, 7, 6, 0, "Dalaran Trade District");
			pPlayer->SEND_GOSSIP_MENU(13980, pCreature->GetGUID());
			break;
	case GOSSIP_ACTION_INFO_DEF + 4: pPlayer->SEND_POI(5831.99, 657.70, 7, 6, 0, "Dalaran Krasus\' Landing");
			pPlayer->SEND_GOSSIP_MENU(14009, pCreature->GetGUID());
			break;
	case GOSSIP_ACTION_INFO_DEF + 5: pPlayer->SEND_POI(5949.71, 669.26, 7, 6, 0, "Dalaran Antonidas Memorial");
			pPlayer->SEND_GOSSIP_MENU(13968, pCreature->GetGUID());
			break;
	case GOSSIP_ACTION_INFO_DEF + 6: pPlayer->SEND_POI(5805.78, 647.68, 7, 6, 0, "Dalaran Runeweaver Square");
			pPlayer->SEND_GOSSIP_MENU(13967, pCreature->GetGUID());
			break;
	case GOSSIP_ACTION_INFO_DEF + 7: pPlayer->SEND_POI(5713.84, 646.22, 7, 6, 0, "Dalaran Eventine");
			pPlayer->SEND_GOSSIP_MENU(13966, pCreature->GetGUID());
			break;
	case GOSSIP_ACTION_INFO_DEF + 8: pPlayer->SEND_POI(5856.75, 697.99, 7, 6, 0, "Dalaran Cemetary");
			pPlayer->SEND_GOSSIP_MENU(13965, pCreature->GetGUID());
			break;
	case GOSSIP_ACTION_INFO_DEF + 9: pPlayer->SEND_POI(5848.27, 766.34, 7, 6, 0, "Dalaran Inscription Trainer");
			pPlayer->SEND_GOSSIP_MENU(14174, pCreature->GetGUID());
			break;
	}
}
void SendTrainersMenu_dalaran(Player* pPlayer, Creature* pCreature, uint32 action)
{
	switch(action)
	{
		case GOSSIP_ACTION_INFO_DEF + 1: pPlayer->SEND_POI(5807.69, 586.88, 7, 6, 0, "Dalaran Mage Trainer");
			pPlayer->SEND_GOSSIP_MENU(13999, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 2: pPlayer->SEND_POI(5815.27, 473.49, 7, 6, 0, "Dalaran Krasus\' Landing");
			pPlayer->SEND_GOSSIP_MENU(13998, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 3: pPlayer->SEND_POI(5807.69, 586.88, 7, 6, 0, "Dalaran Mage Trainer");
			pPlayer->SEND_GOSSIP_MENU(13999, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 4: pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Alchemy", 12, GOSSIP_ACTION_INFO_DEF + 1);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Blacksmithing", 12, GOSSIP_ACTION_INFO_DEF + 2);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Cooking", 12, GOSSIP_ACTION_INFO_DEF + 3);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Enchanting", 12, GOSSIP_ACTION_INFO_DEF + 4);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Engineering", 12, GOSSIP_ACTION_INFO_DEF + 5);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "First Aid", 12, GOSSIP_ACTION_INFO_DEF + 6);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Fishing", 12, GOSSIP_ACTION_INFO_DEF + 7);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Herbalism", 12, GOSSIP_ACTION_INFO_DEF + 8);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Inscription", 12, GOSSIP_ACTION_INFO_DEF + 9);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Jewelcrafting", 12, GOSSIP_ACTION_INFO_DEF + 10);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Leatherworking", 12, GOSSIP_ACTION_INFO_DEF + 11);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Mining", 12, GOSSIP_ACTION_INFO_DEF + 12);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Skinning", 12, GOSSIP_ACTION_INFO_DEF + 13);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Tailoring", 12, GOSSIP_ACTION_INFO_DEF + 14);
		pPlayer->SEND_GOSSIP_MENU(13996, pCreature->GetGUID());
	}

}

void SendVendorsMenu_dalaran(Player* pPlayer, Creature* pCreature, uint32 action)
{

	switch(action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1:pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Cloth Armor", 10, GOSSIP_ACTION_INFO_DEF + 1);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Leather Armor", 10, GOSSIP_ACTION_INFO_DEF + 2);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Mail Armor", 10, GOSSIP_ACTION_INFO_DEF + 3);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Plate Armor", 10, GOSSIP_ACTION_INFO_DEF + 4);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Schields", 10, GOSSIP_ACTION_INFO_DEF + 5);
	pPlayer->SEND_GOSSIP_MENU(14114, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 2:pPlayer->SEND_POI(5786.20, 647.08, 7, 6, 0, "Dalaran Cloth Armor & Clothing");
	pPlayer->SEND_GOSSIP_MENU(14112, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 3: pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Alliance Quarter", 3, GOSSIP_ACTION_INFO_DEF + 1);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "The Horde Quarter", 3, GOSSIP_ACTION_INFO_DEF + 2);
		pPlayer->SEND_GOSSIP_MENU(14108, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 4:pPlayer->SEND_POI(5773.01, 577.11, 7, 6, 0, "Dalaran Flowers");
		pPlayer->SEND_GOSSIP_MENU(14107, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 5:pPlayer->SEND_POI(5753.19, 694.93, 7, 6, 0, "Dalaran Fruit Vendor");
		pPlayer->SEND_GOSSIP_MENU(14106, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 6:pPlayer->SEND_POI(5688.13, 648.05, 7, 6, 0, "Dalaran General & Trade Store");
		pPlayer->SEND_GOSSIP_MENU(14105, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 7:pPlayer->SEND_POI(5871.58, 725.30, 7, 6, 0, "Dalaran Jewelcrafting Trainer");
		pPlayer->SEND_GOSSIP_MENU(13984, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 8:pPlayer->SEND_POI(5833.19, 571.80, 7, 6, 0, "Dalaran Pet Store");
		pPlayer->SEND_GOSSIP_MENU(14103, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 9:pPlayer->SEND_POI(5906.07, 627.51, 7, 6, 0, "Dalaran Pie, Pastry & Cake");
		pPlayer->SEND_GOSSIP_MENU(14102, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 10:pPlayer->SEND_POI(5767.48, 732.60, 7, 6, 0, "Dalaran Reagents");
		pPlayer->SEND_GOSSIP_MENU(14104, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 11:pPlayer->SEND_POI(5819.48, 696.89, 7, 6, 0, "Dalaran Toy Store");
		pPlayer->SEND_GOSSIP_MENU(14100, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 12:pPlayer->SEND_POI(5690.12, 621.70, 7, 6, 0, "Dalaran General & Trade Store");
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Profession Trainer", 8, GOSSIP_ACTION_INFO_DEF + 4);
		pPlayer->SEND_GOSSIP_MENU(14101, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 13:pPlayer->SEND_POI(5762.18, 638.81, 7, 6, 0, "Dalaran Trinkets");
		pPlayer->SEND_GOSSIP_MENU(14110, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 14:pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Melee Weapons", 11, GOSSIP_ACTION_INFO_DEF + 1);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ranged & Thrown Weapons", 11, GOSSIP_ACTION_INFO_DEF + 2);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Staves & Wands", 11, GOSSIP_ACTION_INFO_DEF + 3);
		pPlayer->SEND_GOSSIP_MENU(14113, pCreature->GetGUID());
	break;
	case GOSSIP_ACTION_INFO_DEF + 15:pPlayer->SEND_POI(5879.49, 612.42, 7, 6, 0, "Dalaran Wine & Cheese");
		pPlayer->SEND_GOSSIP_MENU(14095, pCreature->GetGUID());
	break;
	}
}
void SendArmorMenu_dalaran(Player* pPlayer, Creature* pCreature, uint32 action)
{
	switch(action)
	{
		case GOSSIP_ACTION_INFO_DEF + 1:pPlayer->SEND_POI(5786.20, 647.08, 7, 6, 0, "Dalaran Cloth Armor & Clothing");
		pPlayer->SEND_GOSSIP_MENU(14112, pCreature->GetGUID());
		break;
		case GOSSIP_ACTION_INFO_DEF + 2:pPlayer->SEND_POI(5675.09, 630.65, 7, 6, 0, "Dalaran Leather & Mail Armor");
		pPlayer->SEND_GOSSIP_MENU(14111, pCreature->GetGUID());
		break;
		case GOSSIP_ACTION_INFO_DEF + 3:pPlayer->SEND_POI(5675.09, 630.65, 7, 6, 0, "Dalaran Plate Armor & Shields");
		pPlayer->SEND_GOSSIP_MENU(14109, pCreature->GetGUID());
		break;
		case GOSSIP_ACTION_INFO_DEF + 4:pPlayer->SEND_POI(5675.09, 630.65, 7, 6, 0, "Dalaran Plate Armor & Shields");
		pPlayer->SEND_GOSSIP_MENU(14109, pCreature->GetGUID());
		break;
	}
}

void SendWeaponsMenu_dalaran(Player* pPlayer, Creature* pCreature, uint32 action)
{
	switch(action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1:pPlayer->SEND_POI(5731.85, 608.23, 7, 6, 0, "Dalaran Melee Weapons");
		pPlayer->SEND_GOSSIP_MENU(14098, pCreature->GetGUID());
		break;
	case GOSSIP_ACTION_INFO_DEF + 2:pPlayer->SEND_POI(5783.44, 561.81, 7, 6, 0, "Dalaran Ranged Weapons");
		pPlayer->SEND_GOSSIP_MENU(14097, pCreature->GetGUID());
		break;
	case GOSSIP_ACTION_INFO_DEF + 3:pPlayer->SEND_POI(5674.73, 647.00, 7, 6, 0, "Dalaran Staves & Wands");
		pPlayer->SEND_GOSSIP_MENU(14096, pCreature->GetGUID());
	break;
	}
}

void SendProfTrainerMenu_dalaran(Player* pPlayer, Creature* pCreature, uint32 action)
{
	switch(action)
	{
		case GOSSIP_ACTION_INFO_DEF + 1:pPlayer->SEND_POI(5895.75, 711.08, 7, 6, 0, "Dalaran Alchemy Trainer");
			pPlayer->SEND_GOSSIP_MENU(13995, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 2:pPlayer->SEND_POI(5900.17, 686.36, 7, 6, 0, "Dalaran Blacksmithing Trainer");
			pPlayer->SEND_GOSSIP_MENU(13994, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 3:pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Alliance Inn", 5, GOSSIP_ACTION_INFO_DEF + 1);
			pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Horde Inn", 5, GOSSIP_ACTION_INFO_DEF + 2);
			pPlayer->SEND_GOSSIP_MENU(13991, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 4:pPlayer->SEND_POI(5835.09, 719.40, 7, 6, 0, "Dalaran Enchanting Trainer");
			pPlayer->SEND_GOSSIP_MENU(13990, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 5:pPlayer->SEND_POI(5914.88, 728.24, 7, 6, 0, "Dalaran Engineering Trainer");
			pPlayer->SEND_GOSSIP_MENU(13989, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 6:pPlayer->SEND_POI(5864.82, 737.73, 7, 6, 0, "Dalaran First Aid Trainer");
			pPlayer->SEND_GOSSIP_MENU(13988, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 7:pPlayer->SEND_POI(5703.40, 615.44, 7, 6, 0, "Dalaran Fishing Fountain");
			pPlayer->SEND_GOSSIP_MENU(13987, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 8:pPlayer->SEND_POI(5872.11, 687.78, 7, 6, 0, "Dalaran Herbalism Trainer");
			pPlayer->SEND_GOSSIP_MENU(13986, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 9:pPlayer->SEND_POI(5859.55, 700.48, 7, 6, 0, "Dalaran Inscription Trainer");
			pPlayer->SEND_GOSSIP_MENU(13985, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 10:pPlayer->SEND_POI(5872.69, 724.94, 7, 6, 0, "Dalaran Jewelcrafting Trainer");
			pPlayer->SEND_GOSSIP_MENU(13984, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 11:pPlayer->SEND_POI(5902.58, 746.44, 7, 6, 0, "Dalaran Leatherworking Trainer");
			pPlayer->SEND_GOSSIP_MENU(13982, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 12:pPlayer->SEND_POI(5914.93, 710.01, 7, 6, 0, "Dalaran Mining Trainer");
			pPlayer->SEND_GOSSIP_MENU(13983, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 13:pPlayer->SEND_POI(5902.58, 746.44, 7, 6, 0, "Dalaran Leatherworking Trainer");
			pPlayer->SEND_GOSSIP_MENU(13982, pCreature->GetGUID());
			break;
		case GOSSIP_ACTION_INFO_DEF + 14:pPlayer->SEND_POI(5883.58, 741.38, 7, 6, 0, "Dalaran Tailoring Trainer");
			pPlayer->SEND_GOSSIP_MENU(13981, pCreature->GetGUID());
			break;
	}
}

bool GossipHello_dalaran_poi(Player* pPlayer, Creature* pCreature)
{
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Arena", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Action House", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Bank", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Barber", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Battlemasters", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Capital Portals", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Flight Master", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Guild Master", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Inn", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Locksmith", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Mailbox", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Points of Interest", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Stable Master"    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Trainers"    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Vendors"     , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
    pPlayer->SEND_GOSSIP_MENU(14014, pCreature->GetGUID());
    return true;
}
bool GossipSelect_poi_dalaran(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
        case 1:         SendDefaultMenu_dalaran(pPlayer, pCreature, action); break;
		case 2:         SendArenaMenu_dalaran(pPlayer,  pCreature,  action); break;
		case 3:         SendQuarterMenu_dalaran(pPlayer,  pCreature,  action); break;
		case 4:			SendBankMenu_dalaran(pPlayer,  pCreature,  action); break;
		case 5:               SendInnMenu_dalaran(pPlayer,  pCreature,  action); break;
		case 6:            SendMailMenu_dalaran(pPlayer,  pCreature,  action); break;
		case 7:             SendPoiMenu_dalaran(pPlayer,  pCreature,  action); break;
		case 8:        SendTrainersMenu_dalaran(pPlayer,  pCreature,  action); break;
		case 9:         SendVendorsMenu_dalaran(pPlayer,  pCreature,  action); break;
		case 10:           SendArmorMenu_dalaran(pPlayer,  pCreature,  action); break;
		case 11:         SendWeaponsMenu_dalaran(pPlayer,  pCreature,  action); break;
		case 12:     SendProfTrainerMenu_dalaran(pPlayer,  pCreature,  action); break;
    }
    return true;
}

/*######
## npc_hira_snowdawn
######*/
bool GossipHello_npc_hira_snowdawn(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isTrainer())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, GOSSIP_TEXT_TRAIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);
    if (pCreature->isVendor() && pPlayer->getLevel() >= 80 && pPlayer->HasSpell(54197))  // Cold Weather Flying
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}
bool GossipSelect_npc_hira_snowdawn(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());
    if (uiAction == GOSSIP_ACTION_TRAIN)
        pPlayer->SEND_TRAINERLIST(pCreature->GetGUID());
    return true;
}

void AddSC_dalaran()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_zidormi";
    newscript->pGossipHello = &GossipHello_npc_zidormi;
    newscript->pGossipSelect = &GossipSelect_npc_zidormi;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "poi_dalaran";
    newscript->pGossipSelect = &GossipSelect_poi_dalaran;
	newscript->pGossipHello= &GossipHello_dalaran_poi;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_snowdawn";
    newscript->pGossipSelect = &GossipSelect_npc_hira_snowdawn;
	newscript->pGossipHello= &GossipHello_npc_hira_snowdawn;
    newscript->RegisterSelf();
}
