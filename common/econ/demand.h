#ifndef DEMAND_H
#define DEMAND_H

#include "../math/vec2i.h"
#include "../platform.h"
#include "../sim/resources.h"
#include "../sim/utype.h"
#include "../sim/bltype.h"
#include "../sim/build.h"
#include "../sim/player.h"
#include "../debug.h"
#include "../sim/simdef.h"

//debug output for demand calcs?
//#define DEBUGDEM

//debug output
//#define DEBUGDEM2

class CostCompo
{
public:
	int margcost;	//marginal cost
	int fixcost;	//fixed cost
	int transpcost;	//transportation cost
	int ramt;

	CostCompo()
	{
		margcost = 0;
		fixcost = 0;
		transpcost = 0;
		ramt = 0;
	}
};

class Bid
{
public:
	int minbid;
	int maxbid;
	Vec2i tpos;
	Vec2i cmpos;
	//maxdist and marginpr depend on minutil.
	//maxdist and marginpr are adjusted by user; don't rely on them.
	int marginpr;
	int maxdist;
	int minutil;
	std::list<CostCompo> costcompo;	//cost composition for different sources

	Bid()
	{
		minbid = 0;
		maxbid = 0;
		maxdist = -1;
		minutil = -1;
		marginpr = -1;
	}
};

#define DEM_NODE		0	//	unknown node
#define DEM_RNODE		1	//	resource demand
#define DEM_BNODE		2	//	demand at building, for building
#define DEM_UNODE		3	//	demand for unit: worker, transport, or military
#define DEM_CDNODE		4	//	demand for conduit: road, powerline, crude oil pipeline

class DemNode
{
public:
	int demtype;
	DemNode* parent;
	Bid bid;	//reflects the maximum possible gouging price
	int profit;
	
	DemNode* orig;	//if this is part of a DemGraphMod, orig will point to a DemsAtB in the dm DemGraph and will have added demand loads to be applied this original dm DemGraph
	//if this is part of a DemGraphMod, these are the changes

	DemNode* copy;	//if this is the original, this will be the DemGraphMod duplicate

	DemNode();
	virtual ~DemNode()
	{
	}
};

class DemsAtB;
class DemsAtU;

class RDemNode : public DemNode
{
public:
	int rtype;
	int ramt;
	int btype;	//supplier?
	int bi;	//supplier bl index?
	int utype;	//transporter?
	int ui;	//transporter unit index?
	int demui;
	DemsAtB* supbp;
	DemsAtU* supup;
	DemsAtU* opup;
	std::list<DemNode*>* parlist;	//parent list, e.g., the condems of a DemsAtB	//unused

	RDemNode() : DemNode()
	{
		demtype = DEM_RNODE;
		rtype = -1;
		ramt = 0;
		btype = -1;
		bi = -1;
		utype = -1;
		ui = -1;
		supbp = NULL;
		supup = NULL;
		demui = -1;
		parlist = NULL;
	}

	virtual ~RDemNode()
	{
	}
};

class CdDem : public DemNode
{
public:
	int cdtype;
	Vec2i tpos;
	std::list<DemNode*> condems;	// (RDemNode)

	CdDem() : DemNode()
	{
		demtype = DEM_CDNODE;

	}

	virtual ~CdDem()
	{
	}
};

class DemsAtB : public DemNode
{
public:
	int bi;
	int btype;
	std::list<DemNode*> condems;	//construction material (RDemNode)
	std::list<DemNode*> proddems;	//production input raw materials (RDemNode)
	std::list<DemNode*> manufdems;	//manufacturing input raw materials (RDemNode)
	std::list<DemNode*> cddems[CONDUIT_TYPES]; // (CdDem)
	int prodratio;
	int condem[RESOURCES];
	int supplying[RESOURCES];

	DemsAtB() : DemNode()
	{
		demtype = DEM_BNODE;
		prodratio = 0;
		Zero(supplying);
		Zero(condem);
		bi = -1;
		btype = -1;
	}

	virtual ~DemsAtB()
	{
#if 0
		auto riter = condems.begin();
		while(riter != condems.end())
		{
			delete *riter;
			riter = condems.erase(riter);
		}

		riter = proddems.begin();
		while(riter != proddems.end())
		{
			delete *riter;
			riter = proddems.erase(riter);
		}

		riter = manufdems.begin();
		while(riter != manufdems.end())
		{
			delete *riter;
			riter = manufdems.erase(riter);
		}
#endif
	}
};

class DemsAtU : public DemNode
{
public:
	int ui;
	int utype;
	std::list<DemNode*> manufdems;	// (RDemNode)
	std::list<DemNode*> consumdems;	// (RDemNode)
	DemNode* opup;	//operator/driver (DemsAtU)
	int prodratio;
	int timeused;
	int totaldem[RESOURCES];

	DemsAtU() : DemNode()
	{
		demtype = DEM_UNODE;
		prodratio = 0;
		Zero(totaldem);
		opup = NULL;
		timeused = 0;
	}

	virtual ~DemsAtU()
	{
#if 0
		auto riter = manufdems.begin();
		while(riter != manufdems.end())
		{
			delete *riter;
			riter = manufdems.erase(riter);
		}

		riter = consumdems.begin();
		while(riter != consumdems.end())
		{
			delete *riter;
			riter = consumdems.erase(riter);
		}
#endif
	}
};

class DemGraph
{
public:
	//std::list<DemNode*> nodes;
	std::list<DemNode*> supbpcopy;	//master copy, this one will be freed (DemsAtB)
	std::list<DemNode*> supupcopy;	//master copy, this one will be freed (DemsAtU)
	std::list<DemNode*> codems[CONDUIT_TYPES];	//conduit placements (CdDem)
	std::list<DemNode*> rdemcopy;	//master copy, this one will be freed (RDemNode)
	int pyrsup[PLAYERS][RESOURCES];	//player global res supplying

	void drop()	//unsafe
	{
		supbpcopy.clear();
		supupcopy.clear();
		for(int c=0; c<CONDUIT_TYPES; c++)
			codems[c].clear();
		rdemcopy.clear();
	}

	void free()
	{
#if 0
		int* testi = new int;
		CheckMem(__FILE__, __LINE__, "\t\t\t1\ttest0");
		delete testi;
		CheckMem(__FILE__, __LINE__, "\t\t\t\t2\ttest0");
#endif

#if 0
		DemNode* testn = new DemNode;
		CheckMem(__FILE__, __LINE__, "\t\t\t1\ttest1");
		delete testn;
		CheckMem(__FILE__, __LINE__, "\t\t\t\t2\ttest1");
#endif

#if 0
		DemNode* testn2 = new RDemNode;
		CheckMem(__FILE__, __LINE__, "\t\t\t1\ttest2");
		delete testn2;
		CheckMem(__FILE__, __LINE__, "\t\t\t\t2\ttest2");
#endif

		auto biter = supbpcopy.begin();
		while(supbpcopy.size() > 0 && biter != supbpcopy.end())
		{
			CheckMem(__FILE__, __LINE__, "\t\t\t1\tfreeb");
			delete *biter;
			CheckMem(__FILE__, __LINE__, "\t\t\t\t2\tfreeb");
			biter = supbpcopy.erase(biter);
		}

		auto uiter = supupcopy.begin();
		while(supupcopy.size() > 0 && uiter != supupcopy.end())
		{
			delete *uiter;
			uiter = supupcopy.erase(uiter);
		}

#if 0
		auto riter = nodes.begin();
		while(riter != nodes.end())
		{
			delete *riter;
			riter = nodes.erase(riter);
		}
#else
		auto riter = rdemcopy.begin();
		while(rdemcopy.size() > 0 && riter != rdemcopy.end())
		{
			CheckMem(__FILE__, __LINE__, "\t\t\t1\tfreer");
			delete *riter;
			CheckMem(__FILE__, __LINE__, "\t\t\t\t2\tfreer");
			riter = rdemcopy.erase(riter);
		}
#endif

		for(unsigned int i=0; i<CONDUIT_TYPES; i++)
		{
			auto coiter = codems[i].begin();
			while(codems[i].size() > 0 && coiter != codems[i].end())
			{
				delete *coiter;
				coiter = codems[i].erase(coiter);
			}
		}

		for(unsigned int i=0; i<PLAYERS; i++)
			Zero(pyrsup[i]);
	}

	DemGraph()
	{
		for(int i=0; i<PLAYERS; i++)
			Zero(pyrsup[i]);
		//free();
	}

	~DemGraph()
	{
		free();
	}
};

//a list of changes/additions to a demgraph
//each demnode that has an .orig != NULL is a mod
//each demnode with orig=NULL is new
//so each .orig can be modded just by making *n->orig = *n; n->orig->orig = NULL;
//also each n->orig->demnode = n->orig->demnode->orig for n->orig->demnode->orig != 0
class DemGraphMod : public DemGraph
{
public:
	DemGraph* orig;

	DemGraphMod()
	{
		for(int i=0; i<PLAYERS; i++)
			Zero(pyrsup[i]);
		//free();

		orig = NULL;
	}

	~DemGraphMod()
	{
		//free();	//must be manually freed
	}
};

extern DemGraph g_demgraph;
extern DemGraph g_demgraph2[PLAYERS];

void CalcDem1();
void CalcDem2(Player* p, bool blopp);
void CombCo(int btype, Bid* bid, int rtype, int ramt);
bool MaxPro(std::list<CostCompo>& costco, int pricelevel, int demramt, int* proramt, int maxbudget, int* bestrev, int* bestprofit);
bool DemCmPos(DemNode* pardem, Vec2i* demcmpos);
void AddReq(DemGraph* dm, Player* p, std::list<DemNode*>* nodes, DemNode* parent, int rtype, int ramt, Vec2i demtpos, Vec2i demcmpos, int depth, bool* success, int maxbid);
void ApplyDem(DemGraph* dm, DemGraphMod* dmod);
void AddDemMod(DemGraphMod* src, DemGraphMod* dest);
void IniDmMod(DemGraph* dm, DemGraphMod* dmod);
int CountU(int utype);
int CountB(int btype);

#endif