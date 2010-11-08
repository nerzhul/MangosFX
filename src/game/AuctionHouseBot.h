#ifndef AUCTION_HOUSE_BOT_H
#define AUCTION_HOUSE_BOT_H

#include "World.h"
#include "Config/Config.h"
#include "ace/Vector_T.h"

#define AHB_GREY        0
#define AHB_WHITE       1
#define AHB_GREEN       2
#define AHB_BLUE        3
#define AHB_PURPLE      4
#define AHB_GREY_TG     0
#define AHB_WHITE_TG    1
#define AHB_GREEN_TG    2
#define AHB_BLUE_TG     3
#define AHB_PURPLE_TG   4
#define AHB_GREY_I      7
#define AHB_WHITE_I     8
#define AHB_GREEN_I     9
#define AHB_BLUE_I      10
#define AHB_PURPLE_I    11

enum AHBotCste
{
	MAXITEMS		=	1200,
	MINITEMS		=	700,
	MINTIME			=	8,
	MAXTIME			=	48,
};

class AHBConfig
{
private:
    uint32 AHID;
    uint32 AHFID;
    uint32 percentGreyTradeGoods;
    uint32 percentWhiteTradeGoods;
    uint32 percentGreenTradeGoods;
    uint32 percentBlueTradeGoods;
    uint32 percentPurpleTradeGoods;
    uint32 percentOrangeTradeGoods;
    uint32 percentYellowTradeGoods;
    uint32 percentGreyItems;
    uint32 percentWhiteItems;
    uint32 percentGreenItems;
    uint32 percentBlueItems;
    uint32 percentPurpleItems;
    uint32 percentOrangeItems;
    uint32 percentYellowItems;
    uint32 minPriceGrey;
    uint32 maxPriceGrey;
    uint32 minBidPriceGrey;
    uint32 maxBidPriceGrey;
    uint32 maxStackGrey;
    uint32 minPriceWhite;
    uint32 maxPriceWhite;
    uint32 minBidPriceWhite;
    uint32 maxBidPriceWhite;
    uint32 maxStackWhite;
    uint32 minPriceGreen;
    uint32 maxPriceGreen;
    uint32 maxStackGreen;
    uint32 minPriceBlue;
    uint32 maxPriceBlue;
    uint32 maxStackBlue;
    uint32 minPricePurple;
    uint32 maxPricePurple;
    uint32 maxStackPurple;
    uint32 minPriceOrange;
    uint32 maxPriceOrange;
    uint32 minBidPriceOrange;
    uint32 maxBidPriceOrange;
    uint32 maxStackOrange;
    uint32 minPriceYellow;
    uint32 maxPriceYellow;
    uint32 minBidPriceYellow;
    uint32 maxBidPriceYellow;
    uint32 maxStackYellow;

    uint32 buyerPriceGrey;
    uint32 buyerPriceWhite;
    uint32 buyerPriceGreen;
    uint32 buyerPriceBlue;
    uint32 buyerPricePurple;
    uint32 buyerPriceOrange;
    uint32 buyerPriceYellow;
    uint32 buyerBiddingInterval;
    uint32 buyerBidsPerInterval;

    uint32 greytgp;
    uint32 whitetgp;
    uint32 greentgp;
    uint32 bluetgp;
    uint32 purpletgp;
    uint32 greyip;
    uint32 whiteip;
    uint32 greenip;
    uint32 blueip;
    uint32 purpleip;

public:
    AHBConfig(uint32 ahid)
    {
        AHID = ahid;
        switch(ahid)
        {
        case 2:
            AHFID = 55;
            break;
        case 6:
            AHFID = 29;
            break;
        case 7:
            AHFID = 120;
            break;
        default:
            AHFID = 120;
            break;
        }
    }
    AHBConfig()
    {
    }
    uint32 GetAHID()
    {
        return AHID;
    }
    uint32 GetAHFID()
    {
        return AHFID;
    }

    void SetPercentages(uint32 greytg, uint32 whitetg, uint32 greentg, uint32 bluetg, uint32 purpletg, uint32 greyi, uint32 whitei, uint32 greeni, uint32 bluei, uint32 purplei)
    {
        uint32 totalPercent = greytg + whitetg + greentg + bluetg + purpletg + greyi + whitei + greeni + bluei + purplei;
		
		if (totalPercent != 100)
        {
            greytg = 0;
            whitetg = 27;
            greentg = 12;
            bluetg = 10;
            purpletg = 1;
            greyi = 0;
            whitei = 10;
            greeni = 30;
            bluei = 8;
            purplei = 2;
        }
        percentGreyTradeGoods = greytg;
        percentWhiteTradeGoods = whitetg;
        percentGreenTradeGoods = greentg;
        percentBlueTradeGoods = bluetg;
        percentPurpleTradeGoods = purpletg;
        percentGreyItems = greyi;
        percentWhiteItems = whitei;
        percentGreenItems = greeni;
        percentBlueItems = bluei;
        percentPurpleItems = purplei;
        CalculatePercents();
    }
    uint32 GetPercentages(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY_TG:
            return percentGreyTradeGoods;
            break;
        case AHB_WHITE_TG:
            return percentWhiteTradeGoods;
            break;
        case AHB_GREEN_TG:
            return percentGreenTradeGoods;
            break;
        case AHB_BLUE_TG:
            return percentBlueTradeGoods;
            break;
        case AHB_PURPLE_TG:
            return percentPurpleTradeGoods;
            break;
        case AHB_GREY_I:
            return percentGreyItems;
            break;
        case AHB_WHITE_I:
            return percentWhiteItems;
            break;
        case AHB_GREEN_I:
            return percentGreenItems;
            break;
        case AHB_BLUE_I:
            return percentBlueItems;
            break;
        case AHB_PURPLE_I:
            return percentPurpleItems;
            break;
        default:
            return 0;
            break;
        }
    }
    void SetMinPrice(uint32 color, uint32 value)
    {
        switch(color)
        {
        case AHB_GREY:
            minPriceGrey = value;
            break;
        case AHB_WHITE:
            minPriceWhite = value;
            break;
        case AHB_GREEN:
            minPriceGreen = value;
            break;
        case AHB_BLUE:
            minPriceBlue = value;
            break;
        case AHB_PURPLE:
            minPricePurple = value;
            break;
        default:
            break;
        }
    }
    uint32 GetMinPrice(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY:
            {
                if (minPriceGrey == 0)
                    return 100;
                else if (minPriceGrey > maxPriceGrey)
                    return maxPriceGrey;
                else
                    return minPriceGrey;
                break;
            }
        case AHB_WHITE:
            {
                if (minPriceWhite == 0)
                    return 150;
                else if (minPriceWhite > maxPriceWhite)
                    return maxPriceWhite;
                else
                    return minPriceWhite;
                break;
            }
        case AHB_GREEN:
            {
                if (minPriceGreen == 0)
                    return 200;
                else if (minPriceGreen > maxPriceGreen)
                    return maxPriceGreen;
                else
                    return minPriceGreen;
                break;
            }
        case AHB_BLUE:
            {
                if (minPriceBlue == 0)
                    return 250;
                else if (minPriceBlue > maxPriceBlue)
                    return maxPriceBlue;
                else
                    return minPriceBlue;
                break;
            }
        case AHB_PURPLE:
            {
                if (minPricePurple == 0)
                    return 300;
                else if (minPricePurple > maxPricePurple)
                    return maxPricePurple;
                else
                    return minPricePurple;
                break;
            }
        default:
            {
                return 0;
                break;
            }
        }
    }
    void SetMaxPrice(uint32 color, uint32 value)
    {
        switch(color)
        {
        case AHB_GREY:
            maxPriceGrey = value;
            break;
        case AHB_WHITE:
            maxPriceWhite = value;
            break;
        case AHB_GREEN:
            maxPriceGreen = value;
            break;
        case AHB_BLUE:
            maxPriceBlue = value;
            break;
        case AHB_PURPLE:
            maxPricePurple = value;
            break;
        default:
            break;
        }
    }
    uint32 GetMaxPrice(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY:
            {
                if (maxPriceGrey == 0)
                    return 150;
                else
                    return maxPriceGrey;
                break;
            }
        case AHB_WHITE:
            {
                if (maxPriceWhite == 0)
                    return 250;
                else
                    return maxPriceWhite;
                break;
            }
        case AHB_GREEN:
            {
                if (maxPriceGreen == 0)
                    return 300;
                else
                    return maxPriceGreen;
                break;
            }
        case AHB_BLUE:
            {
                if (maxPriceBlue == 0)
                    return 350;
                else
                    return maxPriceBlue;
                break;
            }
        case AHB_PURPLE:
            {
                if (maxPricePurple == 0)
                    return 450;
                else
                    return maxPricePurple;
                break;
            }
        default:
            {
                return 0;
                break;
            }
        }
    }
    void SetMinBidPrice(uint32 color, uint32 value)
    {
        switch(color)
        {
        case AHB_GREY:
            minBidPriceGrey = value;
            break;
        case AHB_WHITE:
            minBidPriceWhite = value;
            break;
        default:
            break;
        }
    }
    uint32 GetMinBidPrice(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY:
            {
                if (minBidPriceGrey > 100)
                    return 100;
                else
                    return minBidPriceGrey;
                break;
            }
        case AHB_WHITE:
            {
                if (minBidPriceWhite > 100)
                    return 100;
                else
                    return minBidPriceWhite;
                break;
            }
        default:
            {
                return 0;
                break;
            }
        }
    }
    void SetMaxBidPrice(uint32 color, uint32 value)
    {
        switch(color)
        {
        case AHB_GREY:
            maxBidPriceGrey = value;
            break;
        case AHB_WHITE:
            maxBidPriceWhite = value;
            break;
        default:
            break;
        }
    }
    uint32 GetMaxBidPrice(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY:
            {
                if (maxBidPriceGrey > 100)
                    return 100;
                else
                    return maxBidPriceGrey;
                break;
            }
        case AHB_WHITE:
            {
                if (maxBidPriceWhite > 100)
                    return 100;
                else
                    return maxBidPriceWhite;
                break;
            }
        default:
            {
                return 0;
                break;
            }
        }
    }
    void SetMaxStack(uint32 color, uint32 value)
    {
        switch(color)
        {
        case AHB_GREY:
            maxStackGrey = value;
            break;
        case AHB_WHITE:
            maxStackWhite = value;
            break;
        case AHB_GREEN:
            maxStackGreen = value;
            break;
        case AHB_BLUE:
            maxStackBlue = value;
            break;
        case AHB_PURPLE:
            maxStackPurple = value;
            break;
        default:
            break;
        }
    }
    uint32 GetMaxStack(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY:
            {
                return maxStackGrey;
                break;
            }
        case AHB_WHITE:
            {
                return maxStackWhite;
                break;
            }
        case AHB_GREEN:
            {
                return maxStackGreen;
                break;
            }
        case AHB_BLUE:
            {
                return maxStackBlue;
                break;
            }
        case AHB_PURPLE:
            {
                return maxStackPurple;
                break;
            }
        default:
            {
                return 0;
                break;
            }
        }
    }
    void SetBuyerPrice(uint32 color, uint32 value)
    {
        switch(color)
        {
        case AHB_GREY:
            buyerPriceGrey = value;
            break;
        case AHB_WHITE:
            buyerPriceWhite = value;
            break;
        case AHB_GREEN:
            buyerPriceGreen = value;
            break;
        case AHB_BLUE:
            buyerPriceBlue = value;
            break;
        case AHB_PURPLE:
            buyerPricePurple = value;
            break;
        default:
            break;
        }
    }
    uint32 GetBuyerPrice(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY:
            return buyerPriceGrey;
            break;
        case AHB_WHITE:
            return buyerPriceWhite;
            break;
        case AHB_GREEN:
            return buyerPriceGreen;
            break;
        case AHB_BLUE:
            return buyerPriceBlue;
            break;
        case AHB_PURPLE:
            return buyerPricePurple;
            break;
        default:
            return 0;
            break;
        }
    }
    void SetBiddingInterval(uint32 value)
    {
        buyerBiddingInterval = value;
    }
    uint32 GetBiddingInterval()
    {
        return buyerBiddingInterval;
    }
    void CalculatePercents()
    {
        greytgp = (uint32) (((double)percentGreyTradeGoods / 100.0) * MAXITEMS);
        whitetgp = (uint32) (((double)percentWhiteTradeGoods / 100.0) * MAXITEMS);
        greentgp = (uint32) (((double)percentGreenTradeGoods / 100.0) * MAXITEMS);
        bluetgp = (uint32) (((double)percentBlueTradeGoods / 100.0) * MAXITEMS);
        purpletgp = (uint32) (((double)percentPurpleTradeGoods / 100.0) * MAXITEMS);
        greyip = (uint32) (((double)percentGreyItems / 100.0) * MAXITEMS);
        whiteip = (uint32) (((double)percentWhiteItems / 100.0) * MAXITEMS);
        greenip = (uint32) (((double)percentGreenItems / 100.0) * MAXITEMS);
        blueip = (uint32) (((double)percentBlueItems / 100.0) * MAXITEMS);
        purpleip = (uint32) (((double)percentPurpleItems / 100.0) * MAXITEMS);
        uint32 total = greytgp + whitetgp + greentgp + bluetgp + purpletgp + greyip + whiteip + greenip + blueip + purpleip;
        int32 diff = (MAXITEMS - total);
        if (diff < 0)
        {
            if ((whiteip - diff) > 0)
                whiteip -= diff;
            else if ((greenip - diff) > 0)
                greenip -= diff;
        }
        else if (diff < 0)
        {
            whiteip += diff;
        }
    }
    uint32 GetPercents(uint32 color)
    {
        switch(color)
        {
        case AHB_GREY_TG:
            return greytgp;
            break;
        case AHB_WHITE_TG:
            return whitetgp;
            break;
        case AHB_GREEN_TG:
            return greentgp;
            break;
        case AHB_BLUE_TG:
            return bluetgp;
            break;
        case AHB_PURPLE_TG:
            return purpletgp;
            break;
        case AHB_GREY_I:
            return greyip;
            break;
        case AHB_WHITE_I:
            return whiteip;
            break;
        case AHB_GREEN_I:
            return greenip;
            break;
        case AHB_BLUE_I:
            return blueip;
            break;
        case AHB_PURPLE_I:
            return purpleip;
            break;
        default:
            return 0;
            break;
        }
    }
    void SetBidsPerInterval(uint32 value)
    {
        buyerBidsPerInterval = value;
    }
    uint32 GetBidsPerInterval()
    {
        return buyerBidsPerInterval;
    }
    ~AHBConfig()
    {
    }
};

class AuctionHouseBot
{
private:
    ACE_Vector<uint32> npcItems;
    ACE_Vector<uint32> lootItems;
    ACE_Vector<uint32> greyTradeGoodsBin;
    ACE_Vector<uint32> whiteTradeGoodsBin;
    ACE_Vector<uint32> greenTradeGoodsBin;
    ACE_Vector<uint32> blueTradeGoodsBin;
    ACE_Vector<uint32> purpleTradeGoodsBin;
    ACE_Vector<uint32> greyItemsBin;
    ACE_Vector<uint32> whiteItemsBin;
    ACE_Vector<uint32> greenItemsBin;
    ACE_Vector<uint32> blueItemsBin;
    ACE_Vector<uint32> purpleItemsBin;

    bool AHBSeller;
    bool AHBBuyer;

    uint32 AHBplayerAccount;
    uint32 AHBplayerGUID;
    uint32 ItemsPerCycle;

	bool Vendor_Items;
    bool Loot_Items;
    bool Other_Items;

    bool No_Bind;
    bool Bind_When_Equipped;
    bool Bind_When_Use;

    AHBConfig AllianceConfig;
    AHBConfig HordeConfig;
    AHBConfig NeutralConfig;

    time_t _lastrun_a;
    time_t _lastrun_h;
    time_t _lastrun_n;

    inline uint32 minValue(uint32 a, uint32 b) { return a <= b ? a : b; };
    void addNewAuctions(Player *AHBplayer, AHBConfig *config);
    void addNewAuctionBuyerBotBid(Player *AHBplayer, AHBConfig *config, WorldSession *session);

public:
    AuctionHouseBot();
    ~AuctionHouseBot();
    void Update();
    void Initialize();
    void LoadValues(AHBConfig*);
    uint32 GetAHBplayerGUID() { return AHBplayerGUID; };
};

#define auctionbot MaNGOS::Singleton<AuctionHouseBot>::Instance()

#endif
