#include <iostream>
#include <ctime>
#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
using namespace std;
#define N 30 //Number of particles in the simulation
#define G 6.673e-11 //Universal gravity constnt-Thanks Newton :D
#define TIMESLICE 1653265
#define xCoord 500
#define yCoord 500
struct Particle{
    double mass;//particle mass
    double rx, ry;//particle position
    double vx, vy;//particle velocity
    double fx, fy;//particle force

};
Particle Update(Particle p, double timeslice)
{
    p.vx += timeslice*p.fx / p.mass;
    p.vy += timeslice*p.fy / p.mass;
    p.rx += timeslice*p.vx;
    p.ry += timeslice*p.vy;
    return p;
}

void PrintParticle(Particle p)
{
    printf("rx == %f ry == %f vx == %f vy == %f mass == %f\n", p.rx*pow(10,-31),p.ry*pow(10,-31),p.vx*pow(10,-30),p.vy*pow(10,-30),p.mass);
}
//Reset the forces on particle
Particle ResetForce(Particle p)
{
    p.fx = 0.0;
    p.fy = 0.0;
    return p;
}
Particle AddForce(Particle a,Particle b)//Calculate and add the force particle b has on particle a
{//http://home.fnal.gov/~cheung/rtes/RTESWeb/LQCD_site/pages/calculatingtheforces.htm
	double nzero = 1000;
	double xdist = b.rx - a.rx;
	double ydist = b.ry - a.ry;
	double distance = sqrt(xdist * xdist + ydist * ydist); //Distance formula learned in High School. 
	double gravForce = G*(a.mass*b.mass/(distance));
	a.fx += gravForce * (xdist/distance); 	//the ratio between gravForce and the distance between a and b 
											//is the same as ratio between the x component of gravForce and the xdist between a and b
	a.fy += gravForce * (ydist/distance); 	//and the ratio between the y component of gravForce and the ydist between a and b
	
			//sin^-1((abs(a.xdist - b.xdist) / distance)) = angle
			//sin(angle) = WantedXDistance / gravForce
			//cos(angle) = WantedYDistance / gravForce
}

void PrintToFile(Particle p)
{
	ofstream myfile;
	myfile.open ("nbodyout.txt",ios::out | ios::app);
	myfile << "rx == %f ry == %f vx == %f vy == %f mass == %f\n", p.rx,p.ry,p.vx,p.vy,p.mass;
	//myfile << "Writing this to a file.\n";
	myfile.close();
}

int main()
{
    Particle particles[N];
    //randomly generating N Particles
	srand(time(NULL)); // Seed the time
	
	for (int i = 0; i < N; i++){
        double rx = 1e18*exp(-1.8)*(rand() % 50000000 - 25000000);
        particles[i].rx = rx;
        double ry = 1e18*exp(-1.8)*(rand() % 50000000 - 25000000);
        particles[i].ry = ry;
        double vx = 1e18*exp(-1.8)*(rand() % 50000000 - 25000000);
        particles[i].vx = vx;
        double vy = 1e18*exp(-1.8)*(rand() % 50000000 - 25000000);
        particles[i].vy = vy;
        double mass = 1.98892e30*rand()*10 + 1e20;
        particles[i].mass = mass;
		
    }

	
    int numberofiterations = 10;
    int count = 0;
    while (count < numberofiterations){ //Runs for number of iterations chosen 
        for (int i = 0; i < N; i++)
        {
            particles[i] = ResetForce(particles[i]);//Resets the forces to 0 on the given particle
            for (int j = 0; j < N; j++)
            {
                if (i != j) //Adds the forces given to a prticle from other particles, but not itself
                {
                    particles[i] = AddForce(particles[i], particles[j]);
                }

            }
        }
        //loop again to update the time slice here
        for (int i = 0; i < N; i++)
        {
            particles[i] = Update(particles[i], TIMESLICE);
        }
	//Print each particle in an image with a for loop
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
		int smallX = particles[i].rx * pow(10,-29);
		int smallY = particles[i].ry * pow(10,-29);
		//put in method or else it's useless

		if (smallY > -500 && smallY < 500 && smallX > -500 && smallX < 500)
			bigString[smallY+500][smallX+500] = 0;

        }
	
	//print 2d array to text file
	ofstream out("nbodyout.pbm");
	out << "P1 1000 1000\n";
	for(int y = 0; y < 1000; y++) {
		for(int x = 0; x < 1000; x++) {
			out << bigString[y][x];
		}
		out << "\n";
	}
	out.close();

        count++;
    }

	



}
