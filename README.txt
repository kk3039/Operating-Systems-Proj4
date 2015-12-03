README.txt
Ying Lu
Qiaoyu Liao


General:
allocateNewInt:
when user asks for a space, it searches RAM. If there is available slot, record the index in page table and return the page table index to the user. If no slot available, try search in SSD, then in Hard Drive. If there is available slot in lower levels, swap it to higher levels. 

when access, if the page is in RAM, return directly. Otherwise find pages to evict and swap it to RAM.


in evict1:
eviction algorithm is least visited


in evict2:
eviction algorithm is first available

in thread:
6 threads are created to access the virtual memory and perform read/write simultaneously.