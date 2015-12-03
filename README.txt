README.txt
Ying Lu
Qiaoyu Liao

Project 4: Implementing Virtual Memory

Virtual memory is a powerful construction that allows programs to believe
they have access to a larger amount of memory resources than is present
in the physical RAM on a computer. In this project, we will emulate a
virtual memory system for our users that uses RAM, SSD, and traditional
spinning hard drives.

Storage Media
We will create three classes of storage: main memory (RAM), SSD, and
magnetic hard disks. Each tier in the hierarchy will have different ca-
pacities and access times (or latencies). The RAM will be limited to
25 integers, the SSD will hold 100 integers, and the hard disk will have
the capacity for 1000 integers. 

serve time:
RAM: immediately
SSD: 0.25s
The same delays occur when values are written to these media as well.
The system is limited to 1000 virtual memory pages. Requests exceeding this
limit are denied.

User API
The user will make the use of the following functions:
• vAddr allocateNewInt(); 
    This function reserves a new memory location, which is sizeof(int)
    in size. This memory block must be created in the emulated RAM, pushing other pages out of the
    emulated RAM into lower layers of the hierarchy, if needed. Returns -1 if no memory is available.

• int * accessIntPtr(vAddr address); 
    This function obtains the indicated memory page from
    lower levels of the hierarchy, if needed, and returns an integer pointer to the location in emulated
    RAM. The page is locked in memory and is immediately considered “dirty.” Returns NULL if the
    pointer cannot be provided (e.g., a page must be brought into RAM, but all of RAM is locked).

• void unlockMemory(vAddr address); 
    When the user is done using the indicated memory page for
    a while, they must unlock it, indicating that it can be swapped out to disk, if needed. Once a user
    calls unlockMemory, any previous pointers they had to the memory are considered invalid and must
    not be used.

• void freeMemory(vAddr address);
    When the user is finally done with the memory page that has
    been allocated, the user can free it. This not only frees the page in memory, but it deletes any swapped
    out copies of the page as well.

General:
allocateNewInt:
when user asks for a space, it searches RAM. If there is available slot, record the index in page table and return the page table index to the user. If no slot available, try search in SSD, then in Hard Drive. If there is available slot in lower levels, swap it to higher levels. 

when access, if the page is in RAM, return directly. Otherwise find pages to evict and swap it to RAM.


------------------------------------
run make to compile

------------------------------------
in evict1:
eviction algorithm is least visited


in evict2:
eviction algorithm is first available

in thread:
6 threads are created to access the virtual memory and perform read/write simultaneously.
