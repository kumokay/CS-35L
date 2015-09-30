//sorting from stdin

#include <stdio.h>
#include <stdlib.h>

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
int rot13cmp(const void * first, const void * second)
{
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
	int allocatedspace = 1000;
	char *input = (char*)malloc(sizeof(char) * allocatedspace);//allocate a lot of space
	if (input == NULL)
	{
		fprintf(stderr, "error, malloc failed");
		exit(1);
	}
	int i = 0;// i will hold the index of the letters
	int empty = 1;//initially empty?
	while (1)
	{
		int in;
		in = getchar();
		if (in == EOF)
			break;	//keep reading until you hit end of file
		empty = 0;
		input[i] = (char)in;	//add the character to the input
		i++;	//iterate i
		if (i == allocatedspace){	//i need to create more space
			allocatedspace *= 2;
			input = (char*)realloc(input, allocatedspace);
			if (input == NULL){
				fprintf(stderr, "error, malloc failed");
				exit(1);
			}
		}
	}
	//ctrl-D to stop reading from stdin
	if (empty == 1)//if it was empty file, then I will just exit
		return 0;
	if (input[i - 1] != '\n')//do something to add a trailing newline to the inputs.
	{
		input[i] = '\n';//set the nullbyte to the newline character
		i++;
	}

	//Now I have a very long string supposedly separated by newlines and newline at end
	int words = 0;
	int j = 0;
	for (; j < i; j++)
		if (input[j] == '\n')
			words++;//counting the number of words that are available

	char **allwords = (char**)malloc(sizeof(char*) * words);//array of strings
	int *sizes = (int*)malloc(sizeof(int) * words);//to keep track of the size of each string
	if (allwords == NULL || sizes == NULL)
	{
		fprintf(stderr, "error, malloc failed");
		exit(1);
	}
	//Now i have allocated array of strings to hold the words and array of ints to hold
	//the sizes of each word as well as proper error handling.
	//loading input into allwords
	int wordsi = 0; //counts through the nubmer of words in the input
	char *ptr = input; //i have a pointer to the beginning of input
	i = 0;//reusing an iterator, starting at 1 because I don't want to count the leading newline
	if (*ptr == '\n')
	{
		allwords[wordsi] = ptr;
		wordsi++;
		i = 1;
		ptr++;
	}
	for (; i < words; i++)
	{
		allwords[wordsi] = ptr;//i didn't need to copy the entire letters over
						//i simply needed each char ptr point to the beginning of the string
		int len = 1;
		while (*ptr != '\n')//i iterate pointer to the next word
		{
			len++;//counting length of a word
			ptr++;
		}
		sizes[wordsi] = len;
		ptr++;//go past the new line
		wordsi++;//go to the next word
	}

	//IMPORTANT NOTE
	//If i input something like test\none\ntwo
	//allwords[0] will be testonetwo
	//allwords[1] will be onetwo
	//allwords[2] will be two
	//I NEED my length array to keep track of this so I know what to print

	qsort(allwords, wordsi, sizeof(char*), rot13cmp);//sorting part

	//REUSING THE ITERATOR TO PRINT
	i = 0;
	for (; i < wordsi; i++)
	{
		char * word = allwords[i];
		int z = 0;
		for (; z < sizes[i]; z++)
		{
			printf("%c", *word);//print char by char based on the size of the collected word
			word++;
		}
	}
	free(sizes);

	free(input);//I have to free all of them together because they point to the same things
	free(allwords);
	return 0; //the program has succeeded
}