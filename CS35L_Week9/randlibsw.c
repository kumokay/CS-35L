//randlibsw.c
//contains software implementation of random number generator
//starts w/ including randlib.h, implements interface described by randlibh
//needs initialization and finalization, implementation also defines initializer + finalizer func
//using GCC's "__attribute__((constructor)) and __attribute__((destructor)) declaration specifiers
//i will create a software

#include "randlib.h"
#include <stdio.h>	//for fopen, fclose, fread
#include <stdlib.h> //for abort

/* Software implementation.  */

/* Input stream containing random bytes.  */
static FILE *urandstream;

/* Initialize the software rand64 implementation.  */
__attribute__((constructor))
void software_rand64_init (void)
{
  urandstream = fopen ("/dev/urandom", "r");
  if (! urandstream)
    abort ();
}

/* Return a random value, using software operations.  */
extern unsigned long long
software_rand64 (void)
{
  unsigned long long int x;
  if (fread (&x, sizeof x, 1, urandstream) != 1)
    abort ();
  return x;
}

/* Finalize the software rand64 implementation.  */
__attribute__((destructor))
void software_rand64_fini (void)
{
  fclose (urandstream);
}