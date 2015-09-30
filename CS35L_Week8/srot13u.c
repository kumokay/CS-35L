//fprintf at the end showing # of comparisons

//sorting from stdin

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h> //for fstat
#include <sys/types.h>

/*In order to test rot13cmp initially, I created an executable called rot13 cmp
and I created a textfile with two test cases. To start it in gdb, I used
>gdb rot13cmp (name of executable I had) then (gdb) run < testcase.txt*/

/*I also got lazy and just created random executables of my program from time
to time with command > gcc -o test -g srot13.c and just kept running the executable
and checking print statements, etc (primitive debugging)*/


//SPEC TOLD ME TO PROGRAM LIKE MEMCPY BUT ALSO TOLD ME TO HAVE ARGUMENTS AS char const * b??
//or did it mean that the arguments that will be passed in are going to be char const * b?...
//int rot13cmp(char const * a, char const * b);
//I changed my original implementation to this since the compiler was giving me a warning
//qsort seemed to need to use a function that had void* arguments

int comps = 0;
int rot13cmp(const void * first, const void * second)
{
	comps ++; //counts # of comparisons
	//takes two arguments a and b and returns an int (-1, 0, 1) (a < b, =, >)
	//each points to array of non-newline bytes followed by newline byte
	//assume that both arrays are ROT13 encoded
	//newline bytes are not part of either array
	const char *a = *(const char **)first;
	const char *b = *(const char **)second;

	//I can use a and b to iterate through the arrays.
	while (1)
	{//I was constantly referring to ASCII chart when doing this
		int firstchar = (int)*a; //character from the first string (a) ASCII value
		int secondchar = (int)*b;//character from the second string (b) ASCII value

		//see if either has been terminated by newline char (ASCII value 10)
		if (firstchar == 10 && secondchar == 10)
			return 0;
		if (firstchar == 10)//first one terminated first
			return -1;
		if (secondchar == 10)//second one terminated first
			return 1;

		//Now I need to "normalize" the values to be unrotated so I can actually compare
		//I am technically not "decoding" the original string...
		int unrotone = firstchar;
		int unrottwo = secondchar;

		//checks the bounds and unrotates the firstchar
		if ((unrotone > 64 && unrotone < 78) || (unrotone > 96 && unrotone < 110))
			unrotone = unrotone + 13;//rotate forwards
		else if ((unrotone > 77 && unrotone < 91) || (unrotone > 109 && unrotone < 123))
			unrotone = unrotone - 13; //rotate backwards
		//doing the exact same thing as the second char
		if ((unrottwo > 64 && unrottwo < 78) || (unrottwo > 96 && unrottwo < 110))
			unrottwo = unrottwo + 13;//rotate forwards
		else if ((unrottwo > 77 && unrottwo < 91) || (unrottwo > 109 && unrottwo < 123))
			unrottwo = unrottwo - 13; //rotate backwards

		int cmp = unrotone - unrottwo;
		if (cmp != 0)//if they're equal, then we continue to the next character to compare
			return cmp;//if they're not, cmp will hold negative or positive value

		a++;//iterate to the next character
		b++;
	}
}

int main(void)
{
	/*int compared;        testing rot13cmp
	compared = rot13cmp("\0Whz\n", "\0Whzcf!\n");
	printf("the value is %d", compared);
	return 0;*/

	//read from standard input in rot13 format, writes to standard out in same format
	//use malloc, realloc, free and qsort to sort the data
	//read until EOF
	//handle long files, empty files, mutating files
	int allocatedspace = 2048;
	int cnt = 0;
	//READ IN FROM THE FILE!
	char * input;
	struct stat metadata; //meta data for the file information
	fstat(0, &metadata);
	int filesize = metadata.st_size;//initial filesize
	int allocsiz;
	int regular = 0;
	
	if (S_ISREG(metadata.st_mode))
	{
		input = (char*) malloc(sizeof(char) * filesize);//allocate based on file size
		regular = 1;
		if (input == NULL)
		{
			fprintf(stderr, "error, malloc failed\n");
			fprintf(stderr, "Number of comps: %d \n", comps);
			exit(1);
		}
	}
	else//non regular file
	{
		input = (char *)malloc(sizeof(char) * allocatedspace);
		if (input == NULL)
		{
			fprintf(stderr, "error, malloc failed\n");
			fprintf(stderr, "Number of comps: %d \n", comps);
			exit(1);
		}
	}
	char ch;
	int empty =0;
	while (read(0, &ch, 1) > 0)
	{
		empty = 1;
		input[cnt++] = ch;
		if (regular == 1)
			filesize = metadata.st_size;//every iteration I reload file size, easy implementation but slow
		if (regular == 1 && cnt == filesize)//realloc regular
		{
			input = (char*) realloc(input, filesize);
			if (input == NULL)
			{
				fprintf(stderr, "error, malloc failed\n");
				fprintf(stderr, "Number of comps: %d \n", comps);
				exit(1);
			}
		}
		if (regular == 0 && cnt == allocatedspace)//realloc non-regular file
		{
			allocatedspace *= 2;
			input = (char*)realloc(input, allocatedspace);
			if (input == NULL)
			{
				fprintf(stderr, "error, malloc failed\n");
				fprintf(stderr, "Number of comps: %d \n", comps);
				exit(1);
			}
		}
	}
	if (empty == 0)//escaped the while loop, it's an empty file
	{
		fprintf(stderr, "Number of comps: %d \n", comps);
		exit(1);
	}
	
	//ctrl-D to stop reading from stdi
	//Now I have a very long string supposedly separated by newlines and newline at end
	int words = 0;//# of words
	int j;
	if (input[cnt - 1] != '\n' && regular == 1)//do something to add a trailing newline to the inputs.
	{
		cnt++;//realloc one more for the new line
		input = (char*) realloc(input, cnt);
		if (input == NULL)
		{
			fprintf(stderr, "error, malloc failed\n");
			fprintf(stderr, "Number of comps: %d \n", comps);
			exit(1);
		}
		input[cnt++] = '\n';//set the nullbyte to the newline character
	}
	else//non regular file -- always appends a nullbyte to the end
	{
		if (input[cnt - 1] != '\n')
			input[cnt++] = '\n';//killing the nullbyte
	}
	
	for (j = 0; j < cnt; j++)
		if (input[j] == '\n')
			words++;//counting the number of words that are available
	
	char **allwords = (char**)malloc(sizeof(char*) * words);//array of strings
	int *sizes = (int*)malloc(sizeof(int) * words);//to keep track of the size of each string
	if (allwords == NULL || sizes == NULL)
	{
		fprintf(stderr, "error, malloc failed\n");
		fprintf(stderr, "Number of comps: %d \n", comps);
		exit(1);
	}
	//Now i have allocated array of strings to hold the words and array of ints to hold
	//the sizes of each word as well as proper error handling.
	//loading input into allwords
	int wordsi = 0; //counts through the nubmer of words in the input
	char *ptr = input; //i have a pointer to the beginning of input
	int a;//reusing an iterator, starting at 1 because I don't want to count the leading newline
	for (a = 0; a < words; a++)
	{
		if (a == 0 && *ptr == '\n')
		{
			allwords[wordsi++] = ptr;
			a++;
			ptr++;
		}
		allwords[wordsi++] = ptr;
		while (*ptr != '\n')//go until new line
			ptr++;
		ptr++;//go 1 past new line
	}

	//IMPORTANT NOTE
	//If i input something like test\none\ntwo
	//allwords[0] will be testonetwo
	//allwords[1] will be onetwo
	//allwords[2] will be two
	//I NEED my length array to keep track of this so I know what to print

	qsort(allwords, wordsi, sizeof(char*), rot13cmp);//sorting part
	
	//I had a problem with getting the length of each word in my 
	//lines so I just pulled out the letter counting for loop
	//out of my allwords loading loop
	int lenind = 0;
	int len = 1;
	for (a = 0; a < words; a++)
	{
		ptr = allwords[a];
		while(*ptr != '\n')
		{
			ptr++;
			len++;
		}
		sizes[lenind++] = len;//loading into the size of word array
		len = 1;//resetting to 1 (minimum of 1)
	}
	for (a = 0; a < words; a++)
	{
		char * word = allwords[a];
		int z;
		for (z = 0; z < sizes[a]; z++)
		{
			write(1, &(*word), 1);//writing to stdout 1 letter at a time
			word++;
		}
	}
	free(sizes);

	free(input);//I have to free all of them together because they point to the same things
	free(allwords);
	
	fprintf(stderr, "Number of comps: %d \n", comps);
	return 0; //the program has succeeded
}