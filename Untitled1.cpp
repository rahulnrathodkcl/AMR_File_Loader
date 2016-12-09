//
// serial.c / serial.cpp
// A simple serial port writing example
// Written by Ted Burke - last updated 13-2-2013
//
// To compile with MinGW:
//
//      gcc -o serial.exe serial.c
//
// To compile with cl, the Microsoft compiler:
//
//      cl serial.cpp
//
// To run:
//
//      serial.exe
//

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

int main()
{
    // Define the five bytes to send ("hello")


//char fname[256]="";
//char fpath[1024]=;

FILE *fd = fopen("D:\\Recordings\\D.amr", "rb");

if(fd == NULL) {
    cout << "Error opening file\n";
    return 0;
}
fseek(fd, 0, SEEK_END);
long fileSize = ftell(fd);
byte *stream = (byte*)malloc(fileSize);
cout << fileSize << '\n';
fseek(fd, 0, SEEK_SET);
long n_read=fread(stream, fileSize, 1, fd);
fclose(fd);

//cout<<stream;

    // Declare variables and structures
    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};

    // Open the highest available serial port number
    fprintf(stderr, "Opening serial port...");
    hSerial = CreateFile(
                "\\\\.\\COM5", GENERIC_READ|GENERIC_WRITE, 0, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    if (hSerial == INVALID_HANDLE_VALUE)
    {
            fprintf(stderr, "Error\n");
            return 1;
    }
    else fprintf(stderr, "OK\n");

    // Set device parameters (38400 baud, 1 start bit,
    // 1 stop bit, no parity)
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error getting device state\n");
        CloseHandle(hSerial);
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if(SetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error setting device parameters\n");
        CloseHandle(hSerial);
        return 1;
    }

    // Set COM port timeout settings
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if(SetCommTimeouts(hSerial, &timeouts) == 0)
    {
        fprintf(stderr, "Error setting timeouts\n");
        CloseHandle(hSerial);
        return 1;
    }

   DWORD bytes_written, total_bytes_written = 0;

    if(!WriteFile(hSerial, "AT\r\n", sizeof("AT\r\n"), &bytes_written, NULL))
    {
        fprintf(stderr, "Error\n");
        CloseHandle(hSerial);
        return 1;
    }

    char buffer[1024];
    int cnt;
    strcpy(buffer,"");
    ReadFile(hSerial,buffer,sizeof(buffer),&bytes_written,NULL);
    cout<<"AT Response : ";
    cout<<buffer;
    //if(strcmp(buffer,"\r\nOK\r\n"))
    //    printf("\nAT OK");

    if(!WriteFile(hSerial, "AT+CFSINIT\r\n", sizeof("AT+CFSINIT\r\n"), &bytes_written, NULL))
    {
        fprintf(stderr, "Error\n");
        CloseHandle(hSerial);
        return 1;
    }
    strcpy(buffer,"");
    ReadFile(hSerial,buffer,sizeof(buffer),&bytes_written,NULL);
    cout<<"AT+CFSINIT Response : ";
    cout<<buffer;

//    if(strcmp(buffer,"\r\nOK\r\n"))
//        printf("\nCFSINIT OK");


    if(!WriteFile(hSerial, "AT+CFSWFILE=\"PWD3.amr\",1,4934,25000\r\n", sizeof("AT+CFSWFILE=\"PWD3.amr\",1,4934,25000\r\n"), &bytes_written, NULL))
    {
        fprintf(stderr, "Error\n");
        CloseHandle(hSerial);
        return 1;
    }
    strcpy(buffer,"");
    ReadFile(hSerial,buffer,sizeof(buffer),&bytes_written,NULL);
    cout<<"AT+CFSWFILE Response : ";
    cout<<buffer;

//    if(strcmp(buffer,"\r\nCONNECT\r\n"))
//        printf("\nCONNECT");

//char buffer[1024];
//while (1) {
    // Read data into buffer.  We may not have enough to fill up buffer, so we
    // store how many bytes were actually read in bytes_read.
//    int xf=open("D:\\Machine\\Final_Reading_Ring_DTMF_RPMOFF\\1.amr",O_RDONLY);
//    int rd;
   // while((rd=read(xf, buffer, sizeof(buffer)))>0)
    //{
      //  printf("%s",&buffer);
        //p
        //WriteFile(hSerial, p, rd, &bytes_written, NULL);
    //}
//{
    // Send specified text (remaining command line arguments)
    fprintf(stderr, "Sending bytes...");


    if(!WriteFile(hSerial, stream, fileSize, &bytes_written, NULL))
    {
        fprintf(stderr, "Error\n");
        CloseHandle(hSerial);
        return 1;
    }

    strcpy(buffer,"");
    ReadFile(hSerial,buffer,sizeof(buffer),&bytes_written,NULL);
    cout<<"SEND FILE Response : ";
    cout<<buffer;

//    if(strcmp(buffer,"\r\nOK\r\n"))
//        printf("\nFILE WRITTEN SUCCESSFULLY OK");

    if(!WriteFile(hSerial, "AT+CFSTERM\r\n", sizeof("AT+CFSTERM\r\n"), &bytes_written, NULL))
    {
        fprintf(stderr, "Error\n");
        CloseHandle(hSerial);
        return 1;
    }
    strcpy(buffer,"");
    ReadFile(hSerial,buffer,sizeof(buffer),&bytes_written,NULL);
    cout<<"AT+CFSTERM Response : ";
    cout<<buffer;

//    if(strcmp(buffer,"\r\nOK\r\n")==0)
//        printf("\nCFSTERM OK");

    //ix++;

//}
    //fprintf(stderr, "%d bytes written\n", bwrite);

    // Close serial port

    fprintf(stderr, "Closing serial port...");
    if (CloseHandle(hSerial) == 0)
    {
        fprintf(stderr, "Error\n");
        return 1;
    }
    fprintf(stderr, "OK\n");

    // exit normally
    return 0;
}
