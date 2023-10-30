#include <iostream>

//---------------------- function definitions

int finish(int countdown);

int main()
{
    const int N_CARS = 2;
    const int N_RIDERS = 5;
    const int T_WANDER = 100; // each wandering time is between 0 and T_WANDER
    const int T_BUMP = 40;

    int countDown = 10; // Sim time: Total number of bumper car rides

    return 0;
}

//----------------------- function declarations

int finish(int countDown)
{
    if (countDown == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
#define N_CARS 2
#define N_RIDERS 5
#define T_WANDER 100 // each wandering time is between 0 to T_WANDER
#define T_BUMP 40 // each bumping time is between 0 to T_BUMP

int COUNT_DOWN = 10; // Sim time: Total number of bumper car rides
int finish() {
if (COUNT_DOWN==0) return TRUE; else return FALSE;
}

/* Rider thread. rid is a number between 1 to N_RIDERS.
void Rider(int rid) {
    while (TRUE){
        /* wander around for a random amount of time
        Wander(rid, random()%T_WANDER);
        GetInLine(rid);
        TakeASeat(rid);
        TakeARide(rid);
        ... /* check for the condition to exit the while loop here
    }
}

void GetInLine(int rid) {
...
printf("Rider %d gets in the waiting line.\n", rid);
}

void TakeASeat(int rid) {
...
}

void TakeARide(int rid) {
printf("Rider %d is taking the ride.\n", rid);
...
}

void Wander(int rid, int interval) {
printf("Rider %d is wandering around the park.\n", rid);
...
}


/* Bumper car thread. cid is a number between 1 to N_CARS.
void Car(int cid) {
while (TRUE) {
Load(cid);
Bump(cid, random()%T_BUMP);
Unload(cid);
... /* decrease the COUNT_DOWN
... /* check for the condition to exit the while loop
}
}
void Load(int cid) {
...
this_guy=WaitArea[LineHead];
LineHead++;
printf("Car %d takes the rider %d.\n", cid, this_guy);
...
}
void Unload(int cid) {
...
printf("This ride of Car %d is over.\n");
...
}
/* Displaying thread
void Display(int dummy) {
while (!finish())
{
printf("The current situation in the park is:\n");
for (i=1 to N_CARS)
if (Car[i] is running)
print "Car i is running. The rider is ???"
else
print "Car i is not running."
for (i=1 to N_RIDERS)
if (Rider[i] is wandering)
print "Rider i is wandering"
else if (Rider[i] is waiting in line)
print "Rider i is waiting in line"
else
print "Rider i is in a car."
}
}
*/