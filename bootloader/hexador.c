#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define NL 12

main(int argc, char **argv)
{
	unsigned char data[NL];
	int i,n,f,fd;
	
	if (argc<2) {
		printf("Uso: Hexador <fichero>\n");
		exit(0);
	}
	
	if ((fd=open(argv[1],O_RDONLY))==-1) {
		perror("open");
		exit(1);
	}

	printf("{\n");

	for(f=0;;f++){
		n=read(fd,data,NL);
		if (n==0) break;
		if (f) printf(",\n");
		for (i=0;i<n;i++) {
			printf("0x%02X",data[i]);
			if (i!=n-1) printf(",");
		}
	}
	printf("\n};\n");
	close(fd);
}
