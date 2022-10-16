#include <stdlib.h>

struct MemoryStruct {
  char *memory;
  size_t size;
};

size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

