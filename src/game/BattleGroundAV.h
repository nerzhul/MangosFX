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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
 
#ifndef __BATTLEGROUNDAV_H
#define __BATTLEGROUNDAV_H
 
class BattleGround;

/* Constantes du BG */

// Score avant annonce de défaite proche
#define BG_AV_SCORE_NEAR_LOSE               120
// Nombre de points dynamiques à gérer
#define BG_AV_NODES_MAX				        15
// Renforts de départ
#define BG_AV_SCORE_INITIAL_POINTS			600
// Temps de capture d'un point
#define BG_AV_CAPTIME 240000 // 4 minutes
// Pour le cimetière central
#define BG_AV_SNOWFALL_FIRSTCAP 300000
// distance maximale de point
#define BG_AV_MAX_NODE_DISTANCE             25 
#define BG_AV_NodeEventSnowfall				61  
#define BG_AV_MAX_NODE_EVENTS				62
#define HORDE_REP							729
#define ALLIANCE_REP						730
// Récompenses de destruction de tour
#define BG_AV_RES_TOWER						75
#define BG_AV_REP_TOWER                     12
#define BG_AV_KILL_TOWER					3
#define BG_AV_BOSS_KILL_QUEST_SPELL         23658


// Concernant les mobs
#define BG_AV_REP_BOSS                      350
#define BG_AV_REP_CAPTAIN                   125
#define BG_AV_KILL_CAPTAIN                  3
#define BG_AV_KILL_BOSS                     4
#define BG_AV_RES_CAPTAIN                   100

	// x			y			z		o		r0			r1			r2		r3
const float BG_AV_DoorPositions[2][8] = {
    {780.487f,		-493.0239f, 99.955299f, 3.0976f, 0.012957f, -0.060288f, 0.344959f,	0.93659f}, // Alliance
    {-1375.189941f, -538.971f,	55.282398f, 0.7217f, 0.050291f, 0.015127f,	0.929217f,	-0.365784f}, //Horde
};

const float BG_AV_SpiritGuides[7][4] = {
	{643.000f,		44.000f,	69.780f,	4.732f},
	{676.000f,		-374.000f,	30.010f,	1.625f},
	{73.417f,		-496.433f,	48.731f,	1.515f},
	{-157.4f,		31.205f,	77.101f,	4.393f},
	{-531.218f,		-405.00f,	49.544f,	1.198f},
	{-1090.469f,	-253.309f,	57.673f,	4.785f},
	{-1496.000f,	-333.299f,	101.135f,	0.547f},
};

const float BG_AV_FlammesForCaptains[18][8] = {
	/* Horde */

	{-501.774f,		-151.580f,	81.202f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-509.975f,		-191.651f,	83.297f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-517.509f,		-200.429f,	80.759f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-568.039f,		-188.707f,	81.550f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-514.361f,		-163.863f,	104.136f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},

	/* -- Fumée -- */
	{-524.276f,		-199.600f,	82.873f,	-1.466f,0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-518.195f,		-173.085f,	102.43f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-500.731f,		-145.358f,	88.533f,	2.44f,	0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-501.084f,		-150.783f,	80.851f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},

	/* Alliance */
	{-3.409f,		-306.287f,	33.340f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-64.267f,		-289.411f,	33.469f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-5.980f,		-324.144f,	38.853f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-48.618f,		-266.916f,	47.816f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-62.947f,		-286.212f,	66.728f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},

	/* -- fumée -- */
	{-60.250f,		-309.231f,	50.240f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-64.498f,		-289.329f,	33.461f,	-2.82f,	0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-48.794f,		-266.532f,	47.791f,	2.443f,	0.0144f,	-0.01f,		0.314f,		0.9413f},
	{-2.678f,		-306.997f,	33.416f,	0.0f,	0.0144f,	-0.01f,		0.314f,		0.9413f},



};
								//	l  c //
const float BG_AV_Capturable_Points[11][8] = {
	{638.591f,		-32.422f,	46.060f,	-1.623f,	0.0144f,	-0.01f,		0.314f,		0.9413f},	// Dun Baldar
	{669.007f,		-294.018f,	30.29f,		2.77f,		0.01548f,	-0.04f,		0.3454f,	0.9354f},	// Cimetière Foudrepique
	{77.801f,		-404.700f,	46.754f,	-0.872f,	0.14f,		0.047f,		0.0114f,	0.974f},	// Gîtepierre
	{-202.580f,		-112.730f,	78.487f,	-0.715f,	-0.0715f,	0.024f,		0.145f,		0.9457f},	// Cimetière des neiges
	{-201.281f,		-134.319f,	78.675f,	-0.942f,	0.01554f,	0.014f,		0.3441f,	0.947f},	// SnowFall 1
	{-191.153f,		-129.867f,	78.559f,	-1.256f,	0.0146f,	0.019f,		0.3454f,	0.9547f},	// SnowFall 2
	{-200.464f,		-96.417f,	79.758f,	1.361f,		0.0159f,	0.0147f,	0.3547f,	0.9571f},	// ShowFall 3
	{-215.981f,		-91.410f,	80.870f,	-1.745f,	0.0157f,	0.0189f,	0.3489f,	0.9359f},	// SnowFall 4
	{-611.961f,		-396.170f,	60.835f,	2.536f,		0.0148f,	0.0148f,	0.364f,		0.9356f},	// GlaceSang
	{-1082.449f,	-346.822f,	54.921f,	-1.535f,	0.0149f,	0.0147f,	0.3597f,	0.9347f},	// Cimetière loup de givre
	{-1402.209f,	-307.431f,	89.422f,	0.191f,		0.0148f,	0.0142f,	0.357f,		0.9341f},	// Hutte de guérison Loup de Givre
};

const float BG_AV_Destroyable_Points[16][8] = {
	{674.000f,		-143.125f,	63.661f,	0.994f,		0.0147f,	0.014f,		0.3458f,	0.9357f},	// Dun Baldar North 1
	{679.338f,		-136.468f,	73.962f,	-2.164f,	0.0158f,	0.015f,		0.3459f,	0.935f},	// Dun Baldar North 2
	{553.778f,		-78.656f,	51.937f,	-1.221f,	0.0157f,	0.014f,		0.0348f,	0.9356f},	// Dun Baldar South 1
	{555.848f,		-84.415f,	64.439f,	3.124f,		0.0147f,	0.014f,		0.0357f,	0.936f},	// Dun Baldar South 2
	{203.281f,		-360.365f,	56.386f,	-0.925f,	0.0148f,	0.015f,		0.0349f,	0.935f},	// Aile de Glace 1
	{208.973f,		-365.971f,	66.740f,	-0.244f,	0.0149f,	0.014f,		0.0349f,	0.9375f},	// Aile de Glace 2
	{-152.436f,		-441.757f,	40.398f,	-1.9577f,	0.0146f,	0.016f,		0.0348f,	0.9386f},	// Fortin de Gîtepierre 1
	{-155.832f,		-449.401f,	52.730f,	0.610f,		0.0148f,	0.014f,		0.0348f,	0.9384f},	// Fortin de Gîtepierre 2
	{-571.880f,		-262.777f,	75.008f,	-0.802f,	0.0147f,	0.015f,		0.0348f,	0.9357f},	// GlaceSang 1
	{-572.328f,		-262.476f,	88.649f,	-0.575f,	0.148f,		0.014f,		0.0348f,	0.9359f},	// GlaceSang 2
	{-768.906f,		-363.709f,	90.894f,	1.0799f,	0.147f,		0.015f,		0.0346f,	0.9354f},	// Tour de la Halte 1
	{-768.198f,		-363.105f,	104.537f,	0.1047f,	0.149f,		0.014f,		0.0346f,	0.9353f},	// Tour de la Halte 2
	{-1297.500f,	-266.766f,	114.150f,	3.310f,		0.148f,		0.017f,		0.0347f,	0.9458f},	// Tour Loup de Givre Occidentale 1
	{-1297.869f,	-266.761f,	127.795f,	0.0698f,	0.142f,		0.012f,		0.0351f,	0.9451f},	// Tour Loup de Givre Occidentale 2
	{-1302.900f,	-316.980f,	113.866f,	2.007f,		0.147f,		0.014f,		0.0352f,	0.9453f},	// Tour Loup de Givre Orientale 1
	{-1302.839f,	-316.582f,	127.515f,	0.122f,		0.144f,		0.011f,		0.0353f,	0.94561f},	// Tour Loup de Givre Orientale 2

};

enum BG_AV_CREATURE_ENTRIES                                 // only those, which are interesting for us
{
    BG_AV_CREATURE_ENTRY_H_CAPTAIN          = 11947,
    BG_AV_CREATURE_ENTRY_A_CAPTAIN          = 11949,
    BG_AV_CREATURE_ENTRY_H_BOSS             = 11946,
    BG_AV_CREATURE_ENTRY_A_BOSS             = 11948,
    // he yells all captures/defends... to all players
    BG_AV_CREATURE_ENTRY_N_HERALD           = 11997,

    // they are needed cause they must get despawned after destroying a tower
    BG_AV_CREATURE_ENTRY_A_MARSHAL_SOUTH    = 14763,
    BG_AV_CREATURE_ENTRY_A_MARSHAL_NORTH    = 14762,
    BG_AV_CREATURE_ENTRY_A_MARSHAL_ICE      = 14764,
    BG_AV_CREATURE_ENTRY_A_MARSHAL_STONE    = 14765,
    BG_AV_CREATURE_ENTRY_H_MARSHAL_ICE      = 14773,
    BG_AV_CREATURE_ENTRY_H_MARSHAL_TOWER    = 14776,
    BG_AV_CREATURE_ENTRY_H_MARSHAL_ETOWER   = 14772,
    BG_AV_CREATURE_ENTRY_H_MARSHAL_WTOWER   = 14777,

    BG_AV_CREATURE_ENTRY_A_GRAVE_DEFENSE_1  = 12050,
    BG_AV_CREATURE_ENTRY_A_GRAVE_DEFENSE_2  = 13326,
    BG_AV_CREATURE_ENTRY_A_GRAVE_DEFENSE_3  = 13331,
    BG_AV_CREATURE_ENTRY_A_GRAVE_DEFENSE_4  = 13422,

    BG_AV_CREATURE_ENTRY_H_GRAVE_DEFENSE_1  = 12053,
    BG_AV_CREATURE_ENTRY_H_GRAVE_DEFENSE_2  = 13328,
    BG_AV_CREATURE_ENTRY_H_GRAVE_DEFENSE_3  = 13332,
    BG_AV_CREATURE_ENTRY_H_GRAVE_DEFENSE_4  = 13421,

    BG_AV_CREATURE_ENTRY_A_TOWER_DEFENSE    = 13358,
    BG_AV_CREATURE_ENTRY_H_TOWER_DEFENSE    = 13359,
};

const float BG_AV_Flammes[64][10] = {
	/* DBS */
	{582.161f,		-81.237f,	37.921f,	0.087f,		0.142f,		0.016f,		0.014f,		0.944f},
	{572.148f,		-93.786f,	52.572f,	0.541f,		0.143f,		0.015f,		0.015f,		0.945f},
	{558.096f,		-70.984f,	52.487f,	0.820f,		0.144f,		0.014f,		0.015f,		0.945f},
	{543.513f,		-94.400f,	52.481f,	0.034f,		0.143f,		0.014f,		0.013f,		0.946f},
	{562.632f,		-88.181f,	61.993f,	0.383f,		0.147f,		0.014f,		0.016f,		0.948f},
	{578.166f,		-71.819f,	38.151f,	2.722f,		0.147f,		0.018f,		0.016f,		0.947f},
	{562.523f,		-74.502f,	37.947f,	-0.052f,	0.146f,		0.018f,		0.013f,		0.946f},
	{556.028f,		-94.924f,	44.819f,	3.054f,		0.149f,		0.017f,		0.016f,		0.948f},

	/* DBN */
	{664.797f,		-143.649f,	64.178f,	-0.453f,	0.142f,		0.021f,		0.013f,		0.941f},
	{674.575f,		-147.100f,	56.542f,	-1.605f,	0.143f,		0.018f,		0.014f,		0.942f},
	{664.505f,		-139.451f,	49.669f,	-0.034f,	0.141f,		0.019f,		0.013f,		0.947f},
	{684.422f,		-146.582f,	63.666f,	0.994f,		0.142f,		0.017f,		0.016f,		0.947f},
	{693.004f,		-144.024f,	64.175f,	2.443f,		0.144f,		0.018f,		0.016f,		0.948f},
	{682.791f,		-127.768f,	62.415f,	1.099f,		0.143f,		0.017f,		0.014f,		0.947f},
	{676.067f,		-124.319f,	49.672f,	-1.012f,	0.143f,		0.018f,		0.016f,		0.948f},
	{655.718f,		-126.672f,	49.913f,	2.809f,		0.148f,		0.019f,		0.015f,		0.945f},
	
	/* Icewing bunker */
	{231.506f,		-356.687f,	42.370f,	0.296f,		0.148f,		0.019f,		0.015f,		0.945f},
	{224.988f,		-348.174f,	42.560f,	1.500f,		0.148f,		0.019f,		0.015f,		0.945f},
	{220.649f,		-368.131f,	42.397f,	-0.261f,	0.148f,		0.019f,		0.015f,		0.945f},
	{209.647f,		-352.631f,	42.395f,	-0.691f,	0.148f,		0.019f,		0.015f,		0.945f},
	{210.619f,		-376.937f,	49.267f,	2.862f,		0.148f,		0.019f,		0.015f,		0.945f},
	{200.259f,		-359.967f,	49.267f,	-2.897f,	0.148f,		0.019f,		0.015f,		0.945f},
	{224.682f,		-374.031f,	57.067f,	0.541f,		0.148f,		0.019f,		0.015f,		0.945f},
	{196.619f,		-378.015f,	56.913f,	1.012f,		0.148f,		0.019f,		0.015f,		0.945f},

	/* StoneHeart */
	{-136.537f,		-459.700f,	26.380f,	3.913f,		0.148f,		0.019f,		0.015f,		0.945f},
	{-163.440f,		-454.187f,	33.279f,	1.937f,		0.148f,		0.019f,		0.015f,		0.945f},
	{-155.488f,		-437.355f,	33.279f,	2.600f,		0.148f,		0.019f,		0.015f,		0.945f},
	{-151.638f,		-439.520f,	40.379f,	0.436f,		0.148f,		0.019f,		0.015f,		0.945f},
	{-143.591f,		-439.750f,	40.927f,	-1.727f,	0.148f,		0.019f,		0.015f,		0.945f},
	{-171.291f,		-444.683f,	40.921f,	2.303f,		0.148f,		0.019f,		0.015f,		0.945f},
	{-154.076f,		-466.928f,	41.063f,	-1.867f,	0.148f,		0.019f,		0.015f,		0.945f},
	{-143.977f,		-445.148f,	26.409f,	-1.867f,	0.148f,		0.019f,		0.015f,		0.945f},

	/* IceBlood */
	{-568.317f,		-267.100f,	75.000f,	1.012f,		0.145f,		0.012f,		0.014f,		0.947f},
	{-561.021f,		-262.688f,	68.458f,	1.378f,		0.145f,		0.012f,		0.014f,		0.947f},
	{-566.064f,		-273.907f,	52.958f,	-0.890f,	0.145f,		0.012f,		0.014f,		0.947f},
	{-559.620f,		-268.596f,	52.898f,	0.052f,		0.145f,		0.012f,		0.014f,		0.947f},
	{-572.666f,		-267.923f,	56.864f,	2.356f,		0.145f,		0.012f,		0.014f,		0.947f},
	{-578.625f,		-267.571f,	68.469f,	0.506f,		0.145f,		0.012f,		0.014f,		0.947f},
	{-580.947f,		-259.769f,	68.469f,	1.466f,		0.145f,		0.012f,		0.014f,		0.947f},
	{-574.770f,		-251.449f,	74.942f,	-1.186f,	0.145f,		0.012f,		0.014f,		0.947f},
	
	/* Tower Point */
	{-768.763f,		-362.734f,	104.612f,	1.1815f,	0.145f,		0.012f,		0.014f,		0.947f},
	{-765.460f,		-357.710f,	90.88f,		0.314f,		0.146f,		0.014f,		0.018f,		0.917f},
	{-760.356f,		-358.895f,	84.355f,	2.1293f,	0.14f,		0.016f,		0.017f,		0.942f},
	{-776.072f,		-368.04f,	84.355f,	2.6354f,	0.154f,		0.014f,		0.016f,		0.942f},
	{-771.966f,		-353.838f,	84.348f,	1.74533f,	0.154f,		0.016f,		0.012f,		0.943f},
	{-767.103f,		-350.737f,	68.793f,	2.809f,		0.153f,		0.015f,		0.013f,		0.942f},
	{-760.114f,		-353.845f,	68.86f,		1.7976f,	0.156f,		0.014f,		0.013f,		0.942f},
	{-764.109f,		-366.069f,	70.093f,	0.383f,		0.150f,		0.014f,		0.0158f,	0.944f},

	/* FrostWolf East*/
	{-1293.890f,	-313.477f, 107.328f,	1.605f,		0.145f,		0.012f,		0.014f,		0.947f},
	{-1311.569f,	-308.079f,	91.733f,	-1.850f,	0.145f,		0.012f,		0.014f,		0.947f},
	{-1304.869f,	-304.524f,	91.836f,	-0.680f,	0.145f,		0.012f,		0.014f,		0.947f},
	{-1301.770f,	-310.973f,	95.825f,	0.907f,		0.145f,		0.012f,		0.014f,		0.947f},
	{-1312.410f,	-312.998f,	107.328f,	1.570f,		0.145f,		0.012f,		0.014f,		0.947f},
	{-1304.599f,	-310.753f,	113.859f,	-0.401f,	0.145f,		0.012f,		0.014f,		0.947f},
	{-1294.310f,	-323.467f,	113.892f,	-1.675f,	0.145f,		0.012f,		0.014f,		0.947f},
	{-1302.650f,	-317.191f,	127.487f,	2.303f,		0.145f,		0.012f,		0.014f,		0.947f},

	/* FrostWolf West */
	{-1308.239f,	-273.260f,	92.051f,	-0.139f,	0.145f,		0.012f,		0.014f,		0.947f},
	{-1309.530f,	-265.950f,	92.141f,	-2.495f,	0.145f,		0.012f,		0.014f,		0.947f},
	{-1302.260f,	-262.858f,	95.926f,	0.418f,		0.145f,		0.012f,		0.014f,		0.947f},
	{-1297.280f,	-267.773f,	126.755f,	2.234f,		0.145f,		0.012f,		0.014f,		0.947f},
	{-1303.410f,	-268.237f,	114.151f,	-1.239f,	0.145f,		0.012f,		0.014f,		0.947f},
	{-1304.430f,	-273.682f,	107.612f,	0.244f,		0.145f,		0.012f,		0.014f,		0.947f},
	{-1299.079f,	-256.890f,	114.108f,	-2.443f,	0.145f,		0.012f,		0.014f,		0.947f},
	{-1289.689f,	-259.520f,	107.612f,	-2.199f,	0.145f,		0.012f,		0.014f,		0.947f},
};

const float BG_AV_Nodes_Coords[15][2] = {
	{638.591f,		-32.422f},
	{669.007f,		-294.018f},
	{77.801f,		-404.700f},
	{-202.580f,		-112.730f},
	{-611.961f,		-396.170f},
	{-1082.449f,	-346.822f},
	{-1402.209f,	-307.431f},
	{553.778f,		-78.656f},
	{674.000f,		-143.125f},
	{203.281f,		-360.365f},
	{-152.436f,		-441.757f},
	{-571.880f,		-262.777f},
	{-768.906f,		-363.709f},
	{-1302.900f,	-316.980f},
	{-1297.500f,	-266.766f},
};
enum BG_AV_States
{
    POINT_NEUTRAL               = 0,
    POINT_ASSAULTED             = 1,
    POINT_DESTROYED             = 2,
    POINT_CONTROLLED            = 3
};

enum BG_AV_ObjectTypes
{
	BG_AV_OBJECTID_GATE		= 180424,
	/* Cimetières */
	NEUTRAL_AURA_BANNER		= 180423,
	HORDE_AURA_BANNER		= 180422,
	ALLI_AURA_BANNER		= 180421,
	HORDE_BANNER			= 178943,
	ALLIANCE_BANNER			= 178365,
	HORDE_CONT_BANNER		= 179287,
	ALLIANCE_CONT_BANNER	= 179286,
	SNOWFALL_BANNER			= 180418,
	SNOWFALL_BANNER_A		= 179044,
	SNOWFALL_BANNER_H		= 179064,
	SNOWFALL_BANNER_H_CONT	= 179425,
	SNOWFALL_BANNER_A_CONT	= 179424,
	/* Tours */
	ALLIANCE_D_BANNER		= 178925,
	HORDE_COND_T_BANNER		= 179436,
	ALLIANCE_T_BANNER		= 178927,
	HORDE_CONT_D_BANNER		= 179435,	
	ALLIANCE_CONT_D_BANNER	= 178940,
	HORDE_D_BANNER			= 178943,
	HORDE_T_BANNER			= 178955,
	ALLIANCE_CONT_T_BANNER	= 179446,
	/* Flammes */
	FLAMMES					= 179065,
	SMOKE					= 179066,
    BG_AV_OBJECTID_MINE_N   = 178785,
    BG_AV_OBJECTID_MINE_S   = 178784,

};

enum BG_AV_MineCreature_Entries
{
    // North_Mine_..._X 4 is always boss
    BG_AV_NORTH_MINE_NEUTRAL_1  = 10987,
    BG_AV_NORTH_MINE_NEUTRAL_2  = 11600,
    BG_AV_NORTH_MINE_NEUTRAL_3  = 11602,
    BG_AV_NORTH_MINE_NEUTRAL_4  = 11657,
    BG_AV_NORTH_MINE_ALLIANCE_1 = 13396,
    BG_AV_NORTH_MINE_ALLIANCE_2 = 13080,
    BG_AV_NORTH_MINE_ALLIANCE_3 = 13098,
    BG_AV_NORTH_MINE_ALLIANCE_4 = 13078,
    BG_AV_NORTH_MINE_HORDE_1    = 13397,
    BG_AV_NORTH_MINE_HORDE_2    = 13099,
    BG_AV_NORTH_MINE_HORDE_3    = 13081,
    BG_AV_NORTH_MINE_HORDE_4    = 13079,
    // South_Mine_..._X 4 is always boss
    BG_AV_SOUTH_MINE_NEUTRAL_1  = 11603,
    BG_AV_SOUTH_MINE_NEUTRAL_2  = 11604,
    BG_AV_SOUTH_MINE_NEUTRAL_3  = 11605,
    BG_AV_SOUTH_MINE_NEUTRAL_4  = 11677,
    BG_AV_SOUTH_MINE_NEUTRAL_5  = 10982,
    BG_AV_SOUTH_MINE_ALLIANCE_1 = 13317,
    BG_AV_SOUTH_MINE_ALLIANCE_2 = 13096,
    BG_AV_SOUTH_MINE_ALLIANCE_3 = 13087,
    BG_AV_SOUTH_MINE_ALLIANCE_4 = 13086,
    BG_AV_SOUTH_MINE_HORDE_1    = 13316,
    BG_AV_SOUTH_MINE_HORDE_2    = 13097,
    BG_AV_SOUTH_MINE_HORDE_3    = 13089,
    BG_AV_SOUTH_MINE_HORDE_4    = 13088
};


enum BG_AV_ObjectType
{
	BG_AV_OBJECT_GATE_A			= 0,
	BG_AV_OBJECT_GATE_H			= 1,
	// StormPike
	BG_AV_OBJECT_STORM_NEU_AURA	= 2,
	BG_AV_OBJECT_STORM_H_AURA	= 3,
	BG_AV_OBJECT_STORM_A_AURA	= 4,
	BG_AV_OBJECT_STORM_H_B		= 5,
	BG_AV_OBJECT_STORM_A_B		= 6,
	BG_AV_OBJECT_STORM_H_B_CONT	= 7,
	BG_AV_OBJECT_STORM_A_B_CONT	= 8,
	// Dun Baldar
	BG_AV_OBJECT_DUNB_NEU_AURA	= 9,
	BG_AV_OBJECT_DUNB_H_AURA	= 10,
	BG_AV_OBJECT_DUNB_A_AURA	= 11,
	BG_AV_OBJECT_DUNB_H_B		= 12,
	BG_AV_OBJECT_DUNB_A_B		= 13,
	BG_AV_OBJECT_DUNB_H_B_CONT	= 14,
	BG_AV_OBJECT_DUNB_A_B_CONT	= 15,
	// Gîtepierre
	BG_AV_OBJECT_GIT_NEU_AURA	= 16,
	BG_AV_OBJECT_GIT_H_AURA		= 17,
	BG_AV_OBJECT_GIT_A_AURA		= 18,
	BG_AV_OBJECT_GIT_H_B		= 19,
	BG_AV_OBJECT_GIT_A_B		= 20,
	BG_AV_OBJECT_GIT_H_B_CONT	= 21,
	BG_AV_OBJECT_GIT_A_B_CONT	= 22,
	// Cimetière des neiges
	// Centre
	BG_AV_OBJECT_SWF_NEU_B		= 23,
	BG_AV_OBJECT_SWF_NEU_AURA	= 24,
	BG_AV_OBJECT_SWF_H_AURA		= 25,
	BG_AV_OBJECT_SWF_A_AURA		= 26,
	BG_AV_OBJECT_SWF_H_B		= 27,
	BG_AV_OBJECT_SWF_A_B		= 28,
	BG_AV_OBJECT_SWF_H_B_CONT	= 29,
	BG_AV_OBJECT_SWF_A_B_CONT	= 30,
	// 1 
	BG_AV_OBJECT_SWF_H_B_1		= 31,
	BG_AV_OBJECT_SWF_A_B_1		= 32,
	BG_AV_OBJECT_SWF_H_B_CONT_1	= 33,
	BG_AV_OBJECT_SWF_A_B_CONT_1	= 34,
	//2
	BG_AV_OBJECT_SWF_H_B_2		= 35,
	BG_AV_OBJECT_SWF_A_B_2		= 36,
	BG_AV_OBJECT_SWF_H_B_CONT_2	= 37,
	BG_AV_OBJECT_SWF_A_B_CONT_2	= 38,
	//3
	BG_AV_OBJECT_SWF_H_B_3		= 39,
	BG_AV_OBJECT_SWF_A_B_3		= 40,
	BG_AV_OBJECT_SWF_H_B_CONT_3	= 41,
	BG_AV_OBJECT_SWF_A_B_CONT_3	= 42,

	//4
	BG_AV_OBJECT_SWF_H_B_4		= 44,
	BG_AV_OBJECT_SWF_A_B_4		= 45,
	BG_AV_OBJECT_SWF_H_B_CONT_4	= 46,
	BG_AV_OBJECT_SWF_A_B_CONT_4	= 47,

	// GlaceSang
	BG_AV_OBJECT_GSG_NEU_AURA	= 48,
	BG_AV_OBJECT_GSG_H_AURA		= 49,
	BG_AV_OBJECT_GSG_A_AURA		= 50,
	BG_AV_OBJECT_GSG_H_B		= 51,
	BG_AV_OBJECT_GSG_A_B		= 52,
	BG_AV_OBJECT_GSG_H_B_CONT	= 53,
	BG_AV_OBJECT_GSG_A_B_CONT	= 54,

	// Cimetière Loup de Givre
	BG_AV_OBJECT_FSW_NEU_AURA	= 55,
	BG_AV_OBJECT_FSW_H_AURA		= 56,
	BG_AV_OBJECT_FSW_A_AURA		= 57,
	BG_AV_OBJECT_FSW_H_B		= 58,
	BG_AV_OBJECT_FSW_A_B		= 59,
	BG_AV_OBJECT_FSW_H_B_CONT	= 60,
	BG_AV_OBJECT_FSW_A_B_CONT	= 61,

	// Hutte de Guérison Loup de Givre
	BG_AV_OBJECT_FSH_NEU_AURA	= 62,
	BG_AV_OBJECT_FSH_H_AURA		= 63,
	BG_AV_OBJECT_FSH_A_AURA		= 64,
	BG_AV_OBJECT_FSH_H_B		= 65,
	BG_AV_OBJECT_FSH_A_B		= 66,
	BG_AV_OBJECT_FSH_H_B_CONT	= 67,
	BG_AV_OBJECT_FSH_A_B_CONT	= 68,

	// Dun Baldar North
	BG_AV_OBJECT_DBN_A_B		= 69,
	BG_AV_OBJECT_DBN_H_B		= 70,
	BG_AV_OBJECT_DBN_A_T		= 71,
	BG_AV_OBJECT_DBN_H_T		= 72,
	BG_AV_OBJECT_DBN_N_AURA		= 73,
	BG_AV_OBJECT_DBN_A_AURA		= 74,

	// Dun Baldar South
	BG_AV_OBJECT_DBS_A_B		= 75,
	BG_AV_OBJECT_DBS_H_B		= 76,
	BG_AV_OBJECT_DBS_A_T		= 77,
	BG_AV_OBJECT_DBS_H_T		= 78,
	BG_AV_OBJECT_DBS_N_AURA		= 79,
	BG_AV_OBJECT_DBS_A_AURA		= 80,

	// Aile de Glace
	BG_AV_OBJECT_ADG_A_B		= 81,
	BG_AV_OBJECT_ADG_H_B		= 82,
	BG_AV_OBJECT_ADG_A_T		= 83,
	BG_AV_OBJECT_ADG_H_T		= 84,
	BG_AV_OBJECT_ADG_N_AURA		= 85,
	BG_AV_OBJECT_ADG_A_AURA		= 86,

	// Fortin de Gîtepierre
	BG_AV_OBJECT_FGP_A_B		= 87,
	BG_AV_OBJECT_FGP_H_B		= 88,
	BG_AV_OBJECT_FGP_A_T		= 89,
	BG_AV_OBJECT_FGP_H_T		= 90,
	BG_AV_OBJECT_FGP_N_AURA		= 91,
	BG_AV_OBJECT_FGP_A_AURA		= 92,
	
	// Tour de GlaceSang
	BG_AV_OBJECT_GST_A_B		= 93,
	BG_AV_OBJECT_GST_H_B		= 94,
	BG_AV_OBJECT_GST_A_T		= 95,
	BG_AV_OBJECT_GST_H_T		= 96,
	BG_AV_OBJECT_GST_N_AURA		= 97,
	BG_AV_OBJECT_GST_H_AURA		= 98,

	// Tour de la Halte
	BG_AV_OBJECT_HAL_A_B		= 99,
	BG_AV_OBJECT_HAL_H_B		= 100,
	BG_AV_OBJECT_HAL_A_T		= 101,
	BG_AV_OBJECT_HAL_H_T		= 102,
	BG_AV_OBJECT_HAL_N_AURA		= 103,
	BG_AV_OBJECT_HAL_H_AURA		= 104,
	
	// Tour FrostWolf Ouest
	BG_AV_OBJECT_FWO_A_B		= 105,
	BG_AV_OBJECT_FWO_H_B		= 106,
	BG_AV_OBJECT_FWO_A_T		= 107,
	BG_AV_OBJECT_FWO_H_T		= 108,
	BG_AV_OBJECT_FWO_N_AURA		= 109,
	BG_AV_OBJECT_FWO_H_AURA		= 110,

	// Tour FrostWolf Est
	BG_AV_OBJECT_FWE_A_B		= 111,
	BG_AV_OBJECT_FWE_H_B		= 112,
	BG_AV_OBJECT_FWE_A_T		= 113,
	BG_AV_OBJECT_FWE_H_T		= 114,
	BG_AV_OBJECT_FWE_N_AURA		= 115,
	BG_AV_OBJECT_FWE_H_AURA		= 116,

	// Flammes
	BG_AV_OBJECT_DBS_F			= 256,
	BG_AV_OBJECT_DBN_F			= 264,
	BG_AV_OBJECT_ADG_F			= 272,
	BG_AV_OBJECT_FGP_F			= 280,
	BG_AV_OBJECT_GST_F			= 288,
	BG_AV_OBJECT_HAL_F			= 296,
	BG_AV_OBJECT_FWE_F			= 304,

	BG_AV_OBJECT_GGS_F			= 350,
	BG_AV_OBJECT_FGT_F			= 360,

	BG_AV_OBJECT_MAX			= 400,
};


// Liste des ID de son a jouer
enum BG_AV_Sounds
{
    BG_AV_SOUND_ALLIANCE_ASSAULTS       = 8212,             // tower,grave + enemy boss if someone tries to attack him
    BG_AV_SOUND_HORDE_ASSAULTS          = 8174,
    BG_AV_SOUND_NEAR_LOSE               = 8456,
	BG_AV_SOUND_ALLIANCE_GOOD			= 8173, 
    BG_AV_SOUND_HORDE_GOOD				= 8213,
	BG_AV_SOUND_BOTH_TOWER_DEFEND       = 8192,

};

struct BG_AV_NodeInfo
{
    uint32       TotalOwner;
    uint32       Owner;
    uint32       PrevOwner;
    BG_AV_States State;
    BG_AV_States PrevState;
    int          Timer;
    bool         Tower;
};

	// { ALLIANCE, ALLIANCE_EN_ATTAQUE, HORDE, HORDE_EN_ATTAQUE }

const uint32 BG_AV_NodeWorldStates[BG_AV_NODES_MAX][4] = {
	// Stormpike first aid station
    {1325,1326,1327,1328},
	// Stormpike Graveyard
    {1333,1335,1334,1336},
    // Stoneheart Grave
    {1302,1304,1301,1303},
    // Snowfall Grave
    {1341,1343,1342,1344},
    // Iceblood grave
    {1346,1348,1347,1349},
    // Frostwolf Grave
    {1337,1339,1338,1340},
    // Frostwolf Hut
    {1329,1331,1330,1332},
    // Dunbaldar South Bunker
    {1361,1375,1370,1378},
    // Dunbaldar North Bunker
    {1362,1374,1371,1379},
    // Icewing Bunker
    {1363,1376,1372,1380},
    // Stoneheart Bunker
    {1364,1377,1373,1381},
    // Iceblood Tower
    {1368,1390,1385,1395},
    // Tower Point
    {1367,1389,1384,1394},
    // Frostwolf East
    {1366,1388,1383,1393},
    // Frostwolf West
    {1365,1387,1382,1392},
};

enum BG_AV_Nodes
{
    BG_AV_NODES_FIRSTAID_STATION = 0,
    BG_AV_NODES_STORMPIKE_GRAVE = 1,
    BG_AV_NODES_STONEHEART_GRAVE = 2,
    BG_AV_NODES_SNOWFALL_GRAVE = 3,
    BG_AV_NODES_ICEBLOOD_GRAVE = 4,
    BG_AV_NODES_FROSTWOLF_GRAVE = 5,
    BG_AV_NODES_FROSTWOLF_HUT = 6,
    BG_AV_NODES_DUNBALDAR_SOUTH = 7,
    BG_AV_NODES_DUNBALDAR_NORTH = 8,
    BG_AV_NODES_ICEWING_BUNKER = 9,
    BG_AV_NODES_STONEHEART_BUNKER = 10,
    BG_AV_NODES_ICEBLOOD_TOWER = 11,
    BG_AV_NODES_TOWER_POINT = 12,
    BG_AV_NODES_FROSTWOLF_ETOWER = 13,
    BG_AV_NODES_FROSTWOLF_WTOWER = 14,
    BG_AV_NODES_ERROR = 255,
};

enum BG_AV_Nodes_State
{
    BG_AV_NODE_TYPE_NEUTRAL             = 0,
    BG_AV_NODE_STATUS_ALLY_CONTESTED    = 1,
    BG_AV_NODE_STATUS_HORDE_CONTESTED   = 2,
    BG_AV_NODE_STATUS_ALLY_OCCUPIED     = 3,
    BG_AV_NODE_STATUS_HORDE_OCCUPIED    = 4,
	BG_AV_NODE_STATUS_DESTROYED			= 5,
};

// { ALLIANCE, NEUTRE , HORDE }
const uint32 BG_AV_MineWorldStates[2][3] = {
    {1358, 1360,1359},
    {1355, 1357,1356}
};

enum BG_AV_WorldStates
{
    //Point capturables
    AV_SNOWFALL_N               = 1966,

    //Scores d'équipe
    BG_AV_Alliance_Score        = 3127,
    BG_AV_Horde_Score           = 3128,
    BG_AV_SHOW_H_SCORE          = 3133,
    BG_AV_SHOW_A_SCORE          = 3134

};

enum BG_AV_Graveyards
{
    BG_AV_GRAVE_STORM_AID          = 751,
    BG_AV_GRAVE_STORM_GRAVE        = 689,
    BG_AV_GRAVE_STONE_GRAVE        = 729,
    BG_AV_GRAVE_SNOWFALL           = 169,
    BG_AV_GRAVE_ICE_GRAVE          = 749,
    BG_AV_GRAVE_FROSTWOLF          = 690,
    BG_AV_GRAVE_FROST_HUT          = 750,
    BG_AV_GRAVE_MAIN_ALLIANCE      = 611,
    BG_AV_GRAVE_MAIN_HORDE         = 610
};

const uint32 BG_AV_GraveyardIds[9]= {
    BG_AV_GRAVE_STORM_AID,
    BG_AV_GRAVE_STORM_GRAVE,
    BG_AV_GRAVE_STONE_GRAVE,
    BG_AV_GRAVE_SNOWFALL,
    BG_AV_GRAVE_ICE_GRAVE,
    BG_AV_GRAVE_FROSTWOLF,
    BG_AV_GRAVE_FROST_HUT,
    BG_AV_GRAVE_MAIN_ALLIANCE,
    BG_AV_GRAVE_MAIN_HORDE
};

enum BG_AV_QuestIds
{
    BG_AV_QUEST_A_SCRAPS1      = 7223, //first quest
    BG_AV_QUEST_A_SCRAPS2      = 6781, //repeatable
    BG_AV_QUEST_H_SCRAPS1      = 7224,
    BG_AV_QUEST_H_SCRAPS2      = 6741,
    BG_AV_QUEST_A_COMMANDER1   = 6942, //soldier
    BG_AV_QUEST_H_COMMANDER1   = 6825,
    BG_AV_QUEST_A_COMMANDER2   = 6941, //leutnant
    BG_AV_QUEST_H_COMMANDER2   = 6826,
    BG_AV_QUEST_A_COMMANDER3   = 6943, //commander
    BG_AV_QUEST_H_COMMANDER3   = 6827,
    BG_AV_QUEST_A_BOSS1        = 7386, // 5 cristal/blood
    BG_AV_QUEST_H_BOSS1        = 7385,
    BG_AV_QUEST_A_BOSS2        = 6881, // 1
    BG_AV_QUEST_H_BOSS2        = 6801,
    BG_AV_QUEST_A_NEAR_MINE    = 5892, //the mine near start location of team
    BG_AV_QUEST_H_NEAR_MINE    = 5893,
    BG_AV_QUEST_A_OTHER_MINE   = 6982, //the other mine ;)
    BG_AV_QUEST_H_OTHER_MINE   = 6985,
    BG_AV_QUEST_A_RIDER_HIDE   = 7026,
    BG_AV_QUEST_H_RIDER_HIDE   = 7002,
    BG_AV_QUEST_A_RIDER_TAME   = 7027,
    BG_AV_QUEST_H_RIDER_TAME   = 7001
};

enum BG_AV_OTHER_VALUES
{
    BG_AV_NORTH_MINE                = 0,
    BG_AV_SOUTH_MINE                = 1,
    BG_AV_MINE_TICK_TIMER           = 45000,
    BG_AV_CAPTAIN_BUFF_TICK_TIMER   = 180000,  // TODO : remettre la bone valeur
    BG_AV_MINE_RECLAIM_TIMER        = 1200000,                  // TODO: get the right value.. this is currently 20 minutes
    BG_AV_NEUTRAL_TEAM              = 0                         // this is the neutral owner of snowfall
};

enum BG_AV_DB_Creatures
{
    BG_AV_CREATURE_HERALD      = 1,
    BG_AV_CREATURE_A_BOSS      = 2,
    BG_AV_CREATURE_H_BOSS      = 3,
    BG_AV_CREATURE_SNIFFLE     = 4,
    BG_AV_CREATURE_MARSHAL     = 5,                         // 4alliance marshals + 4 horde marshals their ids are similar to those of the tower - ids (only here)
    BG_AV_DB_CREATURE_MAX      = 30
};
class BattleGroundAVScore : public BattleGroundScore
{
    public:
        BattleGroundAVScore() : GraveyardsAssaulted(0), GraveyardsDefended(0), TowersAssaulted(0), TowersDefended(0), MinesCaptured(0), LeadersKilled(0), SecondaryObjectives(0) {};
        virtual ~BattleGroundAVScore() {};
        uint32 GraveyardsAssaulted;
        uint32 GraveyardsDefended;
        uint32 TowersAssaulted;
        uint32 TowersDefended;
        uint32 MinesCaptured;
        uint32 LeadersKilled;
        uint32 SecondaryObjectives;
};
 
class BattleGroundAV : public BattleGround
{
    friend class BattleGroundMgr;
 
    public:
        BattleGroundAV();
        ~BattleGroundAV();
        void Update(uint32 diff);
		void UpdateScore(BattleGroundTeamId team, int32 points );	
        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player *plr);
        virtual void OnObjectDBLoad(Creature* creature);
        virtual void OnCreatureRespawn(Creature* creature);

        virtual void StartingEventCloseDoors();
        virtual void StartingEventOpenDoors();
        virtual void FillInitialWorldStates(WorldPacket& data);
		virtual bool SetupBattleGround();
        void RemovePlayer(Player *plr,uint64 guid);
        void HandleAreaTrigger(Player *Source, uint32 Trigger);
		virtual void EventPlayerClickedOnFlag(Player *Source, GameObject* target_obj);
        /* Scorekeeping */
        void UpdatePlayerScore(Player *Source, uint32 type, uint32 value);
		void HandleKillPlayer(Player *player, Player *killer);
		// repop aux cimetières
		virtual WorldSafeLocsEntry const* GetClosestGraveYard(Player *plr);
		// mobs
		virtual void HandleKillUnit(Creature *unit, Player *killer);
        virtual void HandleQuestComplete(uint32 questid, Player *player);
        bool PlayerCanDoMineQuest(int32 GOId,uint32 team);

    private:
		/* Variables */
		uint64 m_DB_Creature[BG_AV_DB_CREATURE_MAX];
		BGCreatures m_MineCreatures[2][3];
		BGCreatures m_GraveCreatures[7][BG_TEAMS_COUNT][4];
		uint32 GetNodeName(BG_AV_Nodes node);
		const BG_AV_Nodes GetNodeThroughCreaturePosition(Creature * cr);
		bool BG_CentralGrave_FirstAttack;
        // Score de renforts actuel
        int32 m_TeamScores[BG_TEAMS_COUNT];
        // Si le chef est encore en vie ou non
        bool m_captainAlive[BG_TEAMS_COUNT];
        // Si informé ou non qu'il est près de la défaite
        bool m_IsInformedNearLose[BG_TEAMS_COUNT];

		BG_AV_NodeInfo m_Nodes[BG_AV_NODES_MAX];
		BG_AV_Nodes m_assault_in_progress;
		const bool IsTower(BG_AV_Nodes node) { return (node == BG_AV_NODES_ERROR)? false : m_Nodes[node].Tower; }
        const bool IsGrave(BG_AV_Nodes node) { return (node == BG_AV_NODES_ERROR)? false : !m_Nodes[node].Tower; }
		/* fonctions */
		// obtenir le statut actuel d'un point
        uint8 GetWorldStateType(uint8 state, uint32 team) const;
		void PopulateNode(BG_AV_Nodes node, Player* plr);
        void AssaultNode(BG_AV_Nodes node, uint32 team);
		void DepopulateNode(BG_AV_Nodes node);
        void DestroyNode(BG_AV_Nodes node);
        void InitNode(BG_AV_Nodes node, uint32 team, bool tower);
		void DefendNode(BG_AV_Nodes node, uint32 team);
		void EventPlayerAssaultsPoint(Player* player);
		void EventPlayerDefendsPoint(Player* player);
		const BG_AV_Nodes GetNodeThroughPlayerPosition(Player* plr);
		void SpawnNode(uint32 oldWS, uint32 newWS, uint32 aura, uint32 banner, uint32 banner2);
		void SpawnNodeEnd(BG_AV_Nodes node);
		void SendMessageEventDestroyPoint(BG_AV_Nodes node);
		void SpawnFlammes(BG_AV_Nodes node);
		void MapCaptureGrave(BG_AV_Nodes node);
		void SpawnCapturedGrave(BG_AV_Nodes node, uint32 owner);
		void SpawnForAlliance(BG_AV_Nodes node);
		void SpawnForHorde(BG_AV_Nodes node);
		void MapDestroyedTower(BG_AV_Nodes node);
		void ChangeMineOwner(uint8 mine, uint32 team);
        void ModifySpawnMode(BG_AV_Nodes node, uint32 respawn, uint32 team);

		/* les Mines */
        // envoi du packet de statut
        void SendMineWorldStates(uint32 mine);
        // Le propriétaire
        uint32 m_Mine_Owner[BG_TEAMS_COUNT];
        // L'ancien propriétaire
        uint32 m_Mine_PrevOwner[BG_TEAMS_COUNT];            // only for worldstates needed
        // le timer
        int32 m_Mine_Timer;                                 // one timer ticks for both teams
        uint32 m_Mine_Reclaim_Timer[BG_TEAMS_COUNT];
		uint32 m_Team_QuestStatus[BG_TEAMS_COUNT][9];
		struct BG_AV_NodeObjects
        {
            BGObjects gameobjects;
            BGCreatures creatures;
        };
		BG_AV_NodeObjects m_NodeObjects[BG_AV_MAX_NODE_EVENTS];
		int32 team_captain_buff_Timer;
		void SpawnBGCreature(uint64 const& guid, uint32 respawntime);
        
};
#endif