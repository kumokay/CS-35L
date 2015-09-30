//translate buffered version using C library functions

//argc is the number of arguments passed in
//argv is the arguments that were passed in

//./tr "abc" "xyz" < test.txt
//cat test.txt | ./tr "abc" "xyz"
//the above would be how one would prompt our command
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc, char ** argv)
{
	if (argc != 3) //tr is argument, "abc" is argument, "xyz" is argument
	{
		printf("%s \n", "Wrong number of arguments passed in");
		exit(-1);
	}
	//argv[1] is "abc"
	//argv[2] is "xyz"
	char* firstarg = argv[1];//should not be aliased because different arguments
	char* secarg = argv[2];
	if (strlen(firstarg) != strlen(secarg))//not same length of arguments...
	{
		printf("%s \n", "Arguments are not of same length");
		exit (-1);
	}
	//Now I need to check if there are any duplicates in the first argument
	//it's okay if there are duplicates in the second argument
	int i;
	int j;
	int len = strlen(firstarg);
	for (i = 0; i < len; i++)
	{
		for (j = 0; j < len; j++)
		{
			if (j == i)
				continue;
			if (firstarg[i] == firstarg[j])
			{
				printf("%s \n", "There cannot be a duplicate in the first argument");
				exit(-1);
			}	
		}
	}
	
	//at this point, we have checked that the arguments are valid
	int c;
	while ((c = getchar()) != EOF)//read until it hits the end of file
	{
		for (i = 0; i < len; i++)
		{
			if (c == firstarg[i])//found a letter to translate
			{
				putchar(secarg[i]);
				break;
			}
			else
			{
				putchar(c);
				break;
			}
		}
	}
	printf("\n");
	return 0;
}