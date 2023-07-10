#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>


int charCount;

struct node{
    int value; //weight
    int parent,lchild,rchild;
    char ch;
    int tag;
    int hufcodetemp[256];
    int *hufcode;
    int codelen;
    };

void CreateHuffmanTree(int nodecount,int max,struct node *huffmanTreeNodeTemp,struct node *huffmanTreeNode);
void initialize(struct node *HuffmanTreeNodeTemp,int count[]);
void compressfile(struct node *HuffmanTreeNode,int wordcount,int nodecount,char FILE1[],char FILE2[]);
void compress(int count[],int wordcount,char FILE1[],char FILE2[]);
void extract(char FILE1[],char FILE2[]);

 /************ Header ************/
void header()
{
    printf("\t\t FILE COMPRESSION AND DECOMPRESSION USING HUFFMAN CODING\n");
    printf("\t\t\t by: BISDA, Rhoss Gerald T.(BSCS-2B-NS)\n");
    printf("\t\t\t\t\t&\n");
    printf("\t\t\t   PILI, Sebastian Bach U. (BSCS-2A-NS)\n\n\n");
}

 /************Construct the Huffman tree************/
void CreateHuffmanTree(int nodecount,int max,struct node *HuffmanTreeNodeTemp,struct node *HuffmanTreeNode)
{
    int i,j,k,m,n,value;
    int min;
    for (i=0;i<nodecount;i++)
    {
        HuffmanTreeNode[i]=HuffmanTreeNodeTemp[i];
    }
    for (i=0;i<2*nodecount-1;i++)
    {
        HuffmanTreeNode[i].tag=0;
        HuffmanTreeNode[i].parent=0;
    }
    for (i=nodecount;i<2*nodecount-1;i++)
    {
        min=INT_MAX;
                 for (j=0;j<i;j++) //Find the maximum value m
        {
            if ((HuffmanTreeNode[j].tag==0) && (HuffmanTreeNode[j].value<=min))
            {
                min=HuffmanTreeNode[j].value;
                                 m=j; //m and n respectively represent the largest value, and the next largest value is the number
            }
        }
        HuffmanTreeNode[m].tag=1;
        min=INT_MAX;
                 for (j=0;j<i;j++) //Find the next largest value n
        {
            if ((HuffmanTreeNode[j].tag==0)  &&( HuffmanTreeNode[j].value<=min))
            {
                min=HuffmanTreeNode[j].value;
                n=j;
            }
        }
        HuffmanTreeNode[n].tag=1; //The value that has been found is marked as 1, and will not be found again next time
        HuffmanTreeNode[i].lchild=m;
        HuffmanTreeNode[i].rchild=n;
        HuffmanTreeNode[m].parent=i;
        HuffmanTreeNode[n].parent=i;
        HuffmanTreeNode[i].value=HuffmanTreeNode[m].value+HuffmanTreeNode[n].value;
    }

         //Generate Huffman code
    int index,temp;

    for (i=0;i<nodecount;i++)
    {
        index=255;
        for (j=i;HuffmanTreeNode[j].parent!=0;)
        {
            temp=HuffmanTreeNode[j].parent;
            if (HuffmanTreeNode[temp].lchild==j)
            {
                HuffmanTreeNode[i].hufcodetemp[index]=1;
                index--;
            }
            else if (HuffmanTreeNode[temp].rchild==j)
            {
                HuffmanTreeNode[i].hufcodetemp[index]=0;
                index--;
            }
            j=temp;
        }

        int length=255-index;
        HuffmanTreeNode[i].hufcode=malloc(length*sizeof(int));
        HuffmanTreeNode[i].codelen=length;

        for (k=0;k<length;k++)
        {
            index++;
            HuffmanTreeNode[i].hufcode[k]=HuffmanTreeNode[i].hufcodetemp[index];
        }
    }
}

 /***********Preparation of compressed files*************/
void initialize(struct node *HuffmanTreeNodeTemp,int count[])
{
	int i,j;
	struct node temp;
	 for (i=0;i<256;i++) //Assign a value to each temporary node
    {
        HuffmanTreeNodeTemp[i].value=count[i];
        HuffmanTreeNodeTemp[i].ch=(char)i;
    }
         for (i=0;i<256;i++) //Filter the characters that have appeared
    {
        for (j=i+1;j<256;j++)
        {
            if (HuffmanTreeNodeTemp[i].value<HuffmanTreeNodeTemp[j].value)
            {
                temp=HuffmanTreeNodeTemp[i];
                HuffmanTreeNodeTemp[i]=HuffmanTreeNodeTemp[j];
                HuffmanTreeNodeTemp[j]=temp;
            }
        }
    }
}

 /**********Compress the file************/
void compressfile(struct node *HuffmanTreeNode,int wordcount,int nodecount,char FILE1[],char FILE2[])
{
	FILE *ptr=fopen(FILE1,"rb");
    FILE *ptw=fopen(FILE2,"wb");
    char readtemp;
    char codetemp;
    int wcount=0,i,j;
    int length,num;
    codetemp='\0';
    						     //Write Huffman code
    fwrite(&nodecount,sizeof(int),1,ptw);
    fwrite(&wordcount,sizeof(int),1,ptw);
    for (i=0;i<nodecount;i++)
    {
        fwrite(&(HuffmanTreeNode[i].ch),sizeof(char),1,ptw);
    }
    for (i=nodecount;i<nodecount*2-1;i++)
    {
        fwrite(&(HuffmanTreeNode[i].lchild),sizeof(int),1,ptw);
        fwrite(&(HuffmanTreeNode[i].rchild),sizeof(int),1,ptw);
    }

    while(!feof(ptr))
    {
        readtemp=getc(ptr);
                 for (j=0;j<nodecount;j++) //Find the corresponding character
        {
            if (HuffmanTreeNode[j].ch==readtemp)
            {
                num=j;
                break;
            }
        }

                 for (i=0;i<HuffmanTreeNode[num].codelen;i++) //bit operation
        {
            codetemp<<=1;
            codetemp|=HuffmanTreeNode[num].hufcode[i];

            wcount++;
                         if (wcount==8) //write to compressed file after full eight
            {
                fwrite(&codetemp,sizeof(char),1,ptw);
                wcount=0;
                codetemp='\0';
            }
        }
       if (feof(ptr))
            break;
    }
         if (wcount>0) //Process the last byte of less than eight bits
    {
        for (i=0;i<8-wcount;i++)
            codetemp<<=1;

        fwrite(&codetemp,sizeof(char),1,ptw);
    }
    fclose(ptr);
    fclose(ptw);
}

 /************Official procedure for compressed files***********/
void compress(int count[],int wordcount,char FILE1[],char FILE2[])
{
    int i,j,nodecount=0;
         struct node *HuffmanTreeNodeTemp=malloc(256*sizeof(struct node)); //First initialize the Huffman node for every possible character

         initialize(HuffmanTreeNodeTemp, count); //Process these nodes and sort

         int max=HuffmanTreeNodeTemp[0].value; //rebuild a Huffman tree containing valid nodes

    for (i=0;i<256;i++)
    {
        if (HuffmanTreeNodeTemp[i].value!=0)
                nodecount++;
        if (HuffmanTreeNodeTemp[i].value>max)
                max=HuffmanTreeNodeTemp[i].value;
    }
    struct node *HuffmanTreeNode=malloc((2*nodecount-1)*sizeof(struct node));
    for (i=0;i<nodecount;i++)
    {
        HuffmanTreeNode[i]=HuffmanTreeNodeTemp[i];
    }

    CreateHuffmanTree(nodecount,max,HuffmanTreeNodeTemp,HuffmanTreeNode);

    printf("\n\nHUFFMAN CODE");
    printf("\n===============================");
    printf("\n\nNo. Char  Freq  Code\n");
    for (i=0;i<nodecount;i++) //Print Huffman code
    {
        printf("\n[%d]",i);
        printf("   %c ",HuffmanTreeNode[i].ch);
        printf("   %d     ",HuffmanTreeNodeTemp[i].value);
        for (j=0;j<HuffmanTreeNode[i].codelen;j++)
            {
                printf("%d",HuffmanTreeNode[i].hufcode[j]);
            }
        printf("\n");
    }

    while(!eof(FILE1)){

    }

         compressfile(HuffmanTreeNode,wordcount,nodecount,FILE1,FILE2); //compress file
    for (i=0;i<nodecount;i++)
    {
    	free(HuffmanTreeNode[i].hufcode);
    }
        free(HuffmanTreeNode);
        free(HuffmanTreeNodeTemp);
}

 /************Official procedures for decompressing files************/
void extract(char FILE2[],char FILE3[])
{
    int i,j;
    FILE *ptr=fopen(FILE2,"rb");
    int countch;
    int curwordcount=0;
    int wordcount;

         fread(&countch,sizeof(int),1,ptr); //Get information about building the Huffman tree from the file
    fread(&wordcount,sizeof(int),1,ptr);
	struct node *extractHuffmanTreeNode = malloc((countch*2-1)*sizeof(struct node));
	for (i=0;i<countch;i++)
    {
        extractHuffmanTreeNode[i].rchild=-1;
        extractHuffmanTreeNode[i].lchild=-1;
    }
    for (i=0;i<countch;i++)
    {
        fread(&(extractHuffmanTreeNode[i].ch),sizeof(char),1,ptr);
    }

    for (i=countch;i<2*countch-1;i++)
    {
        fread(&(extractHuffmanTreeNode[i].lchild),sizeof(int),1,ptr);
        fread(&(extractHuffmanTreeNode[i].rchild),sizeof(int),1,ptr);
    }

    int nextnode=2*countch-2;
    int pose;
    unsigned char chtemp;

    FILE *ptw=fopen(FILE3,"wb");
         while (!feof(ptr)) //Read the file for decoding
    {
        fread(&chtemp,sizeof(unsigned char),1,ptr);
        for (i=0;i<8;i++)
        {
                         if (curwordcount>=wordcount) //Make the number of printed characters the same as the original file. This is because when compressed with huffman encoding, the last less than 8 bits are shifted. When decompressing, it cannot be determined that the last 0 is Not caused by displacement
                break;
                         if (((int)chtemp&128)==128) //Use the mask to determine whether the bit is 0 or 1
                pose=1;
            else
                pose=0;
            chtemp<<=1;
                         if (nextnode>=countch) //Non-leaf node, continue
            {
                if (pose==1)
                {
                    nextnode=extractHuffmanTreeNode[nextnode].lchild;
                }
                else
                {
                    nextnode=extractHuffmanTreeNode[nextnode].rchild;

                }
                                 if (nextnode<countch) // reach the leaf node and output the decompressed characters
                {
                    fwrite(&(extractHuffmanTreeNode[nextnode].ch),sizeof(char),1,ptw);
                    curwordcount++;
                    nextnode=2*countch-2;
                }
            }
        }
    }
    free(extractHuffmanTreeNode);
    fclose(ptw);
    fclose(ptr);
}

 /*************Main function**************/
int main()
{

    int i;
    char ch;
    char opt;
    int a,b;
    char FILE1[100],FILE2[100],FILE3[100], tempFILE2[100];
         int wordcount=0; //Count the number of characters in the file
    int count[256]={0};

    system("cls");
    header();
    printf("\t\t(c) compress \t\t (d) decompress \t\t (0) exit\n\n");
    printf("\n\n Choice >> ");
    scanf("%c", &opt);
    switch(opt)
    {
    case 'c':
        printf("Please enter the file name: ");
        scanf("%s",FILE1);
        FILE *fp=fopen(FILE1,"rb");
            while ((fp=fopen(FILE1,"r"))==NULL)
            {
                printf("Sorry, can't open it.\nPlease enter the file name again.\n");
                scanf("%s",FILE1);
            }
        printf("Please enter the compressed file name: ");
        scanf("%s",FILE2);
        while((ch=getc(fp))!=EOF){
                count[(int)ch]+=1;
                wordcount++;
        }
        fclose(fp);
        a=clock();
        compress(count,wordcount,FILE1,FILE2);
        b=clock();
        printf("\n\ncompress use time:   %d s\n",b-a);
        printf("\n\nCompressed file name: '%s'\n\n\n",FILE2);
    break;
    case 'd':
        printf("Please enter the file name: ");
        scanf("%s",FILE2);
        FILE *fp1=fopen(FILE2,"rb");
        while ((fp1=fopen(FILE2,"r"))==NULL)
        {
            printf("Sorry, can't open it.\nPlease enter the file name again.\n");
            scanf("%s",FILE2);
        }
        printf("Please enter the decompressed file name: ");
        scanf("%s",FILE3);
        while((ch=getc(fp1))!=EOF){
                count[(int)ch]+=1;
                wordcount++;
        }
        fclose(fp1);
        a=clock();
        extract(FILE2,FILE3);
        b=clock();
        printf("\n\nextract use time:   %d s\n",b-a);
        printf("\n\nUncompressed file name: '%s'\n\n\n",FILE3);
    break;
    case 'C':
        printf("Please enter the file name: ");
        scanf("%s",FILE1);
        FILE *fp2=fopen(FILE1,"rb");
            while ((fp2=fopen(FILE1,"r"))==NULL)
            {
                printf("Sorry, can't open it.\nPlease enter the file name again.\n");
                scanf("%s",FILE1);
            }
        printf("Please enter the compressed file name: ");
        scanf("%s",FILE2);
        while((ch=getc(fp2))!=EOF){
                count[(int)ch]+=1;
                wordcount++;
        }
        fclose(fp2);
        a=clock();
        compress(count,wordcount,FILE1,FILE2);
        b=clock();
        printf("\n\ncompress use time:   %d s\n",b-a);
        printf("\n\nCompressed file name: '%s'\n\n\n",FILE2);
    break;
    case 'D':
        printf("Please enter the file name: ");
        scanf("%s",FILE2);
        FILE *fp3=fopen(FILE2,"rb");
        while ((fp3=fopen(FILE2,"r"))==NULL)
        {
            printf("Sorry, can't open it.\nPlease enter the file name again.\n");
            scanf("%s",FILE2);
        }
        printf("Please enter the decompressed file name: ");
        scanf("%s",FILE3);
        while((ch=getc(fp3))!=EOF){
                count[(int)ch]+=1;
                wordcount++;
        }
        fclose(fp3);
        a=clock();
        extract(FILE2,FILE3);
        b=clock();
        printf("\n\nextract use time:   %d s\n",b-a);
        printf("\n\nUncompressed file name: '%s'\n\n\n",FILE3);
    break;
    case '0':
        printf("\n\nExit program...\n\n");
        exit(0);
        getch();
    default:
        printf("\n\nInvalid choice\n\n");
        printf("\n\nExit program...\n\n");
        exit(0);
        getch();
    }


    return 0;
}
