#include <stdio.h>
int isPrime(int num) {
	if (num == 1 || num == 2) {
		return 1;
	}	
	int i = 2;	
	while ( num % i != 0 && i < num/2) {
		i++;	
	}	
	if (num % i == 0) return 0;
	return 1;
}

int isStrong(int num) {
	int num2 = num;
	if (num == 0) {
		return 0;
	}
	while ( num2 > 0) {		
		int digit = num2 % 10;
		int factional = 1;
		for ( int i = 1 ; i <= digit ; i++) {
			factional *= i;
		}
		num -= factional;	
		num2 /= 10;
	}
	if (num == 0) {
		return 1;
	}
	return 0;
}
