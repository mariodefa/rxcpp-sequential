#include <rxcpp/rx.hpp>
#include <iostream>
#include <chrono>
#include <string>
#include <vector>

/* promt:
 tick: 1
 tick: 2
 tick: 3
 tick: 4
 tick: 5
 tick: 6
 ,call1,call2,call3
 tick: 7
 tick: 8
 tick: 9
 Tick Completed!
--------------------------------
interpretation:
tick: 1 // end of call1, call2 starts
 tick: 2
 tick: 3 // end of call2, call3 starts
 tick: 4
 tick: 5
 tick: 6 // end of call3
 ,call1,call2,call3 //printed result in event_loop thread, see .subscribe code chunck
 tick: 7
 tick: 8
 tick: 9
 Tick Completed!
*/

/*
Returned observable pipe will be executed in separated threads if necessary
*/
rxcpp::observable<int> startAsynchronousStream() {
    return rxcpp::observable<>::just(42)//random number, doesn't matter
        .subscribe_on(rxcpp::observe_on_event_loop())
        .map([](int e) { std::this_thread::yield(); return e; });
}

/*
It Simulates a slow, time-consuming task
just for demo purposes
*/
rxcpp::observable<std::string> slow_task_sim(int idSec) {
    return startAsynchronousStream()
        .map([idSec](int i){return "call"+std::to_string(idSec);})
        .delay(std::chrono::seconds(idSec));
}

/*
It generates one event every second starting at time 0.5s
So first event is at 1.5s
*/
rxcpp::observable<int> secondsGenerator(){
    //using starttick 500 ms to give advantage to resultChannel, so clockChannel starts 500ms later than resultChannel
    rxcpp::observable<int> startTick = startAsynchronousStream()
        .map([](int i){return 0;})
        .delay(std::chrono::milliseconds(500));
    rxcpp::observable<int> oneSecTicks = rxcpp::observable<>::interval(std::chrono::seconds(1))
        .filter([](int i){return i!=1;})//discard inmidiate first tick 1
        .map([](int t) { return t-1; });
    return startTick.concat(oneSecTicks)
        .filter([](int i){return i!=0;});//discard startTick fake tick 0
}

/*
Sequential execution example
launch 3 slow tasks in sequence
then print the result
*/
int main() {    
    auto delay1s = slow_task_sim(1);

    auto delay2s = slow_task_sim(2);

    auto delay3s = slow_task_sim(3);

    //join observables
    std::vector<rxcpp::observable<std::string>> allCalls = {delay1s, delay2s, delay3s};
    rxcpp::observable<rxcpp::observable<std::string>> allCallsWrapper = rxcpp::observable<>::iterate(allCalls);
    
    //run them in sequence
    rxcpp::observable<std::string> resultChannel = allCallsWrapper   
        .concat()
        .scan(std::string(" "), [](std::string acc, std::string cur) { return acc + "," + cur; })//accumulate them
        .buffer(3)//collect them altogether, output will looks like ["1","12","123"]
        .map([](const std::vector<std::string>& results) {
            return results.back();
        });//take the latest as result, so following previous example it takes "123" as result  

    rxcpp::observable<std::string> clockChannel = secondsGenerator()
        .take(9)        
        .map([](int value) { return " tick: "+std::to_string(value); });        

    //clockChannel starts 500ms later than resultChannel
    rxcpp::observable<>::from(clockChannel, resultChannel)          
        .merge()//run clock and allCalls in parallel
        .observe_on(rxcpp::observe_on_event_loop())//print stuff in event_loop thread only
        .as_blocking()
        .subscribe(
            [](const std::string& value) { std::cout << value << std::endl; },
            []() { std::cout << " Tick Completed!" << std::endl; }
        );

    return 0;
}
