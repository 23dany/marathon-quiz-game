#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//Const definition
#define MAX_STRING_QUESTION 256
#define MAX_STRING_NICKNAME 30
//Scoring criteria const definition
#define easyModeMultiplier 0.5
#define normalModeMultiplier 1
#define godModeMultiplier 2
//array of category names
const char* categoryNames[] = {
    "Mexican History",
    "General History",
    "Geography",
    "Sports, Movies, Books, Art",
    "Science and Biology",
    "Spanish, Philosophy and Religion"
};

//Player struct definition
typedef struct Player{
	int id;
	char nickname[MAX_STRING_NICKNAME];
	float maxScore;
	struct Player* next;
	struct Player* prev;
}Player;

typedef struct Question {
    int id;
    int category;
	char question[MAX_STRING_QUESTION];
    char options[3][MAX_STRING_QUESTION];
    int correct_answer;
    struct Question* next;
} Question;

typedef struct PlayedRound{
	int difficulty; 
	int playerID;
	int points;
	struct PlayedRound* next;
}PlayedRound;

// Function prototypes
Question* createQuestion(int questionId);
Question* searchQuestion(Question *questionHead, int questionId);
void deleteQuestionById(Question **questionHead, int questionId);
int getLastQuestionId(Question* questionHead, int idStart);
void showMenu(int *choice);

PlayedRound* createPlayedRound(int difficulty, int playerID , int points);
void insertPlayedRound(PlayedRound **head, int difficulty, int playerID , int points);
  
void addQuestion(Question** questionHead);
void playGame(Question* questionHead, Player**, PlayedRound**);
void freeQuestions(Question* questionHead);

//Player core function prototype definition
Player* createPlayer(int,char*,float);
void insertSortedPlayer(Player**,Player*);
Player* findPlayerByNickname(Player*,char*);
void updatePlayerIfHigherScore(Player**,char*,float);
bool isNicknameInList(Player*,char*);
int getLastId(Player*,int);
void freePlayers(Player*);

//Scoring system function prototype definition
float newScore(int,int);

//Nickname creation function prototype definition
void nicknameCreation(char*);

//Category functions
void categoryMenu(int *category);
void displayQuestionsByCategory(Question* head, int category);

int main(){
    Question* questionHead = NULL;
    Player *playerHead=NULL;
    PlayedRound* playedRoundHead = NULL;
    int choice;
    int questionId = 0;
    int category= 0;
    while(1){
        showMenu(&choice);
        switch (choice)
        {
            case 1:
                addQuestion(&questionHead);
                break;
            case 2:
                playGame(questionHead,&playerHead, &playedRoundHead);
                break;
            case 3:
            	printf("\nEnter the id of the question to delete: ");
            	scanf("%d",&questionId);
            	deleteQuestionById(&questionHead, questionId);
            	break;
            case 4:
            	categoryMenu(&category);
            	displayQuestionsByCategory(questionHead, category);
            	break;
            case 5:
                printf("\n============================\n");
                printf("    PROGRAM CREDITS\n");
                printf("============================\n");
                printf("Marathon Quiz Game\n");
                printf("Developed by E13A Group\n\n");
                break;
            case 6:
                printf("Bye bye ...\n");
                freeQuestions(questionHead);
                freePlayers(playerHead);
                return 0;
                break;
            default:
                printf("Invalid choice!\n");
                break;
        }
    }
}

/*
Display the Main Menu.
*/
void showMenu(int *choice){
    ("\n============================\n");
    printf("    MARATHON QUIZ GAME\n");
    printf("============================\n");
    printf("1. Register new question\n");
    printf("2. Play game\n");
    printf("3. Delete question\n");
    printf("4. Display questions by category\n");
    printf("5. Show credits\n");
    printf("6. Exit\n");

    printf("Select an option: ");
    scanf("%d", choice);
    getchar();
}
/*
    Display the category selection menu
*/
void categoryMenu(int *category){
    printf("\n1. Mexican History\n");
    printf("2. General History\n");
    printf("3. Geography\n");
    printf("4. Sports, Movies, Books, Art\n");
    printf("5. Science and Biology\n");
    printf("6. Spanish, Philosophy and Religion\n");
    
    do{
        printf("Select a category (1-6): ");
        scanf("%d", category); 
		while(getchar() != '\n');
    }while(*category < 1 || *category > 6); 
}
/*
Create a question in memory.
*/
Question* createQuestion(int questionId) {
    Question* newQuestion = (Question*)malloc(sizeof(Question));
    if (newQuestion == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    
    printf("\nEnter the question: ");
    fgets(newQuestion->question, MAX_STRING_QUESTION, stdin);
    newQuestion->question[strcspn(newQuestion->question, "\n")] = 0;
    
    for (int i = 0; i < 3; i++) {
        printf("Enter option %d: ", i + 1);
        fgets(newQuestion->options[i], MAX_STRING_QUESTION, stdin);
        newQuestion->options[i][strcspn(newQuestion->options[i], "\n")] = 0;
    }
    
    do {
        printf("Enter the correct answer (1, 2, or 3): ");
        scanf("%d", &newQuestion->correct_answer);
        getchar();
    } while (newQuestion->correct_answer < 1 || newQuestion->correct_answer > 3);
    
    int category;
    categoryMenu(&category);
    newQuestion->category=category;
    
    newQuestion->id = questionId;
    newQuestion->next = NULL;
    return newQuestion;
}

/*
Search for specific question by id
*/
Question* searchQuestion(Question *questionHead, int questionId){
    if (questionHead == NULL) return NULL;

    Question *aux = questionHead;

    while (aux != NULL) {
        if (aux->id == questionId) return aux;
        aux = aux->next;
    }

    printf("ID %d entered is not found\n", questionId);
    return NULL;
}

/*
Delete specific question by id
*/
void deleteQuestionById(Question **questionHead, int questionId) {
    if (*questionHead == NULL) return;

    Question *current = *questionHead;
    Question *previous = NULL;

    // Searches for the node with the specified ID
    current = searchQuestion(*questionHead, questionId);
    
    // If the ID is not found, terminate
    if (current == NULL){
    	printf("\nThe question with the entered id was not found\n");
    	return;
	}
    
    current = *questionHead;
    
    while (current->id != questionId) {
        previous = current;
        current = current->next;
    }

    // If the node to be deleted is the first node to be deleted
    if (previous == NULL) {
        *questionHead = current->next;
    } else {
        previous->next = current->next;
    }

    // Frees the memory of the deleted node
    free(current);
    
    printf("\nThe question was correctly deleted");
}

/*
Function to get assign the last id of the newest question
*/
int getLastQuestionId(Question* questionHead, int idStart){
	if(questionHead == NULL) return idStart;
	Question *last=questionHead;
    int id=idStart;
    while(last->next!=NULL){
        ++id;
        last=last->next;
    }
    return ++id;
}

/*
Add a question into the simple linked list
*/
void addQuestion(Question** questionHead) {
	int questionId = getLastQuestionId(*questionHead, 1);
    Question* newQuestion = createQuestion(questionId);
    
    if (*questionHead == NULL) {
        *questionHead = newQuestion;
    } else {
        Question* current = *questionHead;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newQuestion;
    }
    printf("\nQuestion added successfully!\n");
}

/*
Display specific question with options
*/
void displayQuestion(Question* q, int* questionNumber) {
    printf("\nCategory:  %s\n", categoryNames[(q->category)-1]);
    printf("\nQuestion %d: %s\n", *questionNumber, q->question);
    for (int i = 0; i < 3; i++) {
        printf("%d. %s\n", i + 1, q->options[i]);
    }
}
/*
Display question by specific category
*/
void displayQuestionsByCategory(Question* head, int category){
	Question * current= head;
	int found=0;
	while (current != NULL){
		if(current->category==category){
		    printf("\n-> %s\n", current->question);
		    found=1;
		}
		current= current->next;
	}
	if(found!=1) printf("\nThere are no questions in this category\n" );
	printf("\n");
}
/*
Logic for marathon game
*/
void playGame(Question* questionHead,Player** playerHead, PlayedRound **playerRound) {
    int lives = 3;
    int score = 0;
    int questionNumber = 1;
    Question* current = questionHead;
    //Player variables declaration
    Player *newPlayer=NULL;
    int id=0;
    char playerName[MAX_STRING_NICKNAME]="";
    float totalScore=0;
    //End.

    //Player creation
    //ID assignment
    if(*playerHead==NULL){
        id=1000;
    }else{
        id= getLastId(*playerHead,1000);
    }
    //End.
    nicknameCreation(playerName);
    //End.
    
    printf("\nGame started! You have %d lives.\n", lives);
    
    while (current != NULL && lives > 0) {
        displayQuestion(current, &questionNumber);
        
        int answer;
        printf("Your answer (1-3): ");
        scanf("%d", &answer);
        
        if (answer == current->correct_answer) {
            printf("Correct!\n");
            score++;
        } else {
            lives--;
            printf("Wrong! Lives remaining: %d\n", lives);
        }
        
        current = current->next;
        questionNumber++;

        //Mode multipliers for final score (1 - Easy, 2 - Normal, 3 - God)
        totalScore= newScore(score,1);
        
        if (lives == 0) {
            //Player insertion into the list
            newPlayer=createPlayer(id,playerName,totalScore);
            if(!isNicknameInList(*playerHead,playerName)){
                insertSortedPlayer(playerHead,newPlayer);
            }
            updatePlayerIfHigherScore(playerHead,playerName,totalScore);
            //End.
            printf("\nGame Over! You ran out of lives.\n");
        } else if (current == NULL) {
            //Player insertion into the list
            newPlayer=createPlayer(id,playerName,totalScore);
            if(!isNicknameInList(*playerHead,playerName)){
                insertSortedPlayer(playerHead,newPlayer);
            }
            updatePlayerIfHigherScore(playerHead,playerName,totalScore);
            //End.
            printf("\nCongratulations! You completed all questions!\n");
        }
    }
    
    printf("Final score: %d\n", score);
    /*
    NOTE:
    The difficulty and userID are hardcoded 
    please modify the call
    */
    insertPlayedRound(playerRound, 1, 1, score);
}

/*
Empty the questions linked list.
*/
void freeQuestions(struct Question* questionHead) {
    struct Question* current = questionHead;
    while (current != NULL) {
        struct Question* temp = current;
        current = current->next;
        free(temp);
    }
}

/*
Create the player round.
*/
PlayedRound* createPlayedRound(int difficulty, int playerID, int points){
    PlayedRound *newRound = (PlayedRound*)malloc(sizeof(PlayedRound));
	if (newRound == NULL) {
        printf("ERROR\n");
        return NULL;
    }
	newRound->difficulty = difficulty;
	newRound->playerID = playerID; 
	newRound->points = points;
	newRound->next = NULL; 
	
	return newRound;
}

/*
Insert player round sorted by difficulty (1,2,3) and the points (ascending order).
*/
void insertPlayedRound(PlayedRound **head, int difficulty, int playerID, int points){
	PlayedRound *newRound = createPlayedRound(difficulty, playerID, points);  
	//Go through the list and find the position to insert the node
	if(*head ==NULL || (*head)->difficulty > newRound->difficulty 
	    							&& (*head)->points < newRound->points){
		newRound->next = *head;
		*head = newRound;
		return; 
	}
	PlayedRound *current = *head;
	while(current->next !=NULL && current->next->difficulty < newRound->difficulty 
									&& current->next->points >= newRound->points){
		current = current->next;
	}
	newRound->next = current->next;
	current->next = newRound; 
	
}

	

//Empty all the players of the list
void freePlayers(struct Player* head) {
    struct Player* current = head;
    while (current != NULL) {
        struct Player* temp = current;
        current = current->next;
        free(temp);
    }
}

//New Score function
float newScore(int correctAnswers, int difficultySelection){
    //Scoring system
    float score=0;
    switch(difficultySelection){
        case 1:
            score=easyModeMultiplier*correctAnswers;
            break;
        case 2:
            score=normalModeMultiplier*correctAnswers;
            break;
        case 3:
            score=godModeMultiplier*correctAnswers;
            break;
    }
    return score;
    //End.
}

//Nickname creation
void nicknameCreation(char* playerName){
    //Player creation
    //this option for reading the string obligates the user to insert a username with the correct lenght
    bool correctLength=true;
    do{
        fflush(stdin);
        correctLength=true;
        printf("Enter your nickname: ");
        scanf(" %[^\n]",playerName);
        if(strlen(playerName)>MAX_STRING_NICKNAME){
            printf("\nName is too long\n");
            correctLength=false;
        }
    }while(!correctLength);
    //End.
}

//Functions for the main process for player creation

//Node creation
Player* createPlayer(int id, char *nickname, float score){
    Player *newPlayer=(Player*)malloc(sizeof(Player));
    newPlayer->id=id;
    newPlayer->maxScore=score;
    strcpy(newPlayer->nickname,nickname);
    newPlayer->next=NULL;
    newPlayer->prev=NULL;
    return newPlayer;
}

//Check for repeated nicknames
bool isNicknameInList(Player* head, char* nickname){
    Player* reference = findPlayerByNickname(head,nickname);
    if(reference==NULL){
        return false;
    }else{
        return true;
    }
}

//Function to get assign the last id of the newest player
int getLastId(Player* head, int idStart){
    Player *last=head;
    int id=idStart;
    while(last->next!=NULL){
        ++id;
        last=last->next;
    }
    return ++id;
}

//Insert new player in order from the highest score to the lowest
void insertSortedPlayer(Player **head, Player *newPlayer){
    //this is the case where there is only one node or when the new score is the highest
    if(*head==NULL || (*head)->maxScore <= newPlayer->maxScore){
        newPlayer->next=*head;
        if(*head!=NULL){
            (*head)->prev=newPlayer;
        }
        *head=newPlayer;
        return;
    }

    Player *current=*head;
    //case for the first insertion after the main node
    if(current->next==NULL){
        current->next = newPlayer;
        newPlayer->prev=current;
        return;
    }

    //cicle that runs until the next value is smaller than the value being compared
    while(current->next!=NULL && current->next->maxScore > newPlayer->maxScore){
        current=current->next;
    }

    //insertion of the relocated node
    newPlayer->next=current->next;
    newPlayer->prev=current;
    if(current->next!=NULL){
        current->next->prev=newPlayer;
    }
    current->next=newPlayer;
}

//Function to search for players by using the nickname as a reference
Player* findPlayerByNickname(Player *head, char *nickname){
    Player* current=head;
    while(current!=NULL){
        if(strcmp(current->nickname,nickname)==0){
            return current;
        }
        current=current->next;
    }
    return NULL;
}

//This function will go into effect when the new score is higher and the player needs to be rearranged
void updatePlayerIfHigherScore(Player **head, char *nickname, float newScore){
    //reference adress for locating the data needed
    Player* reference= findPlayerByNickname(*head,nickname);
    //in case the player doesn't exist
    if(reference==NULL){
        printf("\n Player doesn't exist");
        return;
    }
    //initial condition for the sorting algorithm, the new score must be higher than the previous score, if not, do nothing
    if(newScore>reference->maxScore){
        //the new score is stored
        reference->maxScore=newScore;

        //pointers for sorting the nodes
        Player* prevPlayer=reference->prev;
        Player* nextPlayer=reference->next;
        //End of pointers

        //this indicates that the node is the first one of the list, no sorting must be made
        if(prevPlayer==NULL){
            return;
        }

        //pointer redirectioning for the list no to get cropped out
        prevPlayer->next=nextPlayer;
        if(nextPlayer!=NULL){
            nextPlayer->prev=prevPlayer;
        }

        insertSortedPlayer(head,reference);
    }
    return;
}
