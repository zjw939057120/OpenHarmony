#include <stdio.h>
#include <ohos_init.h>


const char panda[] = "\n\
 ______                _       ______                       _ \n\
(_____ \\              | |     (____  \\                     | | \n\
 _____) )___ ____   _ | | ____ ____)  ) ___   ____  ____ _ | | \n\
|  ____/ _  |  _ \\ / || |/ _  |  __  ( / _ \\ / _  |/ ___) || | \n\
| |   ( ( | | | | ( (_| ( ( | | |__)  ) |_| ( ( | | |  ( (_| | \n\
|_|    \\_||_|_| |_|\\____|\\_||_|______/ \\___/ \\_||_|_|   \\____| \n";

static void pdb_helloworld(void)
{
    printf("%s", panda);
    return;
}

APP_FEATURE_INIT(pdb_helloworld);
