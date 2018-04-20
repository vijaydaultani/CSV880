#include <iostream>
#include <iomanip>
//#include <stdio.h>
#include "common.h"

using namespace std;

/*
// Example of node numbering
// of a 3x3 2D-Torus
//
// 0 - 1 - 2
// |   |   |
// 3 - 4 - 5
// |   |   |
// 6 - 7 - 8
//
*/

extern int DIMX, DIMY, numRanks ;
extern void send(pkt*, int, int);
extern void consume(pkt*, int);
extern void wrapper(int);

#define ROW( p )   ( p / DIMY )
#define COL( p )   ( p % DIMY )
#define DOWN( p )  ( (p + DIMY) % numRanks )
#define UP( p )    ( (p + numRanks - DIMY) % numRanks )
#define RIGHT( p ) ( p + ( ( (p % DIMY) == DIMY-1 ) ? ( 1 - DIMY ) : 1 ) )
#define LEFT( p )  ( p + ( ( (p % DIMY) == 0 ) ? ( DIMY - 1 ) : - 1 ) )

void recv(pkt *p, int myRank)
{
    switch(p->hdr1)
    {
        case 1:
            //Source himself because row and col are both same
            if (( ROW(p->src) == ROW(myRank)) && ( COL(p->src) == COL(myRank)))
            {
                send( p, myRank, RIGHT(myRank) );
            }
            //If belong to the same row and different coloumns
            else if (( ROW(p->src) == ROW(myRank)) && ( COL(p->src) != COL(myRank)))
            {
                if( LEFT(p->src) != myRank )
                    send( p, myRank, RIGHT(myRank) );
                send( p, myRank, DOWN(myRank) );
            }
            //If belong to the same col and different rows
            else if (( ROW(p->src) != ROW(myRank)) && ( COL(p->src) == COL(myRank)))
            {
                //Dont forward it to anyone
            }
            //If belong to the different row and different col
            else if (( ROW(p->src) != ROW(myRank)) && ( COL(p->src) != COL(myRank)))
            {
                if((( COL(p->src) + 1 ) % DIMY) == COL(myRank) )
                    send( p, myRank, LEFT(myRank));
                //if((( ROW(p->src) + 1 ) % DIMX) == ROW(myRank) )
                if((( ROW(myRank) + 1 ) % DIMX) != ROW(p->src))
                    send(p, myRank, DOWN(myRank));
            }
            consume(p, myRank);
            break;

        case 2:
            //Source himself because row and col are both same
            if (( ROW(p->src) == ROW(myRank)) && ( COL(p->src) == COL(myRank)))
            {
                send( p, myRank, DOWN(myRank) );
            }
            //If belong to the same row and different coloumns
            else if (( ROW(p->src) == ROW(myRank)) && ( COL(p->src) != COL(myRank)))
            {
                //Dont forward it to anyone
            }
            //If belong to the same col and different rows
            else if (( ROW(p->src) != ROW(myRank)) && ( COL(p->src) == COL(myRank)))
            {
                if((( ROW(myRank) + 1 ) % DIMX) != ROW(p->src))
                    send( p, myRank, DOWN(myRank) );
                send( p, myRank, RIGHT(myRank) );
            }
            //If belong to the different row and different col
            else if (( ROW(p->src) != ROW(myRank)) && ( COL(p->src) != COL(myRank)))
            {
                if((( ROW(p->src) + 1 ) % DIMX) == ROW(myRank))
                    send( p, myRank, UP(myRank));
                if((( COL(myRank) + 1 ) % DIMY) != COL(p->src))
                    send( p, myRank, RIGHT(myRank));
            }
            consume(p, myRank);
            break;

        case 3: 
            //Source himself because row and col are both same
            if (( ROW(p->src) == ROW(myRank)) && ( COL(p->src) == COL(myRank)))
            {
                send( p, myRank, LEFT(myRank) );
            }
            //If belong to the same row and different coloumns
            else if (( ROW(p->src) == ROW(myRank)) && ( COL(p->src) != COL(myRank)))
            {
                if((( COL(p->src) + 1 ) % DIMY) != COL(myRank))
                    send( p, myRank, LEFT(myRank) );
                send( p, myRank, UP(myRank) );       
            }
            //If belong to the same col and different rows
            else if (( ROW(p->src) != ROW(myRank)) && ( COL(p->src) == COL(myRank)))
            {
                //Dont forward it to anyone
            }
            //If belong to the different row and different col
            else if (( ROW(p->src) != ROW(myRank)) && ( COL(p->src) != COL(myRank)))
            {
                if((( COL(myRank) + 1 ) % DIMY) == COL(p->src))
                    send( p, myRank, RIGHT(myRank));
                if((( ROW(p->src) + 1 ) % DIMX) != ROW(myRank))
                    send( p, myRank, UP(myRank));
            }
            consume(p, myRank);
            break;

        case 4:
            //Source himself because row and col are both same
            if (( ROW(p->src) == ROW(myRank)) && ( COL(p->src) == COL(myRank)))
            {
                send( p, myRank, UP(myRank) );
            }
            //If belong to the same row and different coloumns
            else if (( ROW(p->src) == ROW(myRank)) && ( COL(p->src) != COL(myRank)))
            {
                //Dont Forward it to anyone      
            }
            //If belong to the same col and different rows
            else if (( ROW(p->src) != ROW(myRank)) && ( COL(p->src) == COL(myRank)))
            {
                send( p, myRank, LEFT(myRank) );
                if((( ROW(p->src) + 1 ) % DIMX) != ROW(myRank))
                    send( p, myRank, UP(myRank) );
            }
            //If belong to the different row and different col
            else if (( ROW(p->src) != ROW(myRank)) && ( COL(p->src) != COL(myRank)))
            {
                if((( COL(p->src) + 1 ) % DIMY) != COL(myRank))
                    send( p, myRank, LEFT(myRank));
                if((( ROW(myRank) + 1 ) % DIMX) == ROW(p->src))
                    send( p, myRank, DOWN(myRank));
            }
            consume(p, myRank);
            break;
        default:
            printf("Found an unknown packet\n");
    }
}

void broadcast( pkt *p, int src )
{
    //cout << "Number Of Ranks is : " << numRanks << endl;
    int s = p->size;
    p->src = src;
    p->dst = -1;
    p->hdr1 = 1;
    p->size = s/4 + ((s%4 > 0)?1 : 0); recv(p, src);
    p->hdr1 = 2;
    p->size = s/4 + ((s%4 > 1)?1 : 0); recv(p, src);
    p->size = s/4 + ((s%4 > 2)?1 : 0); p->hdr1 = 3;
    recv(p, src);
    p->size = s/4;
    p->hdr1 = 4;
    recv(p, src);
}
