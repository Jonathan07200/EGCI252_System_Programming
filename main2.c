#include <stdio.h>


// char_copy: takes an input file, copies it to output file
int main(void){
    FILE *input, *output; //pointers
    char block[512];

    input = fopen("file.in","r");
    output = fopen("file.out","w");
    
    while (fgets(block,sizeof block,input)){ // end of file
        fputs(block, output);
    }
    fclose(input);
    fclose(output);
    return 0;

}