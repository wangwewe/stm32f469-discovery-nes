#include "nes_game.h"

const nesGameFile gameFileList[2] = {
    {"SuperMario",	SuperMario},
		{"yingzichuanshuo",	yingzichuanshuo},
};

nesGame nes_game[2] = {
    {(&gameFileList[0]),   0},
		{(&gameFileList[1]),   0},
};

int nesReadFile(void *buf, unsigned int len, unsigned short num, pNesGame png) 
{
    volatile char *p = buf;
    
    if (png->gameFile->gameFileSrc == 0) {
        return -1;
    }
    
    for (int i = 0; i < num; i++ ) {
        memcpy((char *)p, png->gameFile->gameFileSrc + png->index , len );
        png->index += len;
        p += len;
    }
    
    return 0;
}



