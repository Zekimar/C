//Kevin Good klg92
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//check sum of roll and point to see if player wins, loses, or keeps playing
// 0 is lose, 1 is win, 2 is keep playing, -1 is ERROR
int check_win(int sum, int point){
	if (point != 0){
		if (sum == 7){
			printf("You Lose!\n");
			return 0;
		}else if (sum == point){
			printf("You Win!\n");
			return 1;
		}else{
			return 2;
		}
	}
	if ((sum == 2 || sum == 3 || sum == 12) && point == 0){
		printf("You Lose!\n");
		return 0;
	}else if ((sum == 7 || sum == 11) && point == 0){
		printf("You Win!\n");
		return 1;
	}else if (point == 0){
		return 2;
	}
	//error, shouldn't ever return this
	return -1;
}
void play(){
	//seed random
	//time_t t;
	//srand((unsigned) time(&t));
	int fp;
	fp = open("/dev/dice", O_RDONLY);
	
	
	//point and sum for craps game
	int point = 0;
	int check;
	while (1){
		char rolls[2];
		read(fp, &rolls, 2);
		//rolls[0] = rand() % 6 + 1;
		//rolls[1] = rand() % 6 + 1;
		rolls[0] = rolls[0] + 1;
		rolls[1] = rolls[1] + 1;
		int sum = rolls[0] + rolls[1];
		printf("You have rolled: %d + %d = %d\n", rolls[0], rolls[1], sum);
		check = check_win(sum, point);
		if (check == 0 || check == 1){
			break;
		}
		if (check == 2 && point == 0){
			point = sum;
			printf("The point is now %d\n", point);
		}
	}	
}
int main(){
	//get player's name to print out
	printf("Welcome to Kevin's Casino! Please enter your name: ");
	char name[256];
	scanf("%s", name);
	//ask if they want to play
	while(1){
		printf("%s, would you like to play or quit? ", name);
		char choice[256];
		scanf("%s", choice);
		if (strcmp(choice, "play") == 0){
			play();
		}else if (strcmp(choice, "quit") == 0){
			break;
		}else{
			printf("err: invalid input. use lowercase.\n");
		}
	}
	
	return 0;
}