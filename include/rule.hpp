#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <sstream>
#include <bitset>
#include "function.h"
#include "cuddInt.h"
#include "tools.hpp"

/**
 * Writes a dot file representing the argument DDs
 * @param the node object
 */
void write_bdd (DdManager *gbm, DdNode *dd, char* filename)
{
    FILE *outfile; // output file pointer for .dot file
    outfile = fopen(filename,"w");
    DdNode **ddnodearray = (DdNode**)malloc(sizeof(DdNode*)); // initialize the function array
    ddnodearray[0] = dd;
    Cudd_DumpDot(gbm, 1, ddnodearray, NULL, NULL, outfile); // dump the function to .dot file
    free(ddnodearray);
    fclose (outfile); // close the file */
}

void write_zdd (DdManager *gzm, DdNode *dd, char* filename)
{
    FILE *outfile; // output file pointer for .dot file
    outfile = fopen(filename,"w");
    DdNode **ddnodearray = (DdNode**)malloc(sizeof(DdNode*)); // initialize the function array
    ddnodearray[0] = dd;
    Cudd_zddDumpDot(gzm, 1, ddnodearray, NULL, NULL, outfile); // dump the function to .dot file
    free(ddnodearray);
    fclose (outfile); // close the file */
}

int get_number_of_nodes(std::vector<DdNode*> G) {
  DdNode** nodeArray = (DdNode**)calloc(G.size(), sizeof(DdNode*));
  for (unsigned i = 0; i < G.size(); ++i)
    nodeArray[i] = G[i];
  int sum = Cudd_SharingSize(nodeArray, G.size());
  free(nodeArray);
  return sum;
}

#define ADDRESS "*******************************"
#define PROTOCOL "********"

#define CLASS_BENCH_RULE_LENGTH 104

class CRule{ /* Class Bench Format Rule */
private :
  char action;
  int ruleNum;
  std::string sa; /* source address */
  std::string da; /* destination address */
  std::pair<int, int> sp; /* source port number */
  std::pair<int, int> dp; /* destination port number */
  std::string prot; /* protocol number */
public :
  static const int ruleLength = 104;
  CRule() : action('D'), ruleNum(-1), sa(ADDRESS), da(ADDRESS), prot(PROTOCOL) {
    sp.first = dp.first = 0;
    sp.second = dp.second = 65535;
  }
  CRule(char& a, int& number, int& sa1, int& sa2, int& sa3, int& sa4, int& sap, int& da1, int& da2, int& da3, int& da4, int& dap, int& spl, int& spr, int& dpl, int& dpr, int& protcol, int& protocolm) : action(a), ruleNum(number) {
    sa = setAddress(sa1, sa2, sa3, sa4, sap);
    da = setAddress(da1, da2, da3, da4, dap);
    sp.first = spl;
    sp.second = spr;
    dp.first = dpl;
    dp.second = dpr;
    prot = setProtocol(protcol, protocolm);
  }
  CRule(const CRule& r) {
    action = r.action;
    ruleNum = r.ruleNum;
    sa = r.sa;
    da = r.da;
    sp.first = r.sp.first;
    sp.second = r.sp.second;
    dp.first = r.dp.first;
    dp.second = r.dp.second;
    prot = r.prot;
  }
  ~CRule() { }
  std::string setAddress(int& o1, int& o2, int& o3, int& o4, int& prefix) {
    std::string s = "";
    std::stringstream s1, s2, s3, s4;
    s1 << std::bitset<8>(o1);
    s2 << std::bitset<8>(o2);
    s3 << std::bitset<8>(o3);
    s4 << std::bitset<8>(o4);
    s += s1.str(); s += s2.str(); s += s3.str(); s += s4.str();
    for (unsigned i = prefix; i < s.length(); ++i)
      s[i] = '*';
    return s;
  }
  std::string setProtocol(int& p, int& pm) {
    std::string p1 = "", p2 = "";
    std::stringstream prot, protm;
    prot << std::bitset<8>(p);
    protm << std::bitset<8>(pm);
    p1 = prot.str(), p2 = protm.str();
    for (unsigned i = 0; i < p1.length(); ++i)
      if ('0' == p2[i])
	p1[i] = '*';
    return p1;
  }

  void changeRuleNumber(int i) { ruleNum = i; }
  char getAction() { return action; }
  int getRuleNum() { return ruleNum; }
  std::string getSA() { return sa; }
  std::string getDA() { return da; }
  std::pair<int, int> getSP() { return sp; }
  std::pair<int, int> getDP() { return dp; }
  std::string getPROT() { return prot; }
  static int getRuleLength() { return ruleLength; }

  friend std::ostream& operator<<(std::ostream&, const CRule&);
};

std::ostream& operator<<(std::ostream& stream, const CRule& r)
{
  stream << "r[" << r.ruleNum << "," << r.action << "] ";
  stream << r.sa << ", ";
  stream << r.da << ", ";
  stream << r.sp.first << " : " << r.sp.second << ", ";
  stream << r.dp.first << " : " << r.dp.second << ", ";
  stream << r.prot;
  return stream;
};

void addDefaultRule(std::vector<CRule>* R) {
  unsigned i = R->size();
  CRule d = CRule();
  d.changeRuleNumber(i);
  R->push_back(d);
}


std::vector<CRule> readClassBenchRulelist(char* rulelistname) {
  FILE* fp;
  if((fp = fopen(rulelistname, "r")) ==NULL){
    fprintf(stderr, "ERROR!");
  }
  std::vector<CRule> R;
  char ACTION;
  int SA1, SA2, SA3, SA4, SAPrefix, DA1, DA2, DA3, DA4, DAPrefix;
  int SPL, SPR, DPL, DPR, PROT, PROTMask;
  char FLAG[255];
  
  // D @245.166.239.143/32 18.116.31.62/32 0 : 65535 65500 : 65500 0x06/0xFF 0x1000/0x1000
  for (int k = 0; EOF != fscanf(fp,"%c @%d.%d.%d.%d/%d %d.%d.%d.%d/%d %d : %d %d : %d %x/%x %s\n", &ACTION, &SA1, &SA2, &SA3, &SA4, &SAPrefix, &DA1, &DA2, &DA3, &DA4, &DAPrefix, &SPL, &SPR, &DPL, &DPR, &PROT, &PROTMask, FLAG); ++k) {
    CRule r(ACTION, k, SA1, SA2, SA3, SA4, SAPrefix, DA1, DA2, DA3, DA4, DAPrefix, SPL, SPR, DPL, DPR, PROT, PROTMask);
    // std::cout << r << std::endl;
    R.push_back(r);
  }
  fclose(fp);

  return R;
}

DdNode* makeBDDsub(DdManager* gbm, std::string cond) {
  DdNode *bdd = Cudd_ReadOne(gbm);
  Cudd_Ref(bdd);  

  DdNode *var, *tmp1, *tmp2, *tmp3;
  int w = cond.size();

  for (int i = w-1; i >= 0; --i) {
    if ('*' == cond[i])
      continue;
    else {
      var = Cudd_bddIthVar(gbm, i);
      Cudd_Ref(var); /*Increases the reference count of a node*/
      if ('0' == cond[i]) {
        tmp1 = Cudd_Not(var);
        Cudd_Ref(tmp1);
        Cudd_RecursiveDeref(gbm, var); var = NULL;
      }
      else
        tmp1 = var;
      tmp2 = Cudd_bddAnd(gbm, tmp1, bdd);
      Cudd_Ref(tmp2);
      tmp3 = bdd;
      bdd = tmp2;
      Cudd_RecursiveDeref(gbm, tmp1);
      Cudd_RecursiveDeref(gbm, tmp3);
      tmp1 = tmp2 = tmp3 = NULL;
    }
  }
  return bdd;
}

DdNode* makeBDD(DdManager* gbm, CRule r) {
  DdNode* bdd = Cudd_ReadLogicZero(gbm);
  Cudd_Ref(bdd);
  std::list<std::string> SP = range_to_01m_strings(r.getSP().first, r.getSP().second, LOW, HIGH);
  std::list<std::string> DP = range_to_01m_strings(r.getDP().first, r.getDP().second, LOW, HIGH);
  for (std::list<std::string>::iterator x = SP.begin(); SP.end() != x; ++x)
    for (std::list<std::string>::iterator y = DP.begin(); DP.end() != y; ++y) {
      std::string cond = "";
      cond += r.getSA();
      cond += r.getDA();
      cond += *x;
      cond += *y;
      cond += r.getPROT();

      DdNode* tmp = makeBDDsub(gbm, cond);
      Cudd_Ref(tmp);
      DdNode* var = Cudd_bddOr(gbm, bdd, tmp);
      Cudd_Ref(var);
      Cudd_RecursiveDeref(gbm, bdd);
      Cudd_RecursiveDeref(gbm, tmp);
      bdd = var;
    }
  return bdd;
}

DdNode* makePolicyBDD(DdManager* gbm, std::vector<CRule> R) {
  DdNode* bdd = Cudd_ReadLogicZero(gbm);
  Cudd_Ref(bdd);
  DdNode *tmp1 = NULL, *tmp2 = NULL, *tmp3 = NULL;
  for (int i = (int)R.size()-1; i > -1; --i) {
    tmp1 = makeBDD(gbm, R[i]);
    Cudd_Ref(tmp1);
    // std::cout << R[i] << std::endl;
    if ('D' == R[i].getAction()) {
      tmp2 = Cudd_bddOr(gbm, bdd, tmp1);
      Cudd_Ref(tmp2);
      Cudd_RecursiveDeref(gbm, tmp1);
      Cudd_RecursiveDeref(gbm, bdd);
      bdd = tmp2;
      tmp1 = tmp2 = NULL;
    }
    else {
      tmp2 = Cudd_Not(tmp1);
      Cudd_Ref(tmp2);
      tmp3 = Cudd_bddAnd(gbm, bdd, tmp2);
      Cudd_Ref(tmp3);
      Cudd_RecursiveDeref(gbm, bdd);
      Cudd_RecursiveDeref(gbm, tmp1);
      Cudd_RecursiveDeref(gbm, tmp2);
      bdd = tmp3;
      tmp1 = tmp2 = tmp3 = NULL;
    }
  }
  return bdd;
}

DdNode* makeZDDsub(DdManager* gzm, std::string cond) {
  // std::cout << cond << std::endl;
  DdNode *zdd;
  DdNode *var, *tmp1, *tmp2;
  int w = 104;

  // zdd = Cudd_ReadZddOne(gzm, 104);
  zdd = Cudd_ReadOne(gzm);
  Cudd_Ref(zdd);
  for (int i = w-1; i >= 0; --i) {
    // printf("i = %d\n", i);
    if ('0' == cond[i])
         continue;
    if ('1' == cond[i]) {
      var = Cudd_zddChange(gzm, zdd, i);
      Cudd_Ref(var); /*Increases the reference count of a node*/
      Cudd_RecursiveDerefZdd(gzm, zdd);
      zdd = var;
      var = NULL;
    }
    else {
      tmp1 = Cudd_zddChange(gzm, zdd, i);
      Cudd_Ref(tmp1);
      tmp2 = Cudd_zddUnion(gzm, zdd, tmp1);
      Cudd_Ref(tmp2);
      Cudd_RecursiveDerefZdd(gzm, zdd);
      Cudd_RecursiveDerefZdd(gzm, tmp1);
      zdd = tmp2;
      tmp1 = tmp2 = NULL;
    }
  }
  return zdd;
}

// /home/xunicr/rules/acl1_seed_1/r_acl_1_1AD

DdNode* makeZDD(DdManager* gzm, CRule r) {
  // DdNode* zdd = Cudd_ReadOne(gzm);
  DdNode* zdd = Cudd_ReadZero(gzm);
  Cudd_Ref(zdd);
  
  std::list<std::string> SP = range_to_01m_strings(r.getSP().first, r.getSP().second, LOW, HIGH);
  std::list<std::string> DP = range_to_01m_strings(r.getDP().first, r.getDP().second, LOW, HIGH);
  for (std::list<std::string>::iterator x = SP.begin(); SP.end() != x; ++x)
    for (std::list<std::string>::iterator y = DP.begin(); DP.end() != y; ++y) {
      // std::cout << "SP = " << *x << std::endl;
      // std::cout << "DP = " << r.getDP().first << ", " << r.getDP().second << ", " << *y << std::endl;
      std::string cond = "";
      cond += r.getSA();
      cond += r.getDA();
      cond += *x;
      cond += *y;
      cond += r.getPROT();
      // std::cout << cond.length() << std::endl;
      DdNode* tmp = makeZDDsub(gzm, cond);
      Cudd_Ref(tmp);
      DdNode* var = Cudd_zddUnion(gzm, zdd, tmp);
      Cudd_Ref(var);
      Cudd_RecursiveDerefZdd(gzm, zdd);
      Cudd_RecursiveDerefZdd(gzm, tmp);
      zdd = var;
      tmp = var = NULL;
    }
  return zdd;
}

DdNode* makePolicyZDD(DdManager* gzm, std::vector<CRule> R) {
  DdNode* z = Cudd_ReadZero(gzm);
  Cudd_Ref(z);
  DdNode *tmp1 = NULL, *tmp2 = NULL;
  for (int i = (int)R.size()-1; i > -1; --i) {
    tmp1 = makeZDD(gzm, R[i]);
    Cudd_Ref(tmp1);
    if ('D' == R[i].getAction()) {
      tmp2 = Cudd_zddUnion(gzm, z, tmp1);
      Cudd_Ref(tmp2);
      Cudd_RecursiveDerefZdd(gzm, tmp1);
      Cudd_RecursiveDerefZdd(gzm, z);
      z = tmp2;
      tmp1 = tmp2 = NULL;
    }
    else {
      tmp2 = Cudd_zddDiff(gzm, z, tmp1);
      Cudd_Ref(tmp2);
      Cudd_RecursiveDerefZdd(gzm, tmp1);
      Cudd_RecursiveDerefZdd(gzm, z);
      z = tmp2;
      tmp1 = tmp2 = NULL;
    }
  }
  return z;
}
