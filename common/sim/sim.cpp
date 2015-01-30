#include "sim.h"
#include "../script/console.h"
#include "infrastructure.h"
#include "road.h"
#include "building.h"
#include "map.h"
#include "../path/pathnode.h"
#include "../math/isomath.h"

long long g_simframe = 0;

// Queue all the game resources and define objects
void Queue()
{
	// Cursor types

#if 0
#define CU_NONE		0	//cursor off?
#define CU_DEFAULT	1
#define CU_MOVE		2	//move window
#define CU_RESZL	3	//resize width (horizontal) from left side
#define CU_RESZR	4	//resize width (horizontal) from right side
#define CU_RESZT	5	//resize height (vertical) from top side
#define CU_RESZB	6	//resize height (vertical) from bottom side
#define CU_RESZTL	7	//resize top left corner
#define CU_RESZTR	8	//resize top right corner
#define CU_RESZBL	9	//resize bottom left corner
#define CU_RESZBR	10	//resize bottom right corner
#define CU_WAIT		11	//shows a hourglass?
#define CU_DRAG		12	//drag some object between widgets?
#define CU_STATES	13
#endif

	DefS("gui/transp.png", &g_cursor[CU_NONE], 0, 0);
	DefS("gui/cursors/default.png", &g_cursor[CU_DEFAULT], 0, 0);
	DefS("gui/cursors/move.png", &g_cursor[CU_MOVE], 16, 16);
	DefS("gui/cursors/reszh.png", &g_cursor[CU_RESZL], 16, 16);
	DefS("gui/cursors/reszh.png", &g_cursor[CU_RESZR], 16, 16);
	DefS("gui/cursors/reszv.png", &g_cursor[CU_RESZT], 16, 16);
	DefS("gui/cursors/reszv.png", &g_cursor[CU_RESZB], 16, 16);
	DefS("gui/cursors/reszd2.png", &g_cursor[CU_RESZTL], 16, 16);
	DefS("gui/cursors/reszd1.png", &g_cursor[CU_RESZTR], 16, 16);
	DefS("gui/cursors/reszd1.png", &g_cursor[CU_RESZBL], 16, 16);
	DefS("gui/cursors/reszd2.png", &g_cursor[CU_RESZBR], 16, 16);
	DefS("gui/cursors/default.png", &g_cursor[CU_WAIT], 16, 16);
	DefS("gui/cursors/default.png", &g_cursor[CU_DRAG], 16, 16);


	// Icons

	g_log<<"DefI"<<std::endl;
	g_log.flush();

#if 0
#define ICON_DOLLARS		0
#define ICON_PESOS			1
#define ICON_EUROS			2
#define ICON_POUNDS			3
#define ICON_FRANCS			4
#define ICON_YENS			5
#define ICON_RUPEES			6
#define ICON_ROUBLES		7
#define ICON_LABOUR			8
#define ICON_HOUSING		9
#define ICON_FARMPRODUCT	10
#define ICON_FOOD			11
#define ICON_CHEMICALS		12
#define ICON_ELECTRONICS	13
#define ICON_RESEARCH		14
#define ICON_PRODUCTION		15
#define ICON_IRONORE		16
#define ICON_URANIUMORE		17
#define ICON_STEEL			18
#define ICON_CRUDEOIL		19
#define ICON_WSFUEL			20
#define ICON_STONE			21
#define ICON_CEMENT			22
#define ICON_ENERGY			23
#define ICON_ENRICHEDURAN	24
#define ICON_COAL			25
#define ICON_TIME			26
#define ICON_RETFUEL		27
#define ICON_LOGS			28
#define ICON_LUMBER			29
#define ICON_WATER			30
#define ICONS				31
#endif

	DefI(ICON_DOLLARS, "gui/icons/dollars.png", ":dollar:");
	DefI(ICON_PESOS, "gui/icons/pesos.png", ":peso:");
	DefI(ICON_EUROS, "gui/icons/euros.png", ":euro:");
	DefI(ICON_POUNDS, "gui/icons/pounds.png", ":pound:");
	DefI(ICON_FRANCS, "gui/icons/francs.png", ":franc:");
	DefI(ICON_YENS, "gui/icons/yens.png", ":yen:");
	DefI(ICON_RUPEES, "gui/icons/rupees.png", ":rupee:");
	DefI(ICON_ROUBLES, "gui/icons/roubles.png", ":rouble:");
	DefI(ICON_LABOUR, "gui/icons/labour.png", ":labour:");
	DefI(ICON_HOUSING, "gui/icons/housing.png", ":housing:");
	DefI(ICON_FARMPRODUCT, "gui/icons/farmproducts.png", ":farmprod:");
	DefI(ICON_WSFOOD, "gui/icons/wsfood.png", ":wsfood:");
	DefI(ICON_RETFOOD, "gui/icons/retfood.png", ":retfood:");
	DefI(ICON_CHEMICALS, "gui/icons/chemicals.png", ":chemicals:");
	DefI(ICON_ELECTRONICS, "gui/icons/electronics.png", ":electronics:");
	DefI(ICON_RESEARCH, "gui/icons/research.png", ":research:");
	DefI(ICON_PRODUCTION, "gui/icons/production.png", ":production:");
	DefI(ICON_IRONORE, "gui/icons/ironore.png", ":ironore:");
	DefI(ICON_URANIUMORE, "gui/icons/uraniumore.png", ":uraniumore:");
	DefI(ICON_STEEL, "gui/icons/steel.png", ":steel:");
	DefI(ICON_CRUDEOIL, "gui/icons/crudeoil.png", ":crudeoil:");
	DefI(ICON_WSFUEL, "gui/icons/fuelwholesale.png", ":wsfuel:");
	DefI(ICON_STONE, "gui/icons/stone.png", ":stone:");
	DefI(ICON_CEMENT, "gui/icons/cement.png", ":cement:");
	DefI(ICON_ENERGY, "gui/icons/energy.png", ":energy:");
	DefI(ICON_ENRICHEDURAN, "gui/icons/uranium.png", ":enricheduran:");
	DefI(ICON_COAL, "gui/icons/coal.png", ":coal:");
	DefI(ICON_TIME, "gui/icons/time.png", ":time:");
	DefI(ICON_RETFUEL, "gui/icons/fuelretail.png", ":retfuel:");
	DefI(ICON_LOGS, "gui/icons/logs.png", ":logs:");
	DefI(ICON_LUMBER, "gui/icons/lumber.png", ":lumber:");
	DefI(ICON_WATER, "gui/icons/water.png", ":water:");
	DefI(ICON_EXCLAMATION, "gui/icons/exclamation.png", ":exclam:");


	// Resource types

	g_log<<"DefR"<<std::endl;
	g_log.flush();

#if 0
#define RES_NONE			-1
#define RES_FUNDS			0
#define RES_LABOUR			1
#define RES_HOUSING			2
#define RES_FARMPRODUCTS	3
#define RES_RETFOOD			4
#define RES_PRODUCTION		5
#define RES_MINERALS		6
#define RES_CRUDEOIL		7
#define RES_WSFUEL			8
#define RES_RETFUEL			9
#define RES_ENERGY			10
#define RES_URANIUM			11
#define RESOURCES			12
#endif

#if 0
	void DefR(int resi, const char* n, const char* depn, int iconindex, bool phys, bool cap, bool glob, float r, float g, float b, float a)
#endif

	DefR(RES_FUNDS,			"Funds",				"",							ICON_DOLLARS,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_NONE);
	DefR(RES_LABOUR,		"Labour",				"",							ICON_LABOUR,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_NONE);
	DefR(RES_HOUSING,		"Housing",				"",							ICON_HOUSING,		true,	true,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_FARMPRODUCTS,	"Farm Products",		"Fertile",					ICON_FARMPRODUCT,	true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_PRODUCTION,	"Production",			"",							ICON_PRODUCTION,	true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_RETFOOD,		"Retail Food",			"",							ICON_RETFOOD,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_MINERALS,		"Minerals",				"Iron Ore Deposit",			ICON_IRONORE,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_CRUDEOIL,		"Crude Oil",			"Oil Deposit",				ICON_CRUDEOIL,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_CRPIPE);
	DefR(RES_WSFUEL,		"Wholesale Fuel",		"",							ICON_WSFUEL,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_RETFUEL,		"Retail Fuel",			"",							ICON_RETFUEL,		true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);
	DefR(RES_ENERGY,		"Energy",				"",							ICON_ENERGY,		true,	true,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_POWL);
	DefR(RES_URANIUM,		"Uranium",				"",							ICON_ENRICHEDURAN,	true,	false,	false,	1.0f,1.0f,1.0f,1.0f,	CONDUIT_ROAD);


	// Various environment textures

	QueueTexture(&g_circle, "gui/circle.png", true, true);

	LoadParticles();
	//LoadSkyBox("textures/terrain/default/skydome");

	// Players

#if 1
	//QueueModel(&g_playerm, "models/brain/brain.ms3d", Vec3f(50, 50, 50), Vec3f(0,0,0), true);

	for(int i=0; i<PLAYERS; i++)
	{
		Player* p = &g_player[i];
		PlayerColor* pyc = &g_pycols[i];

		char name[64];
		sprintf(name, "Player %d (%s)", i, pyc->name);

		DefP(i, pyc->color[0]/255.0f, pyc->color[1]/255.0f, pyc->color[2]/255.0f, 1, RichText(name));

		SubmitConsole(&p->name);
	}
#endif

	// Unit types

	//DefU(UNIT_ROBOSOLDIER, "units/lab00/lab00", Vec3i(125, 250, 125), "Robot Soldier", 100, true, true, false, false, false, 6, true);
	DefU(UNIT_ROBOSOLDIER, "foliage/spruce1/spruce1", Vec3i(125, 250, 125), "Robot Soldier", 100, true, true, false, false, false, 6, true);
	//DefU(UNIT_LABOURER, "units/lab00/lab00", Vec3i(125, 250, 125), "Labourer", 100, true, true, false, false, false, 6, false);
	DefU(UNIT_LABOURER, "units/lab00/lab00", Vec3i(50, 250, 50), "Labourer", 100, true, true, false, false, false, 6, false);
	DefU(UNIT_TRUCK, "", Vec3i(125, 250, 125), "Truck", 100, true, false, true, false, false, 30, false);


	// Foliage types

	DefF(FOLIAGE_TREE1, "models/pine/pine.ms3d", Vec3f(200,200,200), Vec3f(0,0,0), Vec3i(40, 60, 500)*20);
	DefF(FOLIAGE_TREE2, "models/pine/pine.ms3d", Vec3f(200,200,200), Vec3f(0,0,0), Vec3i(40, 60, 500)*20);
	DefF(FOLIAGE_TREE3, "models/pine/pine.ms3d", Vec3f(200,200,200), Vec3f(0,0,0), Vec3i(40, 60, 500)*20);


	// Building types

	//DefB(BL_APARTMENT, "Apartment Building", Vec2i(2,1), "models/apartment1/basebuilding.ms3d", Vec3f(100,100,100), Vec3f(0,0,0), "models/apartment1/basebuilding.ms3d", Vec3f(100,100,100), Vec3f(0,0,0), FOUNDATION_LAND, RES_NONE);
	DefB(BL_APARTMENT, "Apartment Building", Vec2i(1,1),  false, "buildings/apt00/a", FOUNDATION_LAND, RES_NONE);
	BConMat(BL_APARTMENT, RES_MINERALS, 5);
	BConMat(BL_APARTMENT, RES_LABOUR, 50);
	//BInput(BL_APARTMENT, RES_ENERGY, 50);
	BOutput(BL_APARTMENT, RES_HOUSING, 15);
	//BOutput(BL_APARTMENT, RES_HOUSING, 50);
	BDesc(BL_APARTMENT, "Apartments collect rent from labourers. They are required by the labourers to regenerate labour power.");

	DefB(BL_HOUSE1, "House 1", Vec2i(1,1),  false, "buildings/house1/house1", FOUNDATION_LAND, RES_NONE);
	BConMat(BL_HOUSE1, RES_MINERALS, 5);
	BConMat(BL_HOUSE1, RES_LABOUR, 50);
	BOutput(BL_HOUSE1, RES_HOUSING, 15);
	BDesc(BL_HOUSE1, "Apartments collect rent from labourers. They are required by the labourers to regenerate labour power.");

	DefB(BL_CHEMPL, "Chemical Plant", Vec2i(1,1),  false, "buildings/chempl/chempl", FOUNDATION_LAND, RES_NONE);
	DefB(BL_IRONSM, "Iron Smelter", Vec2i(1,1),  false, "buildings/ironsm/ironsm", FOUNDATION_LAND, RES_NONE);

	DefB(BL_FACTORY, "Factory", Vec2i(1,1),  false, "buildings/apt00/a", FOUNDATION_LAND, RES_NONE);
	BConMat(BL_FACTORY, RES_MINERALS, 5);
	BConMat(BL_FACTORY, RES_LABOUR, 10);
	BInput(BL_FACTORY, RES_ENERGY, 50);
	BOutput(BL_FACTORY, RES_PRODUCTION, 5);
	BDesc(BL_FACTORY, "Factories produce units. They generate production necessary for the processing and packaging of farm products to create retail food.");

	DefB(BL_REFINERY, "Oil Refinery", Vec2i(1,1),  false, "buildings/oilref/oilref", FOUNDATION_LAND, RES_NONE);
	BConMat(BL_REFINERY, RES_MINERALS, 5);
	BConMat(BL_REFINERY, RES_LABOUR, 10);
	BInput(BL_REFINERY, RES_ENERGY, 50);
	BInput(BL_REFINERY, RES_CRUDEOIL, 5);
	BOutput(BL_REFINERY, RES_WSFUEL, 5);
	BEmitter(BL_REFINERY, 0, PARTICLE_EXHAUST, Vec3f(TILE_SIZE*5.7/10, TILE_SIZE*3/2, TILE_SIZE*-5/10));
	BEmitter(BL_REFINERY, 1, PARTICLE_EXHAUST2, Vec3f(TILE_SIZE*5.7/10, TILE_SIZE*3/2, TILE_SIZE*-5/10));
	BEmitter(BL_REFINERY, 2, PARTICLE_EXHAUST, Vec3f(TILE_SIZE*-4.5/10, TILE_SIZE*1.75, TILE_SIZE*3.0f/10));
	BEmitter(BL_REFINERY, 3, PARTICLE_EXHAUST2, Vec3f(TILE_SIZE*-4.5/10, TILE_SIZE*1.75, TILE_SIZE*3.0f/10));
	BDesc(BL_REFINERY, "Turn crude oil into wholesale fuel. Must be distributed at gas stations.");

	DefB(BL_NUCPOW, "Nuclear Powerplant", Vec2i(2,2), false, "buildings/nucpow3/nucpow3", FOUNDATION_LAND, RES_NONE);
	BConMat(BL_NUCPOW, RES_MINERALS, 5);
	BConMat(BL_NUCPOW, RES_LABOUR, 10);
	BInput(BL_NUCPOW, RES_URANIUM, 5);
	BOutput(BL_NUCPOW, RES_ENERGY, 1000);
	BEmitter(BL_NUCPOW, 0, PARTICLE_EXHAUSTBIG, Vec3f(TILE_SIZE*-0.63f, TILE_SIZE*1.5f, TILE_SIZE*0));
	BEmitter(BL_NUCPOW, 1, PARTICLE_EXHAUSTBIG, Vec3f(TILE_SIZE*0.17f, TILE_SIZE*1.5f, TILE_SIZE*-0.64f));
	BDesc(BL_NUCPOW, "Generates electricity from uranium.");

	DefB(BL_FARM, "Farm", Vec2i(4,2), true, "buildings/apt00/a", FOUNDATION_LAND, RES_NONE);
	BConMat(BL_FARM, RES_MINERALS, 5);
	BConMat(BL_FARM, RES_LABOUR, 10);
	BInput(BL_FARM, RES_ENERGY, 50);
	BOutput(BL_FARM, RES_FARMPRODUCTS, 1900);
	BDesc(BL_FARM, "Produces farm products, necessary for the production of retail food.");

	DefB(BL_STORE, "Store", Vec2i(2,1), true, "buildings/apt00/a", FOUNDATION_LAND, RES_NONE);
	BConMat(BL_STORE, RES_MINERALS, 5);
	BConMat(BL_STORE, RES_LABOUR, 10);
	BInput(BL_STORE, RES_ENERGY, 50);
	//BInput(BL_STORE, RES_FARMPRODUCTS, 3600);
	BInput(BL_STORE, RES_FARMPRODUCTS, 3600);
	BInput(BL_STORE, RES_PRODUCTION, 5);
	//BOutput(BL_STORE, RES_RETFOOD, 3600);
	BOutput(BL_STORE, RES_RETFOOD, 3600);
	BDesc(BL_STORE, "Generates retail food from farm products and production, necessary for labourers to survive and multiply.");

	DefB(BL_HARBOUR, "Harbour", Vec2i(2,2), false, "buildings/apt00/a", FOUNDATION_COASTAL, RES_NONE);
	BConMat(BL_HARBOUR, RES_MINERALS, 5);
	BConMat(BL_HARBOUR, RES_LABOUR, 10);
	BInput(BL_HARBOUR, RES_ENERGY, 50);
	BDesc(BL_HARBOUR, "Produces sea units. Necessary for transport between sea and roads.");

	DefB(BL_OILWELL, "Oil Well", Vec2i(1,1), false, "buildings/apt00/a", FOUNDATION_LAND, RES_CRUDEOIL);
	BConMat(BL_OILWELL, RES_MINERALS, 5);
	BConMat(BL_OILWELL, RES_LABOUR, 10);
	BInput(BL_OILWELL, RES_ENERGY, 50);
	BOutput(BL_OILWELL, RES_CRUDEOIL, 5);
	BDesc(BL_OILWELL, "Pumps up crude oil, necessary for fuel, which is consumed by all road vehicles.");

	DefB(BL_SHMINE, "Mine", Vec2i(1,1), false, "buildings/shmine/shmine", FOUNDATION_LAND, -1);
	BConMat(BL_SHMINE, RES_MINERALS, 5);
	BConMat(BL_SHMINE, RES_LABOUR, 10);
	BInput(BL_SHMINE, RES_ENERGY, 50);
	BOutput(BL_SHMINE, RES_MINERALS, 500);
	BOutput(BL_SHMINE, RES_URANIUM, 10);
	BDesc(BL_SHMINE, "Digs up minerals necessary for production at factories, and uranium, necessary for electricity generation at nuclear powerplants.");

	DefB(BL_GASSTATION, "Gas Station", Vec2i(1,1), true, "buildings/apt00/a", FOUNDATION_LAND, RES_NONE);
	BConMat(BL_GASSTATION, RES_MINERALS, 5);
	BConMat(BL_GASSTATION, RES_LABOUR, 10);
	BInput(BL_GASSTATION, RES_ENERGY, 50);
	BInput(BL_GASSTATION, RES_WSFUEL, 5);
	BOutput(BL_GASSTATION, RES_RETFUEL, 5);
	BDesc(BL_GASSTATION, "Gas stations get fuel from a central oil refinery and dispense it to road vehicles when they need it.");


	// Conduit types

	Vec3f scale(TILE_SIZE/16.0f, TILE_SIZE/16.0f, TILE_SIZE/16.0f);
	Vec3f trans(0,0,0);

	DefCo(CONDUIT_ROAD, offsetof(Building,roadnetw), offsetof(Selection,roads), ROAD_MAX_FOREW_INCLINE, ROAD_MAX_SIDEW_INCLINE, false, false, Vec2i(TILE_SIZE/2, TILE_SIZE/2), Vec3f(TILE_SIZE/2, 0, TILE_SIZE/2));
	CoConMat(CONDUIT_ROAD, RES_LABOUR, 1);
	CoConMat(CONDUIT_ROAD, RES_MINERALS, 1);
	DefConn(CONDUIT_ROAD, CONNECTION_NOCONNECTION, CONSTRUCTION, "models/road/1_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTH, CONSTRUCTION, "models/road/n_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EAST, CONSTRUCTION, "models/road/e_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_SOUTH, CONSTRUCTION, "models/road/s_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_WEST, CONSTRUCTION, "models/road/w_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEAST, CONSTRUCTION, "models/road/ne_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHSOUTH, CONSTRUCTION, "models/road/ns_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EASTSOUTH, CONSTRUCTION, "models/road/es_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHWEST, CONSTRUCTION, "models/road/nw_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EASTWEST, CONSTRUCTION, "models/road/ew_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_SOUTHWEST, CONSTRUCTION, "models/road/sw_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EASTSOUTHWEST, CONSTRUCTION, "models/road/esw_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHSOUTHWEST, CONSTRUCTION, "models/road/nsw_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEASTWEST, CONSTRUCTION, "models/road/new_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEASTSOUTH, CONSTRUCTION, "models/road/nes_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEASTSOUTHWEST, CONSTRUCTION, "models/road/nesw_c.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NOCONNECTION, FINISHED, "models/road/1.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTH, FINISHED, "models/road/n.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EAST, FINISHED, "models/road/e.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_SOUTH, FINISHED, "models/road/s.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_WEST, FINISHED, "models/road/w.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEAST, FINISHED, "models/road/ne.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHSOUTH, FINISHED, "models/road/ns.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EASTSOUTH, FINISHED, "models/road/es.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHWEST, FINISHED, "models/road/nw.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EASTWEST, FINISHED, "models/road/ew.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_SOUTHWEST, FINISHED, "models/road/sw.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_EASTSOUTHWEST, FINISHED, "models/road/esw.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHSOUTHWEST, FINISHED, "models/road/nsw.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEASTWEST, FINISHED, "models/road/new.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEASTSOUTH, FINISHED, "models/road/nes.ms3d", scale, trans);
	DefConn(CONDUIT_ROAD, CONNECTION_NORTHEASTSOUTHWEST, FINISHED, "models/road/nesw.ms3d", scale, trans);

	DefCo(CONDUIT_POWL, offsetof(Building,pownetw), offsetof(Selection,powls), TILE_SIZE*2, TILE_SIZE*2, true, true, Vec2i(0, 0), Vec3f(0, 0, 0));
	CoConMat(CONDUIT_POWL, RES_LABOUR, 1);
	CoConMat(CONDUIT_POWL, RES_MINERALS, 1);
	DefConn(CONDUIT_POWL, CONNECTION_NOCONNECTION, CONSTRUCTION, "models/powl/1_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTH, CONSTRUCTION, "models/powl/n_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EAST, CONSTRUCTION, "models/powl/e_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_SOUTH, CONSTRUCTION, "models/powl/s_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_WEST, CONSTRUCTION, "models/powl/w_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEAST, CONSTRUCTION, "models/powl/ne_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHSOUTH, CONSTRUCTION, "models/powl/ns_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EASTSOUTH, CONSTRUCTION, "models/powl/es_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHWEST, CONSTRUCTION, "models/powl/nw_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EASTWEST, CONSTRUCTION, "models/powl/ew_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_SOUTHWEST, CONSTRUCTION, "models/powl/sw_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EASTSOUTHWEST, CONSTRUCTION, "models/powl/esw_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHSOUTHWEST, CONSTRUCTION, "models/powl/nsw_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEASTWEST, CONSTRUCTION, "models/powl/new_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEASTSOUTH, CONSTRUCTION, "models/powl/nes_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEASTSOUTHWEST, CONSTRUCTION, "models/powl/nesw_c.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NOCONNECTION, FINISHED, "models/powl/1.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTH, FINISHED, "models/powl/n.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EAST, FINISHED, "models/powl/e.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_SOUTH, FINISHED, "models/powl/s.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_WEST, FINISHED, "models/powl/w.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEAST, FINISHED, "models/powl/ne.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHSOUTH, FINISHED, "models/powl/ns.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EASTSOUTH, FINISHED, "models/powl/es.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHWEST, FINISHED, "models/powl/nw.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EASTWEST, FINISHED, "models/powl/ew.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_SOUTHWEST, FINISHED, "models/powl/sw.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_EASTSOUTHWEST, FINISHED, "models/powl/esw.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHSOUTHWEST, FINISHED, "models/powl/nsw.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEASTWEST, FINISHED, "models/powl/new.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEASTSOUTH, FINISHED, "models/powl/nes.ms3d", scale, trans);
	DefConn(CONDUIT_POWL, CONNECTION_NORTHEASTSOUTHWEST, FINISHED, "models/powl/nesw.ms3d", scale, trans);

	trans = Vec3f(-TILE_SIZE/2, 0, TILE_SIZE/2);
	DefCo(CONDUIT_CRPIPE, offsetof(Building,crpipenetw), offsetof(Selection,crpipes), TILE_SIZE*2, TILE_SIZE*2, true, true, Vec2i(0, 0), Vec3f(0, 0, 0));
	CoConMat(CONDUIT_CRPIPE, RES_LABOUR, 1);
	CoConMat(CONDUIT_CRPIPE, RES_MINERALS, 1);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NOCONNECTION, CONSTRUCTION, "models/crpipe/1_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTH, CONSTRUCTION, "models/crpipe/n_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EAST, CONSTRUCTION, "models/crpipe/e_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_SOUTH, CONSTRUCTION, "models/crpipe/s_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_WEST, CONSTRUCTION, "models/crpipe/w_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEAST, CONSTRUCTION, "models/crpipe/ne_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHSOUTH, CONSTRUCTION, "models/crpipe/ns_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EASTSOUTH, CONSTRUCTION, "models/crpipe/es_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHWEST, CONSTRUCTION, "models/crpipe/nw_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EASTWEST, CONSTRUCTION, "models/crpipe/ew_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_SOUTHWEST, CONSTRUCTION, "models/crpipe/sw_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EASTSOUTHWEST, CONSTRUCTION, "models/crpipe/esw_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHSOUTHWEST, CONSTRUCTION, "models/crpipe/nsw_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEASTWEST, CONSTRUCTION, "models/crpipe/new_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEASTSOUTH, CONSTRUCTION, "models/crpipe/nes_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEASTSOUTHWEST, CONSTRUCTION, "models/crpipe/nesw_c.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NOCONNECTION, FINISHED, "models/crpipe/1.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTH, FINISHED, "models/crpipe/n.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EAST, FINISHED, "models/crpipe/e.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_SOUTH, FINISHED, "models/crpipe/s.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_WEST, FINISHED, "models/crpipe/w.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEAST, FINISHED, "models/crpipe/ne.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHSOUTH, FINISHED, "models/crpipe/ns.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EASTSOUTH, FINISHED, "models/crpipe/es.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHWEST, FINISHED, "models/crpipe/nw.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EASTWEST, FINISHED, "models/crpipe/ew.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_SOUTHWEST, FINISHED, "models/crpipe/sw.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_EASTSOUTHWEST, FINISHED, "models/crpipe/esw.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHSOUTHWEST, FINISHED, "models/crpipe/nsw.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEASTWEST, FINISHED, "models/crpipe/new.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEASTSOUTH, FINISHED, "models/crpipe/nes.ms3d", scale, trans);
	DefConn(CONDUIT_CRPIPE, CONNECTION_NORTHEASTSOUTHWEST, FINISHED, "models/crpipe/nesw.ms3d", scale, trans);


	// Tiles

#if 0
	DefTl(TILE_0000, "tiles/ng/ng_inc0000.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0001, "tiles/ng/ng_inc0001.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0010, "tiles/ng/ng_inc0010.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0011, "tiles/ng/ng_inc0011.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0100, "tiles/ng/ng_inc0100.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0101, "tiles/ng/ng_inc0101.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0110, "tiles/ng/ng_inc0110.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0111, "tiles/ng/ng_inc0111.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1000, "tiles/ng/ng_inc1000.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1001, "tiles/ng/ng_inc1001.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1010, "tiles/ng/ng_inc1010.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1011, "tiles/ng/ng_inc1011.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1100, "tiles/ng/ng_inc1100.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1101, "tiles/ng/ng_inc1101.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1110, "tiles/ng/ng_inc1110.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
#elif 0
	DefTl(TILE_0000, "tiles/gengrass/gg_inc0000.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0001, "tiles/gengrass/gg_inc0001.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0010, "tiles/gengrass/gg_inc0010.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0011, "tiles/gengrass/gg_inc0011.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0100, "tiles/gengrass/gg_inc0100.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0101, "tiles/gengrass/gg_inc0101.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0110, "tiles/gengrass/gg_inc0110.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0111, "tiles/gengrass/gg_inc0111.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1000, "tiles/gengrass/gg_inc1000.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1001, "tiles/gengrass/gg_inc1001.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1010, "tiles/gengrass/gg_inc1010.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1011, "tiles/gengrass/gg_inc1011.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1100, "tiles/gengrass/gg_inc1100.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1101, "tiles/gengrass/gg_inc1101.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1110, "tiles/gengrass/gg_inc1110.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
#elif 1
	DefTl(TILE_0000, "tiles/gengrass - Copy/gg_inc0000.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0001, "tiles/gengrass - Copy/gg_inc0001.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0010, "tiles/gengrass - Copy/gg_inc0010.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0011, "tiles/gengrass - Copy/gg_inc0011.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0100, "tiles/gengrass - Copy/gg_inc0100.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0101, "tiles/gengrass - Copy/gg_inc0101.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0110, "tiles/gengrass - Copy/gg_inc0110.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0111, "tiles/gengrass - Copy/gg_inc0111.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1000, "tiles/gengrass - Copy/gg_inc1000.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1001, "tiles/gengrass - Copy/gg_inc1001.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1010, "tiles/gengrass - Copy/gg_inc1010.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1011, "tiles/gengrass - Copy/gg_inc1011.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1100, "tiles/gengrass - Copy/gg_inc1100.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1101, "tiles/gengrass - Copy/gg_inc1101.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1110, "tiles/gengrass - Copy/gg_inc1110.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
#elif 1
	
	DefTl(TILE_0000, "tiles/farm/farm_inc0000.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0001, "tiles/farm/farm_inc0001.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0010, "tiles/farm/farm_inc0010.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0011, "tiles/farm/farm_inc0011.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0100, "tiles/farm/farm_inc0100.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0101, "tiles/farm/farm_inc0101.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0110, "tiles/farm/farm_inc0110.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0111, "tiles/farm/farm_inc0111.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1000, "tiles/farm/farm_inc1000.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1001, "tiles/farm/farm_inc1001.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1010, "tiles/farm/farm_inc1010.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1011, "tiles/farm/farm_inc1011.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1100, "tiles/farm/farm_inc1100.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1101, "tiles/farm/farm_inc1101.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1110, "tiles/farm/farm_inc1110.png", Vec2i(0,-64)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
#else
	DefTl(TILE_0000, "tiles/gr/gr_inc0000.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0001, "tiles/gr/gr_inc0001.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0010, "tiles/gr/gr_inc0010.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0011, "tiles/gr/gr_inc0011.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0100, "tiles/gr/gr_inc0100.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0101, "tiles/gr/gr_inc0101.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0110, "tiles/gr/gr_inc0110.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_0111, "tiles/gr/gr_inc0111.png", Vec2i(0,0)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1000, "tiles/gr/gr_inc1000.png", Vec2i(0,-TILE_PIXEL_WIDTH/4)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1001, "tiles/gr/gr_inc1001.png", Vec2i(0,-TILE_PIXEL_WIDTH/4)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1010, "tiles/gr/gr_inc1010.png", Vec2i(0,-TILE_PIXEL_WIDTH/4)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1011, "tiles/gr/gr_inc1011.png", Vec2i(0,-TILE_PIXEL_WIDTH/4)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1100, "tiles/gr/gr_inc1100.png", Vec2i(0,-TILE_PIXEL_WIDTH/4)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1101, "tiles/gr/gr_inc1101.png", Vec2i(0,-TILE_PIXEL_WIDTH/4)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
	DefTl(TILE_1110, "tiles/gr/gr_inc1110.png", Vec2i(0,-TILE_PIXEL_WIDTH/4)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());

	//for(int i=0; i<=TILE_1110; i++)
	//	DefTl(i, "tiles/gr/gr_inc1010.png", Vec2i(0,-TILE_PIXEL_WIDTH/4)+Vec2i(0,TILE_PIXEL_WIDTH/4), Vec2i());
#endif

	// Sounds

	g_ordersnd.clear();
	g_ordersnd.push_back(Sound("sounds/aaa000/gogogo.wav"));
	g_ordersnd.push_back(Sound("sounds/aaa000/moveout2.wav"));
	g_ordersnd.push_back(Sound("sounds/aaa000/spreadout.wav"));
	g_ordersnd.push_back(Sound("sounds/aaa000/wereunderattack3.wav"));
	//g_zpainSnd.push_back(Sound("sounds/zpain.wav"));
}
