#undef DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif

#include <doslib.h>
#include <stdlib.h>
#include <string.h>

#define	crlf	"\r\n"

static char output_format[20] = "0x0000:0x00000000 ";
static char table[] = "0123456789abcdef";

void writedata( int i, unsigned int vec ) {
#if 0
	output_format[2] = table[(i/0x1000)&0xf];
	output_format[3] = table[(i/0x100)&0xf];
	output_format[4] = table[(i/0x10)&0xf];
	output_format[5] = table[(i)&0xf];
#else
	output_format[5] = table[(i)&0xf];
	output_format[4] = table[(i/=0x10)&0xf];
	output_format[3] = table[(i/=0x10)&0xf];
	output_format[2] = table[(i/0x10)&0xf];
#endif

#if 0
	output_format[9] =   table[(vec/0x10000000)&0xf];
	output_format[10] =  table[(vec/0x1000000)&0xf];
	output_format[11] =  table[(vec/0x100000)&0xf];
	output_format[12] =  table[(vec/0x10000)&0xf];
	output_format[13] =  table[(vec/0x1000)&0xf];
	output_format[14] =  table[(vec/0x100)&0xf];
	output_format[15] =  table[(vec/0x10)&0xf];
	output_format[16] =  table[(vec)&0xf];
#else
	output_format[16] =  table[(vec)&0xf];
	output_format[15] =  table[(vec/=0x10)&0xf];
	output_format[14] =  table[(vec/=0x10)&0xf];
	output_format[13] =  table[(vec/=0x10)&0xf];
	output_format[12] =  table[(vec/=0x10)&0xf];
	output_format[11] =  table[(vec/=0x10)&0xf];
	output_format[10] =  table[(vec/=0x10)&0xf];
	output_format[9] =   table[(vec/0x10)&0xf];
#endif
}

void print256(int offset, int c) {
	int i,cnt=1;
	unsigned int vec;

	for(i=offset; i<offset+0x100; i++) {
		writedata(i,INTVCG(i));
		PRINT(output_format);
		if ( cnt++%c == 0 ) PRINT(crlf);
	}
	PRINT(crlf);
}

void printall(int c) {
	int i;

	/* interrupt vector */
	PRINT("Int vector\r\n");
	print256(0, c);

	/* iocscall vector */
	PRINT("IOCS call vector\r\n");
	print256(0x100, c);

	/* os call vector */
	PRINT("OS call vector\r\n");
	print256(0xff00, c);
}

void printhelp(char *cmd) {
	PRINT("printvec [options] { all | intno ... }\r\n");
	PRINT("options:\r\n");
	PRINT("  -0 : print interrupt vectors(0x00-0xff)\r\n");
	PRINT("  -1 : print iocscall vectors(0x100-0x1ff)\r\n");
	PRINT("  -f : print doscall vectors(0xff00-0xffff)\r\n");
	PRINT("  -c[num] : print <num> data per line(num:1-5).");
	PRINT("  -? : print this messae.\r\n");
	PRINT("all : print interrupt/iocscall/doscall vectors.\r\n");
	PRINT("intno : number of vector.\r\n");
}

int main(int argc, char *argv[]) {
	int i;
	int c = 1;
	int cnt = 1;
	int no;
	char *ptr, *rptr;

	if (argc==1) {
		printhelp(argv[0]);
		exit(EXIT_SUCCESS);
	}

	for (ptr=argv[i=1]; i<argc; ptr=argv[++i]) {
		if (*ptr=='-') {
			switch (*(ptr+1)) {
			case 'c':
				c = *(ptr+2) - '0';
				if ( c<1 || c>5 ) c = 1;
				break;

			case '0':
				/* interrupt vector */
				PRINT("Int vector\n");
				print256(0, c);
				break;

			case '1':
				/* iocscall vector */
				PRINT("IOCS call vector\r\n");
				print256(0x100, c);
				break;

			case 'f':
				/* os call vector */
				PRINT("OS call vector\r\n");
				print256(0xff00, c);
				break;

			case '?':
				printhelp(argv[0]);
				exit(EXIT_SUCCESS);
			}
		} else if (!strcmp(argv[i], "all")){
			printall(c);
			exit(EXIT_SUCCESS);
		} else {
			no = strtol(argv[i], &rptr, 0);
			if (0<=no && no<=0xffff) {
#ifdef DEBUG
printf("0x%04x:0x%08x\n", no, INTVCG(no));
#endif
				writedata(no, INTVCG(no));
				PRINT(output_format);
				if ( cnt++%c == 0 ) PRINT(crlf);
			}
		}
	}
	PRINT(crlf);
}
