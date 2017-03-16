#include <iostream>
#include <ctime>
#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <pthread.h>

using namespace std;

#define N 30 //Number of particles in the simulation
#define G 6.673e-11 //Universal gravity constant-Thanks Newton :D
#define TIMESLICE 1
#define xCoord 500
#define yCoord 500

//****Put back in if using multi-threading
	//int threadNum = N;

struct Particle{
    double mass;//particle mass
    double rx, ry;//particle position
    double vx, vy;//particle velocity
    double fx, fy;//particle force

};

Particle UpdateParticle(Particle p, double timeslice)
{
    p.vx += timeslice*p.fx / p.mass;
    p.vy += timeslice*p.fy / p.mass;
    p.rx += timeslice*p.vx;
    p.ry += timeslice*p.vy;
    return p;
}

void PrintParticle(Particle p) //Prints coordinate, velocity, and mass data to the stdout screen for debugging. 
{
    printf("rx == %f ry == %f vx == %f vy == %f mass == %f\n", p.rx,p.ry,p.vx,p.vy,p.mass);
}

//Reset the forces on particle to 0s
Particle ZeroForce(Particle p)
{
    p.fx = 0.0;
    p.fy = 0.0;
    return p;
}

//Calculate and add the force particle b has on particle a
Particle AddForce(Particle a,Particle b)
{// idea borrowed from ** http://home.fnal.gov/~cheung/rtes/RTESWeb/LQCD_site/pages/calculatingtheforces.htm ** for first few formulas
	double nzero = 1000;
	double xdist = b.rx - a.rx;
	double ydist = b.ry - a.ry;
	double distance = sqrt(xdist * xdist + ydist * ydist); //Distance formula learned in High School. 
	double gravForce = G*(a.mass*b.mass/(distance*1e7*distance)); // 1e7 --> Makes distance more realistic for visualization
	a.fx += gravForce * (xdist/distance); 	//the ratio between gravForce and the distance between a and b 
											//is the same as ratio between the x component of gravForce and the xdist between a and b
	a.fy += gravForce * (ydist/distance); 	//and the ratio between the y component of gravForce and the ydist between a and b
	
	//Originally kept track of threadID inside method
		//long tid;
		//tid = (long)threadid;
		//pthread_exit(NULL);
	return a;
}

//Prints particle information to file --> format in .pbm
void PrintToFile(Particle p)
{
	ofstream myfile;
	myfile.open ("nbodyout.txt",ios::out | ios::app);
	myfile << "rx == %f ry == %f vx == %f vy == %f mass == %f\n", p.rx,p.ry,p.vx,p.vy,p.mass;
	//myfile << "Writing this to a file.\n";
	myfile.close();
}

//Creates N random particles, and returns a Particle object
Particle createParticles(Particle particles[N])
{
    //randomly generates N Particles
	srand(time(NULL)); // Seed the time
	
	//setting position, velocity, and mass
	for (int i = 0; i < N; i++){
        particles[i].rx = rand()%1000-500; //picking position between -500 and 500 to fit on display coordinates
		particles[i].ry = rand()%1000-500;
        particles[i].vx = rand()%20 - 10;  //picking velocity between -10 units/timeslice and 10 units/timeslice (since display is only 1000 units wide)
        particles[i].vy = rand()%20 - 10;
        particles[i].mass = 1e20;  //can change this to be variable, probably ranging from 1e20 to 1e30
		
    }
	return particles[N];
}	

int main()
{
	//creating threads for multi-threading optimization
	//****Put back in if using threading****
		//pthread_t threads[threadNum];
		//Particle rc;	
	
	//creating the random group of particles
	Particle particles[N];
	createParticles(particles);

    int numberofiterations = 100;
    int count = 0;
    while (count < numberofiterations){ //Runs for number of iterations chosen 
        for (int i = 0; i < N; i++)
        {
            particles[i] = ZeroForce(particles[i]); //Resets the forces to 0 on the given particle
            for (int j = 0; j < N; j++)
            {
                if (i != j)  //Adds the forces given to a particle from other particles, but not itself
                	
					//Creates threads for each particle --> calculates forces from every other particle
					//****Does not run correctly when attempted**** --> thinking that AddParticle() needs to be of type Void * in order for pthread_create to work properly
						//rc = pthread_create(&threads[i], NULL, AddForce, particles[i], particles[j]); 
                    	//particles[i] = rc;
					particles[i] = AddForce(particles[i], particles[j]);
				
            }
        }
	//****Put back in if using threading****
		//pthread_exit(NULL);
	
    //loop again to update the time slice here
    for (int i = 0; i < N; i++)
    {
        particles[i] = UpdateParticle(particles[i], TIMESLICE);
    }
	//Print each particle in an image with a for loop
	//Initialize display to all black --> particles will be white pixels
	int bigString[1000][1000];
	for (int y = 0; y < 1000; y++) {
		for (int x = 0; x < 1000; x++) {
			bigString[y][x] = 1;
		}
	}

	//if particle is in coordinate field, make it visible (change 1 to a 0 in 2d array)
	for (int i = 0; i < N; i++)
        {
            PrintParticle(particles[i]);
			int smallX = particles[i].rx;
			int smallY = particles[i].ry;

			// particles are normally 1 pixel --> change to 3x3 square for better visibility
			if (smallY > -490 && smallY < 490 && smallX > -490 && smallX < 490) {
				bigString[smallY+499][smallX+499] = 0;
				bigString[smallY+499][smallX+500] = 0;
				bigString[smallY+499][smallX+501] = 0;
				bigString[smallY+500][smallX+499] = 0;
				bigString[smallY+500][smallX+500] = 0;
				bigString[smallY+500][smallX+501] = 0;
				bigString[smallY+501][smallX+499] = 0;
				bigString[smallY+501][smallX+500] = 0;
				bigString[smallY+501][smallX+501] = 0;
			}
        }
		
	//dynamically changing .pbm file names for every timeslice
	//FFMPEG creates a video from sequentially named files --> * command is stored in run.sh file *
	char buffer[32];
	snprintf(buffer, sizeof(buffer), "nbodyout%d.pbm", count);
	//print 2d array to text file
	ofstream out(buffer);
	out << "P1 1000 1000\n";
	for(int y = 0; y < 1000; y++) {
		for(int x = 0; x < 1000; x++) {
			out << bigString[y][x];
		}
		out << "\n";
	}
	out.close();
	count++;
    } //End While (higher up)
} //End Main

