#include "AuctionHouseBot.h"
#include "ObjectMgr.h"
#include "AuctionHouseMgr.h"

#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1( AuctionHouseBot );

using namespace std;

AuctionHouseBot::AuctionHouseBot()
{
    AHBSeller = 0;
    AHBBuyer = 0;

    Vendor_Items = 0;
    Loot_Items = 0;
    Other_Items = 0;

    No_Bind = 0;
    Bind_When_Equipped = 0;
    Bind_When_Use = 0;

    AllianceConfig = AHBConfig(2);
    HordeConfig = AHBConfig(6);
    NeutralConfig = AHBConfig(7);
}

AuctionHouseBot::~AuctionHouseBot()
{

}

void AuctionHouseBot::addNewAuctions(Player *AHBplayer, AHBConfig *config)
{
    if (!AHBSeller)
        return;
    AuctionHouseEntry const* ahEntry = sAuctionMgr.GetAuctionHouseEntry(config->GetAHFID());
	if(!ahEntry)
		return;

    AuctionHouseObject* auctionHouse = sAuctionMgr.GetAuctionsMap(ahEntry);
    uint32 items = 0;
    uint32 minItems = MINITEMS;
    uint32 maxItems = MAXITEMS;
    uint32 auctions = auctionHouse->Getcount();

    if (auctions >= minItems)
        return;

    if (auctions <= maxItems)
    {
        if ((maxItems - auctions) > ItemsPerCycle)
            items = ItemsPerCycle;
        else
            items = (maxItems - auctions);
    }
    uint32 greyTGcount = config->GetPercents(AHB_GREY_TG);
    uint32 whiteTGcount = config->GetPercents(AHB_WHITE_TG);
    uint32 greenTGcount = config->GetPercents(AHB_GREEN_TG);
    uint32 blueTGcount = config->GetPercents(AHB_BLUE_TG);
    uint32 purpleTGcount = config->GetPercents(AHB_PURPLE_TG);
    uint32 greyIcount = config->GetPercents(AHB_GREY_I);
    uint32 whiteIcount = config->GetPercents(AHB_WHITE_I);
    uint32 greenIcount = config->GetPercents(AHB_GREEN_I);
    uint32 blueIcount = config->GetPercents(AHB_BLUE_I);
    uint32 purpleIcount = config->GetPercents(AHB_PURPLE_I);
    uint32 total = greyTGcount + whiteTGcount + greenTGcount + blueTGcount
        + purpleTGcount + whiteIcount + greenIcount + blueIcount + purpleIcount;

    uint32 greyTGoods = 0;
    uint32 whiteTGoods = 0;
    uint32 greenTGoods = 0;
    uint32 blueTGoods = 0;
    uint32 purpleTGoods = 0;

    uint32 greyItems = 0;
    uint32 whiteItems = 0;
    uint32 greenItems = 0;
    uint32 blueItems = 0;
    uint32 purpleItems = 0;

    for (AuctionHouseObject::AuctionEntryMap::const_iterator itr = auctionHouse->GetAuctionsBegin();itr != auctionHouse->GetAuctionsEnd();++itr)
    {
        AuctionEntry *Aentry = itr->second;
        Item *item = sAuctionMgr.GetAItem(Aentry->item_guidlow);
        if (item)
        {
            ItemPrototype const *prototype = item->GetProto();
			if (prototype)
            {
                switch (prototype->Quality)
                {
					case 0:
						if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
							++greyTGoods;
						else
							++greyItems;
						break;
					case 1:
						if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
							++whiteTGoods;
						else
							++whiteItems;
						break;
					case 2:
						if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
							++greenTGoods;
						else
							++greenItems;
						break;
					case 3:
						if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
							++blueTGoods;
						else
							++blueItems;
						break;
					case 4:
						if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
							++purpleTGoods;
						else
							++purpleItems;
						break;
					default:
						//qualité non demandée
						break;
                }
            }
        }
    }

    // only insert a few at a time, so as not to peg the processor
    for (uint32 cnt = 1;cnt <= items;cnt++)
    {
        uint32 itemID = 0;
        uint32 loopBreaker = 0;                     // This will prevent endless looping condition where AHBot
        while (itemID == 0 && loopBreaker < 15)     //  cannot allocate an item.
        {
            uint32 choice = urand(0, 11);
            switch (choice)
            {
            case 0:
                if ((greyItemsBin.size() > 0) && (greyItems < greyIcount))
                {
                    itemID = greyItemsBin[urand(0, greyItemsBin.size() - 1)];
                    ++greyItems;
                }
                break;
            case 1:
                if ((whiteItemsBin.size() > 0) && (whiteItems < whiteIcount))
                {
                    itemID = whiteItemsBin[urand(0, whiteItemsBin.size() - 1)];
                    ++whiteItems;
                }
                break;
            case 2:
                if ((greenItemsBin.size() > 0) && (greenItems < greenIcount))
                {
                    itemID = greenItemsBin[urand(0, greenItemsBin.size() - 1)];
                    ++greenItems;
                }
                break;
            case 3:
                if ((blueItemsBin.size() > 0) && (blueItems < blueIcount))
                {
                    itemID = blueItemsBin[urand(0, blueItemsBin.size() - 1)];
                    ++blueItems;
                }
                break;
            case 4:
                if ((purpleItemsBin.size() > 0) && (purpleItems < purpleIcount))
                {
                    itemID = purpleItemsBin[urand(0, purpleItemsBin.size() - 1)];
                    ++purpleItems;
                }
                break;
            case 7:
                if ((greyTradeGoodsBin.size() > 0) && (greyTGoods < greyTGcount))
                {
                    itemID = whiteTradeGoodsBin[urand(0, whiteTradeGoodsBin.size() - 1)];
                    ++greyTGoods;
                }
                break;
            case 8:
                if ((whiteTradeGoodsBin.size() > 0) && (whiteTGoods < whiteTGcount))
                {
                    itemID = whiteTradeGoodsBin[urand(0, whiteTradeGoodsBin.size() - 1)];
                    ++whiteTGoods;
                }
                break;
            case 9:
                if ((greenTradeGoodsBin.size() > 0) && (greenTGoods < greenTGcount))
                {
                    itemID = greenTradeGoodsBin[urand(0, greenTradeGoodsBin.size() - 1)];
                    ++greenTGoods;
                }
                break;
            case 10:
                if ((blueTradeGoodsBin.size() > 0) && (blueTGoods < blueTGcount))
                {
                    itemID = blueTradeGoodsBin[urand(0, blueTradeGoodsBin.size() - 1)];
                    ++blueTGoods;
                }
                break;
            case 11:
                if ((purpleTradeGoodsBin.size() > 0) && (purpleTGoods < purpleTGcount))
                {
                    itemID = purpleTradeGoodsBin[urand(0, purpleTradeGoodsBin.size() - 1)];
                    ++purpleTGoods;
                }
                break;
            default:
                // couleur d'item non demandée
                break;
            }

            ++loopBreaker;
        }

        if (itemID == 0)
            continue;

        ItemPrototype const* prototype = sObjectMgr.GetItemPrototype(itemID);
        if (prototype == NULL)
            continue;

        Item* item = Item::CreateItem(itemID, 1, AHBplayer);
        item->AddToUpdateQueueOf(AHBplayer);
        if (item == NULL)
            break;

        uint32 randomPropertyId = Item::GenerateItemRandomPropertyId(itemID);
        if (randomPropertyId != 0)
            item->SetItemRandomProperties(randomPropertyId);

        uint32 buyoutPrice;
        uint32 bidPrice = 0;
        uint32 stackCount = urand(1, item->GetMaxStackCount());

        buyoutPrice  = prototype->BuyPrice * item->GetCount();

        switch (prototype->Quality)
        {
        case ITEM_QUALITY_POOR:
            if (config->GetMaxStack(AHB_GREY) != 0)
            {
                stackCount = urand(1, minValue(item->GetMaxStackCount(), config->GetMaxStack(AHB_GREY)));
            }
            buyoutPrice *= urand(config->GetMinPrice(AHB_GREY), config->GetMaxPrice(AHB_GREY)) * stackCount;
            buyoutPrice /= 100;
            bidPrice = buyoutPrice * urand(config->GetMinBidPrice(AHB_GREY), config->GetMaxBidPrice(AHB_GREY));
            bidPrice /= 100;
            break;
        case ITEM_QUALITY_NORMAL:
            if (config->GetMaxStack(AHB_WHITE) != 0)
            {
                stackCount = urand(1, minValue(item->GetMaxStackCount(), config->GetMaxStack(AHB_WHITE)));
            }
			if(prototype->ItemLevel == 45 && prototype->Spells[0].SpellId > 0)
			{
				stackCount = 1;
				buyoutPrice *= urand(35,40) * prototype->RequiredLevel / 50;
			}
			else
				buyoutPrice *= urand(config->GetMinPrice(AHB_WHITE), config->GetMaxPrice(AHB_WHITE)) * stackCount;
            buyoutPrice /= 100;
            bidPrice = buyoutPrice * urand(config->GetMinBidPrice(AHB_WHITE), config->GetMaxBidPrice(AHB_WHITE));
            bidPrice /= 100;
            break;
        case ITEM_QUALITY_UNCOMMON:
            if (config->GetMaxStack(AHB_GREEN) != 0)
                stackCount = urand(1, minValue(item->GetMaxStackCount(), config->GetMaxStack(AHB_GREEN)));

            if(prototype->ItemLevel > 120) // 75+
				buyoutPrice *= urand(30, 55) * stackCount * prototype->ItemLevel / 115;
			else if(prototype->ItemLevel > 45) // 63+
				buyoutPrice *= urand(15, 30) * stackCount * prototype->ItemLevel / 40;
			else if(prototype->ItemLevel > 0)
			{
				if(prototype->GemProperties == 0)
					buyoutPrice *= urand(3, 6) * stackCount * prototype->ItemLevel / 10;
				else
					buyoutPrice *= urand(10,20) * stackCount;
			}
			else
				buyoutPrice *= urand(config->GetMinPrice(AHB_GREEN), config->GetMaxPrice(AHB_GREEN)) * stackCount;

			buyoutPrice /= 100 ;
			bidPrice = buyoutPrice * urand(80, 100);
			bidPrice /= 100;
            break;
        case ITEM_QUALITY_RARE:
            if (config->GetMaxStack(AHB_BLUE) != 0)
                stackCount = urand(1, minValue(item->GetMaxStackCount(), config->GetMaxStack(AHB_BLUE)));

            if(prototype->ItemLevel > 160) // 75+
				buyoutPrice *= urand(120, 150) * stackCount * prototype->ItemLevel / 160;
			else if(prototype->ItemLevel > 60) // 63+
				buyoutPrice *= urand(70, 90) * stackCount * prototype->ItemLevel / 75;
			else if(prototype->ItemLevel > 0)
			{
				if(prototype->GemProperties == 0)
					buyoutPrice *= urand(30, 60) * stackCount * prototype->ItemLevel / 32;
				else
					buyoutPrice *= urand(30, 60) * stackCount;
			}
			else
				buyoutPrice *= urand(config->GetMinPrice(AHB_PURPLE), config->GetMaxPrice(AHB_PURPLE)) * stackCount;

			if(prototype->BagFamily & BAG_FAMILY_MASK_LEATHERWORKING_SUPP)
				buyoutPrice *= 5;

			buyoutPrice /= 100;
			bidPrice = buyoutPrice * urand(80, 100);
			bidPrice /= 100;
            break;
        case ITEM_QUALITY_EPIC:
            if (config->GetMaxStack(AHB_PURPLE) != 0)
                stackCount = urand(1, minValue(item->GetMaxStackCount(), config->GetMaxStack(AHB_PURPLE)));

			switch(prototype->ItemId)
			{
				case 44413:
				case 41508:
					buyoutPrice *= urand(15000, 22000) * stackCount;
					break;
				default:
					if(prototype->ItemLevel > 190) // 75+
					{
						buyoutPrice *= urand(350, 550) * stackCount * prototype->ItemLevel / 186;
						if(prototype->InventoryType == INVTYPE_TRINKET)
							buyoutPrice *= 5;
					}
					else if(prototype->ItemLevel > 80) // 63+
					{
						buyoutPrice *= urand(150, 200) * stackCount * prototype->ItemLevel / 90;
						if(prototype->InventoryType == INVTYPE_TRINKET)
							buyoutPrice *= 5;
					}
					else if(prototype->ItemLevel > 0) 
					{
						
						if(prototype->GemProperties == 0)
							buyoutPrice *= urand(150, 200) * stackCount * prototype->ItemLevel / 32;
						else
							buyoutPrice *= urand(120, 250) * stackCount;

						if(prototype->InventoryType == INVTYPE_TRINKET)
							buyoutPrice *= 5;
					}
					else
						buyoutPrice *= urand(config->GetMinPrice(AHB_PURPLE), config->GetMaxPrice(AHB_PURPLE)) * stackCount;

					if(prototype->BagFamily & BAG_FAMILY_MASK_LEATHERWORKING_SUPP)
						buyoutPrice *= 5;
					break;
			}
			buyoutPrice /= 100 ;
			bidPrice = buyoutPrice * urand(80, 100);
			bidPrice /= 100;
            break;
		default:
			break;
        }

        item->SetCount(stackCount);

        AuctionEntry* auctionEntry = new AuctionEntry;
        auctionEntry->Id = sObjectMgr.GenerateAuctionID();
        auctionEntry->item_guidlow = item->GetGUIDLow();
        auctionEntry->item_template = item->GetEntry();
        auctionEntry->owner = AHBplayer->GetGUIDLow();
        auctionEntry->startbid = bidPrice;
        auctionEntry->buyout = buyoutPrice;
		error_log("%u %u %u",auctionEntry->item_template,auctionEntry->startbid,auctionEntry->buyout);
        auctionEntry->bidder = 0;
        auctionEntry->bid = 0;
        auctionEntry->deposit = 0;
        auctionEntry->expire_time = (time_t) (urand(MINTIME, MAXTIME) * 60 * 60 + time(NULL));
        auctionEntry->auctionHouseEntry = ahEntry;
        item->SaveToDB();
        item->RemoveFromUpdateQueueOf(AHBplayer);
        sAuctionMgr.AddAItem(item);
        auctionHouse->AddAuction(auctionEntry);
        auctionEntry->SaveToDB();
    }
}

void AuctionHouseBot::addNewAuctionBuyerBotBid(Player *AHBplayer, AHBConfig *config, WorldSession *session)
{
    if (!AHBBuyer)
        return;

    // Fetches content of selected AH
	AuctionHouseEntry const* ahEntry = sAuctionMgr.GetAuctionHouseEntry(config->GetAHFID());
	if(!ahEntry)
		return;

    AuctionHouseObject* auctionHouse = sAuctionMgr.GetAuctionsMap(ahEntry);
    vector<uint32> possibleBids;

    for (AuctionHouseObject::AuctionEntryMap::const_iterator itr = auctionHouse->GetAuctionsBegin();itr != auctionHouse->GetAuctionsEnd();++itr)
    {
        // Check if the auction is ours
        // if it is, we skip this iteration.
        if (itr->second->owner == AHBplayerGUID)
        {
            continue;
        }
        // Check that we haven't bidded in this auction already.
        if (itr->second->bidder != AHBplayerGUID)
        {
            uint32 tmpdata = itr->second->Id;
            possibleBids.push_back(tmpdata);
        }
    }

    for (uint32 count = 0;count < config->GetBidsPerInterval();++count)
    {

        // Do we have anything to bid? If not, stop here.
        if (possibleBids.empty())
        {
            count = config->GetBidsPerInterval();
            continue;
        }

        // Choose random auction from possible auctions
        uint32 vectorPos = urand(0, possibleBids.size() - 1);
        uint32 auctionID = possibleBids[vectorPos];

        // Erase the auction from the vector to prevent bidding on item in next iteration.
        vector<uint32>::iterator iter = possibleBids.begin();
        advance(iter, vectorPos);
        possibleBids.erase(iter);

        // from auctionhousehandler.cpp, creates auction pointer & player pointer
        AuctionEntry* auction = auctionHouse->GetAuction(auctionID);
        if (!auction)
            continue;

        // get exact item information
        Item *pItem = sAuctionMgr.GetAItem(auction->item_guidlow);
        if (!pItem)
            continue;

        // get item prototype
        ItemPrototype const* prototype = sObjectMgr.GetItemPrototype(auction->item_template);

        // check which price we have to use, startbid or if it is bidded already
        
        uint32 currentprice;
        if (auction->bid)
            currentprice = auction->bid;
        else
            currentprice = auction->startbid;
         
        uint32 bidprice;

        // Prepare portion from maximum bid
        uint32 tmprate2 = urand(0, 100);
        double tmprate = static_cast<double>(tmprate2);
        double bidrate = tmprate / 100;

        long double bidMax = 0;

        // check that bid has acceptable value and take bid based on vendorprice, stacksize and quality

        switch (prototype->Quality)
        {
        case 0:
            if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_GREY))
            {
                bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_GREY);
            }
            break;
        case 1:
            if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_WHITE))
            {
                bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_WHITE);
            }
            break;
        case 2:
            if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_GREEN))
            {
                bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_GREEN);
            }
            break;
        case 3:
            if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_BLUE))
            {
                bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_BLUE);
            }
            break;
        case 4:
            if (currentprice < prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_PURPLE))
            {
                bidMax = prototype->BuyPrice * pItem->GetCount() * config->GetBuyerPrice(AHB_PURPLE);
            }
            break;
        default:
            // quality is something it shouldn't be, let's get out of here
            continue;
            break;
        }

        // check some special items, and do recalculating to their prices
        switch (prototype->Class)
        {
            // ammo
        case 6:
            bidMax = 0;
            break;
        default:
            break;
        }

        if (bidMax == 0)
        {
            // quality check failed to get bidmax, let's get out of here
            continue;
        }

        // Calculate our bid
        long double bidvalue = currentprice + ((bidMax - currentprice) * bidrate);

        // Convert to uint32
        bidprice = static_cast<uint32>(bidvalue);

        // Check our bid is high enough to be valid. If not, correct it to minimum.
        if ((currentprice + auction->GetAuctionOutBid()) > bidprice)
            bidprice = currentprice + auction->GetAuctionOutBid();

        // Check wether we do normal bid, or buyout
        if ((bidprice < auction->buyout) || (auction->buyout == 0))
        {

            if (auction->bidder > 0)
            {
                if (auction->bidder == AHBplayer->GetGUIDLow())
                {
                    //pl->ModifyMoney(-int32(price - auction->bid));
                }
                else
                {
                    // mail to last bidder and return money
                    session->SendAuctionOutbiddedMail(auction , bidprice);
                    //pl->ModifyMoney(-int32(price));
                }
            }

            auction->bidder = AHBplayer->GetGUIDLow();
            auction->bid = bidprice;

            // Saving auction into database
            CharacterDatabase.PExecute("UPDATE auctionhouse SET buyguid = '%u',lastbid = '%u' WHERE id = '%u'", auction->bidder, auction->bid, auction->Id);
        }
        else
        {
            //buyout
            if (AHBplayer->GetGUIDLow() == auction->bidder)
            {
                //pl->ModifyMoney(-int32(auction->buyout - auction->bid));
            }
            else
            {
                //pl->ModifyMoney(-int32(auction->buyout));
                if (auction->bidder)
                {
                    session->SendAuctionOutbiddedMail(auction, auction->buyout);
                }
            }
            auction->bidder = AHBplayer->GetGUIDLow();
            auction->bid = auction->buyout;

            // Send mails to buyer & seller
            sAuctionMgr.SendAuctionSuccessfulMail(auction);
            sAuctionMgr.SendAuctionWonMail(auction);

            // Remove item from auctionhouse
            sAuctionMgr.RemoveAItem(auction->item_guidlow);
            // Remove auction
            auctionHouse->RemoveAuction(auction->Id);
            // Remove from database
            auction->DeleteFromDB();
        }
    }
}

void AuctionHouseBot::Update()
{
    time_t _newrun = time(NULL);
    if ((!AHBSeller) && (!AHBBuyer))
        return;

    WorldSession _session(AHBplayerAccount, NULL, SEC_PLAYER, true, 0, LOCALE_enUS);
    Player _AHBplayer(&_session);
    _AHBplayer.MinimalLoadFromDB(NULL, AHBplayerGUID);
    ObjectAccessor::Instance().AddObject(&_AHBplayer);

    // Add New Bids
    if (!sWorld.getConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
    {
        addNewAuctions(&_AHBplayer, &AllianceConfig);
        if (((_newrun - _lastrun_a) > (AllianceConfig.GetBiddingInterval() * 60)) && (AllianceConfig.GetBidsPerInterval() > 0))
        {
            addNewAuctionBuyerBotBid(&_AHBplayer, &AllianceConfig, &_session);
            _lastrun_a = _newrun;
        }

        addNewAuctions(&_AHBplayer, &HordeConfig);
        if (((_newrun - _lastrun_h) > (HordeConfig.GetBiddingInterval() *60)) && (HordeConfig.GetBidsPerInterval() > 0))
        {
            addNewAuctionBuyerBotBid(&_AHBplayer, &HordeConfig, &_session);
            _lastrun_h = _newrun;
        }
    }
    addNewAuctions(&_AHBplayer, &NeutralConfig);
    if (((_newrun - _lastrun_n) > (NeutralConfig.GetBiddingInterval() * 60)) && (NeutralConfig.GetBidsPerInterval() > 0))
    {
        addNewAuctionBuyerBotBid(&_AHBplayer, &NeutralConfig, &_session);
        _lastrun_n = _newrun;
    }

    ObjectAccessor::Instance().RemoveObject(&_AHBplayer);
	
}

void AuctionHouseBot::Initialize()
{
    AHBSeller = sConfig.GetBoolDefault("AuctionHouseBot.EnableSeller", 0);
    AHBBuyer = sConfig.GetBoolDefault("AuctionHouseBot.EnableBuyer", 0);
    AHBplayerAccount = sConfig.GetIntDefault("AuctionHouseBot.Account", 0);
    AHBplayerGUID = sConfig.GetIntDefault("AuctionHouseBot.GUID", 0);
    No_Bind = sConfig.GetBoolDefault("AuctionHouseBot.No_Bind", 1);
    Bind_When_Equipped = sConfig.GetBoolDefault("AuctionHouseBot.Bind_When_Equipped", 1);
    Bind_When_Use = sConfig.GetBoolDefault("AuctionHouseBot.Bind_When_Use", 1);
    ItemsPerCycle = sConfig.GetIntDefault("AuctionHouseBot.ItemsPerCycle", 200);

	if (!sWorld.getConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
    {
        LoadValues(&AllianceConfig);
        LoadValues(&HordeConfig);
    }
    LoadValues(&NeutralConfig);

    if (AHBSeller)
    {
        Vendor_Items = sConfig.GetBoolDefault("AuctionHouseBot.VendorItems", 0);
        Loot_Items = sConfig.GetBoolDefault("AuctionHouseBot.LootItems", 1);
        Other_Items = sConfig.GetBoolDefault("AuctionHouseBot.OtherItems", 0);

        QueryResult* results = (QueryResult*) NULL;
        char npcQuery[] = "SELECT distinct `item` FROM `npc_vendor`";
        results = WorldDatabase.PQuery(npcQuery);
        if (results != NULL)
        {
            do
            {
                Field* fields = results->Fetch();
                npcItems.push_back(fields[0].GetUInt32());

            } while (results->NextRow());

            delete results;
        }
        else
        {
            sLog.outString("AuctionHouseBot: \"%s\" failed", npcQuery);
        }

        char lootQuery[] = "SELECT `item` FROM `creature_loot_template` UNION "
            "SELECT `item` FROM `disenchant_loot_template` UNION "
            "SELECT `item` FROM `fishing_loot_template` UNION "
            "SELECT `item` FROM `gameobject_loot_template` UNION "
            "SELECT `item` FROM `item_loot_template` UNION "
            "SELECT `item` FROM `milling_loot_template` UNION "
            "SELECT `item` FROM `pickpocketing_loot_template` UNION "
            "SELECT `item` FROM `prospecting_loot_template` UNION "
            "SELECT `item` FROM `skinning_loot_template`";

        results = WorldDatabase.PQuery(lootQuery);
        if (results != NULL)
        {
            do
            {
                Field* fields = results->Fetch();
                lootItems.push_back(fields[0].GetUInt32());

            } while (results->NextRow());

            delete results;
        }
        else
        {
            sLog.outString("AuctionHouseBot: \"%s\" failed", lootQuery);
        }

        for (uint32 itemID = 0; itemID < sItemStorage.MaxEntry; itemID++)
        {
            ItemPrototype const* prototype = sObjectMgr.GetItemPrototype(itemID);

            if (!prototype)
                continue;

			if(prototype->ItemLevel >= 245)
				continue;

			if(prototype->ItemId > 47000)
				continue;

			if(prototype->Quality == 2)
				continue;

			if(prototype->InventoryType == INVTYPE_BODY)
				continue;

            switch (prototype->Bonding)
            {
            case 0:
                if (!No_Bind)
                    continue;
                break;
            case 2:
				if (!Bind_When_Equipped || prototype->ItemLevel > 200)
                    continue;
                break;
            case 3:
                if (!Bind_When_Use)
                    continue;
                break;
            default:
                continue;
                break;
            }

            if (prototype->BuyPrice == 0)
                continue;

            if ((prototype->Quality < 0) || (prototype->Quality > 6))
                continue;

            if (Vendor_Items == 0)
            {
                bool isVendorItem = false;

                for (unsigned int i = 0; (i < npcItems.size()) && (!isVendorItem); i++)
                {
                    if (itemID == npcItems[i])
                        isVendorItem = true;
                }

                if (isVendorItem)
                    continue;
            }

            if (Loot_Items == 0)
            {
                bool isLootItem = false;

                for (unsigned int i = 0; (i < lootItems.size()) && (!isLootItem); i++)
                {
                    if (itemID == lootItems[i])
                        isLootItem = true;
                }

                if (isLootItem)
                    continue;
            }

            if (Other_Items == 0)
            {
                bool isVendorItem = false;
                bool isLootItem = false;

                for (unsigned int i = 0; (i < npcItems.size()) && (!isVendorItem); i++)
                {
                    if (itemID == npcItems[i])
                        isVendorItem = true;
                }
                for (unsigned int i = 0; (i < lootItems.size()) && (!isLootItem); i++)
                {
                    if (itemID == lootItems[i])
                        isLootItem = true;
                }
                if ((!isLootItem) && (!isVendorItem))
                    continue;
            }

            switch (prototype->Quality)
            {
            case 0:
                if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
                    greyTradeGoodsBin.push_back(itemID);
                else
                    greyItemsBin.push_back(itemID);
                break;

            case 1:
                if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
                    whiteTradeGoodsBin.push_back(itemID);
                else
                    whiteItemsBin.push_back(itemID);
                break;

            case 2:
                if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
                    greenTradeGoodsBin.push_back(itemID);
                else
                    greenItemsBin.push_back(itemID);
                break;

            case 3:
				if(itemID < 47000)
				{
					if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
						blueTradeGoodsBin.push_back(itemID);
					else
						blueItemsBin.push_back(itemID);
				}
                break;

            case 4:
				if(itemID != 17203 && itemID != 18562 && itemID < 47000)
				{
					if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
						purpleTradeGoodsBin.push_back(itemID);
					else
						purpleItemsBin.push_back(itemID);
				}
                break;
			default:
				// non demandé
				break;
            }
        }

        if (
            (greyTradeGoodsBin.size() == 0) &&
            (whiteTradeGoodsBin.size() == 0) &&
            (greenTradeGoodsBin.size() == 0) &&
            (blueTradeGoodsBin.size() == 0) &&
            (purpleTradeGoodsBin.size() == 0) &&
            (greyItemsBin.size() == 0) &&
            (whiteItemsBin.size() == 0) &&
            (greenItemsBin.size() == 0) &&
            (blueItemsBin.size() == 0) &&
            (purpleItemsBin.size() == 0)
            )
        {
            sLog.outString("AuctionHouseBot: No items");
            AHBSeller = 0;
        }

        sLog.outString("AuctionHouseBot:");
        sLog.outString("loaded %d grey trade goods", greyTradeGoodsBin.size());
        sLog.outString("loaded %d white trade goods", whiteTradeGoodsBin.size());
        sLog.outString("loaded %d green trade goods", greenTradeGoodsBin.size());
        sLog.outString("loaded %d blue trade goods", blueTradeGoodsBin.size());
        sLog.outString("loaded %d purple trade goods", purpleTradeGoodsBin.size());
        sLog.outString("loaded %d grey items", greyItemsBin.size());
        sLog.outString("loaded %d white items", whiteItemsBin.size());
        sLog.outString("loaded %d green items", greenItemsBin.size());
        sLog.outString("loaded %d blue items", blueItemsBin.size());
        sLog.outString("loaded %d purple items", purpleItemsBin.size());
    }
    sLog.outError("AuctionHouseBot is now loaded");
}

void AuctionHouseBot::LoadValues(AHBConfig *config)
{
    if (AHBSeller)
    {
        //load percentages
        uint32 greytg = CharacterDatabase.PQuery("SELECT percentgreytradegoods FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32();
        uint32 whitetg = CharacterDatabase.PQuery("SELECT percentwhitetradegoods FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32();
        uint32 greentg = CharacterDatabase.PQuery("SELECT percentgreentradegoods FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32();
        uint32 bluetg = CharacterDatabase.PQuery("SELECT percentbluetradegoods FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32();
        uint32 purpletg = CharacterDatabase.PQuery("SELECT percentpurpletradegoods FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32();
        uint32 greyi = CharacterDatabase.PQuery("SELECT percentgreyitems FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32();
        uint32 whitei = CharacterDatabase.PQuery("SELECT percentwhiteitems FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32();
        uint32 greeni = CharacterDatabase.PQuery("SELECT percentgreenitems FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32();
        uint32 bluei = CharacterDatabase.PQuery("SELECT percentblueitems FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32();
        uint32 purplei = CharacterDatabase.PQuery("SELECT percentpurpleitems FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32();
        config->SetPercentages(greytg, whitetg, greentg, bluetg, purpletg, greyi, whitei, greeni, bluei, purplei);
        
        //load min and max prices
        config->SetMinPrice(AHB_GREY, CharacterDatabase.PQuery("SELECT minpricegrey FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMaxPrice(AHB_GREY, CharacterDatabase.PQuery("SELECT maxpricegrey FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMinPrice(AHB_WHITE, CharacterDatabase.PQuery("SELECT minpricewhite FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMaxPrice(AHB_WHITE, CharacterDatabase.PQuery("SELECT maxpricewhite FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMinPrice(AHB_GREEN, CharacterDatabase.PQuery("SELECT minpricegreen FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMaxPrice(AHB_GREEN, CharacterDatabase.PQuery("SELECT maxpricegreen FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMinPrice(AHB_BLUE, CharacterDatabase.PQuery("SELECT minpriceblue FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMaxPrice(AHB_BLUE, CharacterDatabase.PQuery("SELECT maxpriceblue FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMinPrice(AHB_PURPLE, CharacterDatabase.PQuery("SELECT minpricepurple FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMaxPrice(AHB_PURPLE, CharacterDatabase.PQuery("SELECT maxpricepurple FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        
        //load min and max bid prices
        config->SetMinBidPrice(AHB_GREY, CharacterDatabase.PQuery("SELECT minbidpricegrey FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMaxBidPrice(AHB_GREY, CharacterDatabase.PQuery("SELECT maxbidpricegrey FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMinBidPrice(AHB_WHITE, CharacterDatabase.PQuery("SELECT minbidpricewhite FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMaxBidPrice(AHB_WHITE, CharacterDatabase.PQuery("SELECT maxbidpricewhite FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());

		//load max stacks
        config->SetMaxStack(AHB_GREY, CharacterDatabase.PQuery("SELECT maxstackgrey FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMaxStack(AHB_WHITE, CharacterDatabase.PQuery("SELECT maxstackwhite FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMaxStack(AHB_GREEN, CharacterDatabase.PQuery("SELECT maxstackgreen FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMaxStack(AHB_BLUE, CharacterDatabase.PQuery("SELECT maxstackblue FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetMaxStack(AHB_PURPLE, CharacterDatabase.PQuery("SELECT maxstackpurple FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
    }
    if (AHBBuyer)
    {
        //load buyer bid prices
        config->SetBuyerPrice(AHB_GREY, CharacterDatabase.PQuery("SELECT buyerpricegrey FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetBuyerPrice(AHB_WHITE, CharacterDatabase.PQuery("SELECT buyerpricewhite FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetBuyerPrice(AHB_GREEN, CharacterDatabase.PQuery("SELECT buyerpricegreen FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetBuyerPrice(AHB_BLUE, CharacterDatabase.PQuery("SELECT buyerpriceblue FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        config->SetBuyerPrice(AHB_PURPLE, CharacterDatabase.PQuery("SELECT buyerpricepurple FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        
        //load bidding interval
        config->SetBiddingInterval(CharacterDatabase.PQuery("SELECT buyerbiddinginterval FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
        //load bids per interval
        config->SetBidsPerInterval(CharacterDatabase.PQuery("SELECT buyerbidsperinterval FROM auctionhousebot WHERE auctionhouse = %u",config->GetAHID())->Fetch()->GetUInt32());
    }
}
