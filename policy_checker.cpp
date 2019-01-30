#include <policy_checker.hpp>

int main (int argc, char *argv[])
{
  if (3 != argc) { 
    fprintf(stderr, "ERROR! Arguments are invalid.\nUsage: $ ./policy_checker [-c] rulelist1\nExample: $ ./filter -c rulelist1\n");
    exit(1);
  }

  /* Arbitrary Bitmask Rules */
  if (!strcmp(argv[1], "-m")) {
    ;
  }
  else if (!strcmp(argv[1], "-c")) { /* Class Bench Rule List */
    std::vector<CRule> R = readClassBenchRulelist(argv[2]);
    addDefaultRule(&R);
    
    // for (unsigned i = 0; i < R.size(); ++i)
    //   std::cout << R[i] << std::endl;
    // char filename[30];
    
    DdManager *gbm; /* Global BDD manager. */
    gbm = Cudd_Init(CLASS_BENCH_RULE_LENGTH, CLASS_BENCH_RULE_LENGTH, CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS, 0); /* Initialize a new BDD manager. */
    DdNode* PB = makePolicyBDD(gbm, R);
    printf("The number of nodes (BDD) = %d\n", Cudd_DagSize(PB));
    Cudd_Quit(gbm);

    DdManager *gzm = Cudd_Init(CLASS_BENCH_RULE_LENGTH, CLASS_BENCH_RULE_LENGTH, CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS, 0);
    DdNode* PZ = makePolicyZDD(gzm, R);
    printf("The number of nodes (ZDD) = %d\n", Cudd_DagSize(PZ));
    Cudd_Quit(gzm);
    
  }
  return 0; 
}
