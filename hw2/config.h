/*********************************************************
 * config.h - Configuration data for the driver.c program.
 *********************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

/* 
 * CPEs for the baseline (naive) version of the rotate function that
 * was handed out to the students. Rd is the measured CPE for a dxd
 * image. Run the driver.c program on your system to get these
 * numbers.  
 */
#define R64    5.1
#define R128   6.4
#define R256   10.7
#define R512   15.3
#define R1024  20.4
#define R2048  102.3
#define R4096  114.1
#define R8192  183.6 // not used, not yet measured on UG machines



/*
 * Baseline hitrates for 16k 2 way set assosiative cache 
 * NOT USED IN 2010
 */
#define C64   0.989       
#define C128  0.932       
#define C256  0.932       
#define C512  0.932       
#define C1024 0.932 
#define C2048  0.932
#define C4096  0.932
#define C8192  0.933
#endif /* _CONFIG_H_ */
