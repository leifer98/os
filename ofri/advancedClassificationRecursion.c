#include <stdio.h>
int power(int num, int n) {
	if (n == 0) {
		return 1;
	}
	int pow = power(num, n-1);
	return (pow*num);
}

int len(int num) {
	if (num == 0) {
		return 0;
	}
	int length = len(num/10);
	return length+1;
}
int isArmstrongRecursive(int countdown, int numForOnes,int num, int length) {
	if (countdown == 0 && numForOnes == 0) {
		return 1;
	}
	if (countdown < 0 || numForOnes == 0) {
		return 0;
	}
	int sum = power((numForOnes%10), length);
	return isArmstrongRecursive((countdown-sum), (numForOnes/10), num, length);
}
int isArmstrong(int num)  {
	int length = len(num) ;
	return isArmstrongRecursive(num, num, num, length);
}

int isPalindrome2(int num, int length)  {
    if (length < 2) {
        return 1;
    }
   
    int ones = num % 10;
    int size = power(10, length-1);
    int bigs = num / size;
    if (ones != bigs) {
        return 0;
    }
   
    return isPalindrome2((num%size)/10, length-2);
}

int isPalindrome(int num)  {
    int length = len(num) ;
    if (length < 2) {
        return 1;
    }
 
    return isPalindrome2(num, length);
}
