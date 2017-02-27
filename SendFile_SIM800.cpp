
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sstream>


using namespace std;


class SerialPort
{
private:
    HANDLE hSerial;
    DCB dcbSerialParams;
    COMMTIMEOUTS timeouts;



    bool write(char [],int);
    bool write(byte[],int);

    bool writeAndResponse(char [],int,char []);
    bool writeAndResponse(byte [],int,char []);


public:
    SerialPort()
    {
        dcbSerialParams = {0};
        timeouts = {0};
    }
    bool init(char COMPORT[],long int BaudRate);

    int writeFile(char path[],char filename[]);
    int close();
};


int SerialPort::writeFile(char path[],char filename[])
{
    string file;
    stringstream x;
    x<<path<<filename;
    x>>file;
    x.clear();

    char cstr[file.length()+1];
    sprintf(cstr,"%s%s",path,filename);
    cstr[file.length()+1]='\0';
    cout<<endl<<(cstr);
    cout<<endl<<sizeof(cstr);
    delete [] cstr;

    FILE *fd = fopen(cstr, "rb");

    if(fd == NULL) {
        cout << "Error opening file\n";
        return 0;
    }
    fseek(fd, 0, SEEK_END);
    long fileSize = ftell(fd);
    byte *stream = (byte*)malloc(fileSize);
    fseek(fd, 0, SEEK_SET);
    long n_read=fread(stream, fileSize, 1, fd);
    fclose(fd);

    cout << endl <<"File Name : "<< filename;
    cout << endl <<"File Size :"<<fileSize;
    cout <<endl;

    if(!writeAndResponse("AT\r\n",sizeof("AT\r\n"),"\nAT Response:"))
    {
        close();
        return -1;
    }


    // string c1;
    // x<<"AT+FSCREATE="<<filename;
    // x>>c1;
    // char c2[c1.length()+3];
    // sprintf(c2,"%s%s","AT+FSCREATE=",filename);
    // c2[c1.length()]='\r';
    // c2[c1.length()+1]='\n';
    // c2[c1.length()+2]='\0';


    // if(!writeAndResponse(c2,sizeof(c2),"\nAT+FSCREATE Response:"))
    // {
    //     close();
    //     return -1;
    // }

    string cmd;
    x<<"AT+FSWRITE="<<filename<<",0,"<<fileSize<<",25000";
    x>>cmd;
    char cmd2[cmd.length()+3];
    sprintf(cmd2,"%s%s%s%d%s","AT+FSWRITE=",filename,",0,",fileSize,",25000");
    cmd2[cmd.length()]='\r';
    cmd2[cmd.length()+1]='\n';
    cmd2[cmd.length()+2]='\0';

    if(!writeAndResponse(cmd2,sizeof(cmd2),"\nAT+FSWRITE Response:"))
    {
        close();
        return -1;
    }
    delete [] cmd2;

    fprintf(stderr, "Sending bytes...");
    // Send specified text (remaining command line arguments)
    if(!writeAndResponse(stream,fileSize,"Send File Response:"))
    {
        close();
        return -1;
    }

    /*if(!writeAndResponse("AT+CFSTERM\r\n", sizeof("AT+CFSTERM\r\n"),"AT+CFSTERM Response : "))
    {
        close();
        return -1;
    }*/

    return 0;
}

bool SerialPort::init(char COMPORT[],long int BaudRate)
{
    fprintf(stderr, "Opening serial port...");
    hSerial = CreateFile(
                COMPORT, GENERIC_READ|GENERIC_WRITE, 0, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    if (hSerial == INVALID_HANDLE_VALUE)
    {
            fprintf(stderr, "Error\n");
            return false;
    }
    else fprintf(stderr, "OK\n");

    // Set device parameters (38400 baud, 1 start bit,
    // 1 stop bit, no parity)
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error getting device state\n");
        CloseHandle(hSerial);
        return false;
    }

    dcbSerialParams.BaudRate = BaudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if(SetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error setting device parameters\n");
        CloseHandle(hSerial);
        return false;
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
        return false;
    }
    return true;
}

bool SerialPort::writeAndResponse(char ws[],int sze,char prompt[])
{
    if(write(ws,sze))
    {
        char buffer[1024];
        int cnt;
        strcpy(buffer,"");
        ReadFile(hSerial,buffer,sizeof(buffer),NULL,NULL);
        cout<<endl<<prompt;
        cout<<buffer;
            return true;
    }
    return false;
}

bool SerialPort::writeAndResponse(byte ws[],int sze,char prompt[])
{
    if(write(ws,sze))
    {
        char buffer[1024];
        int cnt;
        strcpy(buffer,"");
        ReadFile(hSerial,buffer,sizeof(buffer),NULL,NULL);
        cout<<endl<<prompt;
        cout<<buffer;
            return true;
    }
    return false;
}

bool SerialPort::write(byte s[],int sze)
{
    if(!WriteFile(hSerial, s, sze, NULL, NULL))
    {
        fprintf(stderr, "Error\n");
        CloseHandle(hSerial);
        return false;
    }
    return true;
}

bool SerialPort::write(char s[],int sze)
{
    if(!WriteFile(hSerial, s, sze, NULL, NULL))
    {
        fprintf(stderr, "Error\n");
        CloseHandle(hSerial);
        return false;
    }
    return true;
}

int SerialPort::close()
{
    fprintf(stderr, "Closing serial port...");
    if (CloseHandle(hSerial) == 0)
    {
        fprintf(stderr, "Error\n");
        return 1;
    }
    fprintf(stderr, "OK\n");
    return 0;
}


int main()
{

SerialPort p1;
DIR *dir;
struct dirent *ent;

stringstream x;
char path[]="D:\\r\\";
//char filename[]="L.amr";

if(!p1.init("\\\\.\\COM4",CBR_19200))
    return -1;


if ((dir = opendir (path)) != NULL) {
  /* print all the files and directories within directory */
  while ((ent = readdir (dir)) != NULL) {

    char fname[ent->d_namlen];
    sprintf(fname,"%s",ent->d_name);
        if(strcmp(fname,".")==0 || strcmp(fname,"..")==0)
            continue;
        printf ("%s\n", ent->d_name);
        printf ("%s\n", fname);
        if(p1.writeFile(path,fname)==-1)
            return p1.close();
  }
  closedir (dir);
} else {
  /* could not open directory */
  perror ("");
  return EXIT_FAILURE;
}
    return p1.close();
}
