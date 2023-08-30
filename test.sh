#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./out/ECC "$input" > out/tmp.s # Redirecting the output of ECC to an assembly file
  cc -o out/tmp out/tmp.s out/func.o out/memory.o # Linking the generated assembly with the function file in an executable
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

  assert 0 'int main() {return 0;}'
  assert 42 'int main() {return 42;}'
  assert 21 'int main() {return 5+20-4;}'
  assert 21 'int main() {return 5 + 20 - 4;}'
  assert 47 'int main() {return 5+6*7;}'
  assert 15 'int main() {return 5*(9-6);}'
  assert 4 'int main() {return (3+5)/2;}'
  assert 10 'int main() {return -10+20;}'
  assert 2 'int main() {return ++1;}'
  assert 0 'int main() {return --1;}'

  printf "OK\n\n"
}

comparisons() {
  printf "Testing comparisons\n"

  assert 1 'int main() {return 10>5;}'
  assert 0 'int main() {return 10<5;}'
  assert 1 'int main() {return 5<10;}'
  assert 0 'int main() {return 5>10;}'
  assert 0 'int main() {return 0==10;}'
  assert 1 'int main() {return 9!=10;}'
  assert 1 'int main() {return 10>=10;}'
  assert 1 'int main() {return 10>=2;}'
  assert 0 'int main() {return 10>=50;}'

  printf "OK\n\n"
}

variables() {
  printf "Testing variables\n"

  assert 10 'int main() {int a=5; return a+5;}'
  assert 25 'int main() {int a=20; int b=5; return a+b;}'
  assert 10 'int main() {int foo=5; return foo+5;}'
  assert 25 'int main() {int foo=20; int bar=5; return foo+bar;}'
  assert 5 'int main() {int foo; foo=5; return foo;}'

  printf "OK\n\n"
}

branching() {
  printf "Testing branching\n"

  assert 1 'int main() {if(5==5) return 1;}'
  assert 1 'int main() {if(5==5) { return 1; } else { return 0; }}'
  assert 0 'int main() {if(4==5) { return 1; } else { return 0; }}'
  assert 10 'int main() {int foo = 0; while(foo != 10) { foo = foo + 1; } return foo;}'
  assert 10 'int main() {int foo = 0; for(int count = 0; count < 10; count = count + 1) { foo = foo + 1; } return foo;}'
  assert 80 'int main() {int foo = 0; for(int count = 0; count < 10; count = count + 1) { int val = 2 * 4; foo = foo + val; } return foo;}'

  printf "OK\n\n"
}

functions() {
  printf "Testing functions\n"

  # assertPrintf 'This works!' 'main() { helloWorld(); return 0; }'
  # assertPrintf '6' 'main() { helloArgs(1,2,3); return 0; }'
  assert 5 'int five() { int foo = 5; return foo; } int main() { return five(); }'
  assert 12 'int add(int a, int b) { return a + b; } int main() { return add(5,7); }'
  assert 200 'int foo(int a, int b, int c) { int foo = 25; return a * b + c + foo; } int main() { return foo(25,4,75); }'

  printf "OK\n\n"
}

pointers() {
  printf "Testing pointers\n"

  # TODO: Fix "int* y = &x" notation not working
  assert 3 'int main() {int x=3; int y=&x; return *y;}'
  assert 3 'int main() {int x; int *y; y=&x; *y=3; return x;}'
  assert 4 'int main() {int* p; alloc4(&p, 1, 2, 4, 8); int *q; q = p + 2; return *q;}'
  assert 8 'int main() {int* p; alloc4(&p, 1, 2, 4, 8); int *q; q = p + 3; return *q;}'
  # TODO: Test double pointer arithmetic

  printf "OK\n\n"
}

sizeof() {
  printf "Testing sizeof operator\n"

  assert 4 'int main() { return sizeof(10); }'
  assert 4 'int main() { int i = 10; return sizeof(i); }'
  assert 8 'int main() { int i = 10; return sizeof(&i); }'
  assert 8 'int main() { int x=3; int* y; y = &x; return sizeof(y); }'
  assert 4 'int main() { int x=3; int* y; y = &x; return sizeof(*y); }'
  assert 4 'int main() { int x=3; return sizeof(x+3); }'
  assert 8 'int main() { int* x; return sizeof(x+3); }'

  printf "OK\n\n"
}

fibonacci() {
  assert 55 "
int fib(n)
{
     int a = 0;
     int b = 1;
     int c;
     if (n == 0)
         return a;
     for (int i = 2; i <= n; i = i + 1) {
         c = a + b;
         a = b;
         b = c;
     }
     return b;
 }
int main()
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
  sizeof
}

cc -c out/func.c -o out/func.o # Compiling the functions file to test cross file functions
cc -c out/memory.c -o out/memory.o # Compiling the memory file
all