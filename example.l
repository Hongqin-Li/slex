if
  return IF;

[a-z][a-z0-9]*
  return ID;

[0-9][0-9]*
  return NUM;

([0-9][0-9]*\.[0-9]*)|([0-9]*\.[0-9][0-9]*)
  return REAL;

"(\\.|[^\\"\n])*"
  return STRING;
