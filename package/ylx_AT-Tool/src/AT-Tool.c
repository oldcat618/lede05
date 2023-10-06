#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

int set_opt(int,int,int,char,int);

void printUsage() {
    printf("Usage: -p <serial_port_device> -c <command> [-fix <fix_value>]\n");
}

/*
void extractLineWithKeyword(const char *text, const char *keyword) {
    // Tokenize the text into lines
    char *line = strtok((char *)text, "\n");

    // Iterate through lines
    while (line != NULL) {
        // Check if the line contains the keyword
        if (strstr(line, keyword) != NULL) {
            // Print or process the line as needed
            printf("Found: %s\n", line);
            return; // Stop searching after finding the first occurrence
        }

        // Move to the next line
        line = strtok(NULL, "\n");
    }

    // If the keyword is not found
    printf("Keyword not found in the text.\n");
}
*/

int main(int argc, char *argv[])
{
    int opt;
    char *serialPort = NULL;
    char *command = NULL;
    char *fix = NULL;

    while ((opt = getopt(argc, argv, "p:c:f:")) != -1) {
        switch (opt) {
            case 'p':
                serialPort = optarg;
                break;
            case 'c':
                command = optarg;
                break;
            case 'f':
                fix = optarg;
                break;
            case '?':
                printUsage();
                return 1;
            default:
                break;
        }
    }

    if (serialPort == NULL || command == NULL) {
        printUsage();
        return 1;
    }

    int fd, nByte;
    char *uart3 = strdup(serialPort);
    char buffer[512]={0};
    strcpy(buffer, command);
    strcat(buffer, "\r\n");
    char *uart_out = strdup(buffer);

    memset(buffer, 0, sizeof(buffer));

    if ((fd = open(uart3, O_RDWR | O_NOCTTY)) < 0) {
        perror("open failed");
        return 1;
    }

    if (set_opt(fd, 9600, 8, 'N', 1) != 0) {
        fprintf(stderr, "Failed to set serial port options\n");
        close(fd);
        return 1;
    }

    if (write(fd, uart_out, strlen(uart_out)) < 0) {
        perror("write failed");
        close(fd);
        return 1;
    }

    fd_set fds;
    struct timeval timeout;

    while (1) {
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        timeout.tv_sec = 1; 
        timeout.tv_usec = 0;

        int result = select(fd + 1, &fds, NULL, NULL, &timeout);

        if (result == -1) {
            perror("select error");
            break;
        } else if (result == 0) {
            printf("Timeout occurred. Exiting...\n");
            break;
        } else {
            nByte = read(fd, buffer, sizeof(buffer) - 1);
            if (nByte > 0) {
                buffer[nByte] = '\0';
                printf("%s", buffer);

                if (strstr(buffer, "OK") || strstr(buffer, "ERR")) {
                    goto EXIT;
                }

                memset(buffer, 0, sizeof(buffer));
                nByte = 0;
            }
        }
    }

EXIT:
    close(fd);
    return 0;
}

int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio,oldtio;
	if  ( tcgetattr( fd,&oldtio)  !=  0) {
		perror("SetupSerial");
		return -1;
	}
	bzero( &newtio, sizeof( newtio ) );
	newtio.c_cflag  |=  CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;

	switch( nBits ) 
	{
		case 7:
			newtio.c_cflag |= CS7;
			break;
		case 8:
			newtio.c_cflag |= CS8;
			break;
	}

	switch( nEvent )
	{
	case 'O':
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E': 
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N':  
		newtio.c_cflag &= ~PARENB;
		break;
	}

	switch( nSpeed )
	{
		case 2400:
			cfsetispeed(&newtio, B2400);
			cfsetospeed(&newtio, B2400);
			break;
		case 4800:
			cfsetispeed(&newtio, B4800);
			cfsetospeed(&newtio, B4800);
			break;
		case 9600:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
		case 115200:
			cfsetispeed(&newtio, B115200);
			cfsetospeed(&newtio, B115200);
			break;
		case 460800:
			cfsetispeed(&newtio, B460800);
			cfsetospeed(&newtio, B460800);
			break;
		default:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
	}
	if( nStop == 1 )
		newtio.c_cflag &=  ~CSTOPB;
	else if ( nStop == 2 )
		newtio.c_cflag |=  CSTOPB;
		newtio.c_cc[VTIME]  = 0;
		newtio.c_cc[VMIN] = 0;
		tcflush(fd,TCIFLUSH);
	if((tcsetattr(fd,TCSANOW,&newtio))!=0) 
	{
		perror("com set error");
		return -1;
	}
	
	return 0;
}
