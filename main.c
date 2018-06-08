/*
Assignment 6: Process Alarms and Terminal I/O Control
CSC 400 Operating Systems 
Professor Sumey
Written by Kory Bilak BIL7208@calu.edu, Anthony Mooney MOO3629@calu.edu, Priya Sobti @calu.edu
Started 4/17/2017
Finished 4/23/2017
Due 4/26/2017 

Description:
This program will ask the user for a password, and display time, which is moving around the screen, until the user types in their password to exit (Replaces all input with asterisks). They cannot use control + C

Compilation Instructions:
Step1: gcc a6main.c -o a6main
Step2:./ a5main

Changelog:
version 0.1 (4/17/2017): finished parts 1, and 2, and started part 3 (clearscreen) it works well, it even hits the wall
version 0.2 (4/18/2017): got prompts to work, asks for 2 passwords and checks using srcmp()
version 0.3 (4/19/2017): trying to get my coordinate for password 2 saved and restored.
version 0.4 (4/19/2017): part 4 and 5 are done
version 1.0 (4/19/2017): part 6 is done
version 2.0 (4/23/2017): bonus is done changed strings to characters and display asterisks, fixed our broken echo function
version 3.0 (4/23/2017): made prompts user freindly with better explanations, added comments

*/

#include <stdio.h> //standard I/O
#include <ctype.h> // for alphanumeric checks
#include <termios.h> //for our setecho()
#include <string.h> //for strcmp
#include <sys/ioctl.h> //for forcing fullscreen
#include <stdlib.h> //exit()
#include <math.h> // for math if we need it
#include <errno.h> //error handing on system calls
#include <signal.h> //signal handling
#include <sys/times.h> //for tms struct
#include <sys/types.h> // for time type, others
#include <unistd.h> //for the sysconf() 
#include <sys/ipc.h> //for communication
#include <sys/msg.h> //for messages
#include <time.h> // for clock
#include <string.h> // for strings

#define CLEARSCREEN() printf("\x1B[H\033[J") //clears our screen
#define SAVE() printf("\x1B[s") //saves our coordinates for input
#define RESTORE() printf("\x1B[u") //restores our saved coordinates
#define BACK_SPACE 0x7f //for asterisks handling



int alrm_cnt = 0; //initializes alarm count to zero

struct termios old, new; //for our supression keep global or no input at prompt after program is done
struct winsize w; //used for getwindowsize keep global, or cant adjust windowsize
int r=1, c=2; // new coords must be global or time wont delete right
int u=1, v=2; // old coords must be global or time wont delete right
int charsize2= 0; //needs to be global to fix charsize if controlc is pressed after other stuff is typed

void setecho(int onoff); //toggle to mask our input
void gotorc(int x,int y); //goto xy for linux
void cancel(int); // turns off control c
void timeout(int); // declare timeout() as a void function

//our main program
int main()
{

//intergers
char c;
char password1[25];
char password2[25];
int i, charsize1 = 0, flag = 0; // does not need to be global
int onoff;


ioctl(STDOUT_FILENO, TIOCGWINSZ, &w); //for detecting window size

printf("You can type in up to 25 alphanumeric characters, any other characters wont count\n");//that way arrow keys and stuff dont save to the password
printf("Please enter the password that will unlock your screensaver: "); // our first prompt for password

//our long string of while stuff to replace characters with asterisks
while((password1[charsize1] = c = getchar() ) && c != '\n' && password1[charsize1] != '\r' && charsize1 < 25)
{
	// ONLY save digits and characters
	if(isdigit(c)  || isalpha(c))
	{
	//putchar('*');
	//fflush(stdout);
	charsize1++;
	}
	// deals with backspaces
	/*else if( c &= BACK_SPACE && charsize1 > 0)
	{
	putchar('\b');
	printf(" ");
	putchar('\b');
	fflush(stdout);
	charsize1--;
	}*/
	
}



sleep(1); //sleeps for a second
CLEARSCREEN(); //calls our clearscreen function

signal(SIGINT, cancel); // to deal with control c
signal(SIGALRM, timeout);   // alarm timer routine

  alarm(1);                   // set alarm timer for 1 second
  /*** an infinite loop, only if you dont remember your password! ***/
onoff = 0; //value to turn off echo
setecho(onoff); //turns off echo
  while(1) 
{
// detects control c input and cancels it

printf("Please enter the password to unlock your screensaver: "); // prompt for second password
charsize2 = 0; // resets charsize 2 if incorrect password is typed
//while loop for second password mask with asterisks

while((password2[charsize2] = c = getchar() ) && c != '\n' && password2[charsize2] != '\r' && charsize2 < 25)
{	
	//only save digits and chars
	if(isdigit(c)  || isalpha(c))
	{
		putchar('*');
		fflush(stdout);
		charsize2++;
	}
	//deals with backspaces
	else if(( c == BACK_SPACE || c == 0x08) && charsize2 > 0)
	{
	putchar('\b');
		printf(" ");
		putchar('\b');
		fflush(stdout);
		charsize2--;
	}

}

	// Compare our character arrays
	for( i = 0; i < charsize2; i++)
	{
		
		if(password1[i] != password2[i])
			flag = 1;
		else
			flag = 0;
	}
	//compare our character array sizes (catches correct passwords + extra character)
	if (charsize2 != charsize1)
		flag = 1;


 	if (flag == 1) //if passwords dont match
	{

		CLEARSCREEN(); //clears screen
		fflush(stdout);
		printf("Incorrect Password"); //displays prompt
		fflush(stdout);
		sleep(2);
		CLEARSCREEN(); //clear screen
	}
	else //if password is right
	{
		onoff = 1; //resets terminals settings
		setecho(onoff); 
		CLEARSCREEN(); //clears screen so we can use terminal again
		exit(0); // exits our program
	}


   };

}


//displays time
void timeout(int signo)
{
SAVE();	//saves current cursor location for password


char str[12]; // our array of characters for time
time_t rawtime; // for our time
struct tm *timeinfo; 
time(&rawtime); //grabs time from computer
timeinfo = localtime (&rawtime); // gets local time
int hr = timeinfo->tm_hour; //gets hour
int mn = timeinfo->tm_min; //gets minute
int sc = timeinfo->tm_sec; //gets second
int xmin= 1 , xmax = 200, ymin = 2, ymax = 50; //size of screen

srand(time(NULL)); // gets time for r

xmax = w.ws_col-12; //adjusts xmax to fit terminal
ymax = w.ws_row; //adjusts ymax to fit terminal

u = r;// our old x coordinate
v = c;// our old y coordinate

gotorc(u,v); //goto old coords
sprintf(str, "           "); //blank the old string
fputs(str,stdout);
fflush(stdout);

r = rand() % (xmax-xmin) + (xmin); //random number for new coord
srand(time(NULL)); //gets time for c
c = rand() % (ymax-ymin) + (ymin); //ramdom number for y coord
gotorc(r,c); //goto our time display coordinates

	if(hr < 12) //displays Am if true
	{
		sprintf(str,"%2d:%02d:%02d AM",hr,mn,sc); //displays our time
		fputs(str,stdout);
		fflush(stdout);
	}
	else //displays PM
	{
		sprintf(str,"%2d:%02d:%02d PM",hr,mn,sc); //displays our time
		fputs(str,stdout);
		fflush(stdout);
	}
 

signal(SIGALRM, timeout);  // keep signal handler in tact
RESTORE(); //restores coordinates for our password
fflush(stdout);

alarm(1);                  // rearm process timer for next interval
 
}

//our version of gotoxy
void gotorc(int x,int y)
{
    printf("\x1B[%d;%df",y,x); //goto this location

}

//sets echo
void setecho(int onoff)
{

	if(onoff == 0) //if 0 turn off
	{
		if (tcgetattr(STDIN_FILENO, &old) != 0) //gets attributes for terminal stuff
		{
			perror("tcgetattr");
			exit(-1);
		}

	new = old;  //saves old settings before we make changes          
	new.c_lflag &= ~(ICANON|ECHO);   //echo is turned off here
             
   	 if (tcsetattr(fileno(stdin), TCSAFLUSH, &new) != 0) // turned off echo sent here
		{
       		 	perror("tcsetattr()");
			exit(-1);
		}
	return;
	}
    
	if(onoff == 1) //if 1 turn back on
	{

	
		if (tcsetattr(0, TCSANOW, &old) != 0) //resets old attributes before exiting
		{
			perror("tcsetattr()"); //print error if something goes wrong
			exit(-1);
		}
	return;
	}

}

// use for signal handling if control c is hit
void cancel(int sig) 
{
	
	signal(sig, SIG_IGN);
	CLEARSCREEN();
	printf("This is a screensaver, you need to type in your password to exit."); //long winded explanation as to why you cant use control f (it used to say) "sorry dave I cant do that (space oddysey), maybe if you SUDO"
	fflush(stdout);
	sleep(2); //display for a while
	CLEARSCREEN();
	printf("Enter Your Password:");
	charsize2 = 0;
	signal(SIGINT, cancel);

}
