/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
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

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "World.h"
#include "Player.h"
#include "Opcodes.h"
#include "Chat.h"
#include "ObjectAccessor.h"
#include "Language.h"
#include "AccountMgr.h"
#include "SystemConfig.h"
#include "revision.h"
#include "revision_nr.h"
#include "Util.h"
#include "WorldSession.h"
#include "GossipDef.h"

bool ChatHandler::HandleHelpCommand(const char* args)
{
    char* cmd = strtok((char*)args, " ");
    if(!cmd)
    {
        ShowHelpForCommand(getCommandTable(), "help");
        ShowHelpForCommand(getCommandTable(), "");
    }
    else
    {
        if(!ShowHelpForCommand(getCommandTable(), cmd))
            SendSysMessage(LANG_NO_HELP_CMD);
    }

    return true;
}

bool ChatHandler::HandleCommandsCommand(const char* /*args*/)
{
    ShowHelpForCommand(getCommandTable(), "");
    return true;
}

bool ChatHandler::HandleAccountCommand(const char* /*args*/)
{
    AccountTypes gmlevel = m_session->GetSecurity();
    PSendSysMessage(LANG_ACCOUNT_LEVEL, uint32(gmlevel));
    return true;
}

bool ChatHandler::HandleStartCommand(const char* /*args*/)
{
    Player *chr = m_session->GetPlayer();

    if(chr->isInFlight())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    if(chr->isInCombat())
    {
        SendSysMessage(LANG_YOU_IN_COMBAT);
        SetSentErrorMessage(true);
        return false;
    }

    // cast spell Stuck
    chr->CastSpell(chr,7355,false);
    return true;
}

bool ChatHandler::HandleServerInfoCommand(const char* /*args*/)
{
    uint32 activeClientsNum = sWorld.GetActiveSessionCount();
    uint32 queuedClientsNum = sWorld.GetQueuedSessionCount();
    uint32 maxActiveClientsNum = sWorld.GetMaxActiveSessionCount();
    uint32 maxQueuedClientsNum = sWorld.GetMaxQueuedSessionCount();
    std::string str = secsToTimeString(sWorld.GetUptime());

    PSendSysMessage("MangosFX rev 78 based on Mangos 9313 developped by Ner'zhul");
    PSendSysMessage(LANG_USING_SCRIPT_LIB,"LibDevFS rev 63, EDC 2.0, ICC 0.1");
    PSendSysMessage(LANG_CONNECTED_USERS, activeClientsNum, maxActiveClientsNum, queuedClientsNum, maxQueuedClientsNum);
    PSendSysMessage(LANG_UPTIME, str.c_str());

    return true;
}

bool ChatHandler::HandleDismountCommand(const char* /*args*/)
{
    //If player is not mounted, so go out :)
    if (!m_session->GetPlayer( )->IsMounted())
    {
        SendSysMessage(LANG_CHAR_NON_MOUNTED);
        SetSentErrorMessage(true);
        return false;
    }

    if(m_session->GetPlayer( )->isInFlight())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    m_session->GetPlayer()->Unmount();
    m_session->GetPlayer()->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
    return true;
}

bool ChatHandler::HandleSaveCommand(const char* /*args*/)
{
    Player *player=m_session->GetPlayer();

    // save GM account without delay and output message (testing, etc)
    if(m_session->GetSecurity() > SEC_PLAYER)
    {
        player->SaveToDB();
        SendSysMessage(LANG_PLAYER_SAVED);
        return true;
    }

    // save or plan save after 20 sec (logout delay) if current next save time more this value and _not_ output any messages to prevent cheat planning
    uint32 save_interval = sWorld.getConfig(CONFIG_INTERVAL_SAVE);
    if (save_interval==0 || (save_interval > 20*IN_MILISECONDS && player->GetSaveTimer() <= save_interval - 20*IN_MILISECONDS))
        player->SaveToDB();

    return true;
}

bool ChatHandler::HandleGMListIngameCommand(const char* /*args*/)
{
    bool first = true;

    HashMapHolder<Player>::MapType &m = HashMapHolder<Player>::GetContainer();
    HashMapHolder<Player>::MapType::const_iterator itr = m.begin();
    for(; itr != m.end(); ++itr)
    {
        AccountTypes itr_sec = itr->second->GetSession()->GetSecurity();
        if ((itr->second->isGameMaster() || (itr_sec > SEC_MODERATOR && itr_sec <= sWorld.getConfig(CONFIG_GM_LEVEL_IN_GM_LIST))) &&
            (!m_session || itr->second->IsVisibleGloballyFor(m_session->GetPlayer())))
        {
            if(first)
            {
                SendSysMessage(LANG_GMS_ON_SRV);
                first = false;
            }

            SendSysMessage(GetNameLink(itr->second).c_str());
        }
    }

    if(first)
        SendSysMessage(LANG_GMS_NOT_LOGGED);

    return true;
}

bool ChatHandler::HandleAccountPasswordCommand(const char* args)
{
    if(!*args)
        return false;

    char *old_pass = strtok ((char*)args, " ");
    char *new_pass = strtok (NULL, " ");
    char *new_pass_c  = strtok (NULL, " ");

    if (!old_pass || !new_pass || !new_pass_c)
        return false;

    std::string password_old = old_pass;
    std::string password_new = new_pass;
    std::string password_new_c = new_pass_c;

    if (password_new != password_new_c)
    {
        SendSysMessage (LANG_NEW_PASSWORDS_NOT_MATCH);
        SetSentErrorMessage (true);
        return false;
    }

    if (!sAccountMgr.CheckPassword (m_session->GetAccountId(), password_old))
    {
        SendSysMessage (LANG_COMMAND_WRONGOLDPASSWORD);
        SetSentErrorMessage (true);
        return false;
    }

    AccountOpResult result = sAccountMgr.ChangePassword(m_session->GetAccountId(), password_new);

    switch(result)
    {
        case AOR_OK:
            SendSysMessage(LANG_COMMAND_PASSWORD);
            break;
        case AOR_PASS_TOO_LONG:
            SendSysMessage(LANG_PASSWORD_TOO_LONG);
            SetSentErrorMessage(true);
            return false;
        case AOR_NAME_NOT_EXIST:                            // not possible case, don't want get account name for output
        default:
            SendSysMessage(LANG_COMMAND_NOTCHANGEPASSWORD);
            SetSentErrorMessage(true);
            return false;
    }

    return true;
}

bool ChatHandler::HandleAccountLockCommand(const char* args)
{
    if (!*args)
    {
        SendSysMessage(LANG_USE_BOL);
        return true;
    }

    std::string argstr = (char*)args;
    if (argstr == "on")
    {
        loginDatabase.PExecute( "UPDATE account SET locked = '1' WHERE id = '%d'",m_session->GetAccountId());
        PSendSysMessage(LANG_COMMAND_ACCLOCKLOCKED);
        return true;
    }

    if (argstr == "off")
    {
        loginDatabase.PExecute( "UPDATE account SET locked = '0' WHERE id = '%d'",m_session->GetAccountId());
        PSendSysMessage(LANG_COMMAND_ACCLOCKUNLOCKED);
        return true;
    }

    SendSysMessage(LANG_USE_BOL);
    return true;
}

/// Display the 'Message of the day' for the realm
bool ChatHandler::HandleServerMotdCommand(const char* /*args*/)
{
    PSendSysMessage(LANG_MOTD_CURRENT, sWorld.GetMotd());
    return true;
}

bool ChatHandler::HandleCoffreCommand(const char *args)
{
	int diamant = 0,saphir = 0;
	QueryResult *result = loginDatabase.PQuery("SELECT credit_diamond FROM account WHERE id = '%u'", m_session->GetAccountId());
	if(result)
	{
		diamant = result->Fetch()->GetInt32();
		delete result;
	}

	result = loginDatabase.PQuery("SELECT point_vote FROM vote_user WHERE uid = '%u'", m_session->GetAccountId());
	if(result)
	{
		saphir = result->Fetch()->GetInt32();
		delete result;
	}
	if(!*args)
	{
		PSendSysMessage("Vous avez %u diamants et %u saphirs",diamant,saphir);
		return true;
	}

	std::string argstr = (char*)args;

	if(argstr == "tigrespectral")
	{
		if(diamant >= 3)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 33225, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				if(Item* pItem = m_session->GetPlayer()->StoreNewItem(dest, 33225, true))
					m_session->GetPlayer()->SendNewItem(pItem, 1, true, false);
				SendSysMessage("Vous avez recu un tigre spectral");
				loginDatabase.PExecute("UPDATE account SET credit_diamond = credit_diamond - 3 WHERE id = '%u'", m_session->GetAccountId());
			}
			else
				SendSysMessage("Vous n'avez plus de place !");
		}
		else
			SendSysMessage("Vous n'avez pas assez de diamants !");
	}
	else if(argstr == "artifice")
	{
		if(diamant >= 1)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 49703, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				if(Item* pItem = m_session->GetPlayer()->StoreNewItem(dest, 49703, true))
					m_session->GetPlayer()->SendNewItem(pItem, 1, true, false);
				SendSysMessage("Vous avez recu des feu d'artifice !");
				loginDatabase.PExecute("UPDATE account SET credit_diamond = credit_diamond - 1 WHERE id = '%u'", m_session->GetAccountId());
			}
			else
				SendSysMessage("Vous n'avez plus de place !");
		}
		else
			SendSysMessage("Vous n'avez pas assez de diamants !");
	}
	else if(argstr == "diablo")
	{
		if(diamant >= 2)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 13584, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				if(Item* pItem = m_session->GetPlayer()->StoreNewItem(dest, 13584, true))
					m_session->GetPlayer()->SendNewItem(pItem, 1, true, false);
				loginDatabase.PExecute("UPDATE account SET credit_diamond = credit_diamond - 2 WHERE id = '%u'", m_session->GetAccountId());
			}
			else
				SendSysMessage("Vous n'avez plus de place !");
		}
		else
			SendSysMessage("Vous n'avez pas assez de diamants !");
	}
	else if(argstr == "tyrael")
	{
		if(diamant >= 2)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 39656, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				if(Item* pItem =	m_session->GetPlayer()->StoreNewItem(dest, 39656, true))
					m_session->GetPlayer()->SendNewItem(pItem, 1, true, false);
				loginDatabase.PExecute("UPDATE account SET credit_diamond = credit_diamond - 2 WHERE id = '%u'", m_session->GetAccountId());
			}
			else
				SendSysMessage("Vous n'avez plus de place !");
		}
		else
			SendSysMessage("Vous n'avez pas assez de diamants !");
	}
	else if(argstr == "dragonnet")
	{
		if(diamant >= 2)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 25535, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				m_session->GetPlayer()->StoreNewItem(dest, 25535, true);
				loginDatabase.PExecute("UPDATE account SET credit_diamond = credit_diamond - 2 WHERE id = '%u'", m_session->GetAccountId());
			}
		}
		else
			SendSysMessage("Vous n'avez pas assez de diamants !");
	}
	else if(argstr == "vip")
	{
		if(diamant >= 1)
		{
			if(m_session->GetSecurity() < 1)
			{
				m_session->GetPlayer()->GetSession()->SetSecurity(AccountTypes(1));

				loginDatabase.PExecute("UPDATE account SET gmlevel = '1', credit_diamond = credit_diamond - 1, vip_end = (SELECT ADDDATE(NOW(), INTERVAL 30 DAY)) WHERE id = '%u'", m_session->GetAccountId());
			}
			else
				SendSysMessage("Vous n'avez pas assez de diamants !");
		}

	}
	else if(argstr == "rename")
    {
        if(diamant >= 2)
        {
			m_session->GetPlayer()->SetAtLoginFlag(AT_LOGIN_RENAME);
            PSendSysMessage("Demande de rename pour %s en cours. Veuillez vous reconnecter pour changer de nom", m_session->GetPlayer()->GetName());
            CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '1' WHERE guid = '%u'", m_session->GetPlayer()->GetGUIDLow());
			loginDatabase.PExecute("UPDATE account SET credit_diamond = credit_diamond - 2 WHERE id = '%u'", m_session->GetAccountId());
        }
    }
    else if(argstr == "relook")
    {
        if(diamant >= 4)
        {
			m_session->GetPlayer()->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
            PSendSysMessage("Demande de relook pour %s en cours. Veuillez vous reconnecter pour changer de skin", m_session->GetPlayer()->GetName());
            CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '8' WHERE guid = '%u'", m_session->GetPlayer()->GetGUIDLow());
			loginDatabase.PExecute("UPDATE account SET credit_diamond = credit_diamond - 4 WHERE id = '%u'", m_session->GetAccountId());
        }
    }
	else if(argstr == "lev60")
	{
		if(m_session->GetPlayer()->getLevel() > 60)
			PSendSysMessage("Vous avez deja un niveau superieur a celui demandé !");
		else
		{
			if(diamant >= 5)
			{
				if(m_session->GetPlayer()->getClass() == CLASS_DEATH_KNIGHT)
					return false;
				Player* plr = m_session->GetPlayer();
				HandleCharacterLevel(plr,plr->GetGUID(),plr->getLevel(),60);
				ItemPosCountVec dest;
				uint8 msg;
				
				plr->learnSpell(34092, 0,true);
				plr->ModifyMoney(9500000);
				switch(plr->getClass())
				{
					case CLASS_WARRIOR:
						plr->learnSpell(71, 0,false);
						plr->learnSpell(2458, 0,false);
						plr->learnSpell(20252, 0,false);
						plr->learnSpell(355, 0,false);
						plr->learnSpell(750, 0,false);
						
						msg = plr->AddItem(22418);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22419, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22419, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22416, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22416, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22423, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22423, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22421, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22421, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22422, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22422, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22417, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22417, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22420, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22420, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 23059, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 23059, true);

						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 21134, 2, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 21134, true);
						break;
					case CLASS_PALADIN:
						plr->learnSpell(7328, 0,false);
						plr->learnSpell(750, 0,false);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22428, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22428, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22429, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22429, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22426, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22426, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22425, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22425, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22424, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22424, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22431, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22431, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22427, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22427, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22430, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22430, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 23066, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 23066, true);

						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 21134, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 21134, true);
						break;
					case CLASS_DEATH_KNIGHT:
						// todo
						break;
					case CLASS_SHAMAN:
						plr->learnSpell(5394, 0,false);
						plr->learnSpell(8071, 0,false);
						plr->learnSpell(3599, 0,false);
						plr->learnSpell(8737, 0,false);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22466, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22466, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22467, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22467, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22464, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22464, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22471, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22471, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22469, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22469, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22470, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22470, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22465, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22465, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22468, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22468, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 23065, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 23065, true);

						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 21839, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 21839, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 21664, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 21664, true);
						break;
					case CLASS_HUNTER:
						plr->learnSpell(6991, 0,false);
						plr->learnSpell(982, 0,false);
						plr->learnSpell(1515, 0,false);
						plr->learnSpell(883, 0,false);
						plr->learnSpell(2641, 0,false);
						plr->learnSpell(8737, 0,false);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22438, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22438, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22439, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22439, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22436, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22436, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22443, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22443, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22441, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22441, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22442, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22442, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22437, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22437, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22440, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22440, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 23067, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 23067, true);

						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 21616, 2, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 21616, true);
						break;
					case CLASS_ROGUE:
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22483, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22483, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22476, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22476, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22481, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22481, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22478, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22478, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22477, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22477, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22479, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22479, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22480, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22480, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22482, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22482, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 23060, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 23060, true);

						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 21126, 2, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 21126, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 21650, 2, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 21650, true);
						break;
					case CLASS_DRUID:
						plr->learnSpell(9634, 0,false);
						plr->learnSpell(768, 0,false);
						plr->learnSpell(6795, 0,false);
						plr->learnSpell(1066, 0,false);
						plr->learnSpell(6807, 0,false);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22492, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22492, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22494, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22494, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22493, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22493, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22490, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22490, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22489, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22489, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22491, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22491, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22488, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22488, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22495, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22495, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 23064, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 23064, true);

						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 21839, 2, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 21839, true);
						break;
					case CLASS_PRIEST:
						plr->AddItem(22518);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22519, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22519, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22514, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22514, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22517, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22517, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22513, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22513, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22512, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22512, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22516, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22516, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22515, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22515, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 23061, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 23061, true);

						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 21839, 2, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 21839, true);
						break;
					case CLASS_MAGE:
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22502, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22502, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22503, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22503, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22498, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22498, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22501, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22501, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22497, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22497, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22496, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22496, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22500, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22500, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22499, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22499, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 23062, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 23062, true);

						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 21622, 2, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 21622, true);
						break;
					case CLASS_WARLOCK:
						plr->learnSpell(688, 0,false);
						plr->learnSpell(697, 0,false);
						plr->learnSpell(712, 0,false);
						plr->learnSpell(691, 0,false);
						plr->AddItem(22510);
						plr->AddItem(22511);
						plr->AddItem(22506);
						plr->AddItem(22509);
						plr->AddItem(22505);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22504, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22504, true);
						msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22508, 1, false);
						if (msg == EQUIP_ERR_OK)
							plr->StoreNewItem(dest, 22508, true);
						plr->AddItem(22507);
						plr->AddItem(23063);
						plr->AddItem(21273);
						break;
					default:
						break;
				}
				loginDatabase.PExecute("UPDATE account SET credit_diamond = credit_diamond - 5 WHERE id = '%u'", m_session->GetAccountId());
			}
		}
	}
	else if(argstr == "lev70")
	{
		if(diamant >= 7)
		{
			Player* plr = m_session->GetPlayer();
			if(plr->getLevel() > 70)
				PSendSysMessage("Vous avez deja un niveau superieur a celui demandé !");
			else
			{
				HandleCharacterLevel(plr,plr->GetGUID(),plr->getLevel(),60);
				
				// monte
				plr->learnSpell(34093, 0,false);
				// secou
				plr->learnSpell(45542,0,false);
				plr->SetSkill(129,plr->GetSkillStep(129),375,375);
				// cuisine
				plr->learnSpell(33359,0,false);
				plr->SetSkill(185,plr->GetSkillStep(185),375,375);
				// peche
				plr->learnSpell(33095,0,false);
				plr->SetSkill(129,plr->GetSkillStep(129),375,375);
				plr->ModifyMoney(60000000);
				switch(player->getClass())
				{
					case CLASS_WARRIOR:
						plr->learnSpell(71, 0,false);
						plr->learnSpell(2458, 0,false);
						plr->learnSpell(20252, 0,false);
						plr->learnSpell(355, 0,false);
						plr->learnSpell(750, 0,false);
						break;
					case CLASS_DEATH_KNIGHT:
					case CLASS_PALADIN:
					case CLASS_HUNTER:
					case CLASS_SHAMAN:
					case CLASS_DRUID:
					case CLASS_ROGUE:
					case CLASS_PRIEST:
					case CLASS_MAGE:
					case CLASS_WARLOCK:
						plr->learnSpell(688, 0,false);
						plr->learnSpell(697, 0,false);
						plr->learnSpell(712, 0,false);
						plr->learnSpell(691, 0,false);
						break;
				}
				
				loginDatabase.PExecute("UPDATE account SET credit_diamond = credit_diamond - 7 WHERE id = '%u'", m_session->GetAccountId());
			}
		}
		else
			PSendSysMessage("Vous n'avez pas assez de diamants !");
	}
	else if(argstr == "xp")
	{
		if(diamant >= 2)
		{
			Player* plr = m_session->GetPlayer();
			
			if(plr->getLevel() > 72)
				PSendSysMessage("Vous ne pouvez pas gagner de niveaux !");
			else
			{
				switch((plr->getLevel() / 10) % 10)
				{
					case 0:
						HandleCharacterLevel(plr,plr->GetGUID(),plr->getLevel(),plr->getLevel() + 9);
						break;
					case 1:
						HandleCharacterLevel(plr,plr->GetGUID(),plr->getLevel(),plr->getLevel() + 7);
						break;
					case 2:
					case 3:
						HandleCharacterLevel(plr,plr->GetGUID(),plr->getLevel(),plr->getLevel() + 5);;
						break;
					case 4:
					case 5:
						HandleCharacterLevel(plr,plr->GetGUID(),plr->getLevel(),plr->getLevel() + 3);
						break;
					case 6:
						HandleCharacterLevel(plr,plr->GetGUID(),plr->getLevel(),plr->getLevel() + 2);
						break;
					case 7:
						HandleCharacterLevel(plr,plr->GetGUID(),plr->getLevel(),plr->getLevel() + 1);
						break;
					default:
						break;
				}
				loginDatabase.PExecute("UPDATE account SET credit_diamond = credit_diamond - 2 WHERE id = '%u'", m_session->GetAccountId());
			}
		}
		else
			PSendSysMessage("Vous n'avez pas assez de diamants !");
	}
	else if(argstr == "money")
	{
		if(saphir >= 40)
		{
			m_session->GetPlayer()->ModifyMoney(1000000);
			loginDatabase.PExecute("UPDATE vote_user SET point_vote = point_vote - 40 WHERE uid = '%u'", m_session->GetAccountId());
		}
		else
			SendSysMessage("Vous n'avez pas assez de saphirs !");
	}
	else if(argstr == "pouletfusee")
	{
		if(saphir >= 50)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 34492, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				m_session->GetPlayer()->StoreNewItem(dest, 34492, true);
				loginDatabase.PExecute("UPDATE vote_user SET point_vote = point_vote - 50 WHERE uid = '%u'", m_session->GetAccountId());
			}
		}
		else
			SendSysMessage("Vous n'avez pas assez de saphirs !");
	}
	else if(argstr == "murloc")
	{
		if(saphir >= 50)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 22114, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				m_session->GetPlayer()->StoreNewItem(dest, 22114, true);
				loginDatabase.PExecute("UPDATE vote_user SET point_vote = point_vote - 50 WHERE uid = '%u'", m_session->GetAccountId());
			}
		}
		else
			SendSysMessage("Vous n'avez pas assez de saphirs !");
	}
	else if(argstr == "putois")
	{
		if(saphir >= 50)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 40653, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				m_session->GetPlayer()->StoreNewItem(dest, 40653, true);
				loginDatabase.PExecute("UPDATE vote_user SET point_vote = point_vote - 50 WHERE uid = '%u'", m_session->GetAccountId());
			}
		}
		else
			SendSysMessage("Vous n'avez pas assez de saphirs !");
	}
	else if(argstr == "moine")
	{
		if(saphir >= 60)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 49665, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				m_session->GetPlayer()->StoreNewItem(dest, 49665, true);
				loginDatabase.PExecute("UPDATE vote_user SET point_vote = point_vote - 60 WHERE uid = '%u'", m_session->GetAccountId());
			}
		}
		else
			SendSysMessage("Vous n'avez pas assez de saphirs !");
	}
	else if(argstr == "miniXT")
	{
		if(saphir >= 65)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 45942, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				m_session->GetPlayer()->StoreNewItem(dest, 45942, true);
				loginDatabase.PExecute("UPDATE vote_user SET point_vote = point_vote - 65 WHERE uid = '%u'", m_session->GetAccountId());
			}
		}
		else
			SendSysMessage("Vous n'avez pas assez de saphirs !");
	}
	else if(argstr == "cerfvolanttuskarr")
	{
		if(saphir >= 60)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 49287, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				m_session->GetPlayer()->StoreNewItem(dest, 49287, true);
				loginDatabase.PExecute("UPDATE vote_user SET point_vote = point_vote - 60 WHERE uid = '%u'", m_session->GetAccountId());
			}
		}
		else
			SendSysMessage("Vous n'avez pas assez de saphirs !");
	}
	else if(argstr == "tabardfureur")
	{
		if(saphir >= 75)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 38313, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				m_session->GetPlayer()->StoreNewItem(dest, 38313, true);
				loginDatabase.PExecute("UPDATE vote_user SET point_vote = point_vote - 75 WHERE uid = '%u'", m_session->GetAccountId());
			}
		}
		else
			SendSysMessage("Vous n'avez pas assez de saphirs !");
	}
	else if(argstr == "tabardarcanes")
	{
		if(saphir >= 75)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 38310, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				m_session->GetPlayer()->StoreNewItem(dest, 38310, true);
				loginDatabase.PExecute("UPDATE vote_user SET point_vote = point_vote - 75 WHERE uid = '%u'", m_session->GetAccountId());
			}
		}
		else
			SendSysMessage("Vous n'avez pas assez de saphirs !");
	}
	else if (argstr == "oursbataille")
	{
		if(saphir >= 100)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 49282, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				m_session->GetPlayer()->StoreNewItem(dest, 49282, true);
				loginDatabase.PExecute("UPDATE vote_user SET point_vote = point_vote - 100 WHERE uid = '%u'", m_session->GetAccountId());
			}
		}
		else
			SendSysMessage("Vous n'avez pas assez de saphirs !");
	}
	else if (argstr == "pouletmagique")
	{
		if(saphir >= 150)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 49290, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				m_session->GetPlayer()->StoreNewItem(dest, 49290, true);
				loginDatabase.PExecute("UPDATE vote_user SET point_vote = point_vote - 150 WHERE uid = '%u'", m_session->GetAccountId());
			}
		}
		else
			SendSysMessage("Vous n'avez pas assez de saphirs !");
	}
	else if(argstr == "drake")
	{
		if(saphir >= 210)
		{
			ItemPosCountVec dest;
			uint8 msg = m_session->GetPlayer()->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 30609, 1, false);
			if (msg == EQUIP_ERR_OK)
			{
				m_session->GetPlayer()->StoreNewItem(dest, 30609, true);
				loginDatabase.PExecute("UPDATE vote_user SET point_vote = point_vote - 210 WHERE uid = '%u'", m_session->GetAccountId());
			}
		}
		else
			SendSysMessage("Vous n'avez pas assez de saphirs !");
	}
	else
	{
		return false;
		// todo : commande inconnue*
	}

	return true;
} 
