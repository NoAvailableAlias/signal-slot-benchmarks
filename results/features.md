# Activation

1. Will the callback be called if the signal is triggered?
2. Are the callbacks called in the order they are registered?
3. Will the signal not execute a callback registered while the signal is triggered?
4. Will the signal not execute a callback unregistered while the signal is triggered?
5. Can a signal be triggered while it is triggered?
6. Will the registration of the same callback twice will cause the callback to be called twice by triggering the signal?

 Library |    1    |    2    |    3    |    4    |    5    |    6    
---------|---------|---------|---------|---------|---------|---------
 aco     | **yes** | **yes** |   no    |   no    | **yes** | **yes** 
 asg     | **yes** | **yes** |   no    | **yes** | **yes** | **yes** 
 bs2     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 cls     | **yes** |   no    |    X    |    X    |    X    | **yes** 
 cps     | **yes** |   no    |    X    |    X    |    X    | **yes** 
 cps_st  | **yes** |   no    | **yes** |   no    | **yes** | **yes** 
 css     | **yes** | **yes** |   no    | **yes** | **yes** | **yes** 
 dob     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 evl     | **yes** | **yes** |   no    |   no    | **yes** | **yes** 
 ics     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 jls     | **yes** | **yes** |   no    | **yes** | **yes** | **yes** 
 jos     | **yes** | **yes** |   no    | **yes** | **yes** | **yes** 
 ksc     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 lfs     | **yes** | **yes** |   no    | **yes** | **yes** | **yes** 
 lss     | **yes** | **yes** |   no    | **yes** | **yes** | **yes** 
 mws     | **yes** | **yes** |   no    | **yes** | **yes** | **yes** 
 nes     | **yes** | **yes** |   no    | **yes** | **yes** | **yes** 
 nls     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 nls_st  | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 nod     | **yes** | **yes** | **yes** |   no    | **yes** | **yes** 
 nod_st  | **yes** | **yes** | **yes** |   no    | **yes** | **yes** 
 nss_sts | **yes** |   no    | **yes** |   no    | **yes** | **yes** 
 nss_ts  | **yes** |   no    |    X    |    X    |    X    | **yes** 
 nss_tss | **yes** |   no    |   no    |   no    | **yes** | **yes** 

# Activation with argument

1. Can the callbacks have an argument?
2. Will triggering the signal not make unecessary copies of its argument?

 Library |    1    |    2    
---------|---------|---------
 aco     | **yes** | **yes** 
 asg     | **yes** | **yes** 
 bs2     | **yes** | **yes** 
 cls     | **yes** | **yes** 
 cps     | **yes** | **yes** 
 cps_st  | **yes** | **yes** 
 css     | **yes** | **yes** 
 dob     | **yes** | **yes** 
 evl     | **yes** | **yes** 
 ics     | **yes** | **yes** 
 jls     | **yes** | **yes** 
 jos     | **yes** | **yes** 
 ksc     | **yes** | **yes** 
 lfs     | **yes** | **yes** 
 lss     | **yes** | **yes** 
 mws     | **yes** | **yes** 
 nes     | **yes** | **yes** 
 nls     | **yes** | **yes** 
 nls_st  | **yes** | **yes** 
 nod     | **yes** | **yes** 
 nod_st  | **yes** | **yes** 
 nss_sts | **yes** | **yes** 
 nss_ts  | **yes** | **yes** 
 nss_tss | **yes** | **yes** 

# Connection management

1. Is the signal empty when constructed?
2. Is the signal not empty when a callback is registered?
3. Will the callback be called if the signal is triggered even if the connection is not stored?
4. Will the callback not be called if the signal is triggered once the connection has been cut?
5. Will the callback not be called if the signal is cleared?

 Library |    1    |    2    |    3    |    4    |    5    
---------|---------|---------|---------|---------|---------
 aco     |    ?    |    ?    |   no    | **yes** | **yes** 
 asg     |    ?    |    ?    | **yes** | **yes** | **yes** 
 bs2     | **yes** | **yes** | **yes** | **yes** | **yes** 
 cls     |    ?    |    ?    |   no    | **yes** | **yes** 
 cps     |    ?    |    ?    |   no    | **yes** |   no    
 cps_st  |    ?    |    ?    |   no    | **yes** |   no    
 css     |    ?    |    ?    |   no    | **yes** |   n/a   
 dob     |    ?    |    ?    | **yes** | **yes** |   no    
 evl     |    ?    |    ?    |   no    | **yes** | **yes** 
 ics     | **yes** | **yes** | **yes** | **yes** | **yes** 
 jls     | **yes** | **yes** |   no    | **yes** | **yes** 
 jos     |    ?    |    ?    | **yes** | **yes** | **yes** 
 ksc     |    ?    |    ?    | **yes** | **yes** | **yes** 
 lfs     | **yes** | **yes** | **yes** | **yes** | **yes** 
 lss     | **yes** | **yes** | **yes** | **yes** |   n/a   
 mws     | **yes** | **yes** | **yes** | **yes** |   no    
 nes     | **yes** | **yes** | **yes** | **yes** | **yes** 
 nls     |    ?    |    ?    | **yes** | **yes** | **yes** 
 nls_st  |    ?    |    ?    | **yes** | **yes** | **yes** 
 nod     | **yes** | **yes** | **yes** | **yes** | **yes** 
 nod_st  | **yes** | **yes** | **yes** | **yes** | **yes** 
 nss_sts | **yes** | **yes** |   no    | **yes** | **yes** 
 nss_ts  | **yes** | **yes** |   no    | **yes** | **yes** 
 nss_tss | **yes** | **yes** |   no    | **yes** | **yes** 

# Swap

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
 cls     |   no    |   no    |   no    |   no    |   no    |   no    |    X    |   no    
 cps     | **yes** |   no    |   no    |   no    |   no    |   no    |    X    |   no    
 cps_st  | **yes** |   no    |   no    |   no    |   no    |   no    |   no    |   no    
 css     |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 dob     | **yes** |   no    |   no    |   no    |   no    |   no    |   no    |   no    
 evl     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** |   no    
 ics     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 jls     | **yes** | **yes** | **yes** |   no    |   no    |   no    |   no    |   no    
 jos     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** |   no    | **yes** 
 ksc     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** 
 lfs     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** |   no    
 lss     |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 mws     |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 nes     | **yes** | **yes** | **yes** | **yes** | **yes** | **yes** |   no    | **yes** 
 nls     | **yes** |   no    |   no    |   no    |   no    |   no    |   no    |   no    
 nls_st  | **yes** |   no    |   no    |   no    |   no    |   no    |   no    |   no    
 nod     |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 nod_st  |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 nss_sts |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   
 nss_ts  |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |    X    |   n/a   
 nss_tss |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   |   n/a   


