use Random;
use Math;
use Time;

config const numRuns = 1e9:int;


var timer:Timer;

timer.start();

var xs: [1..numRuns] real;
var ys: [1..numRuns] real;
fillRandom(xs);
fillRandom(ys);

var inside = 0;
forall (x,y) in zip(xs,ys) with (+ reduce inside) {
    var r = sqrt(x*x+y*y);
    if (r < 1){
        inside+=1;
    }
}

var pi = inside:real*4/numRuns:real;
timer.stop();

writeln("Approximation over ", numRuns, " steps took ", timer.elapsed(), " seconds, approximation of pi is ", pi);