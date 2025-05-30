#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "liboftp.h"

#define HOSTNAME "YOUR FTP HOSTNAME HERE"
#define USERNAME "USER NAME HERE"
#define PASSWORD "PASSWORD HERE"

int total_err = 0;

void err_handle( int err, LIBOFTP *ftp )
{
    printf( "===> error code %d", err );
    if( err < 0 ) {
	total_err++;
	printf( " : %s", ftp->error_message );
    }
    printf( "\n\n" );
}
    


int main()
{
    LIBOFTP ftp;
    int err;

    ftp_initialize( &ftp );

    printf( "=====[ Connect and login ]=====\n" );
    printf( "<=== Trying ftp_open()\n" );
    err = ftp_open( &ftp, HOSTNAME, 0 );
    err_handle( err, &ftp );

    printf( "<=== Trying ftp_user()\n" );
    err = ftp_user( &ftp, USERNAME, PASSWORD );
    err_handle( err, &ftp );

    err = ftp_passive( &ftp, 1 );

    printf( "<=== Trying ftp_type()\n" );
    err = ftp_type( &ftp, "image" );
    err_handle( err, &ftp );


    printf( "<=== Trying ftp_put_descriptor()\n" );
    err = ftp_put_descriptor( &ftp, "test.txt" );
    err_handle( err, &ftp );
    if( err > 0 ) {
	char *buf[] = { "The quick\n", "brown fox jumps\n", "over the lazy dogs.\n" };
	int i, fd = err;

	for( i = 0; i < 3; i++ ) {
	    write( fd, buf[i], strlen( buf[i] ) );
	}

	printf( "<=== Trying ftp_put_descriptor_close()\n" );
	err = ftp_put_descriptor_close( &ftp, fd );
	err_handle( err, &ftp );
    }

    printf( "<=== Trying ftp_get_descriptor()\n" );
    err = ftp_get_descriptor( &ftp, "test.txt" );
    err_handle( err, &ftp );

    if( err > 0 ) {
	FILE *fp;
	int fd = err;
	char buf[256];
	int n = 1;

	fp = fdopen( fd, "r" );
	if( fp == NULL ) {
	    perror( "" );
	    return 1;
	}

	printf( "READING...\n" );
	while( fgets( buf, 256, fp ) ) {
	    printf( "%d: %s", n, buf );
	    n++;
	}
	printf( "...DONE\n" );
	fclose( fp );

	printf( "<=== Trying ftp_get_descriptor_close()\n" );
	err = ftp_get_descriptor_close( &ftp, fd );
	err_handle( err, &ftp );
    }

    printf( "<=== Trying ftp_quit()\n" );
    err = ftp_quit( &ftp );
    err_handle( err, &ftp );

    printf( "Total error(s): %d\n", total_err );
    
    return 0;
}
