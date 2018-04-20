#include <stdio.h>
#include "common.h"

int main( int argc, char ** argv )
{
    pkt p ;

    for(int x = 3; x < 15; x++)
    {
    	for(int y = 3; y < 15; y++)
    	{
		    setup( TORUS, x, y ) ;
		    for(int i = 0 ; i < (x*y); i++)
		    {
		    	create_packet( &p, i, -1, 1, 1, 1 ) ;
		    	initStats() ;
		    	broadcast( &p, i ) ;
		    	//printStats() ;
		    }
		}
	}    
    return 0 ;
}
