#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

// char_copy: takes an input file, copies it to output file
int main(void){
    FILE *input, *output; //pointers
    char ch;

    input = fopen("file.in","r");
    output = fopen("file.out","w");
    
    while ((ch=fgetc(input))!= EOF){ // end of file
        putc(ch,output);
    }
    fclose(input);
    fclose(output);
    return 0;

}

