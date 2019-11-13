#include <policy_checker.hpp>

int main (int argc, char *argv[])
{
  if (4 != argc) { 
    fprintf(stderr, "ERROR! Arguments are invalid.\nUsage: $ ./policy_checker [-c] rulelist1\nExample: $ ./filter -c rulelist1\n");
    exit(1);
  }

  /* Arbitrary Bitmask Rules */
  if (!strcmp(argv[1], "-m")) {
    ;
  }
  else if (!strcmp(argv[1], "-c")) { /* Class Bench Rule List */
    std::vector<CRule> R = readClassBenchRulelist(argv[2]);
    // addDefaultRule(&R);

    std::vector<CRule> R2 = readClassBenchRulelist(argv[3]);
    
    // for (unsigned i = 0; i < R.size(); ++i)
    //   std::cout << R[i] << std::endl;
    // char filename[30];

    printSugarForm(R, 0, true);
        
    std::cout << ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n\n;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n";
    printSugarForm(R2, 2, false);
    std::cout << "(not (iff R0 R2))" << std::endl;
  }
  return 0; 
}
