#ifndef DEBUG_H
#define DEBUG_H

#include <bits/stdc++.h>
using namespace std;

template<typename T1, typename T2>
string to_string(const pair<T1, T2>& p) {
  return '(' + to_string(p.first) + ", " + to_string(p.second) + ')';
}
string to_string(const string& s) {
  return '\"' + s + '\"';
}

template<typename T>
string to_string(T a) {
  string res = "[";
  int i = 0;
  for (auto x: a) {
    if (i != 0) res += ' ';
    res += to_string(x);
    i ++;
  }
  return res + "]";
}

void debug_out() { cerr << '\n'; }
template<typename H, typename... T>
void debug_out(H h, T... t) {
  cerr << to_string(h) << ' ';
  debug_out(t...);
}

#define debug(...) cerr << "[" << #__VA_ARGS__ << "]: ", debug_out(__VA_ARGS__)

#endif
