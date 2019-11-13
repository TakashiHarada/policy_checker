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

  // For Sugar
  std::string toSugarForm(const unsigned, const unsigned);
  std::string makeRuleString(const unsigned, const unsigned);
  std::string getSugarSA(const unsigned, const unsigned);
  std::string makeSATerm(const unsigned);
  std::string getSugarDA(const unsigned, const unsigned);
  std::string makeDATerm(const unsigned);
  std::string getSugarSP(const unsigned, const unsigned);
  std::string makeSPTerm(const unsigned, const std::string);
  std::string getSugarDP(const unsigned, const unsigned);
  std::string makeDPTerm(const unsigned, const std::string);
  std::string makePROTTerm(const unsigned);
  std::string getSugarPROT(const unsigned, const unsigned);
  friend std::ostream& operator<<(std::ostream&, const CRule&);
};

std::string CRule::makeRuleString(const unsigned rulelist_number, const unsigned rule_number) {
  std::string ss = "r";
  ss += std::to_string(rulelist_number);
  ss += "_";
  ss += std::to_string(rule_number);
  return ss;
}

std::string trueTerm(const std::string s) {
  std::string ss = "(iff ";
  ss += s;
  ss += " true)";
  return ss;
}

bool notAllMask(const std::string s, const unsigned l) {
  for (unsigned i = 0; i < l; ++i)
    if ('*' != s[i])
      return true;
  return false;
}

std::string CRule::makeSATerm(const unsigned bitPos) {
  std::string t = "(= sa";
  t += std::to_string(bitPos);
  if ('1' == sa[bitPos])
    t += " 1)";
  else
    t += " 0)";
  return t;
}

std::string CRule::getSugarSA(const unsigned rulelist_number, const unsigned rule_number) {
  if (!notAllMask(sa, 32))
      return "";
  std::string ssa = "(bool "; ssa += makeRuleString(rulelist_number, rule_number); ssa += "_sa)\n";
  ssa += "(iff "; ssa += makeRuleString(rulelist_number, rule_number); ssa += "_sa";
  ssa += " (and ";
  for (unsigned i = 0; i < 32; ++i) {
    if ('*' != sa[i]) {
      ssa += makeSATerm(i);
      if (31 != i)
	ssa += " ";
    }
  }
  ssa += "))\n";
  return ssa;
}

std::string CRule::makeDATerm(const unsigned bitPos) {
  std::string t = "(= da";
  t += std::to_string(bitPos);
  if ('1' == da[bitPos])
    t += " 1)";
  else 
    t += " 0)";
  return t;
}

std::string CRule::getSugarDA(const unsigned rulelist_number, const unsigned rule_number) {
  if (!notAllMask(da, 32))
    return "";

  std::string sda = "(bool "; sda += makeRuleString(rulelist_number, rule_number); sda += "_da)\n";
  sda += "(iff "; sda += makeRuleString(rulelist_number, rule_number); sda += "_da";
  sda += " (and ";
  for (unsigned i = 0; i < 32; ++i) {
    if ('*' != da[i]) {
      sda += makeDATerm(i);
      if (31 != i)
	sda += " ";
    }
  }
  sda += "))\n";
   
  return sda;
}

// std::string CRule::getSugarSP(const unsigned rulelist_number, const unsigned rule_number) {
//   std::string ssp = "(bool "; ssp += makeRuleString(rulelist_number, rule_number); ssp += "_sp)\n";
//   std::string ssp0 = makeRuleString(rulelist_number, rule_number); ssp0 += "_sp";
//   // ssp += trueTerm(ssp0); ssp += "\n";
//   ssp += "(iff "; ssp += makeRuleString(rulelist_number, rule_number); ssp += "_sp";
//   ssp += " (and (<= "; ssp += std::to_string(sp.first); ssp += " sp) (<= sp "; ssp += std::to_string(sp.second); ssp += ")))";
//   return ssp;
// }

std::string CRule::makeSPTerm(const unsigned bitPos, const std::string s) {
  std::string t = "(= sp";
  t += std::to_string(bitPos);
  if ('1' == s[bitPos])
    t += " 1)";
  else
    t += " 0)";
  return t;
}

std::string CRule::getSugarSP(const unsigned rulelist_number, const unsigned rule_number) {
  // ssp += " (and (<= "; ssp += std::to_string(sp.first); ssp += " sp) (<= sp "; ssp += std::to_string(sp.second); ssp += ")))";

  std::list<std::string> SP = range_to_01m_strings(sp.first, sp.second, LOW, HIGH);
  unsigned counter = 0;
  for (std::list<std::string>::iterator its = SP.begin(); SP.end() != its; ++its) {
    if (notAllMask(*its, 16))
      ++counter;
  }

  if (0 < counter) {
    std::string ssp = "(bool "; ssp += makeRuleString(rulelist_number, rule_number); ssp += "_sp)\n";
    ssp += "(iff "; ssp += makeRuleString(rulelist_number, rule_number); ssp += "_sp";
    ssp += " (or";
    for (std::list<std::string>::iterator its = SP.begin(); SP.end() != its; ++its)
      if (notAllMask(*its, 16)) {
	ssp += " (and ";
	for (unsigned i = 0; i < 16; ++i)
	  if ('*' != (*its)[i]) {
	    ssp += makeSPTerm(i, *its);
	    if (15 != i)
	      ssp += " ";
	  }
	ssp += ")";
      }
    if (1 == counter)
      ssp += " false";
    ssp += "))\n";
    return ssp;
  }
  return "";
}

// std::string CRule::getSugarDP(const unsigned rulelist_number, const unsigned rule_number) {
//   std::string sdp = "(bool "; sdp += makeRuleString(rulelist_number, rule_number); sdp += "_dp)\n";
//   sdp += "(iff "; sdp += makeRuleString(rulelist_number, rule_number); sdp += "_dp";
//   sdp += " (and (<= "; sdp += std::to_string(dp.first); sdp += " dp) (<= dp "; sdp += std::to_string(dp.second); sdp += ")))";
//   return sdp;
// }

std::string CRule::makeDPTerm(const unsigned bitPos, const std::string s) {
  std::string t = "(= dp";
  t += std::to_string(bitPos);
  if ('1' == s[bitPos])
    t += " 1)";
  else
    t += " 0)";
  return t;
}

std::string CRule::getSugarDP(const unsigned rulelist_number, const unsigned rule_number) {
  // sdp += " (and (<= "; sdp += std::to_string(dp.first); sdp += " dp) (<= dp "; sdp += std::to_string(dp.second); sdp += ")))";

  std::list<std::string> DP = range_to_01m_strings(dp.first, dp.second, LOW, HIGH);
  unsigned counter = 0;
  for (std::list<std::string>::iterator its = DP.begin(); DP.end() != its; ++its) {
    if (notAllMask(*its, 16))
      ++counter;
  }

  if (0 < counter) {
    std::string sdp = "(bool "; sdp += makeRuleString(rulelist_number, rule_number); sdp += "_dp)\n";
    sdp += "(iff "; sdp += makeRuleString(rulelist_number, rule_number); sdp += "_dp";
    sdp += " (or";
    for (std::list<std::string>::iterator its = DP.begin(); DP.end() != its; ++its)
      if (notAllMask(*its, 16)) {
	sdp += " (and ";
	for (unsigned i = 0; i < 16; ++i)
	  if ('*' != (*its)[i]) {
	    sdp += makeDPTerm(i, *its);
	    if (15 != i)
	      sdp += " ";
	  }
	sdp += ")";
      }
    if (1 == counter)
      sdp += " true";
    sdp += "))\n";
    return sdp;
  }
  return "";
}

std::string CRule::makePROTTerm(const unsigned bitPos) {
  std::string t = "(= prot";
  t += std::to_string(bitPos);
  if ('1' == prot[bitPos])
    t += " 1)";
  else
    t += " 0)";
  return t;
}

std::string CRule::getSugarPROT(const unsigned rulelist_number, const unsigned rule_number) {
  if (!notAllMask(prot, 8))
    return "";
  std::string sprot = "(bool "; sprot += makeRuleString(rulelist_number, rule_number); sprot += "_prot)\n";
  sprot += "(iff "; sprot += makeRuleString(rulelist_number, rule_number); sprot += "_prot";
  sprot += " (and ";
  for (unsigned i = 0; i < 8; ++i)
    if ('*' != prot[i]) {
      sprot += makePROTTerm(i);
      if (7 != i)
	sprot += " ";
    }
  sprot += "))\n";
  return sprot;
}

std::string CRule::toSugarForm(const unsigned rulelist_number, const unsigned rule_number) {
  std::string lf = "";
  std::string ssa = getSugarSA(rulelist_number, rule_number);
  lf += ssa;
  std::string sda = getSugarDA(rulelist_number, rule_number);
  lf += sda;
  std::string ssp = getSugarSP(rulelist_number, rule_number);
  lf += ssp;
  std::string sdp = getSugarDP(rulelist_number, rule_number);
  lf += sdp; lf += "\n";
  std::string sprot = getSugarPROT(rulelist_number, rule_number);
  lf += sprot;

  std::string rst = makeRuleString(rulelist_number, rule_number);
  lf += "(iff "; lf += rst; lf += " (and ";
  if (notAllMask(sa, 32))
    lf += rst, lf += "_sa ";
  if (notAllMask(da, 32))
    lf += rst, lf += "_da ";
  if (!(0 == sp.first && 65535 == sp.second))
    lf += rst, lf += "_sp ";
  if (!(0 == dp.first && 65535 == dp.second))
    lf += rst, lf += "_dp ";
  if (notAllMask(prot, 8))
    lf += rst, lf += "_prot";
  lf += "))";
  
  return lf;
}

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

void declareVariables() {
  // Source Address
  for (unsigned i = 0; i < 32; ++i)
    std::cout << "(int sa" << i << " 0 1)" << std::endl;
  
  // Destination Address
  for (unsigned i = 0; i < 32; ++i)
    std::cout << "(int da" << i << " 0 1)" << std::endl;

  // Source Port Number
  for (unsigned i = 0; i < 16; ++i)
    std::cout << "(int sp" << i << " 0 1)" << std::endl;
  // std::cout << "(int sp 0 65535)" << std::endl;
  
  // Destination Port Number
  // std::cout << "(int dp 0 65535)" << std::endl;
  for (unsigned i = 0; i < 16; ++i)
    std::cout << "(int dp" << i << " 0 1)" << std::endl;
  

  // Protocol
  for (unsigned i = 0; i < 8; ++i)
    std::cout << "(int prot" << i << " 0 1)" << std::endl;
}

void declareRules(const unsigned n, const unsigned rulelist_number) {
  for (unsigned i = 0; i < n; ++i)
    std::cout << "(bool r" << rulelist_number << "_" << i << ")" << std::endl;
}

int getLastPermitRuleNumber(std::vector<CRule>& R) {
  int last = -1;
  for (unsigned i = 0; i < R.size(); ++i)
    if ('A' == R[i].getAction())
      last = i;
  return last;
}

void printSugarForm(std::vector<CRule>& R, const unsigned rulelist_number, const bool flag) {
  const int last_permit = getLastPermitRuleNumber(R);
  if (-1 == last_permit) {
    // FIXME
    // Input Rule List has no Accept Rule.
    return;
  }
  if (0 == last_permit) {
    // FIXME
    // Input Rule List has just one Accept Rule.
    return;
  }

  if (flag)
    declareVariables();
  declareRules(R.size(), rulelist_number);
  
  for (unsigned i = 0; i <= (unsigned)last_permit; ++i)
    std::cout << R[i].toSugarForm(rulelist_number, i) << std::endl;

  std::cout << "(bool R" << std::to_string(rulelist_number) << ")\n";
  std::cout << "(iff R" << std::to_string(rulelist_number) << " (or";
  for (unsigned i = 0; i < (unsigned)last_permit; ++i) {
    std::string tt = R[i].makeRuleString(rulelist_number, i);
    if ('A' == R[i].getAction())
      std::cout << " (or " << tt;
    else
      std::cout << " (and (not " << tt << ")";
  }
  std::cout << " " << R[last_permit].makeRuleString(rulelist_number, last_permit);
  for (unsigned i = 0; i <= (unsigned)last_permit; ++i)
    std::cout << ")";
  std::cout << ")" << std::endl;
}

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
  // printf("R0.size() = %lu\n", R.size());
  
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

  // printf("R.size() = %lu\n", R.size());
  
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
