#include <stdio.h>

unsigned char table_1[8] = {0x08, 0x02, 0x01, 0x40, 0x10, 0x20, 0x04, 0x80};
unsigned char table_2[8] = {4,6,7,1,3,2,5,0};

//32 bit table..
unsigned int table_3[32] = { 0x80000000, 0x00800000, 0x00008000, 0x00000080, 0x40000000, 0x00400000, 0x00004000, 0x00000040,
			     0x20000000, 0x00200000, 0x00002000, 0x00000020, 0x10000000, 0x00100000, 0x00001000, 0x00000010,
			     0x08000000, 0x00080000, 0x00000800, 0x00000008, 0x04000000, 0x00040000, 0x00000400, 0x00000004,
			     0x02000000, 0x00020000, 0x00000200, 0x00000002, 0x01000000, 0x00010000, 0x00000100, 0x00000001 };

int table_4[32] = {0, 8, 16, 24, 1, 9, 17, 25, 2, 10, 18, 26, 3, 11, 19, 27, 4, 12, 20, 28, 5, 13, 21, 29, 6, 14, 22, 30, 7, 15, 23, 31};

unsigned int tab_32(unsigned int l1, int num){
   	
	//unsigned char temp_1 = 0;
	unsigned int temp=0;
	int i;
	
	//temp_1 = (((l1&0x08)<<4)|((l1&0x02)<<5)|((l1&0x01)<<5)|((l1&0x40)>>2)|((l1&0x10)>>1)|((l1&0x20)>>3)|((l1&0x04)>>1)|((l1&0x80)>>7));

	for(i=0;i<num+1;i++){
		temp = temp | (((l1&table_3[i])>>(num-table_4[i]))<<(num-i));
	}

	//printf("after tabling:: %u %u %u\n", l1, temp , temp_1);
	return temp;
}

//Changes the bit values of the given char according to the look up table.
unsigned char tab(unsigned char l1, int num){
   	
	unsigned char temp_1 = 0;
	unsigned char temp=0;
	int i;
	
	//temp_1 = (((l1&0x08)<<4)|((l1&0x02)<<5)|((l1&0x01)<<5)|((l1&0x40)>>2)|((l1&0x10)>>1)|((l1&0x20)>>3)|((l1&0x04)>>1)|((l1&0x80)>>7));

	for(i=0;i<num+1;i++){
		temp = temp | (((l1&table_1[i])>>(num-table_2[i]))<<(num-i));
	}

	//printf("after tabling:: %d %d %d\n", l1, temp , temp_1);
	return temp;
}

// Encrypts given unsigned int with unsigned int key..
unsigned int encrypt(unsigned int input, unsigned int key){
	
	unsigned char char_key_array[4];
	char *ch;
	unsigned int output = 0;
	int i;
	
	key = tab_32(key,31);

	//Swapping the key byte blocks 1 to 4 and 2 to 3 to add confusion.
	ch = (char *)&key;
	char_key_array[0] = *(ch);
	char_key_array[1] = *(++ch);
	char_key_array[2] = *(++ch);
	char_key_array[3] = *(++ch);

	printf("%d %d %d %d\n", char_key_array[0],char_key_array[1],char_key_array[2],char_key_array[3]);

	// Use the look up table to change the bit values of the key..
	for(i=0;i<4;i++){
	   char_key_array[i] = tab(char_key_array[i],7);
	   printf("%d ",char_key_array[i]);
	}
	printf("\n");

	//Now Xor the key with input and then reverse to get the final cipher output..  i.e i/p XOR key' and 1 to 4 and 2 to 3
	output = (((input&0xFF000000)>>24)^char_key_array[0]) | (((input&0x00FF0000)>>16)^char_key_array[1])<<8 | (((input&0x0000FF00)>>8)^char_key_array[2])<<16 | (((input&0x000000FF))^char_key_array[3])<<24;
	printf("%u\n",output);
	
	return output;

}


//Decrypts the given unsigned int cipher..
unsigned int decrypt(unsigned int cipher, unsigned int key){
	unsigned int plain_text=0;
	unsigned int intermediate_cipher = 0;
	unsigned char char_key_array[4];
	char *ch;
	int i;

	
	key = tab_32(key,31);
	//Swapping the key byte blocks 1 to 4 and 2 to 3 to add confusion.
	ch = (char *)&key;
	char_key_array[0] = *(ch);
	char_key_array[1] = *(++ch);
	char_key_array[2] = *(++ch);
	char_key_array[3] = *(++ch);

	//printf("%d %d %d %d\n", char_key_array[0],char_key_array[1],char_key_array[2],char_key_array[3]);

	// Use the look up table to change the bit values of the key..
	for(i=0;i<4;i++){
	   char_key_array[i] = tab(char_key_array[i],7);
	   printf("%d ",char_key_array[i]);
	}
	printf("\n");
	
	// Cipher was reversed, hence we un reverse it first..
	intermediate_cipher = (cipher&0xFF000000)>>24|(cipher&0x00FF0000)>>8|(cipher&0x0000FF00)<<8|(cipher&0x000000FF)<<24;

	// Xor again with key to get the original plain text..
	plain_text = (((intermediate_cipher&0xFF000000)>>24)^char_key_array[0])<<24 | (((intermediate_cipher&0x00FF0000)>>16)^char_key_array[1])<<16 | (((intermediate_cipher&0x0000FF00)>>8)^char_key_array[2])<<8 | (((intermediate_cipher&0x000000FF))^char_key_array[3]);

	printf("%u\n", plain_text);

	return plain_text;
}

void  encbc(unsigned int input[],int size,int key,unsigned int iv,unsigned int cipher1[]){
   int i=0;
   unsigned int k,cipher;
  do{
     k=input[i]^iv;
     cipher=encrypt(k,key);
     cipher1[i]=cipher;
     iv=cipher;
     i++;
   }while(i<size);  

} 

void decbc(unsigned int cipher1[],int size,int key,unsigned int iv,unsigned int dicipher1[]){
   int i=0;
   unsigned int k,dicipher;
   do{
        dicipher=decrypt(cipher1[i],key);
        k=dicipher^iv;
        dicipher1[i]=k;
        iv=cipher1[i];
        i++; 
    }while(i<size);
 
}



int main(void) {

unsigned int key = 1976853412;
unsigned int input[4] = {77777777,99999999,3333333,4444444},cipher1[4],dicipher1[4];
unsigned int iv=1768398782;
int i,size=4;

 encbc(input,size,key,iv,cipher1);
   printf("\n\n After Encrypting\n");
  for(i=0;i<4;i++)
   printf("%u\t",cipher1[i]);
  printf("\n\n");
  decbc(cipher1,size,key,iv,dicipher1);
   printf("\n\n After Decrypting\n");
  for(i=0;i<4;i++)
   printf("%u\t",dicipher1[i]);
return 0;
}
