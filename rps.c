#include <stdio.h>

int main(){
	//variables to track status of game
	int playerScore = 0;
	int computerScore = 0;

	//variables to generate random numbers
	int hi = 3;
	int lo = 1;
	
	printf("Welcome to Rock, Paper, Scissors by Kevin Good!\n");
	//seed rand
	srand((unsigned int)time(NULL));

	//while loop to make sure we can keep going if they want to
	while (1){

		//see if user wants to play
		char choice[10];
		char line[100];
		printf("Would you like to play?:\n");
		fgets(line, sizeof(line), stdin);
		sscanf(line, "%s", &choice);

		//determine what user said in response to prompt
		if (strcmp(choice, "yes") == 0){
			while (playerScore < 3 && computerScore < 3){

				//determine computer's choice in the game.
				char* computerChoice;
				int random = rand() % (hi - lo + 1) + lo;
				switch(random){
					case 1:
						computerChoice = "rock";
						break;
					case 2:
						computerChoice = "paper";
						break;
					case 3:
						computerChoice = "scissors";
						break;
					default:
						printf("error in input. restart.\n");
						break;
				}
				//get player's choice for game
				char playerChoice[10];
				char line2[100];
				printf("Enter your choice, please use lowercase:\n");
				fgets(line2, sizeof(line2), stdin);
				sscanf(line2, "%s", &playerChoice);
				printf("computer chooses: %s\n", computerChoice);
				
				//modify scores if player is rock
				if(strcmp(playerChoice,"rock") == 0){
					if (strcmp(computerChoice,"rock") == 0){
						printf("Both Players chose rock. It is a tie.\n");
					}else if (strcmp(computerChoice,"paper") == 0){
						computerScore = computerScore + 1;
						printf("User lost!\n");
					}else if (strcmp(computerChoice,"scissors") == 0){
						playerScore = playerScore + 1;
						printf("User won!\n");
					}
				//modify scores if player is paper
				}else if (strcmp(playerChoice,"paper") == 0){
					if (strcmp(computerChoice,"rock") == 0){
						playerScore = playerScore + 1;
						printf("User won!\n");
					}else if (strcmp(computerChoice,"paper") == 0){
						printf("It is a tie.\n");
					}else if (strcmp(computerChoice,"scissors") == 0){
						computerScore = computerScore + 1;
						printf("Computer won!\n");
					}
				//modify scores if player is scissors
				}else if (strcmp(playerChoice,"scissors") == 0){
					if (strcmp(computerChoice,"rock") == 0){
						computerScore = computerScore + 1;
						printf("Computer won!\n");
					}else if (strcmp(computerChoice,"paper") == 0){
						playerScore = playerScore + 1;
						printf("User won!\n");
					}else if (strcmp(computerChoice,"scissors") == 0){
						printf("It is a tie.\n");
					}
				}else{
					printf("Input error, use lowercase.\n");
				}
				printf("The score is now you: %d computer: %d.\n", playerScore, computerScore);
			}
		}else if (strcmp(choice, "no") == 0){
			break;
		}
		else{
			printf("error in input.\n");
		}
	}
	return 0;
}
