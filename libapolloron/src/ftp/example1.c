#include <stdio.h>
#include <stdlib.h>

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


  printf( "=====[ List directory ]=====\n" );
  printf( "<=== Trying ftp_list()\n" );
  {
    char buf[10240];
    err = ftp_list( &ftp, 0, buf, sizeof(buf) );
    printf( "directry list: '%s'\n", buf );
  }
  err_handle( err, &ftp );
  printf( "<=== Trying ftp_nlist()\n" );
  {
    char buf[10240];
    err = ftp_nlist( &ftp, 0, buf, sizeof(buf) );
    printf( "directry nlist: '%s'\n", buf );
  }
  err_handle( err, &ftp );


  printf( "=====[ put/get commands ]=====\n" );
  printf( "<=== Trying ftp_put_buffer()\n" );
  {
    char buf[] = "The quick brown fox jumps over the lazy dogs.\n";
    err = ftp_put_buffer( &ftp, buf, sizeof(buf)-1, "test.txt" );
  }
  err_handle( err, &ftp );

  printf( "<=== Trying ftp_append_buffer()\n" );
  {
    char buf[] = "Boz Scaggs. Live at great american music hall, San Fransisco.\n";
    err = ftp_append_buffer( &ftp, buf, sizeof(buf)-1, "test.txt" );
  }
  err_handle( err, &ftp );

  printf( "<=== Trying ftp_get_buffer()\n" );
  {
    char buf[1024];
    err = ftp_get_buffer( &ftp, "test.txt", buf, sizeof(buf) );
  }
  err_handle( err, &ftp );

  printf( "<=== Trying ftp_get_file()\n" );
  err = ftp_get_file( &ftp, "test.txt", "test1.txt" );
  err_handle( err, &ftp );

  printf( "<=== Trying ftp_put_file()\n" );
  err = ftp_put_file( &ftp, "test1.txt", "test2.txt" );
  err_handle( err, &ftp );


  printf( "=====[ Directory commands ]=====\n" );
  printf( "<=== Trying ftp_pwd()\n" );
  {
    char buf[512];
    err = ftp_pwd( &ftp, buf, sizeof(buf) );
    if( err == 0 ) printf( "Current Directory is '%s'\n", buf );
  }
  err_handle( err, &ftp );

  printf( "<=== Trying ftp_mkdir()\n" );
  err = ftp_mkdir( &ftp, "testdir" );
  err_handle( err, &ftp );

  printf( "<=== Trying ftp_cd()\n" );
  err = ftp_cd( &ftp, "testdir" );
  err_handle( err, &ftp );
  err = ftp_cd( &ftp, ".." );
  err_handle( err, &ftp );

  printf( "<=== Trying ftp_rmdir()\n" );
  err = ftp_rmdir( &ftp, "testdir" );
  err_handle( err, &ftp );



  printf( "=====[ Other commands ]=====\n" );
  printf( "<=== Trying ftp_rename()\n" );
  err = ftp_rename( &ftp, "test.txt", "test1.txt" );
  err_handle( err, &ftp );

  printf( "<=== Trying ftp_site()\n" );
  err = ftp_site( &ftp, "chmod 700 test1.txt" );
  err_handle( err, &ftp );

  printf( "<=== Trying ftp_delete()\n" );
  err = ftp_delete( &ftp, "test1.txt" );
  err_handle( err, &ftp );


  printf( "<=== Trying ftp_quit()\n" );
  err = ftp_quit( &ftp );
  err_handle( err, &ftp );

  printf( "Total error(s): %d\n", total_err );

  return 0;
}
