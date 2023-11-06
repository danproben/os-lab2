/*
	main.cpp

	Daniel Proben
	Completed Nov 5, 2023
*/

#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// constants
const int N_CARS = 2;
const int N_RIDERS = 5;
const int T_WANDER = 100;
const int T_BUMP = 40;

// Total number of bumper car rides
const int COUNT_DOWN = 10;

// counter
int countDown = COUNT_DOWN;

// Waiting area
int line[COUNT_DOWN + N_RIDERS];
int frontOfLine = 0;
int backOfLine = 0;

// semaphores
sem_t waitForRideToBegin[N_RIDERS];
sem_t waitForRideOver[N_RIDERS];
sem_t waitForRider;
sem_t displaySem; // this gets used in a way similar to simply calling a function. It's likely I have too many uses of it, but it works...
sem_t updateCarsStatus;
sem_t updateWanderStatus;
sem_t updateLine;
sem_t countDownSem;

// check if the simulation is finished or not
bool finish();

// Rider functions
void *Rider(void *rid);
void Wander(int rid, int interval);
void GetInLine(int rid);
void TakeASeat(int rid);
void TakeARide(int rid);

// Car functions
void *Car(void *cid);
void Bump(int cid, int interval);
int Load(int cid);
void Unload(int cid, int rid);

// display function
void *Display(void *args);

// status arrays used for display
int carStatus[N_CARS];
int wanderingStatus[N_RIDERS];

int main()
{
	// seed random number generator
	srand(time(0));

	// initialized id arrays... this is the only way I could think to pass references to ids to the pthread_create
	int rid[N_RIDERS];
	int cid[N_CARS];

	// initialize rider ids (1 to N_RIDERS)
	for (int i = 0; i < N_RIDERS; i++)
	{
		rid[i] = i + 1;
	}

	// initialize car ids (1 to N_CARS)
	for (int i = 0; i < N_CARS; i++)
	{
		cid[i] = i + 1;
	}

	// initialize semaphores. Last argument determines starting value
	sem_init(&waitForRider, 0, 0);
	sem_init(&displaySem, 0, 0);
	sem_init(&updateCarsStatus, 0, 1);
	sem_init(&updateWanderStatus, 0, 1);
	sem_init(&updateLine, 0, 1);
	sem_init(&countDownSem, 0, 1);

	// declare thread variables
	pthread_t riders[N_RIDERS];
	pthread_t cars[N_CARS];
	pthread_t display;

	// create the display thread
	pthread_create(&display, NULL, &Display, &riders);

	for (int i = 0; i < N_RIDERS; i++)
	{
		// initialize arrays of semaphores associated with riders
		sem_init(&waitForRideToBegin[i], 0, 0);
		sem_init(&waitForRideOver[i], 0, 0);

		// create the rider threads
		pthread_create(&riders[i], NULL, &Rider, &rid[i]);
	}

	// create car threads
	for (int i = 0; i < N_CARS; i++)
	{
		pthread_create(&cars[i], NULL, &Car, &cid[i]);
	}

	for (int i = 0; i < N_RIDERS; i++)
	{
		// destroy rider semaphores
		sem_destroy(&waitForRideToBegin[i]);
		sem_destroy(&waitForRideOver[i]);

		// join rider threads
		pthread_join(riders[i], NULL);
	}

	// Join car threads
	for (int i = 0; i < N_CARS; i++)
	{
		pthread_join(cars[i], NULL);
	}

	// join display thread
	pthread_join(display, NULL);

	// destroy the rest of the semaphores
	sem_destroy(&waitForRider);
	sem_destroy(&displaySem);
	sem_destroy(&updateCarsStatus);
	sem_destroy(&updateWanderStatus);
	sem_destroy(&updateLine);
	sem_destroy(&countDownSem);
	sem_destroy(&waitForRider);

	std::cout << "\nEnd of simulation" << std::endl;
	return 0;
}

bool finish()
{
	// decrement countDownSem to guard while reading
	sem_wait(&countDownSem);
	if (countDown <= 1)
	{
		sem_post(&countDownSem);
		return true;
	}
	else
	{
		sem_post(&countDownSem);
		return false;
	}
}

void *Rider(void *id)
{
	int rid = *(int *)id;

	while (true)
	{
		// while it is not finished
		if (!finish())
		{
			//  if statement prevents riders from entering the simulation after countDown has reached a point where they won't get to ride.
			if ((countDown - 1) > (N_RIDERS - N_CARS))
			{
				Wander(rid, rand() % T_WANDER);
				sem_post(&displaySem);
				GetInLine(rid);
				sem_post(&displaySem);
				TakeASeat(rid);
				sem_post(&displaySem);
				TakeARide(rid);
				sem_post(&displaySem);
			}
		}
		else
		{
			break;
		}
	}
	sem_post(&displaySem);
	return NULL;
}

// wander the park for a random amount of time between 0 and T_WANDER
void Wander(int rid, int interval)
{

	// update the wandering status
	sem_wait(&updateWanderStatus);
	wanderingStatus[rid - 1] = interval;
	sem_post(&displaySem);
	sem_post(&updateWanderStatus);

	sleep(interval);
}

// adds a rider to the line once they are finished wandering
void GetInLine(int rid)
{
	// this updates the wandering status
	sem_wait(&updateWanderStatus);
	wanderingStatus[rid - 1] = 0;
	sem_post(&displaySem);
	sem_post(&updateWanderStatus);

	// deposit the rid in the back of the line
	sem_wait(&updateLine);
	line[backOfLine] = rid;
	backOfLine++;
	sem_post(&displaySem);
	sem_post(&updateLine);
}

void TakeASeat(int rid)
{
	// this wakes a car so that the car can then wake the rider. Prevents a car from taking off without a rider.
	sem_post(&waitForRider);
	sem_wait(&waitForRideToBegin[rid - 1]);
}

void TakeARide(int rid)
{
	// wait to be woken by the car after a random amount of time between 0 and T_BUMP
	sem_wait(&waitForRideOver[rid - 1]);
	sem_post(&displaySem);
}

void *Car(void *id)
{
	int cid = *(int *)id;

	while (true)
	{
		// wait for a rider to enter the line
		sem_wait(&waitForRider);

		int rid = Load(cid);
		sem_post(&displaySem);
		Bump(cid, rand() % T_BUMP);
		sem_post(&displaySem);
		Unload(cid, rid);

		// update car status to idle since it's been unloaded
		sem_wait(&updateCarsStatus);
		carStatus[cid - 1] = 0;
		sem_post(&displaySem);
		sem_post(&updateCarsStatus);

		if (finish())
		{
			break;
		}
	}
	sem_post(&displaySem);
	return NULL;
}

int Load(int cid)
{
	// increment the front of the line to be the next rider
	sem_wait(&updateLine);
	int rid = line[frontOfLine];
	frontOfLine++;
	sem_post(&displaySem);
	sem_post(&updateLine);

	// update the car status to running since we just loaded it.
	sem_wait(&updateCarsStatus);
	carStatus[cid - 1] = rid;
	sem_post(&displaySem);
	sem_post(&updateCarsStatus);

	// wake the rider to begin bumping
	sem_post(&waitForRideToBegin[rid - 1]);

	return rid;
}

void Unload(int cid, int rid)
{

	// decrement countDown since we unloaded. This counts as a ride
	sem_wait(&countDownSem);
	countDown--;
	sem_post(&displaySem);
	sem_post(&countDownSem);

	// wake the rider to be done bumping
	sem_post(&waitForRideOver[rid - 1]);
	sem_post(&displaySem);
}

// bump for a random about of time between 0 and T_BUMP
void Bump(int cid, int interval)
{
	sleep(interval);
}

// gives an event-driven display of park status
void *Display(void *args)
{

	while (true)
	{
		// function will wait here until called upon when something changes status
		sem_wait(&displaySem);

		// ANSI escape characters to clear the screen and position cursor at bottom left of terminal... ugly, but I'm trying to avoid usying std::system
		std::cout << "\033[2J\033[1;1H";

		std::cout << "The current situation in the park is: \n"
				  << std::endl;

		// display the number of rides remaining
		sem_wait(&countDownSem);
		std::cout << "Rides Left = " << countDown << std::endl
				  << std::endl;
		sem_post(&countDownSem);

		// display the status of each car
		sem_wait(&updateCarsStatus);
		for (int i = 0; i < N_CARS; i++)
		{
			if (carStatus[i] > 0)
			{
				std::cout << "Car " << (i + 1) << " is running. The rider is " << carStatus[i] << std::endl;
			}
			else
			{
				std::cout << "Car " << (i + 1) << " is idle." << std::endl;
			}
		}
		sem_post(&updateCarsStatus);

		// display which riders are waiting in line
		sem_wait(&updateLine);
		for (int i = frontOfLine; i < backOfLine; i++)
		{

			if (line[i] > 0)
			{
				std::cout << "Rider " << line[i] << " is in line..." << std::endl;
			}
		}

		sem_post(&updateLine);

		// display which riders are wandering the park
		sem_wait(&updateWanderStatus);
		for (int i = 0; i < N_RIDERS; i++)
		{
			if (wanderingStatus[i] > 0)
			{
				std::cout << "Rider " << i + 1 << " is wandering..." << std::endl;
			}
		}
		sem_post(&updateWanderStatus);

		// if countDown <= 1 and both cars are empty, break from the loop and exit the display thread
		if (finish() && carStatus[0] == 0 && carStatus[1] == 0)
		{
			break;
		}
	}
	return NULL;
}
