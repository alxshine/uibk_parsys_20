use Random;
use Math;
use Time;

config const numRuns = 1e9:int;
config const numThreads = 8:int;

const runsPerThread = numRuns/numThreads;
var timer:Timer;
var randStream = new RandomStream(real);

timer.start();

var inside = 0;
coforall i in 1..numThreads with (+ reduce inside) {
    var xs: [1..runsPerThread] real;
    randStream.fillRandom(xs);
    var ys: [1..runsPerThread] real;
    randStream.fillRandom(ys);

    for (x,y) in zip(xs,ys){
        inside += (x*x+y*y < 1):int;
    }
}

var pi = inside:real*4/numRuns:real;
timer.stop();

writeln("Approximation over ", numRuns, " steps took ", timer.elapsed(), " seconds, approximation of pi is ", pi);