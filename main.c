// Rodney McGrath 55352053
// Ashton Garcia 86229480
// ICS53 Winter 2018
// Lab 3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LIMIT 30

struct virtualPage{
  int contents[2];
  int address1;
  int address2;
  int vpage;
  int mpage;
  int valid;
  int dirty;
};

struct mainAddress{
  int isPage; //bool
  int page;
  int age;
  struct virtualPage VP;
};

struct virtualPage VM[8];
struct mainAddress MM[4];
int isValid(int page)
{
	return MM[page].VP.valid == 1;
}
int updateMain(int add)//just updates main memory
{
	int i;
	for(i = 0; i < 4; i++)
	{
		if(MM[i].isPage == 0)
		{
			int j;
			for(j = 0; j < 8; j++)
			{
				if(VM[j].address1 == add || VM[j].address2 == add)
				{

					MM[i].isPage = 1;
					MM[i].VP.contents[0] = VM[j].contents[0];
					MM[i].VP.contents[1] = VM[j].contents[1];
					MM[i].VP.address1 = VM[j].address1;
					MM[i].VP.address2 = VM[j].address2;
					MM[i].VP.vpage = VM[j].vpage;
					MM[i].VP.mpage = MM[i].page;
					MM[i].VP.valid = 1;
					VM[j].valid = 1;
					VM[j].mpage = MM[i].page;
					MM[i].VP.dirty = VM[j].dirty;
					return MM[i].page;
				}
			}
		}
	}
	return -1;
}

void updateAge(int page){
  int i;
  for (i = 0; i < 4; i++){
    if (MM[i].isPage == 1){
      if (MM[i].page == page){
        MM[i].age = 100;
      }
      else{
        MM[i].age--;
      }
    }
  }
}

int find_add(int pageNum, int addr){
  if (MM[pageNum].VP.address1 == addr)
    return 0;
  return 1;
}

int oldestPage(){
  int i;
  int page = -1;
  int min = 500;
  for (i = 0; i < 4; i++){
    if (MM[i].age < min){
      min = MM[i].age;
      page = i;
    }
  }
  return page;
}

int loaded_address(int addr){
  int i;
  for (i = 0; i < 4; i++){
    if(MM[i].VP.address1 == addr || MM[i].VP.address2 == addr)
      return 1;
  }
  return 0;
}

int loaded_pages(){
  int i;
  for (i = 0; i < 4; i++){
    if (MM[i].isPage == 0){
      return -1;
    }
  }
  return 1;
}

int addressToPage(int addr){
  int page;
  if (addr == 0 || addr == 1)
    page = 0;
  else if (addr == 2 || addr == 3)
    page = 1;
  else if (addr == 4 || addr == 5)
    page = 2;
  else if (addr == 6 || addr == 7)
    page = 3;
  else if (addr == 8 || addr == 9)
    page = 4;
  else if (addr == 10 || addr == 11)
    page = 5;
  else if (addr == 12 || addr == 13)
    page = 6;
  else if (addr == 14 || addr == 15)
    page = 7;
  return page;
}

int isAddressInMM(int addr){
  int i;
  for (i = 0; i < 4; i++){
    if (MM[i].VP.address1 == addr || MM[i].VP.address2 == addr)
      return i;
  }
  return -1;
}

void copyMMtoVM(int pageNum){
  int vp = addressToPage(MM[pageNum].VP.address1);
  VM[vp].contents[0] = MM[pageNum].VP.contents[0];
  VM[vp].contents[1] = MM[pageNum].VP.contents[1];
  VM[vp].dirty = 0;
}

void pageFault(){
  int oldest = oldestPage();
  if(VM[MM[oldest].VP.vpage].dirty == 1){
	copyMMtoVM(oldest);
  }
  VM[MM[oldest].VP.vpage].valid = 0;
  VM[MM[oldest].VP.vpage].mpage = MM[oldest].VP.vpage;
  MM[oldest].VP.address1 = -1;
  MM[oldest].VP.address2 = -1;
  MM[oldest].VP.contents[0] = -1;
  MM[oldest].VP.contents[1] = -1;
  MM[oldest].VP.vpage = -1;
  MM[oldest].VP.mpage = -1;
  MM[oldest].VP.dirty = -1;
  MM[oldest].VP.valid = -1;
  MM[oldest].age = 100;
  MM[oldest].isPage = 0;
}

int main ()
{
  int i;
  int addressCounter = 0;
  // VM init
  for(i = 0; i < 8; i++){
    VM[i].contents[0] = -1;
    VM[i].contents[1] = -1;
    VM[i].address1 = addressCounter++;
    VM[i].address2 = addressCounter++;
    VM[i].valid = 0;
    VM[i].dirty = 0;
    VM[i].vpage = i;
    VM[i].mpage = i;

  }
  // MM init
  for(i = 0; i < 4; i++){
    MM[i].isPage = 0;
    MM[i].age = 100;
    MM[i].page = i;
    // init vp struct
    MM[i].VP.contents[0] = -1;
    MM[i].VP.contents[1] = -1;
    MM[i].VP.address1 = -1;
    MM[i].VP.address2 = -1;
    MM[i].VP.vpage = -1;
    MM[i].VP.mpage = -1;
    MM[i].VP.valid = -1;
    MM[i].VP.dirty = -1;
    MM[i].VP.mpage = -1;
  }

  // command input
  char inputString[MAX_LIMIT];
  printf("$ ");
  scanf("%[^\n]%*c", inputString);
  while(strcmp(inputString, "quit") != 0){
    // token
    char * pch;
    pch = strtok (inputString," ");

    //read <address>
    if (strcmp(pch, "read") == 0){
      pch = strtok (NULL, " "); // <address>
      int address = atoi(pch);
      int mmpn = isAddressInMM(address);
      if(mmpn != -1)//the address is in main memory
      {
      		printf("%d\n", MM[mmpn].VP.contents[find_add(mmpn,address)]);
		updateAge(mmpn);
      }
      else
      {
		if(loaded_pages() == 1)
		{
			VM[addressToPage(address)].valid = 0;
			pageFault();
			int page = updateMain(address);
			if(page == -1)
			{
				printf("ERROR\n");
				return 0;
			}
			updateAge(page);
			printf("%d\n", MM[page].VP.contents[find_add(page,address)]);
		}
		else
		{
			int page = updateMain(address);
			if(page == -1)
			{
				printf("ERROR\n");
				return 0;
			}
			updateAge(page);
			printf("%d\n", MM[page].VP.contents[find_add(page,address)]);
		}
      }
    }

    // write <address> <num>
    else if (strcmp(pch, "write") == 0){
      int address = atoi(strtok(NULL, " "));
      int num = atoi(strtok(NULL, " "));
      int mmpn = isAddressInMM(address);
	if(mmpn != -1)//the address is in main memory
      {
		MM[mmpn].VP.contents[find_add(mmpn,address)] = num;
		MM[mmpn].VP.dirty = 1;
		VM[addressToPage(address)].dirty = 1;
		updateAge(mmpn);
      }
      else
      {
		if(loaded_pages() == 1)
		{
			VM[addressToPage(address)].valid = 0;
			VM[addressToPage(address)].mpage = -1;
			pageFault();
			int page = updateMain(address);
			if(page == -1)
			{
				printf("ERROR\n");
				return 0;
			}
			updateAge(page);
			MM[page].VP.contents[find_add(page,address)] = num;
			VM[addressToPage(address)].dirty = 1;
			MM[page].VP.dirty = 1;

		}
		else
		{
			int page = updateMain(address);
			if(page == -1)
			{
				printf("ERROR\n");
				return 0;
			}

			updateAge(page);
			MM[page].VP.contents[find_add(page,address)] = num;
			VM[addressToPage(address)].dirty = 1;
			MM[page].VP.dirty = 1;
		}
      }

    }

    // showmain <ppn>
    else if (strcmp(pch, "showmain") == 0){
      int ppn = atoi(strtok(NULL, " "));
      printf("Address\tContents\n");
      printf("%d\t%d\n", MM[ppn].VP.address1, MM[ppn].VP.contents[0]);
      printf("%d\t%d\n", MM[ppn].VP.address2, MM[ppn].VP.contents[1]);
    }

    // showdisk <dpn>
    else if (strcmp(pch, "showdisk") == 0){
      int dpn = atoi(strtok(NULL, " "));
      printf("Address\tContents\n");
      printf("%d\t%d\n", VM[dpn].address1, VM[dpn].contents[0]);
      printf("%d\t%d\n", VM[dpn].address2, VM[dpn].contents[1]);
    }

    // showptable
    else if (strcmp(pch, "showptable") == 0){
      printf("VPageNum\tValid\tDirty\tPN\n");
      int i;
      for (i = 0; i < 8; i++){
        printf("\t%d\t%d\t%d\t%d\n", VM[i].vpage, VM[i].valid, VM[i].dirty,
          VM[i].mpage);
      }
    }
    printf("$ ");
    scanf("%[^\n]%*c", inputString);
  }

  return 0;
}
