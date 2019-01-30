/* tools.hpp */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <list>

/* for ipv4 */
#define LOW 0
#define HIGH 65535
#define BIT_LENGTH 16

//std::list<std::string> range_to_01m_strings
std::list<std::string> int_pair_to_01m_string(int a, int b, int w) {
  std::string bin(w, '\0');
  
  double a1 = (double)a, b1 = (double)b, l;
  int d;
  l = log2(b1-a1+1);
  d = (int)l;

  for (int i = w-1; i >= 0; --i)
    bin[w-i-1] = 48 + ((a>>i)&1);
  // bin[w] = '\0';

  for (int i = 0; i < d; ++i)
    bin[w-i-1] = '*';

  std::list<std::string> L;
  L.push_back(bin);
  
  return L;
}

/* LOW (=0) and HIGH (=65535) are predefined in the tool.h
     a-b  => 010101..10**, 111101..1111, ...
e.g. 3-17 => 0000000000000011,00000000000001**,0000000000001***,000000000001000*
   
*/

std::list<std::string> range_to_01m_strings(int a, int b, int low, int high) {
  int m = (low+high-1)/2;
  int n = (low+high+1)/2;
  std::list<std::string> X;
  std::list<std::string> Y;
  /* printf("a = %d, b = %d, low = %d, high = %d, m = %d, n = %d\n", a, b, low, high, m, n); */

  if (a == low && b == high) { return int_pair_to_01m_string(a,b,BIT_LENGTH); }
  else if (b <= m) { return range_to_01m_strings(a,b,low,m); }
  else if (a <= m && n <= b) { 
    Y = range_to_01m_strings(a, m, low, m);
    X = range_to_01m_strings(n, b, n, high);
    // list_strings_concat(X,Y);
    X.splice(X.end(), Y);
  }
  else { return range_to_01m_strings(a,b,n,high); }

  return X;
}
