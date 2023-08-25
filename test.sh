#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./out/ECC "$input" > out/tmp.s # Redirecting the output of ECC to an assembly file
  cc -o out/tmp out/tmp.s out/func.o # Linking the generated assembly with the function file in an executable
  ./out/tmp

  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assertPrintf() {
    expected="$1"
    input="$2"

    ./out/ECC "$input" > out/tmp.s # Redirecting the output of ECC to an assembly file
    cc -o out/tmp out/tmp.s out/func.o # Linking the generated assembly with the function file in an executable
    ./out/tmp

    actual=$(./out/tmp | tail -1)

    if [ "$actual" = "$expected" ]; then
      echo "$input => $actual"
    else
      echo "$input => $expected expected, but got $actual"
      exit 1
    fi
}

arithmetic() {
  printf "Testing artithmetic\n"

  assert 0 'main() {return 0;}'
  assert 42 'main() {return 42;}'
  assert 21 'main() {return 5+20-4;}'
  assert 21 'main() {return 5 + 20 - 4;}'
  assert 47 'main() {return 5+6*7;}'
  assert 15 'main() {return 5*(9-6);}'
  assert 4 'main() {return (3+5)/2;}'
  assert 10 'main() {return -10+20;}'
  assert 2 'main() {return ++1;}'
  assert 0 'main() {return --1;}'

  printf "OK\n\n"
}

comparisons() {
  printf "Testing comparisons\n"

  assert 1 'main() {return 10>5;}'
  assert 0 'main() {return 10<5;}'
  assert 1 'main() {return 5<10;}'
  assert 0 'main() {return 5>10;}'
  assert 0 'main() {return 0==10;}'
  assert 1 'main() {return 9!=10;}'
  assert 1 'main() {return 10>=10;}'
  assert 1 'main() {return 10>=2;}'
  assert 0 'main() {return 10>=50;}'

  printf "OK\n\n"
}

variables() {
  printf "Testing variables\n"

  assert 10 'main() {a=5; return a+5;}'
  assert 25 'main() {a=20; b=5; return a+b;}'
  assert 10 'main() {foo=5; return foo+5;}'
  assert 25 'main() {foo=20; bar=5; return foo+bar;}'

  printf "OK\n\n"
}

branching() {
  printf "Testing branching\n"

  assert 1 'main() {if(5==5) return 1;}'
  assert 1 'main() {if(5==5) { return 1; } else { return 0; }}'
  assert 0 'main() {if(4==5) { return 1; } else { return 0; }}'
  assert 10 'main() {foo = 0; while(foo != 10) { foo = foo + 1; } return foo;}'
  assert 10 'main() {foo = 0; for(count = 0; count < 10; count = count + 1) { foo = foo + 1; } return foo;}'
  assert 80 'main() {foo = 0; for(count = 0; count < 10; count = count + 1) { val = 2 * 4; foo = foo + val; } return foo;}'

  printf "OK\n\n"
}

functions() {
  printf "Testing functions\n"

  assertPrintf 'This works!' 'main() { helloWorld(); return 0; }'
  assertPrintf '6' 'main() { helloArgs(1,2,3); return 0; }'
  assert 5 'five() { foo = 5; return foo; } main() { return five(); }'
  assert 12 'add(a, b) { return a + b; } main() { return add(5,7); }'
  assert 200 'foo(a, b, c) { foo = 25; return a * b + c + foo; } main() { return foo(25,4,75); }'

  printf "OK\n\n"
}

pointers() {
  assert 3 'main() {x=3; y=&x; return *y;}'
  assert 3 'main() {x=3; y=5; z=&y + 8; return *z;}'
}

fibonacci() {
  assert 55 "
fib(n)
{
     a = 0;
     b = 1;
     c = 0;
     if (n == 0)
         return a;
     for (i = 2; i <= n; i = i + 1) {
         c = a + b;
         a = b;
         b = c;
     }
     return b;
 }
main()
{
   return fib(10);
}"
}

all() {
  arithmetic
  comparisons
  variables
  branching
  functions
  pointers
}

cc -c out/func.c -o out/func.o # Compiling the functions file to test cross file functions
all