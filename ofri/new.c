#include <stdio.h>
#include "NumClass.h"

int main()
{
	int num1, num2;
	//printf("Enter your first number: ");
	scanf("%d", &num1);
	//printf("\nEnter your second number: ");
	scanf("%d", &num2);
	printf("The Armstrong numbers are:");
	for(int i=num1; i<=num2; i++)
	{
		int result = isArmstrong(i);
		if (result==1)
		{
			printf(" %d", i);
		}
	}
	printf("\nThe Palindromes are:");
	for(int i=num1; i<=num2; i++)
	{
		int result = isPalindrome(i);
		if (result==1)
		{
			printf(" %d", i);
		}
	}
	printf("\nThe Prime numbers are:");
	for(int i=num1; i<=num2; i++)
	{
		int result = isPrime(i);
		if (result==1)
		{
			printf(" %d", i);
		}
	}
	printf("\nThe Strong numbers are:");
	for(int i=num1; i<=num2; i++)
	{
		int result = isStrong(i);
		if (result==1)
		{
			printf(" %d", i);
		}
	}
	printf("\n");
	return 0;
}
