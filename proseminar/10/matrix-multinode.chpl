use Random;
use Time;

config const N = 2552;

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

var rowsPerLocale = N/numLocales;
writeln("Running on ", numLocales, " locale, calculating ", rowsPerLocale, " rows per locale");
var timer: Timer;

timer.start();
for loc in Locales {
    on loc {
        var firstRow = here.id*rowsPerLocale+1;
        var lastRow = (here.id+1)*rowsPerLocale;
        writeln("Running on ", here.id, ", calculating rows ", firstRow, " up to ", lastRow);

        forall (i,j) in {firstRow..lastRow,1..N} {
            for k in 1..N {
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