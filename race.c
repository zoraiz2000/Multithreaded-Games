// Author: Zoraiz Khan
// UCID: 30103088
// Date: 20-02-2023

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int finish_line;

//shared structure
struct shared{
    int race[100];
    int flagDropped[100];
    int raceFinished;
    int rank[100];
    int ranked[100];
    int moved;
};

struct shared s;

// initializing race conditions
void raceInit(int count) {
    for (int i = 0; i < count; i++) {
        s.race[i] = 0;
        s.flagDropped[i] = -1;
        s.raceFinished = 0;
        s.rank[i] = -1;
        // boolean to check if a hiker has already been ranked
        s.ranked[i] = 0;
        // boolean to check if a hiker recently moved a step
        s.moved = 0;
    }
}

// displaying race progress
void displayProgress(int count) {
    for (int i = 0; i < count; i++) {
        printf("Hiker %d: ", i);
        
        // displaying the number of steps and whether the hiker
        // has dropped their flag
        if (s.race[i] != 0) {
            for (int j = 0; j <= s.race[i]; j++) {
                if (s.flagDropped[i] == j) {
                    printf("*");
                } else {
                    printf("-");
                }
            }
        }
        
        // differentiating between hiker still racing vs
        // hiker at the finish line
        if (s.race[i] != finish_line || s.flagDropped[i] > -1) {
            printf(">\n");
        } else {
            printf("|\n");
        }
    }
    printf("\n");
}

void raceEnded(int count) {
    // if all hikers reached the finished line then return true
    for (int i = 0; i < count; i++) {
        if (s.race[i] != finish_line || s.flagDropped[i] > -1) {
            return;
        }
    }
    s.raceFinished = 1;
}

int setRanking(int rank_current, int count) {
    int rank = rank_current;
    for (int i = 0; i < count; i++) {
        // if the hiker has reached the finished line and their flag is not dropped
        // then rank them
        if (s.race[i] == finish_line && s.flagDropped[i] == -1 && s.ranked[i] == 0) {
            s.rank[rank] = i;
            s.ranked[i] = 1;
            rank++;
        }
    }
    return rank;
}

void showResults(int count) {
    // display the final results
    for (int i = 0; i < count; i++) {
        printf("Position #%d: Hiker %d \n", i+1, s.rank[i]); 
    }
}

void *hike(void *arg) {
    int *h_id = (int *)arg;

    srand( time(0));

    while (s.race[*h_id] < finish_line || s.flagDropped[*h_id] > -1) {
        // sleeping randomingly between 2 or 3 seconds
        sleep((rand() % 2) + 2);

        // hiker takes a step only if they haven't reached the finish line
        if (s.race[*h_id] < finish_line) {
            s.race[*h_id]++;
        }

        
        // if flag is not dropped
        if (s.flagDropped[*h_id] == -1) {

            // flip coin to check if flag drops
            if ((rand() % 2) > 0) {
                s.flagDropped[*h_id] = s.race[*h_id];
            }
        }

        // if flag has dropped
        if (s.flagDropped[*h_id] > -1) {

            //flip a coin to see if the hiker notices
            if ((rand() % 2) > 0) {
                // return to where the flag was dropped
                s.race[*h_id] = s.flagDropped[*h_id];
                s.flagDropped[*h_id] = -1;
            }
        }
        s.moved = 1;
    }
    pthread_exit(0);
}

int main() {
    
    // next finishers rank placement
    int rank = 0;
    int c;
    
    // Initialize the finish line randomly between 5-10
    srand( time(0));
    finish_line = (rand() % 6) + 5;
    printf("Finish line randomnly picked (from 5-10): %d steps\n", finish_line);

    // number of hikers participating
    printf("Please enter the number hiker (1-100): ");
    scanf("%d", &c);

    // initialize the shared structure
    raceInit(c);
    
    // Create hiker threads
    int h_id[c];
    pthread_t hikers[c];
    
    for (int i = 0; i < c; i++) {
        h_id[i] = i;
        pthread_create(&hikers[i], NULL, hike, &h_id[i]);
    }

    // while the race is on going
    while (s.raceFinished != 1) {
        while (s.moved == 0);
        displayProgress(c);

        // use to set the ending condtion of the race
        raceEnded(c);

        // updates the rank to the next available spot
        rank = setRanking(rank, c);

        // resetting the moved flag for the next iteration of loop
        s.moved = 0;
        
    }

    showResults(c);

    // join all hiker threads
    for (int i = 0; i < c; i++) {
        pthread_join(hikers[i],NULL);
    }

}
