#include <cstdio>
#include <cstring>
#include <cassert>

#include <iostream>
#include <functional>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>
#include <vector>
using namespace std;

inline int SPECIAL(int i) { return -i; }

struct Node { int head, rule; };
struct Edge { int to, nxt, w; };
unordered_map<int, Node> node;
unordered_map<int, Edge> edge;
int nnodes, nedges;
int nalloc() { return ++nnodes; }
void addedge(int u, int v, int w) {
  edge[++nedges] = {v, node[u].head, w};
  node[u].head = nedges;
}

/* Print the graph with given root r in dot format. */
static void gstat(int r) {
  unordered_map<int, int> vis;

  function<void(int)> stat1 = [&](int u) {
    if (vis[u]) return;
    vis[u] = 1;
    if (node[u].rule) printf("n%d [label=r%d];\n", u, node[u].rule);
    for (int i = node[u].head; i; i = edge[i].nxt) {
      int v = edge[i].to, w = edge[i].w;
      printf("n%d -> n%d ", u, v);
      if (w == 0) ;
      else if (w > 0) printf("[label=\"%c\"]", w);
      else printf("[label=\"%c(s)\"]", -w);
      printf(";\n");
      if (!vis[v]) stat1(v);
    }
  };

  printf("digraph {\n");
  stat1(r);
  printf("}\n");
}

/* Special characters are negative. */
int eat(char **ps) {
  int c = **ps;
  if (**ps == '\\') {
    ++*ps;
    switch (**ps) {
    case 'n': c = '\n'; break;
    default: c = **ps;
    }
  } else {
    switch (**ps) {
    case '[': case ']': case '(': case ')': case '*': case '?': case '-':
      c = -c;
      break;
    }
  }
  ++*ps;
  
  // if (c == 0) printf("eat: \n");
  // else if (c > 0) printf("eat: '%c'\n", c);
  // else printf("eat: '%c'(s)\n", -c);
  return c;
}

struct NFA { int head, tail; };

NFA nfa_init() {
  return {nalloc(), nalloc()};
}

NFA nfa_empty() {
  int h = nalloc(), t = nalloc();
  addedge(h, t, 0);
  return {h, t};
}

NFA nfa_cat(NFA a, NFA b) {
  addedge(a.tail, b.head, 0);
  return {a.head, b.tail};
}

NFA nfa_alt(NFA a, NFA b) {
  addedge(a.head, b.head, 0);
  addedge(b.tail, a.tail, 0);
  return a;
}

NFA charset(char **ps) {
  assert(**ps != '\0');
  int h = nalloc(), t = nalloc();
  int c;
  if ((c = eat(ps)) == SPECIAL('[')) {
    for (int start, span = 0; (c = eat(ps)) != SPECIAL(']') && c != '\0'; ) {
      if (c == SPECIAL('-')) {  
        span = 1;
      } else if (span) {
        assert(start < c);
        for (int i = start + 1; i <= c; i++)
          addedge(h, t, i);
        span = 0;
      } else {
        start = c;
        addedge(h, t, c);
      }
    }
  } else addedge(h, t, c);
  return {h, t};
}

// TODO support '+'
NFA repete(NFA a, char **ps) {
  switch (**ps) {
    case '*':
      addedge(a.tail, a.head, 0);
    case '?':
      addedge(a.head, a.tail, 0);
      ++*ps;
      return a;
  }
  return a;
}

/* Parse regular expression to NFA. */
NFA parse(char **ps) {
  NFA res = {nalloc(), nalloc()}, cur = nfa_empty();
  for (int stop = 0; !stop; ) {
    NFA a;
    switch (**ps) {
    case '\0':
    case ')':
      nfa_alt(res, cur);
      stop = 1;
      break;
    case '|':
      nfa_alt(res, cur);
      cur = nfa_empty();
      ++*ps;
      break;
    case '(':
      ++*ps;
      a = parse(ps);
      assert(**ps == ')');
      ++*ps;
      cur = nfa_cat(cur, repete(a, ps));
      break;
    default:
      a = charset(ps);
      cur = nfa_cat(cur, repete(a, ps));
    }
  }
  return res;
}

/* Compute e-closure of states in s. */
set<int> closure(const set<int>& s) {
  set<int> res;
  queue<int> q;
  for (auto s: s) q.push(s);
  for (; q.size(); q.pop()) {
    int u = q.front();
    if (res.count(u)) continue;
    res.insert(u);
    for (int i = node[u].head; i; i = edge[i].nxt) {
      int v = edge[i].to, w = edge[i].w;
      if (!w && !res.count(v)) q.push(v);
    }
  }
  return res;
}

/* Compte adjacent states of states s with edge c. */
set<int> dfaedge(const set<int>& s, int c) {
  set<int> res;
  for (auto u: s) {
    for (int i = node[u].head; i; i = edge[i].nxt) {
      int v = edge[i].to;
      if (edge[i].w == c && !res.count(v))
        res.insert(v);
    }
  }
  return closure(res);
}


/*
 * A state d is final in the DFA if any NFA-state in states[d]
 * is final in the NFA. And we label it with the first regular
 * expressions that constitute the lexical specification.
 * This is how rule priority is implement.
 */
int prule(const set<int>& s) {
  int r = 0;
  for (auto s: s)
    if (node[s].rule)
      r = r ? min(r, node[s].rule) : node[s].rule;
  return r;
}

/* Convert NFA to DFA. */
int n2d(NFA a) {
  // FIXME
  vector<int> alpha;
  for (int i = 1; i < 128; i++) alpha.push_back(i);

  map<int, set<int>> ss;
  map<set<int>, int> ni;

  auto s0 = closure({a.head});
  auto& u0 = ni[s0];
  ss[u0 = nalloc()] = s0;
  node[u0].rule = prule(s0);

  queue<int> q;
  q.push(u0);
  for (; q.size(); q.pop()) {
    int u = q.front();
    for (auto c: alpha) {
      auto s = dfaedge(ss[u], c);
      if (s.size()) {
        auto& v = ni[s];
        if (!v) {
          ss[v = nalloc()] = s;
          node[v].rule = prule(s);
          q.push(v);
        }
        addedge(u, v, c);
      }
    }
  }
  return u0;
}

int main() {
  int h = nalloc(), t = nalloc();
  vector<string> rule;
  for (string s; getline(cin, s); ) {
    rule.push_back(s);
    char *cs = &s[0];
    NFA a = parse(&cs);
    node[a.tail].rule = rule.size();
    addedge(h, a.head, 0);
    addedge(a.tail, t, 0);
  }
  gstat(n2d({h, t}));
  return 0;
}
