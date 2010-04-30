#include "precompiled.h"
#include "trial_of_the_crusader.h"

void npc_toc10_announcerAI::Reset()
{
	Tasks.SetObjects(this,me);
	EventStarted = false;
	HeroicMode = false;
	Event_Timer = TEN_MINS*1000;
	Event_Type = EVENT_TYPE_BEASTS_INTRO;
	Fordring = NULL;
	Varian = NULL;
	check_Timer = 1000;
	Event_phase = 1;
	Flopboum = NULL;
	Champions_Guid.clear();
	switch(Difficult)
	{
		case RAID_DIFFICULTY_10MAN_HEROIC:
		case RAID_DIFFICULTY_25MAN_HEROIC:
			HeroicMode = true;
			Try_Heroic = 50;
			break;
	}
	for(uint8 i=0;i<8;i++)
		ID[i] = 0;
}

void npc_toc10_announcerAI::SpawnChampions()
{
	uint8 nb_mobs,nb_heals,nb_cac;

	switch(Difficult)
	{
		case RAID_DIFFICULTY_10MAN_NORMAL:
		case RAID_DIFFICULTY_10MAN_HEROIC:
			nb_heals = urand(1,2);
			nb_mobs = 6;
			nb_cac = urand(nb_mobs - nb_heals - 3,nb_mobs - nb_heals - 1);
			break;
		case RAID_DIFFICULTY_25MAN_NORMAL:
		case RAID_DIFFICULTY_25MAN_HEROIC:
			nb_heals = urand(2,3);
			nb_mobs = 8;
			nb_cac = urand(nb_mobs - nb_heals - 4,nb_mobs - nb_heals - 2);
			break;
	}
	uint8 popZone = 1,fac = 0;
	if(team == ALLIANCE)
	{
		popZone = 0;
		fac = 1;
	}

	if(ID[0] && ID[0] != 0)
	{
		for(uint8 i=0;i<nb_mobs;i++)
		{
			if(Creature* tmp = Tasks.CallCreature(ID[i],TEN_MINS,PREC_COORDS,NOTHING,Champion_spawn[i][popZone][0],
			Champion_spawn[i][popZone][1],Champion_spawn[i][popZone][2]))
			{
				/*if(team == ALLIANCE)
					tmp->setFaction(85);
				else
					tmp->setFaction(12);*/
				Champions_Guid.push_back(tmp->GetGUID());
			}
		}
	}
	else
	{
		for(uint8 i=0;i<nb_heals;i++)
		{
			ID[i] = Champion_Entries[urand(0,3)][fac];
			if(i < 3)
			{
				switch(i)
				{
					case 1:
						if(ID[i] == ID[0])
							i--;
						break;
					case 2:
						if(ID[i] == ID[0] || ID[i] == ID[1])
							i--;
						break;
				}
			}
			else if(i == 3)
			{
				if(ID[i] == ID[0] || ID[i] == ID[1] || ID[i] == ID[2])
					i--;
			}
		}

		for(uint8 i=nb_heals;i<nb_heals + nb_cac;i++)
		{
			ID[i] = Champion_Entries[urand(4,7)][fac];
			if(i != nb_heals)
			{
				if(i == nb_heals + 1)
				{
					if(ID[i] == ID[nb_heals])
						i--;
				}
				else if(i == nb_heals + 2)
				{
					if(ID[i] == ID[nb_heals] || ID[i] == ID[nb_heals + 1])
						i--;
				}
				else if(i == nb_heals + 3)
				{
					if(ID[i] == ID[nb_heals] || ID[i] == ID[nb_heals + 1] || ID[i] == ID[nb_heals + 2])
						i--;
				}
				else if(i == nb_heals + 4)
				{
					if(ID[i] == ID[nb_heals] || ID[i] == ID[nb_heals + 1] || ID[i] == ID[nb_heals + 2]
					|| ID[i] == ID[nb_heals + 3])
						i--;
				}
				else if(i == nb_heals + 5)
				{
					if(ID[i] == ID[nb_heals] || ID[i] == ID[nb_heals + 1] || ID[i] == ID[nb_heals + 2]
					|| ID[i] == ID[nb_heals + 3] || ID[i] == ID[nb_heals + 4])
						i--;
				}
			}
		}
		for(uint8 i=(nb_heals + nb_cac);i<(nb_mobs);i++)
		{
			ID[i] = Champion_Entries[urand(8,13)][fac];
			if(i != (nb_heals + nb_cac))
			{
				if(i == (nb_heals + nb_cac) + 1)
				{
					if(ID[i] == ID[(nb_heals + nb_cac)])
						i--;
				}
				else if(i == (nb_heals + nb_cac) + 2)
				{
					if(ID[i] == ID[(nb_heals + nb_cac)] || ID[i] == ID[(nb_heals + nb_cac) + 1])
						i--;
				}
				else if(i == (nb_heals + nb_cac) + 3)
				{
					if(ID[i] == ID[(nb_heals + nb_cac)] || ID[i] == ID[(nb_heals + nb_cac) + 1] || ID[i] == ID[(nb_heals + nb_cac) + 2])
						i--;
				}
				else if(i == (nb_heals + nb_cac) + 4)
				{
					if(ID[i] == ID[(nb_heals + nb_cac)] || ID[i] == ID[(nb_heals + nb_cac) + 1] || ID[i] == ID[(nb_heals + nb_cac) + 2]
					|| ID[i] == ID[(nb_heals + nb_cac) + 3])
						i--;
				}
			}
		}
		
		for(uint8 i=0;i<nb_mobs;i++)
		{
			if(Creature* tmp = Tasks.CallCreature(ID[i],TEN_MINS,PREC_COORDS,NOTHING,Champion_spawn[i][popZone][0],
				Champion_spawn[i][popZone][1],Champion_spawn[i][popZone][2]))
			{
				/*if(team == ALLIANCE)
					tmp->setFaction(85);
				else
					tmp->setFaction(12);*/

				Champions_Guid.push_back(tmp->GetGUID());
			}
		}
	}
}
void npc_toc10_announcerAI::StartEvent(Unit* pwho, uint32 type)
{
	EventStarted = true;
	Event_phase = 1;

	if (Creature* tmp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_FORDRING))))
		Fordring = tmp;
	if (Creature* tmp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_VARIAN))))
		Varian = tmp;
	if (Creature* tmp = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(DATA_GARROSH))))
		Garrosh = tmp;
	if(pwho && pwho->GetTypeId() == TYPEID_PLAYER)
		team = ((Player*)pwho)->GetTeam();
	if(type > 0)
		Event_Type = type;
	else
	{
		if (pInstance->GetData(TYPE_CHAMPIONS) == DONE)
			Event_Type = EVENT_TYPE_VALKYR;
		else if (pInstance->GetData(TYPE_JARAXXUS) == DONE)
			Event_Type = EVENT_TYPE_CHAMPIONS;
		else if (pInstance->GetData(TYPE_EVENT_BEAST) == DONE)
			Event_Type = EVENT_TYPE_JARAXXUS;
		else 
			Event_Type = EVENT_TYPE_BEASTS_INTRO;
	}
	Event_Timer = 1000;
	if(type == EVENT_TYPE_JARAXXUS_OUTRO)
		Event_Timer = 6000;
}

void npc_toc10_announcerAI::UpdateAI(const uint32 diff)
{
	if(check_Timer <= diff)
	{
		Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
		bool playerAlive = false;
		if (!lPlayers.isEmpty())
		{
			for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				if (Player* pPlayer = itr->getSource())
					if(pPlayer->isAlive() && !pPlayer->isGameMaster())
						playerAlive = true;
		}

		if(!playerAlive)
		{
			if (Creature* Gormok = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_GormoktheImpaler))))
				Gormok->RemoveFromWorld();
			if (Creature* Acidmaw = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_Acidmaw))))
				Acidmaw->RemoveFromWorld();
			if (Creature* Dreadscale = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_Dreadscale))))
				Dreadscale->RemoveFromWorld();
			if (Creature* Icehowl = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_Icehowl))))
				Icehowl->RemoveFromWorld();
			if (Creature* Jaraxxus = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_JARAXXUS))))
				Jaraxxus->RemoveFromWorld();
			if (Creature* Darkbane = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_Eydis_Darkbane))))
				Darkbane->RemoveFromWorld();
			if (Creature* Lightbane = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_Fjola_Lightbane))))
				Lightbane->RemoveFromWorld();
			for(std::vector<uint32>::const_iterator itr = Champions_Guid.begin(); itr != Champions_Guid.end(); ++itr)
			{
				if (Creature* tmp = ((Creature*)Unit::GetUnit((*me), (*itr))))
					if(tmp->isAlive())
						tmp->RemoveFromWorld();
			}
			Champions_Guid.clear();
			EventStarted = false;
			me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
		}
		check_Timer = 1000;
	}
	else
		check_Timer -= diff;

	if(!EventStarted)
	        return;

	if(pInstance->GetData(TYPE_TRY) <= 0)
		return;

	switch(Event_Type)
	{
		case EVENT_TYPE_BEASTS_INTRO:
			if(Event_Timer <= diff)
			{
				switch(Event_phase)
				{
					case 1:
						me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
						UpdateWStates();
						Tasks.Speak(CHAT_TYPE_SAY,16036,"Bienvenue champions ! Vous avez entendu l'appel de la croisade d'argent et vaillament répondu. C'est ici dans le colisée d'argent que vous affronterez vos plus grands défis. Ceux d'entre vous qui auront survécu aux vigueurs du colisée se joindrons à la marche de la croisade sur la Citadelle de la Couronne de Glace",Fordring);
						Event_Timer = 21000;
						break;
					case 2:
						Tasks.Speak(CHAT_TYPE_YELL,16037,"Que les jeux commencent!",Fordring);
						if(Creature* Gormok = Tasks.CallCreature(34796,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,563.135f,	243.306f,395.0f))
						{
							Gormok->GetMotionMaster()->MovePoint(0, 563.832f, 180.223f, 395.0f);
						}
						pInstance->DoUseDoorOrButton(pInstance->GetData64(TYPE_DOOR));
						Event_Timer = 2000;
						break;
					case 3:
						Tasks.Speak(CHAT_TYPE_YELL,16038,"Arrivant tout droit des plus noires et profondes cavernes des pics foudroyés, Gormok l'empaleur !",Fordring);
						Event_Timer = 8000;
						break;
					case 4:
						if(team == ALLIANCE)
							Tasks.Speak(CHAT_TYPE_SAY,16069,"Vos bêtes ne feront pas le poids face à mes champions Tyrion.",Varian);
						else
							Tasks.Speak(CHAT_TYPE_SAY,16026,"J'ai vu des défis plus ardus dans l'arène de sang. Vous nous faîtes perdre notre temps Paladin !",Garrosh);
						Event_Timer = TEN_MINS;
						
						pInstance->DoUseDoorOrButton(pInstance->GetData64(TYPE_DOOR));
						EventStarted = false;
						break;
				}
				Event_phase++;
			}
			else 
				Event_Timer -= diff;
			break;
		case EVENT_TYPE_BEASTS_JORM:
			if(Event_Timer <= diff)
			{
				switch(Event_phase)
				{
					case 1:
						Tasks.Speak(CHAT_TYPE_YELL,16039,"Apprêtez vous héros car voici que les terreurs jumelles, Gueule d'Acide et Ecaille d'effroi pénètrent dans l'arène !",Fordring);
						if(Creature* Acidmaw = Tasks.CallCreature(35144,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,572.243f,244.680f,395.0f))
						{
							Acidmaw->GetMotionMaster()->MovePoint(0, 574.376f,180.478f,396.0f);
							Acidmaw->SetSpeedRate(MOVE_RUN,1.3f,true);
						}
						if(Creature* Dreadscale = Tasks.CallCreature(34799,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,554.910f,244.888f,395.0f))
						{
							Dreadscale->GetMotionMaster()->MovePoint(0, 563.832f, 180.223f, 395.0f);
							Dreadscale->SetSpeedRate(MOVE_RUN,1.3f,true);
						}
						pInstance->DoUseDoorOrButton(pInstance->GetData64(TYPE_DOOR));
						Event_Timer = 8000;
						break;
					case 2:
						if (Creature* Acidmaw = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_Dreadscale))))
						{
							((ScriptedAI*)Acidmaw->AI())->SetCombatMovement(false);
						}
						pInstance->DoUseDoorOrButton(pInstance->GetData64(TYPE_DOOR));
						Event_Timer = TEN_MINS;
						EventStarted = false;
						break;
				}
				Event_phase++;
			}
			else
				Event_Timer -= diff;
			break;
		case EVENT_TYPE_BEASTS_YETI:
			if(Event_Timer <= diff)
			{
				switch(Event_phase)		
				{
					case 1:
						Tasks.Speak(CHAT_TYPE_YELL,16040,"L'air se gèle à l'entrée de notre prochain combattant : Glace-Hurlante ! Tuez ou soyez tués, champions !",Fordring);
						if(Creature* Icehowl = Tasks.CallCreature(34797,TEN_MINS,PREC_COORDS,AGGRESSIVE_RANDOM,563.135f,	243.306f,395.0f))
						{
							Icehowl->GetMotionMaster()->MovePoint(0, 563.832f, 180.223f, 395.0f);
						}
						pInstance->DoUseDoorOrButton(pInstance->GetData64(TYPE_DOOR));
						Event_Timer = 10000;
						break;
					case 2:
						if (Creature* Icehowl = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_Icehowl))))
							((ScriptedAI*)Icehowl->AI())->SetCombatMovement(false);
						pInstance->DoUseDoorOrButton(pInstance->GetData64(TYPE_DOOR));
						EventStarted = false;
						break;
				}
				Event_phase++;
			}
			else
				Event_Timer -= diff;
			break;
		case EVENT_TYPE_JARAXXUS:
			if(Event_Timer <= diff)
			{
				switch(Event_phase)
				{
					case 1:
						pInstance->DoUseDoorOrButton(pInstance->GetData64(TYPE_DOOR));
						Event_Timer = 1000;
						break;
					case 2:
						Tasks.Speak(CHAT_TYPE_YELL,16043,"Le Grand démoniste Wilfried Flopboum va invoquer votre prochain défi, ne bougez pas il arrive !",Fordring);
						if(Flopboum = Tasks.CallCreature(35458,TEN_MINS,PREC_COORDS,NOTHING,564.057f,203.706f,395.2f))
						{
							Flopboum->GetMotionMaster()->MovePoint(0, 563.659f,145.059,395.0f);
							//Flopboum->SetSpeedRate(MOVE_RUN,2.0f);
						}
						Event_Timer = 8000;
						break;
					case 3:
						pInstance->DoUseDoorOrButton(pInstance->GetData64(TYPE_DOOR));
						Tasks.Speak(CHAT_TYPE_YELL,16268,"Merci généralissime, et maintenant aspirant je vais commencer le rituel d'invocation. Quand j'en aurais fini un terrible garde funeste apparaîtra.",Flopboum ? Flopboum : NULL);
						Event_Timer = 10000;
						break;
					case 4:
						Tasks.Speak(CHAT_TYPE_YELL,16269,"Préparez vous à l'oubli !",Flopboum ? Flopboum : NULL);
						Event_Timer = 3000;
						break;
					case 5:
						Tasks.CallCreature(35651,30000,PREC_COORDS,NOTHING,563.711f,139.268f,394.0f);
						Event_Timer = 2000;
						break;
					case 6:
						if(Flopboum)
							Flopboum->CastSpell(Flopboum,45927,false);

						Tasks.CallCreature(19224,11000,PREC_COORDS,NOTHING,563.711f,139.268f,394.0f);
						Event_Timer = 10000;
						break;
					case 7:
						Tasks.Speak(CHAT_TYPE_YELL,16270,"Haha ! J'ai réussi ! Comtemplez l'absolue puissante de Wielfried Flopboum, Maître invocateur. Tu es mon esclave démon",Flopboum ? Flopboum : NULL);
						if(Creature* Jaraxxus = Tasks.CallCreature(34780,TEN_MINS,PREC_COORDS,NOTHING,563.711f,139.268f,394.0f))
						{
							Jaraxxus->setFaction(35);
							Tasks.SetAuraStack(67924,1,Jaraxxus,me,0);
							//((ScriptedAI*)Jarraxus->AI())->DoResetThreat();
						}
						Event_Timer = 11000;
						break;
					case 8:
						if (Creature* Jaraxxus = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_JARAXXUS))))
						{
							Tasks.Speak(CHAT_TYPE_SAY,16143,"Misérable gnome, ton arrogance te perdra !",Jaraxxus);
							Jaraxxus->RemoveAurasDueToSpell(67924);
						}
						Event_Timer = 3500;
						break;
					case 9:
						Tasks.Speak(CHAT_TYPE_SAY,16271,"Mais c'est moi qui commande ici. Rurrlg...",Flopboum ? Flopboum : NULL);
						Event_Timer = 3000;
						break;
					case 10:
						if (Creature* Jaraxxus = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_JARAXXUS))))
						{
							if(Flopboum)
								Jaraxxus->CastSpell(Flopboum,31008,false);
						}
						Kill(Flopboum);
						Tasks.Speak(CHAT_TYPE_YELL,16044,"Vite héros anéantissez le seigneur démon avant qu'il ne puisse ouvrir un portail vers son épouvantable royaume démoniaque !",Fordring);
						if (Creature* Jaraxxus = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_JARAXXUS))))
							Jaraxxus->setFaction(14);
						Event_Timer = DAY * HOUR;
						break;

				}
				UpdateWStates();
				Event_phase++;
			}
			else
				Event_Timer -= diff;
			break;
		case EVENT_TYPE_JARAXXUS_OUTRO:
			if(Event_Timer <= diff)
			{
				Tasks.Speak(CHAT_TYPE_SAY,16045,"La perte de Wielfred Plopboum bien que malheureuse servira de leçon à ceux qui s'adonnent à la magie noire. Hélas vous êtes victorieux et devez affronter le prochain défi.",Fordring);
				EventStarted = false;
			}
			else
				Event_Timer -= diff;
			break;
		case EVENT_TYPE_CHAMPIONS:
			if(Event_Timer <= diff)
			{
				switch(Event_phase)
				{
					case 1:
						Tasks.Speak(CHAT_TYPE_YELL,16047,"La prochaine bataille sera contre les chevaliers de la Croisade d'argent. Ce ne sera qu'après les avoir vaincus que vous serez déclarés dignes.",Fordring);
						UpdateWStates();
						Event_Timer = 7500;
						break;
					case 2:
						if(team == ALLIANCE)
						{
							Tasks.Speak(CHAT_TYPE_YELL,16065,"Combattez pour la gloire de l'Alliance héros, faîtes honneur à votre Roi et à votre peuple !",Varian);
							Event_Timer = 6000;
						}
						else
						{
							Tasks.Speak(CHAT_TYPE_YELL,16022,"N'ayez aucune pitié champions de la Horde ! Lok'tar O'gar !",Garrosh);
							Event_Timer = 5500;
						}
						break;
					case 3:
						if(team == ALLIANCE)
						{
							Tasks.Speak(CHAT_TYPE_YELL,16023,"La Horde exige réparation, nous défions l'Alliance. Permettez nous de combattre à la place de vos chevaliers, paladin. Nous allons montrer à ces chiens ce qu'il en coûte d'insulter la Horde !",Garrosh);
							Event_Timer = 14000;
						}
						else
						{
							Tasks.Speak(CHAT_TYPE_YELL,16066,"Notre honneur a été souillé, il lance des affirmations absurdes contre nous, de fausses accusations. J'exige réparation. Permettez à mes champions de combattre à la place de vos chevaliers Tyrion. Nous défions la Horde !",Varian);
							Event_Timer = 15000;
						}
						break;
					case 4:
						Tasks.Speak(CHAT_TYPE_YELL,16048,"Très bien. Je vous y autorise. Combattez avec honneur !",Fordring);
						Event_Timer = 4000;
						break;
					case 5:
						SpawnChampions();
						EventStarted = false;
						Event_Timer = DAY * 3600;
						break;
				}
				Event_phase++;
			}
			else
				Event_Timer -= diff;
			break;
		case EVENT_TYPE_VALKYR:
			if(Event_Timer <= diff)
			{
				switch(Event_phase)
				{
					case 1:
						UpdateWStates();
						if(Creature* Darkbane = Tasks.CallCreature(34496,TEN_MINS,PREC_COORDS,NOTHING,572.243f,244.680f,395.0f))
						{
							Darkbane->GetMotionMaster()->MovePoint(0, 574.376f,180.478f,396.0f);
						}
						if(Creature* Lightbane = Tasks.CallCreature(34497,TEN_MINS,PREC_COORDS,NOTHING,554.910f,244.888f,395.0f))
						{
							Lightbane->GetMotionMaster()->MovePoint(0, 563.832f, 180.223f, 395.0f);
						}
						Tasks.Speak(CHAT_TYPE_SAY,16050,"Ce n'est qu'en travaillant côte à côte que vous pourrez triompher de l'ultime défi. Venues des profondeurs de la Couronne de Glace, voici deux des lieutenants les plus puissants du fléau, deux redoutables valkyrs, messagères ailées du Roi Liche",Fordring);
						Tasks.CallCreature(34568,TEN_MINS,PREC_COORDS,NOTHING,ValkyrPortal_spawn[0][0],ValkyrPortal_spawn[0][1],ValkyrPortal_spawn[0][2]);
						Tasks.CallCreature(34567,TEN_MINS,PREC_COORDS,NOTHING,ValkyrPortal_spawn[1][0],ValkyrPortal_spawn[1][1],ValkyrPortal_spawn[1][2]);
						Tasks.CallCreature(34567,TEN_MINS,PREC_COORDS,NOTHING,ValkyrPortal_spawn[2][0],ValkyrPortal_spawn[2][1],ValkyrPortal_spawn[2][2]);
						Tasks.CallCreature(34568,TEN_MINS,PREC_COORDS,NOTHING,ValkyrPortal_spawn[3][0],ValkyrPortal_spawn[3][1],ValkyrPortal_spawn[3][2]);
						pInstance->DoUseDoorOrButton(pInstance->GetData64(TYPE_DOOR));
						Event_Timer = 13000;
						break;
					case 2:
						pInstance->DoUseDoorOrButton(pInstance->GetData64(TYPE_DOOR));
						Event_Timer = DAY*HOUR;
						break;
				}
				Event_phase++;
			}
			else
				Event_Timer -= diff;
			break;
		case EVENT_TYPE_VALKYR_OUTRO:
			if(Event_Timer <= diff)
			{
				switch(Event_phase)
				{
					case 1:
						Tasks.Speak(CHAT_TYPE_SAY,16051,"C'est un rude coup qui vient d'être porté au Roi Liche. Vous avez prouvé que vous êtes aptes à servir comme Champion de la Croisade d'Argent. Ensemble nous frapperons la Citadelle de la Couronne de Glace et détruirons ce qui reste du fléau. Aucun défi ne pourra résister à notre unité !",Fordring);
						Event_Timer = 19500;
						break;
					case 2:
						if(team == ALLIANCE)
						{
							Tasks.Speak(CHAT_TYPE_SAY,16068,"Même les plus puissants serviteurs du Roi Liche ne sont pas de taille face à l'Alliance. Hourra pour nos vainqueurs !",Varian);
							Event_Timer = 61000;
						}
						else
						{
							Tasks.Speak(CHAT_TYPE_SAY,16025,"Doutez vous encore de la puissance de la Horde, Paladin ? Nous relèverons tous les gants !",Garrosh);
							Event_Timer = 60000;
						}
						break;
					case 3:
						pInstance->DoUseDoorOrButton(pInstance->GetData64(TYPE_DOOR));
						Event_Timer = 1000;
						break;
					case 4:
						if(LichKing = Tasks.CallCreature(31301,50000,PREC_COORDS,NOTHING,564.057f,	203.706f,395.2f))
						{
							LichKing->GetMotionMaster()->MovePoint(0, 563.734f,139.577,394.0f);
							LichKing->setFaction(35);
							LichKing->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
							Tasks.Speak(CHAT_TYPE_YELL,16321,"Vous allez l'avoir votre défi, Fordring !",LichKing);
						}
						Event_Timer = 5000;
						break;
					case 5:
						Tasks.Speak(CHAT_TYPE_SAY,16052,"Arthas, tu es en infériorité, c'est sans espoir. Dépose Deuillegivre et je t'accorderai une mort juste.",Fordring);
						Event_Timer = 7000;
						break;
					case 6:
						if(LichKing)
							LichKing->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
						Tasks.Speak(CHAT_TYPE_YELL,16322,"Hahahahaha ! Les nérubiens ont batti un empire sous les terres gelées du Norfendre. Un empire sur lequel vous avez si bêtement construit vos bâtiments. Mon empire...",LichKing);
						Event_Timer = 20000;
						break;
					case 7:
						Tasks.Speak(CHAT_TYPE_YELL,16323,"Les âmes de vos champions seront miennes, Fordring.",LichKing);
						Event_Timer = 9000;
						break;
					case 8:
						if(LichKing)
						{
							LichKing->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
							LichKing->CastSpell(LichKing,68198,false);
						}
						Event_Timer = 1000;
						break;
					case 15:
						UpdateWStates();
						if (GameObject* Floor = me->GetMap()->GetGameObject(pInstance->GetData64(TYPE_FLOOR)))
						{
							if (Floor)
							{
								Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
								if (!lPlayers.isEmpty())
									for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
										if (Player* pPlayer = itr->getSource())
											Floor->DestroyForPlayer(pPlayer,true);

								//Floor->SetFlag(GAMEOBJECT_FLAGS,GO_FLAG_DESTROYED);

								Floor->SetUInt32Value(GAMEOBJECT_DISPLAYID,9060);
								Floor->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_NODESPAWN);
								Floor->SetUInt32Value(GAMEOBJECT_BYTES_1,8449);
							}
						}
						Event_Timer = 3000;
						break;
					case 16:
						Map::PlayerList const& lPlayers = me->GetMap()->GetPlayers();
						if (!lPlayers.isEmpty())
							for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
								if (Player* pPlayer = itr->getSource())
									if(pPlayer->isAlive() && urand(0,3) == 3)
										Kill(pPlayer);

						if (Creature* AnubArak = ((Creature*)Unit::GetUnit((*me), pInstance->GetData64(TYPE_ANUBARAK))))
						{
							Tasks.Speak(CHAT_TYPE_YELL,16235,"Ah. Nos invités sont arrivés... Comme l'avait promis le maître",AnubArak);
						}

						Event_Timer = DAY*HOUR;
						EventStarted = false;
						break;
				}
				Event_phase++;
			}
			else
				Event_Timer -= diff;
			break;
	}
}

CreatureAI* GetAI_npc_toc10_announcer(Creature* pCreature)
{
    return new npc_toc10_announcerAI(pCreature);
}

bool GossipHello_npc_toc10_announcer(Player* pPlayer, Creature* pCreature)
{
	if(((npc_toc10_announcerAI*)pCreature->AI())->EventStarted)
		return false;
	pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Lancer le défi", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

	if(pPlayer->isGameMaster())
	{
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Debug_Valkyrs", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Debug_Jaraxxus", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
		pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Debug_Outro_Valkyr", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
	}
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_toc10_announcer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
	pPlayer->CLOSE_GOSSIP_MENU();
    switch(uiAction)
    {
		case GOSSIP_ACTION_INFO_DEF+1:
			((npc_toc10_announcerAI*)pCreature->AI())->StartEvent(pPlayer,0);
			break;
		case GOSSIP_ACTION_INFO_DEF+2:
			((npc_toc10_announcerAI*)pCreature->AI())->StartEvent(pPlayer,EVENT_TYPE_VALKYR);
			break;
		case GOSSIP_ACTION_INFO_DEF+3:
			((npc_toc10_announcerAI*)pCreature->AI())->StartEvent(pPlayer,EVENT_TYPE_JARAXXUS);
			break;
		case GOSSIP_ACTION_INFO_DEF+4:
			((npc_toc10_announcerAI*)pCreature->AI())->StartEvent(pPlayer,EVENT_TYPE_VALKYR_OUTRO);
			break;
    }

    return true;
}

void AddSC_trial_of_the_crusader()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "npc_toc10_announcer";
	NewScript->GetAI = &GetAI_npc_toc10_announcer;
    NewScript->pGossipHello = &GossipHello_npc_toc10_announcer;
    NewScript->pGossipSelect = &GossipSelect_npc_toc10_announcer;
    NewScript->RegisterSelf();
}
