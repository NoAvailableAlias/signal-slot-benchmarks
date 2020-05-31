# Feature Tests

This page displays the results of several tests on the features of the signal libraries. See below for a legend of the output:

* **yes**: the test passed successfully,
* no: the test did not produce the expected result,
* n/a: the library does not provide the API required for this test,
* X: the test crashed in a way or another.

The tests check the behaviour of the library when the signal is activated in various ways, how the connections are handled and what happens when signals are swapped.

None of the tested libraries passes all the tests.

On the tests that could be run (i.e. tests for which the required API is available in the tested library), none of the tested libraries passes all the tests.

# Detailed Test results

## Activation

1. Will the callback be called if the signal is triggered?
2. Are the callbacks called in the order they are registered?
3. Will the signal not execute a callback registered while the signal is triggered?
4. Will the signal not execute a callback unregistered while the signal is triggered?
5. Can a signal be triggered while it is triggered?
6. Will the registration of the same callback twice will cause the callback to be called twice by triggering the signal?

 Library |    1    |    2    |    3    |    4    |    5    |    6    
---------|---------|---------|---------|---------|---------|---------
 aco     | **yes** | **yes** |    X    |   no    | **yes** |   n/a   
 asg     | **yes** | **yes** |   no    | **yes** | **yes** | **yes** 
 bs2     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 bs2_st  | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 cls     | **yes** |   no    |    X    |    X    |    X    | **yes** 
 cps     | **yes** |   no    |    X    |    X    |    X    |   n/a   
 cps_st  | **yes** |   no    | **yes** |   no    | **yes** |   n/a   
 css     | **yes** | **yes** |   no    | **yes** | **yes** |   n/a   
 dob     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 evl     | **yes** | **yes** |    X    |    X    | **yes** | **yes** 
 ics     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 jls     | **yes** | **yes** |   no    | **yes** | **yes** |   n/a   
 jos     | **yes** | **yes** |   no    | **yes** | **yes** | **yes** 
 ksc     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 ktn     | **yes** | **yes** | **yes** | **yes** |   no    |   no    
 lfs     | **yes** | **yes** |   no    | **yes** | **yes** | **yes** 
 lss     | **yes** | **yes** |   no    | **yes** | **yes** | **yes** 
 mws     | **yes** | **yes** |   no    | **yes** | **yes** | **yes** 
 nes     | **yes** | **yes** |    X    | **yes** | **yes** |   n/a   
 nls     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 nls_st  | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 nod     | **yes** | **yes** | **yes** |   no    | **yes** | **yes** 
 nod_st  | **yes** | **yes** | **yes** |   no    | **yes** | **yes** 
 nss_st  | **yes** | **yes** | **yes** |   no    | **yes** |   n/a   
 nss_sts | **yes** | **yes** | **yes** |   no    | **yes** |   n/a   
 nss_ts  | **yes** |   no    |    X    |    X    |    X    |   n/a   
 nss_tss | **yes** | **yes** | **yes** |   no    | **yes** |   n/a   
 psg     | **yes** | **yes** | **yes** |   no    | **yes** |   n/a   
 pss     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 pss_st  | **yes** | **yes** |    X    | **yes** | **yes** | **yes** 
 sss     | **yes** | **yes** |   no    | **yes** | **yes** |   n/a   
 vdk     | **yes** | **yes** |   no    | **yes** | **yes** |   n/a   
 wnk     | **yes** | **yes** |    X    | **yes** | **yes** |   n/a   
 yas     | **yes** | **yes** |    X    |    X    | **yes** |   n/a   

## Activation with argument

1. Can the callbacks have an argument?
2. Will triggering the signal with an argument by reference not make unecessary copies of its argument?
3. Will triggering the signal with an argument by value not make unecessary copies of its argument?
4. Will triggering the signal with a temporary will pass it to every callback?

 Library |    1    |    2    |    3    |    4    
---------|---------|---------|---------|---------
 aco     | **yes** | **yes** |   no    | **yes** 
 asg     | **yes** | **yes** |   no    | **yes** 
 bs2     | **yes** | **yes** |   no    | **yes** 
 bs2_st  | **yes** | **yes** |   no    | **yes** 
 cls     | **yes** | **yes** |   no    |   no    
 cps     | **yes** | **yes** |   no    |   no    
 cps_st  | **yes** | **yes** |   no    |   no    
 css     | **yes** | **yes** |   no    | **yes** 
 dob     | **yes** | **yes** |   no    |   no    
 evl     | **yes** | **yes** |   no    | **yes** 
 ics     | **yes** | **yes** |   no    |   no    
 jls     | **yes** | **yes** |   no    | **yes** 
 jos     | **yes** | **yes** |   no    | **yes** 
 ksc     | **yes** | **yes** |   no    |   no    
 ktn     | **yes** | **yes** |   no    |   no    
 lfs     | **yes** | **yes** | **yes** | **yes** 
 lss     | **yes** | **yes** |   no    | **yes** 
 mws     | **yes** | **yes** |   no    | **yes** 
 nes     | **yes** | **yes** |   no    |   no    
 nls     | **yes** | **yes** |   no    |   no    
 nls_st  | **yes** | **yes** |   no    |   no    
 nod     | **yes** | **yes** |   no    | **yes** 
 nod_st  | **yes** | **yes** |   no    | **yes** 
 nss_st  | **yes** | **yes** |   no    |   no    
 nss_sts | **yes** | **yes** |   no    |   no    
 nss_ts  | **yes** | **yes** |   no    |   no    
 nss_tss | **yes** | **yes** |   no    |   no    
 psg     | **yes** | **yes** |   no    | **yes** 
 pss     | **yes** | **yes** |   no    | **yes** 
 pss_st  | **yes** | **yes** |   no    | **yes** 
 sss     | **yes** | **yes** |   no    | **yes** 
 vdk     | **yes** | **yes** |   no    | **yes** 
 wnk     | **yes** | **yes** |   no    |   no    
 yas     | **yes** | **yes** |   no    |   no    

## Connection management

1. Is the signal empty when constructed?
2. Is the signal not empty when a callback is registered?
3. Will the callback be called if the signal is triggered even if the connection is not stored?
4. Will the callback not be called if the signal is triggered once the connection has been cut?
5. Will the callback not be called if the signal is cleared?

 Library |    1    |    2    |    3    |    4    |    5    
---------|---------|---------|---------|---------|---------
 aco     |   n/a   |   n/a   |   no    | **yes** | **yes** 
 asg     |   n/a   |   n/a   | **yes** | **yes** | **yes** 
 bs2     | **yes** | **yes** | **yes** | **yes** | **yes** 
 bs2_st  | **yes** | **yes** | **yes** | **yes** | **yes** 
 cls     |   n/a   |   n/a   |   no    | **yes** | **yes** 
 cps     |   n/a   |   n/a   |   no    | **yes** |   no    
 cps_st  |   n/a   |   n/a   |   no    | **yes** |   no    
 css     |   n/a   |   n/a   |   no    | **yes** |   n/a   
 dob     |   n/a   |   n/a   | **yes** | **yes** |   no    
 evl     |   n/a   |   n/a   |   no    | **yes** | **yes** 
 ics     | **yes** | **yes** | **yes** | **yes** | **yes** 
 jls     | **yes** | **yes** |   no    | **yes** | **yes** 
 jos     |   n/a   |   n/a   | **yes** | **yes** | **yes** 
 ksc     |   n/a   |   n/a   | **yes** | **yes** | **yes** 
 ktn     | **yes** | **yes** | **yes** | **yes** | **yes** 
 lfs     | **yes** | **yes** | **yes** | **yes** | **yes** 
 lss     | **yes** | **yes** | **yes** | **yes** |   n/a   
 mws     | **yes** | **yes** | **yes** | **yes** |   no    
 nes     | **yes** | **yes** | **yes** | **yes** | **yes** 
 nls     |   n/a   |   n/a   | **yes** | **yes** | **yes** 
 nls_st  |   n/a   |   n/a   | **yes** | **yes** | **yes** 
 nod     | **yes** | **yes** | **yes** | **yes** | **yes** 
 nod_st  | **yes** | **yes** | **yes** | **yes** | **yes** 
 nss_st  | **yes** | **yes** |   no    | **yes** | **yes** 
 nss_sts | **yes** | **yes** |   no    | **yes** | **yes** 
 nss_ts  | **yes** | **yes** |   no    | **yes** | **yes** 
 nss_tss | **yes** | **yes** |   no    | **yes** | **yes** 
 psg     | **yes** | **yes** | **yes** | **yes** | **yes** 
 pss     |   n/a   |   n/a   | **yes** | **yes** | **yes** 
 pss_st  |   n/a   |   n/a   | **yes** | **yes** | **yes** 
 sss     |   n/a   |   n/a   |   no    | **yes** |    X    
 vdk     | **yes** | **yes** | **yes** | **yes** | **yes** 
 wnk     | **yes** | **yes** | **yes** | **yes** | **yes** 
 yas     |   n/a   |   n/a   | **yes** | **yes** | **yes** 

## Swap

1. Can an empty signal be swapped with another empty signal?
2. Can an empty signal be swapped with a signal with one callback?
3. Can an empty signal be swapped with a signal with two callbacks?
4. Can a signal with one callback be swapped with another signal with one callback?
5. Can a signal with one callback be swapped with another signal with two callbacks?
6. Can a signal with two callbacks be swapped with another signal with two callbacks?
7. Can a signal be swapped with another signal while it is triggered?
8. When swapping two signals, will the connections be swapped too?

 Library |    1    |    2    |    3    |    4    |    5    |    6    |    7    |    8    
---------|---------|---------|---------|---------|---------|---------|---------|---------
 aco     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** |   no    | **yes** 
 asg     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** |   no    | **yes** 
 bs2     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 bs2_st  | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 cls     |    X    |    X    |    X    |    X    |    X    |    X    |    X    |    X    
 cps     | **yes** |   no    |   no    |   no    |   no    |   no    |    X    |   no    
 cps_st  | **yes** |   no    |   no    |   no    |   no    |   no    |   no    |   no    
 css     |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 dob     | **yes** |   no    |   no    |   no    |   no    |   no    |   no    |   no    
 evl     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** |   no    
 ics     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 jls     | **yes** | **yes** | **yes** |    X    |    X    |    X    |   no    |    X    
 jos     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** |    X    | **yes** 
 ksc     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 ktn     |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 lfs     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** |   no    
 lss     |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 mws     |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 nes     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** |   no    | **yes** 
 nls     | **yes** |   no    |   no    |   no    |   no    |   no    |   no    |   no    
 nls_st  | **yes** |   no    |   no    |   no    |   no    |   no    |   no    |   no    
 nod     |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 nod_st  |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 nss_st  |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 nss_sts |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 nss_ts  |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 nss_tss |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 psg     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** |    X    | **yes** 
 pss     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 pss_st  | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 sss     | **yes** |    X    |    X    |    X    |    X    |    X    |    X    |    X    
 vdk     |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 wnk     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** |    X    | **yes** 
 yas     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
