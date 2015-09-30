//randmain.c
//main program that glues everything together, includes randcpuidh
//depending on whether randcpuid reports that hardware supports RDRAND insn, main 
//dynamically links hardware oriented or software oriented implementation of randlib
//doing dynamic linking via dlopen and dlsym, calls dlclose to clean up before exiting

#include "randcpuid.h"
#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Main program, which outputs N bytes of random data.  */
int
main (int argc, char **argv)
{
  /* Check arguments.  */
  bool valid = false;
  long long nbytes;
  if (argc == 2)
    {
      char *endptr;
      errno = 0;
      nbytes = strtoll (argv[1], &endptr, 10);
      if (errno)
	perror (argv[1]);
      else
	valid = !*endptr && 0 <= nbytes;
    }
  if (!valid)
    {
      fprintf (stderr, "%s: usage: %s NBYTES\n", argv[0], argv[0]);
      return 1;
    }

  /* If there's no work to do, don't worry about which library to use.  */
  if (nbytes == 0)
    return 0;

  /* Now that we know we have work to do, arrange to use the
   appropriate library.  */
	/*******************
	START MODIFYING HERE
	*********************/
  void (*initialize) (void);
  unsigned long long (*rand64) (void);
  void (*finalize) (void);
  
	char * err = NULL; //to check out error messages
	void * sw;
	void * hw;
  
  //This if-else block will initialize the function pointers for main
  
  if (rdrand_supported ())//rdrand_supported() is statically linked, don't need to do any
  {	//I call the hardware library's functions
	hw = dlopen("randlibhw.so", RTLD_NOW);//TODO -- get path of hardware library
	if (hw == NULL){//error loading library
		printf("failed to open library: %s\n", dlerror());
		exit(1);
	}
	sw = NULL;
    initialize = dlsym(hw, "hardware_rand64_init");
	err = dlerror();
	if (err){
		printf("failed to locate init function %s\n", err);
		exit(1);
	}
	initialize(); //does this even do anything??...
    rand64 = dlsym(hw, "hardware_rand64");
	err = dlerror();
	if (err){
		printf("failed to locate rand64 function %s\n", err);
		exit(1);
	}
	  
    finalize = dlsym(hw, "hardware_rand64_fini");
	err = dlerror();
	if (err){
		printf("failed to locate fini function %s\n", err);
		exit(1);
	}
	finalize();//empty function couldnt hurt b/c I commented out
				//the other ones
  }
  else
   {//I call the software library's functions
	sw = dlopen("randlibsw.so", RTLD_NOW);//TODO -- get path of software library
	if (sw == NULL){//error loading library
		printf("failed to open library: %s\n", dlerror());
		exit(1);
	}	
	hw = NULL;
      initialize = dlsym(sw, "software_rand64_init");
	  err = dlerror();
	  if (err){
		printf("failed to locate init function %s\n", err);
		exit(1);
	  }
	  
      rand64 = dlsym(sw, "software_rand64");
	  err = dlerror();
	  if (err){
		printf("failed to locate init function %s\n", err);
		exit(1);
	  }
	  
      finalize = dlsym(sw, "software_rand64_fini");
	  err = dlerror();
	  if (err){
		printf("failed to locate init function %s\n", err);
		exit(1);
	  }
    }

	//end modifying
	
  //initialize (); --> software was already initialized w/ attribute constructor
  int wordsize = sizeof rand64 ();
  int output_errno = 0;
  
  do
    {
      unsigned long long x = rand64 ();
      size_t outbytes = nbytes < wordsize ? nbytes : wordsize;
      if (fwrite (&x, 1, outbytes, stdout) != outbytes)
	{
	  output_errno = errno;
	  break;
	}
      nbytes -= outbytes;
    }
  while (0 < nbytes);

  if (fclose (stdout) != 0)
    output_errno = errno;

  if (output_errno)
    {
      errno = output_errno;
      perror ("output");
      finalize ();
      return 1;
    }

  //finalize (); -- attribute destructor takes care
  
  if (hw == NULL)
	dlclose(sw);
  else
	dlclose(hw);
  
  return 0;
}

