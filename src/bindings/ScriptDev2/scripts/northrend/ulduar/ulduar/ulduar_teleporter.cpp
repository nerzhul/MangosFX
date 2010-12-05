#include "precompiled.h"
#include "ulduar.h"

/*
The teleporter appears to be active and stable.

- Expedition Base Camp
- Formation Grounds
- Colossal Forge
- Scrapyard
- Antechamber of Ulduar
- Shattered Walkway
- Conservatory of Life
*/

#define BASE_CAMP    200
#define GROUNDS      201
#define FORGE        202
#define SCRAPYARD    203
#define ANTECHAMBER  204
#define WALKWAY      205
#define CONSERVATORY 206
#define MADNESS		 207

bool GoHello_ulduar_teleporter( Player *pPlayer, GameObject *pGO )
{
    InstanceData *pInstance = pGO->GetInstanceData();
    if(!pInstance) return true;

    pPlayer->ADD_GOSSIP_ITEM(0, "Teleportation au camp de base", GOSSIP_SENDER_MAIN, BASE_CAMP);
    pPlayer->ADD_GOSSIP_ITEM(0, "Teleportation to the Formation Grounds", GOSSIP_SENDER_MAIN, GROUNDS);
    /*if(pInstance->GetData(TYPE_LEVIATHAN) == DONE)
    {*/
        pPlayer->ADD_GOSSIP_ITEM(0, "Teleportation a la Forge Colossale", GOSSIP_SENDER_MAIN, FORGE);
        if(pInstance->GetData(TYPE_XT002) == DONE)
        {
            pPlayer->ADD_GOSSIP_ITEM(0, "Teleport to the Scrapyard", GOSSIP_SENDER_MAIN, SCRAPYARD);
            pPlayer->ADD_GOSSIP_ITEM(0, "Teleportation dans l'Antichambre d'Ulduar", GOSSIP_SENDER_MAIN, ANTECHAMBER);
            if(pInstance->GetData(TYPE_KOLOGARN) == DONE)
            {
                pPlayer->ADD_GOSSIP_ITEM(0, "Teleport to the Shattered Walkway", GOSSIP_SENDER_MAIN, WALKWAY);
                if(pInstance->GetData(TYPE_AURIAYA) == DONE)
				{
                    pPlayer->ADD_GOSSIP_ITEM(0,	 "Teleport to the Conservatory of Life", GOSSIP_SENDER_MAIN, CONSERVATORY);
					if(pInstance->GetData(TYPE_VEZAX) == DONE)
						pPlayer->ADD_GOSSIP_ITEM(0,	 "Teleportation a la prison de Yogg Saron", GOSSIP_SENDER_MAIN, MADNESS);
				}
            }
        }
    //}
    pPlayer->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pGO->GetGUID());

    return true;
}

bool GOSelect_ulduar_teleporter( Player *pPlayer, GameObject *pGO, uint32 sender, uint32 action )
{
    if(sender != GOSSIP_SENDER_MAIN) return true;
    if(!pPlayer->getAttackers().empty()) return true;

    switch(action)
    {
    case BASE_CAMP:
        pPlayer->TeleportTo(603, -706.122, -92.6024, 429.876, 0);
        pPlayer->CLOSE_GOSSIP_MENU(); break;
    case GROUNDS:
        pPlayer->TeleportTo(603, 131.248, -35.3802, 409.804, 0);
        pPlayer->CLOSE_GOSSIP_MENU(); break;
    case FORGE:
        pPlayer->TeleportTo(603, 553.233, -12.3247, 409.679, 0);
        pPlayer->CLOSE_GOSSIP_MENU(); break;
    case SCRAPYARD:
        pPlayer->TeleportTo(603, 926.292, -11.4635, 418.595, 0);
        pPlayer->CLOSE_GOSSIP_MENU(); break;
    case ANTECHAMBER:
        pPlayer->TeleportTo(603, 1498.09, -24.246, 420.967, 0);
        pPlayer->CLOSE_GOSSIP_MENU(); break;
    case WALKWAY:
        pPlayer->TeleportTo(603, 1859.45, -24.1, 448.9, 0);
        pPlayer->CLOSE_GOSSIP_MENU(); break;
    case CONSERVATORY:
        pPlayer->TeleportTo(603, 2086.27, -24.3134, 421.239, 0);
        pPlayer->CLOSE_GOSSIP_MENU(); break;
	case MADNESS:
		pPlayer->TeleportTo(603, 1854.82f, -11.71f, 334.25f, 0);
        pPlayer->CLOSE_GOSSIP_MENU(); break;
    }

    return true;
}

void AddSC_ulduar_teleporter()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "ulduar_teleporter";
    newscript->pGOHello = &GoHello_ulduar_teleporter;
    newscript->pGOSelect = &GOSelect_ulduar_teleporter;
    newscript->RegisterSelf();
}
