/* 
* proj4.c
*Ying Lu ylu6@wpi.edu
*Qiaoyu Liao qliao@wpi.edu
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

#define Empty 0
#define RAM 1
#define SSD 2
#define DISK 3
#define PT 3

#define RAM_SIZE 25
#define SSD_SIZE 100
#define DISK_SIZE 1000

sem_t ram_lock;
sem_t ssd_lock;
sem_t disk_lock;
sem_t pt_lock;

typedef signed short vAddr;
//page table 
//node
 typedef struct page{
 	int array; //if not allocated:0, otherwise: RAM 1, SSD 2, DISK 3
 	vAddr address; //where is page located
 	int last_access; //when the page is last accessed
 }Page;
//array
Page pageTable[1000];


sem_t ram_sem[RAM_SIZE];
int ram[RAM_SIZE];
int ssd[SSD_SIZE];
int disk[DISK_SIZE];

typedef struct info{
	int lock;
	int allocated;
//	int last_access;
}Info;

//store flag of memory array lock
Info ram_info[RAM_SIZE];
Info ssd_info[SSD_SIZE];
Info disk_info[DISK_SIZE];
//=================================================================================
void print_ram(){
	int i;
	printf("\nRAM INFO\n");
	printf("allocated  lock\t   data\n");
	for(i=0;i<25;i++){
		printf("%d\t    %d\t    %d\t\n", ram_info[i].allocated, ram_info[i].lock, ram[i]);
	}
	printf("\n");
}

void print_ssd(){
	int i;
	printf("SSD INFO\n");
	printf("allocated  lock\t   data\n");
	for(i=0;i<100;i++){
		printf("%d\t    %d\t    %d\t\n", ssd_info[i].allocated, ssd_info[i].lock, ssd[i]);
	}
	printf("\n");
}

void print_disk(){
	int i;
	printf("DISK INFO\n");
	printf("allocated  lock\t   data\n");
	for(i=0;i<1000;i++){
		printf("%d\t    %d\t   %d\t\n", disk_info[i].allocated, disk_info[i].lock, disk[i]);
	}
	printf("\n");
}

void print_pt(){
	int i;
	printf("PAGE TABLE INFO\n");
	printf("array   address last_access\n");
	for(i=0;i<1000;i++){
		printf("%d\t    %d\t    %d\t \n",pageTable[i].array,pageTable[i].address,pageTable[i].last_access);
	}
	printf("\n");
}

vAddr find_empty(int array){
	vAddr emptySlot;
	int i;
	//printf("array %d\n", array);
	switch(array){
		case RAM:
			//sem_wait(&ram_lock);
				//printf("enter RAM \n");
			i=0;
			while((ram_info[i].lock!=0||ram_info[i].allocated!=0)&&i<RAM_SIZE){ 
			//stop traversing until find allocated==0
				i++;
				//printf("i= %d\n",i);
			}
			if(i==RAM_SIZE){
				emptySlot=-1; //if no empty slot is left, return a false value
			}
			else{
				emptySlot=i;
				ram_info[i].lock=1;
				ram_info[i].allocated=1;
			}
			//printf("end of RAM\n");
			//sem_post(&ram_lock);
			break;
		case SSD:
			//sem_wait(&ssd_lock);
		//printf("enter SSD \n");
			i=0;
			while((ssd_info[i].lock==1||ssd_info[i].allocated!=0)&&i<SSD_SIZE){
				i++;
			}
			if(i==SSD_SIZE){
				emptySlot=-1;
			}
			else{
				emptySlot=i;
				ssd_info[i].lock=1;
				ssd_info[i].allocated=1;
			}
			//printf("end of SSD \n");
			break;
			//sem_post(&ssd_lock);

		case DISK:
			//sem_wait(&disk_lock);
			i=0;
			while((disk_info[i].lock==1||disk_info[i].allocated!=0)&&i<DISK_SIZE){
				i++;
			}
			if(i==DISK_SIZE){
				emptySlot=-1;
			}
			else{
				emptySlot=i;
				disk_info[i].lock=1;
				disk_info[i].allocated=1;
			}
			break;
			//sem_post(&disk_lock);
	}

	//printf("emptySlot in array %d, %d\n", array,emptySlot);
	return emptySlot;
}

vAddr find_evict(int array){ //find the one that is visited the least
	//printf("enter find_evict in array %d\n",array);
	vAddr evictSlot;
	int i;
	int least;
	i=0;
	least=pageTable[i].last_access;
	i++;
	//printf("ready to loop pageTable?\n");
	//getchar();
	int inPT=0;
	for(i=1;i<1000;i++){
		//printf("i=%d",i);
		//for the allocated slots in the indicated array, find the one visited least
		if(pageTable[i].array==array && pageTable[i].last_access<=least){
			vAddr index=pageTable[i].address;
			//int array=pageTable[i].array;

			int isLock;
			if(array==1){
				isLock=ram_info[index].lock;
			}
			else if(array==2){
				isLock=ssd_info[index].lock;
			}
			else if(array==3){
				isLock=disk_info[index].lock;
			}
			//printf("is Lock == %d\n",isLock);
			if(isLock==0){
				least=pageTable[i].last_access;
				evictSlot=pageTable[i].address;
				inPT=i;
				//printf("find unlock when i=%d",i);
			}//end of if(!isLock)
		}//end of checking least
	}//end of for loop

	if(array==1){
		ram_info[evictSlot].lock=1;

	}
	else if(array==2){
		ssd_info[evictSlot].lock=1;
		//usleep(250000);
	}
	else if(array==3){
		disk_info[evictSlot].lock=1;
		//usleep(2500000);
	}	

	//printf("evictSlot in array %d, %d\n", array,evictSlot);
	return evictSlot;
}

//=================================================================================
vAddr find_in_pt(int place,vAddr target){ //if to find empty spot, search for "-9999"
//on success, return index in pageTable array, on failure, return -1
	sem_wait(&pt_lock);
	int i=0;
	vAddr returnPT;
	while((pageTable[i].array!=place||pageTable[i].address!=target)&&i<1000){
		i++;
	}
	if(i==1000){
		returnPT=-1;
	}
	else{
		returnPT=i;
	}
	sem_post(&pt_lock);
	//printf("page table %d, target %d\n", returnPT,target);
	return returnPT;
}


//==================================================================================
void swap(int swap_out_location,vAddr swap_out, int take_in_location,vAddr take_in){
	int temp;
	Info tempInfo;
	if(swap_out_location==RAM){//swap out from ram to ssd
		temp=ram[swap_out];
		// tempInfo.lock=ram_info[swap_out].lock;
		// tempInfo.allocated=ram_info[swap_out].allocated;
		 if(take_in_location==SSD){
			ram[swap_out]=ssd[take_in];
			ssd[take_in]=temp;
		}else{
			printf("error in swap: swap out ");
		}

	}
	else if(swap_out_location==SSD){//swap out from ssd
		temp=ssd[swap_out];
		// tempInfo.lock=ssd_info[swap_out].lock;
		// tempInfo.allocated=ssd_info[swap_out].allocated;
		if(take_in_location==RAM){//to ram
			ssd[swap_out]=ram[take_in];
			ram[take_in]=temp;
		}
		else if(take_in_location==DISK){//to disk
			ssd[swap_out]=disk[take_in];
			disk[take_in]=temp;
		}else{
			printf("error in swap: swap out ");
		}
	}
	else if(swap_out_location==DISK){//swap out from disk to ssd
		temp=disk[swap_out];
		// tempInfo.lock=disk_info[swap_out].lock;
		// tempInfo.allocated=disk_info[swap_out].allocated;
		if(take_in_location==SSD){
			disk[swap_out]=ssd[take_in];
			ssd[take_in]=temp;
		}else{
			printf("error in swap: swap out ");
		}
	}
	vAddr take_in_pt, swap_out_pt;
	take_in_pt=find_in_pt(take_in_location,take_in);
	swap_out_pt=find_in_pt(swap_out_location,swap_out);

	if(swap_out_pt!=-1){
		pageTable[swap_out_pt].address=take_in;
		pageTable[swap_out_pt].array=take_in_location;
		
		
	}
	if(take_in_pt!=-1){
		pageTable[take_in_pt].address=swap_out;
		pageTable[swap_out_pt].array=swap_out_location;		
	}

}


//==================================================================================
vAddr allocateNewInt(){
	vAddr virtual_addr;
	vAddr ram_empty;

	sem_wait(&ram_lock);
	ram_empty=find_empty(RAM);
	//printf("ram_empty index %d\n",ram_empty);

	
	if(ram_empty==-1){//if no empty slot in ram
		ram_empty=find_evict(RAM); //find a slot to swap out
		sem_post(&ram_lock);
		//printf("after sem_post &ram_lock\n");
		sem_wait(&ssd_lock);
		//printf("begin searching ssd\n");
		vAddr ssd_empty=find_empty(SSD);//find an empty slot to take in the swapped out RAM 
		if (ssd_empty==-1){//if no empty slot in ssd to take in
			ssd_empty=find_evict(SSD);//find a slot to evict
			sem_post(&ssd_lock);
			sem_wait(&disk_lock);
			vAddr disk_empty=find_empty(DISK);//find a slot in disk to take in
			if(disk_empty==-1){//if no place in disk
				virtual_addr=-1; //return false value
				printf("no space avaible\n");
			}
			else{//if find a slot in disk
				sem_post(&disk_lock);
				swap(SSD,ssd_empty,DISK,disk_empty);
				disk_info[disk_empty].lock=0;
				swap(RAM,ram_empty,SSD,ssd_empty);
				ssd_info[ssd_empty].lock=0;

			}
		}
		else{//if there is empty slot in ssd
			sem_post(&ssd_lock);
			swap(RAM,ram_empty,SSD,ssd_empty);
			ssd_info[ssd_empty].lock=0;
		}
	}
	else{
		sem_post(&ram_lock);
	}
	
	virtual_addr=find_in_pt(0,-9999);
	if(virtual_addr!=-1){
		pageTable[virtual_addr].address=ram_empty;
		pageTable[virtual_addr].array=RAM;
		ram_info[ram_empty].lock=0;
	}else{
		virtual_addr=-1;
		printf("page table is full\n");
	}
	//mark as selected
	//store in pageTable

	//printf("recorded in pagetable: %d \n",virtual_addr);
	return virtual_addr;
}



int * accessIntPtr(vAddr address){
	//printf("enter\n");
	int * returnPtr;
	//printf("%d\n",address);
	vAddr access=pageTable[address].address;
	int access_place=pageTable[address].array;
	pageTable[address].last_access=pageTable[address].last_access+1;
	//printf("1\n");

	if(pageTable[address].array==1){//if in RAM
		returnPtr=ram+(pageTable[address].address);
		sem_wait(ram_sem+(pageTable[address].address));

	}
	else if(pageTable[address].array==2){
		usleep(250000);
		vAddr evict_ram =find_evict(RAM);
		swap(RAM,evict_ram,SSD,pageTable[address].address);
		returnPtr=ram+evict_ram;
		sem_wait(ram_sem+evict_ram);
		
	}
	else if(pageTable[address].array==3){
		usleep(2750000);
		vAddr evict_ssd =find_evict(SSD);
		swap(SSD,evict_ssd,DISK,pageTable[address].address);	
		vAddr evict_ram =find_evict(RAM);
		swap(RAM,evict_ram,SSD,evict_ssd);
		returnPtr=ram+evict_ram;
		sem_wait(ram_sem+evict_ram);
		
	}

	return returnPtr;
}


//release the slot so it can be swapped away
void unlockMemory(vAddr address){
	int unlock=pageTable[address].address;
	//int unlock_place=pageTable[address].array;
	sem_post(ram_sem+unlock);
}

void freeMemory(vAddr address){
	int freeM=pageTable[address].address;
	int free_place=pageTable[address].array;
	if(free_place==1){
		ram_info[freeM].lock=0;
		ram_info[freeM].allocated=0;
		ram[freeM]=0;
	}
	else if(free_place==2){
		ssd_info[freeM].lock=0;
		ssd_info[freeM].allocated=0;
		ssd[freeM]=0;
	}
	else if(free_place==3){
		disk_info[freeM].lock=0;
		disk_info[freeM].allocated=0;
		disk[freeM]=0;
	}
	pageTable[address].address=-9999;
	pageTable[address].array=0;
	pageTable[address].last_access=0;
}

int main(){
	int i;
	//initiate virtual memory and pageTable
	sem_init(&ram_lock,0,1);
	sem_init(&ssd_lock,0,1);
	sem_init(&disk_lock,0,1);
	sem_init(&pt_lock,0,1);

	for(i=0;i<25;i++){
		sem_init(ram_sem+i,0,1);
	}

	Page defaultPage;
	defaultPage.address=-9999;
	defaultPage.array=0;
	defaultPage.last_access=0;

	Info defaultInfo;
	defaultInfo.lock=0;
	defaultInfo.allocated=0;



	for(i=0;i<DISK_SIZE;i++){
		pageTable[i]=defaultPage;
		disk_info[i]=defaultInfo;
	}

	for(i=0;i<RAM_SIZE;i++){
		ram_info[i]=defaultInfo;
	}
	for(i=0;i<SSD_SIZE;i++){
		ssd_info[i]=defaultInfo;
	}


//double check print_pt size!!!!!!!!!!!!!!!!!!!!!!!
	print_ram();
	print_ssd();
	print_pt();

	printf("\n");
	vAddr indexes[1002];
	int index;

	for(index=0;index<1002;index++){

		indexes[index]=allocateNewInt();
		printf("allocatedNewInt returns: %d\n", indexes[index]);
		if(indexes[index]!=-1){
			int *value=accessIntPtr(indexes[index]);
			(*value)=index;
			unlockMemory(indexes[index]);
			
			if(index%5==0){
				value=accessIntPtr(indexes[index]);
				(*value)++;
			}
			unlockMemory(indexes[index]);

			if(index%3==0){
				value=accessIntPtr(indexes[index]);
				(*value)++;
			}
			unlockMemory(indexes[index]);
			printf("\n");
		}

	}

	// for(index=0;index<10;index++){
	// 	accessIntPtr(indexes[index]);
	// 	printf("access %d",index);
	// 	//unlockMemory(indexes[index]);
	// }

	print_ram();
	print_ssd();
	print_pt();

	for(i=0;i<1002;i++){
		freeMemory(indexes[i]);
	}

	printf("free success\n");
	print_ram();
	print_ssd();
	print_pt();
	return 0;
}

