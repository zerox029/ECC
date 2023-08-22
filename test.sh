#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./out/ECC "$input" > out/tmp.s
  cc -o out/tmp out/tmp.s
  ./out/tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

arithmetic() {
  printf "Testing artithmetic\n"
  assert 0 "return 0;"
  assert 42 "return 42;"
  assert 21 'return 5+20-4;'
  assert 21 'return 5 + 20 - 4;'
  assert 47 'return 5+6*7;'
  assert 15 'return 5*(9-6);'
  assert 4 'return (3+5)/2;'
  assert 10 'return -10+20;'
  printf "OK\n\n"
}

comparisons() {
  printf "Testing comparisons\n"
  assert 1 'return 10>5;'
  assert 0 'return 10<5;'
  assert 1 'return 5<10;'
  assert 0 'return 5>10;'
  assert 0 'return 0==10;'
  assert 1 'return 9!=10;'
  assert 1 'return 10>=10;'
  assert 1 'return 10>=2;'
  assert 0 'return 10>=50;'
  printf "OK\n\n"
}

variables() {
  printf "Testing variables\n"
  assert 10 'a=5; return a+5;'
  assert 25 'a=20; b=5; return a+b;'
  assert 10 'foo=5; return foo+5;'
  assert 25 'foo=20; bar=5; return foo+bar;'
  printf "OK\n\n"
}

branching() {
  printf "Testing branching\n"
  assert 1 'if(5==5) return 1;'
  assert 1 'if(5==5) return 1; else return 0;'
  assert 0 'if(4==5) return 1; else return 0;'
  printf "OK\n\n"
}

all() {
  arithmetic
  comparisons
  variables
  branching
}

all