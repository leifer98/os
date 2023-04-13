#include <stdio.h>
int isArmstrong(int num)
{
	int num2 = num, length = 0;
	while ( num2 > 0) {		
		num2 /= 10;
		length++;
	}
	num2 = num;
	while ( num2 > 0 ) {
		int digit = num2 % 10;		
		num2 /= 10;
		int power = 1;
		for (int i = 0 ; i < length ; i++) {
			power = power * digit;
		}
		num -= power;
	}
	
	if (num == 0) {
		return 1;
	}
	return 0;
}

int isPalindrome(int num)
{
	int size = 1, num2 = num/10, count = 1;
	while (num2 > 0) {
		size *= 10;
		num2 /= 10;
		count ++;
	}
	count /= 2;
	num2 = num;
	int ones;
	while (count > 0) {
		ones = num2 % 10;
		if (ones != num2/size) {
			return 0;
		}
		num2 %= size;
		num2 /= 10;
		size /= 100;
		count--;
	}
	return 1;
}
