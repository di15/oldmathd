

#ifndef UTILITY_H
#define UTILITY_H

//#define MAX_UTIL	1000000000	//possible overflow?
//make sure this value is bigger than any combined obtainable revenue of building from labourer spending
//edit: and multiplied by distance?
#define MAX_UTIL	200000000

int PhUtil(int price, int cmdist);
int GlUtil(int price);
int InvPhUtilD(int util, int price);
int InvPhUtilP(int util, int cmdist);
int InvGlUtilP(int util);
int JobUtil(int wage, int cmdist, int workdelay);

#endif
