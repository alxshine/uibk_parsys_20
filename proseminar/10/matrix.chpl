use Random;
use Time;

config const N = 2552;
config const numThreads = 8;
const sliceSize = N/numThreads;
// writeln("Slice size is ", sliceSize);

var A: [1..N,1..N] int;
var B: [1..N,1..N] int;
var C: [1..N,1..N] int;

forall (i,j) in A.domain{
    A(i,j) = i*N+j;
    if(i==j){
        B(i,j) = 1;
    }else{
        B(i,j) = 0;
    }
    C(i,j) = 0;
}

var timer: Timer;

timer.start();
coforall t in 0..numThreads-1{
    // writeln("This is thread ", t, " working on rows ", t*sliceSize, " up to ", (t+1)*sliceSize-1);
    for i in (t*sliceSize+1)..((t+1)*sliceSize){
        for j in 1..N{
            for k in 1..N{
                // writeln("i: ", i, ", j: ", j, ", k: ", k);
                C(i,j) += A(i,k) * B(k,j);
            }
        }
    }
}
timer.stop();

var hasErrors = false;
forall (i,j) in C.domain with (|| reduce hasErrors){
    if(A(i,j) != C(i,j)){
        hasErrors = true;
    }
}

if(hasErrors){
    writeln("ERROR: result is not correct");
}else{
    writeln("Matrix multiplication took ", timer.elapsed(), " seconds");
}