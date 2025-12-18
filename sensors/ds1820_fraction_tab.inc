
;we have only 16 possible fraction - there is no reason to calculate it
;better to simply translate each possible ds18b20 fraction byte to its 
;numerical representation
;created with Python
;for i in range(0, 16): print("dt ",round(10*(i * 1/16.))," ; ", i*1/16. )
;by seperating it we will not have issue with loacting in ROM
;best will be to place somwehre in the code beginning - to never observe pages crossing
tab_fraction
        addwf PCL,f 
        dt  0  ;  0.0
        dt  1  ;  0.0625
        dt  1  ;  0.125
        dt  2  ;  0.1875
        dt  2  ;  0.25
        dt  3  ;  0.3125
        dt  4  ;  0.375
        dt  4  ;  0.4375
        dt  5  ;  0.5
        dt  6  ;  0.5625
        dt  6  ;  0.625
        dt  7  ;  0.6875
        dt  8  ;  0.75
        dt  8  ;  0.8125
        dt  9  ;  0.875
        dt  9  ;  0.9375