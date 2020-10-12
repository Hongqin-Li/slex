#include <stdio.h>
#include <assert.h>
#include <string.h>

#define SPECIAL(i)  (-(i))
#define N (100003)

struct Edge { int to, nxt, w; } edge[N];
int nnodes, head[N], nedges;
int nalloc() { return ++nnodes; }
void addedge(int u, int v, int w) {
  edge[++nedges] = (struct Edge){v, head[u], w};
  head[u] = nedges;
}

static int vis[N];
static void stat1(int u) {
  if (vis[u]) return;
  vis[u] = 1;
  for (int i = head[u]; i; i = edge[i].nxt) {
    int v = edge[i].to, w = edge[i].w;
    printf("n%d -> n%d ", u, v);
    if (w == 0) printf("\n");
    else if (w > 0) printf("[label=%c]\n", w);
    else printf("[label=%c(s)]\n", -w);
    if (!vis[v]) stat1(v);
  }
}
/* Print the graph with given root r in dot format. */
static void gstat(int r) {
  memset(vis, 0, sizeof(vis));
  printf("digraph {\n");
  stat1(r);
  printf("}\n");
}

typedef struct NFA { int head, tail; } NFA;
NFA nfa_empty() {
  int h = nalloc(), t = nalloc();
  addedge(h, t, 0);
  return (NFA){h, t};
}
NFA nfa_cat(NFA a, NFA b) {
  addedge(a.tail, b.head, 0);
  return (NFA){a.head, b.tail};
}
NFA nfa_alt(NFA a, NFA b) {
  addedge(a.head, b.head, 0);
  addedge(b.tail, a.tail, 0);
  return a;
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
  return (NFA){h, t};
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
  NFA res = (NFA){nalloc(), nalloc()}, cur = nfa_empty();
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

/* Convert NFA to DFA. */
NFA n2d(NFA a) {

}

int main() {
  char t[1000];
  scanf("%s", t);
  char *s = t;
  char **ps = &s;

  // NFA a = charset(&s);
  NFA a = parse(&s);
  gstat(a.head);
  return 0;
}
