llvm-as fib.ll -o fib.bc
opt -inline -loop-unroll fib.bc
llvm-dis fib.bc -o fib_improve2.ll