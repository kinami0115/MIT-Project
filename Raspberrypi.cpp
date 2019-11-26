#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <raspicam/raspicam.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <iostream>
#include "uuid/uuid.h"
#include <mysql.h>
#include <softPwm.h>


#define LED 4
int insert_db(int fire, int led, int temp, int frontdoor, int pic, int window, int open, int close);
using namespace std;

std::string uuidGen() { uuid_t uuid; char uuid_s[37]; 
  uuid_generate(uuid); 
  uuid_unparse(uuid, uuid_s); 
  return std::string(uuid_s); 
  } 

 
int main ( int argc,char **argv )
{   
	int fd;
    int window = 0;    
   
    raspicam::RaspiCam Camera;
 
    //Open camera 
    cout<<"Opening Camera..."<<endl;
    if ( !Camera.open()) {
        cerr<<"Error opening camera"<<endl;
        return -1;
    }
 
    //wait a while until camera stabilizes
    cout<<"wait a minutes"<<endl;
    usleep(3000000);
    if(wiringPiSetup() == -1){return 1;} 
     pinMode(LED, OUTPUT);
     
     if ((fd = serialOpen ("/dev/ttyACM0", 9600)) < 0)
   {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
   }
 
// Loop, getting and printing characters
    
  for (;;)
  {
    
    std::string uuid = uuidGen(); 
    std::string line1= uuid.c_str();
    std::string line2= "/home/pi/mitproject/pic/";
    std::string totalline= line2+line1;
    
    char newChar = serialGetchar (fd) ; // putchar()함수로 serialGetchar()함수로 받아오는 값을 뿌림                   
    printf("%c", newChar);
    fflush(stdout);
   
    if(newChar=='1'){
            insert_db(0,0,0,0,1,0,0,0);
            Camera.grab();
 
            //allocate memory
            unsigned char *data=new unsigned char[  Camera.getImageTypeSize ( raspicam::RASPICAM_FORMAT_RGB )];
 
            //extract the image in rgb format
            Camera.retrieve ( data,raspicam::RASPICAM_FORMAT_RGB );//get camera image
 
            //save
            std::ofstream outFile ( totalline,std::ios::binary );
 
            outFile<<"P6\n"<<Camera.getWidth() <<" "<<Camera.getHeight() <<" 255\n";
            outFile.write ( ( char* ) data, Camera.getImageTypeSize ( raspicam::RASPICAM_FORMAT_RGB ) );
 
            cout<<"Image saved at raspicam_image.ppm"<<endl;
        
            //free resrources    
 
            delete data;
          }
    else if(newChar=='2'){
         insert_db(0,0,0,1,0,0,1,0);
    }else if(newChar=='3'&& window==0){
        window =1;
        insert_db(0,0,0,0,0,1,1,0);
    }else if(newChar=='4'&& window==1){
        window =0;
      insert_db(0,0,0,0,0,1,0,1);
    }else if(newChar=='5'){
        insert_db(1,0,0,0,0,0,0,0);
        int i=0;
        while(i<5){
        digitalWrite(LED, 1);
        delay(1000);
        digitalWrite(LED, 0);
        delay(1000);
        i++;
        }
        
    }else if(newChar=='6'){
        insert_db(0,1,0,0,0,0,0,0);
    }
  }
  return 0;
}

int insert_db(int fire, int led, int temp, int frontdoor, int pic, int window, int open, int close)
{
	MYSQL *connector;
	char query[1024];
	int result=1;
		
	

	//MYSQL Connection
	connector = mysql_init(NULL);
	if (!mysql_real_connect(connector, "192.168.0.28", "java", "java", "db", 3307, NULL, 0))
	{
		fprintf(stderr, "%s\n", mysql_error(connector));
		printf("마리아 DB 연결실패\n");
		return -1;
	}
	
	printf("마리아 DB 연결성공\n");
	
	//쿼리(데이터 저장)
	sprintf(query,"INSERT INTO `db`.`project` (`fire`, `led`, `temp`, `frontdoor`, `pic`, `window`, `open`, `close`) VALUES ('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d');",
	fire,led,temp,frontdoor,pic,window,open,close);
	
	
	result=mysql_query(connector, query);
	

	if(result==0) 
		printf("데이터가 정상적으로 입력되었습니다.\n");
	else
		printf("입력 실패했습니다.결과값=%d\n",result);
	
	mysql_close(connector);
	
	return 0;
}

    




