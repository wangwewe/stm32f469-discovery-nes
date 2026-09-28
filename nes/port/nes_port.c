#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "K6502.h"
#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_Mapper.h"
#include "InfoNES_pAPU.h"
#include "nes_board.h"
#include "nes_audio.h"

#include "nes_game.h"

#define SELECT_GAME     NES_GAME_INDEX            




// Palette data
WORD NesPalette[ 64 ] =
{
    0x39ce, 0x1071, 0x0015, 0x2013, 0x440e, 0x5402, 0x5000, 0x3c20,
    0x20a0, 0x0100, 0x0140, 0x00e2, 0x0ceb, 0x0000, 0x0000, 0x0000,
    0x5ef7, 0x01dd, 0x10fd, 0x401e, 0x5c17, 0x700b, 0x6ca0, 0x6521,
    0x45c0, 0x0240, 0x02a0, 0x0247, 0x0211, 0x0000, 0x0000, 0x0000,
    0x7fff, 0x1eff, 0x2e5f, 0x223f, 0x79ff, 0x7dd6, 0x7dcc, 0x7e67,
    0x7ae7, 0x4342, 0x2769, 0x2ff3, 0x03bb, 0x0000, 0x0000, 0x0000,
    0x7fff, 0x579f, 0x635f, 0x6b3f, 0x7f1f, 0x7f1b, 0x7ef6, 0x7f75,
    0x7f94, 0x73f4, 0x57d7, 0x5bf9, 0x4ffe, 0x0000, 0x0000, 0x0000
    
};


void nesStart(void)
{
    WorkFrame = (WORD *)NES_WORK_FRAME_ADDR;
    
    if(0 != InfoNES_Load("embedded")) {

        NES_BoardFatal(5);
    }
    
    InfoNES_Main();
}



/*===================================================================*/
/*                                                                   */
/*                  InfoNES_Menu() : Menu screen                     */
/*                                                                   */
/*===================================================================*/
int InfoNES_Menu()
{
/*
 *  Menu screen
 *
 *  Return values
 *     0 : Normally
 *    -1 : Exit InfoNES
 */

  if ( PAD_PUSH( PAD_System, PAD_SYS_QUIT) )
    return -1;	 	

  // Nothing to do here
  return 0;
}


/*===================================================================*/
/*                                                                   */
/*               InfoNES_ReadRom() : Read ROM image file             */
/*                                                                   */
/*===================================================================*/
int InfoNES_ReadRom( const char *pszFileName )
{
/*
 *  Read ROM image file
 *
 *  Parameters
 *    const char *pszFileName          (Read)
 *
 *  Return values
 *     0 : Normally
 *    -1 : Error
 */

//  FILE *fp;

  /* Open ROM file */
//  fp = fopen( pszFileName, "rb" );
//  if ( fp == NULL )
//    return -1;

  /* Read ROM Header */
  nes_game[SELECT_GAME].index = 0;
  nesReadFile(&NesHeader, sizeof NesHeader, 1, &nes_game[SELECT_GAME]);
  
//  fread( &NesHeader, sizeof NesHeader, 1, fp );
  if ( memcmp( NesHeader.byID, "NES\x1a", 4 ) != 0 )
  {
    /* not .nes file */
//    fclose( fp );
    return -1;
  }

  /* Clear SRAM */
  memset( SRAM, 0, SRAM_SIZE );

  /* If trainer presents Read Triner at 0x7000-0x71ff */
  if ( NesHeader.byInfo1 & 4 )
  {
//    fread( &SRAM[ 0x1000 ], 512, 1, fp );
      nesReadFile(&SRAM[ 0x1000 ], 512, 1, &nes_game[SELECT_GAME]);
  }

  /* Allocate Memory for ROM Image */
  ROM = (BYTE *)malloc( NesHeader.byRomSize * 0x4000 );
  if (ROM == NULL) return -1;

  /* Read ROM Image */
//  fread( ROM, 0x4000, NesHeader.byRomSize, fp );
  nesReadFile(ROM, 0x4000, NesHeader.byRomSize, &nes_game[SELECT_GAME]);
  
  if ( NesHeader.byVRomSize > 0 )
  {
    /* Allocate Memory for VROM Image */
    VROM = (BYTE *)malloc( NesHeader.byVRomSize * 0x2000 );
    if (VROM == NULL) { free(ROM); ROM = NULL; return -1; }

    /* Read VROM Image */
//    fread( VROM, 0x2000, NesHeader.byVRomSize, fp );
    nesReadFile(VROM, 0x2000, NesHeader.byVRomSize, &nes_game[SELECT_GAME]);
  }

//  /* File close */
//  fclose( fp );

  /* Successful */
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*           InfoNES_ReleaseRom() : Release a memory for ROM         */
/*                                                                   */
/*===================================================================*/
void InfoNES_ReleaseRom()
{
/*
 *  Release a memory for ROM
 *
 */

  if ( ROM )
  {
    free( ROM );
    ROM = NULL;
  }

  if ( VROM )
  {
    free( VROM );
    VROM = NULL;
  }
}

/*===================================================================*/
/*                                                                   */
/*      InfoNES_LoadFrame() :                                        */
/*           Transfer the contents of work frame on the screen       */
/*                                                                   */
/*===================================================================*/
void InfoNES_LoadFrame(void)
{
    NES_BoardPresent(WorkFrame);
}

/*===================================================================*/
/*                                                                   */
/*             InfoNES_PadState() : Get a joypad state               */
/*                                                                   */
/*===================================================================*/
void InfoNES_PadState(DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem)
{
    *pdwPad1 = NES_BoardReadPad();
    *pdwPad2 = 0;
    *pdwSystem = 0;
}

/*===================================================================*/
/*                                                                   */
/*             InfoNES_MemoryCopy() : memcpy                         */
/*                                                                   */
/*===================================================================*/
void *InfoNES_MemoryCopy( void *dest, const void *src, int count )
{
/*
 *  memcpy
 *
 *  Parameters
 *    void *dest                       (Write)
 *      Points to the starting address of the copied block's destination
 *
 *    const void *src                  (Read)
 *      Points to the starting address of the block of memory to copy
 *
 *    int count                        (Read)
 *      Specifies the size, in bytes, of the block of memory to copy
 *
 *  Return values
 *    Pointer of destination
 */

  memcpy( dest, src, count );
  return dest;
}


/*===================================================================*/
/*                                                                   */
/*             InfoNES_MemorySet() : memset                          */
/*                                                                   */
/*===================================================================*/
void *InfoNES_MemorySet( void *dest, int c, int count )
{
/*
 *  memset
 *
 *  Parameters
 *    void *dest                       (Write)
 *      Points to the starting address of the block of memory to fill
 *
 *    int c                            (Read)
 *      Specifies the byte value with which to fill the memory block
 *
 *    int count                        (Read)
 *      Specifies the size, in bytes, of the block of memory to fill
 *
 *  Return values
 *    Pointer of destination
 */

  memset( dest, c, count);  
  return dest;
}


/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundInit() : Sound Emulation Initialize           */
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundInit( void ) 
{
NES_AudioReset();
}



/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundOpen() : Sound Open                           */
/*                                                                   */
/*===================================================================*/
int InfoNES_SoundOpen(int samples_per_sync, int sample_rate)
{
    return NES_AudioOpen(samples_per_sync, sample_rate);
}

/*===================================================================*/
/*                                                                   */
/*        InfoNES_SoundClose() : Sound Close                         */
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundClose(void)
{
    NES_AudioClose();
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_SoundOutput() : Sound Output 5 Waves           */           
/*                                                                   */
/*===================================================================*/
void InfoNES_SoundOutput(int samples, BYTE *wave1, BYTE *wave2, BYTE *wave3, BYTE *wave4, BYTE *wave5)
{
    NES_AudioOutput(samples, wave1, wave2, wave3, wave4, wave5);
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_Wait() : Wait Emulation if required            */
/*                                                                   */
/*===================================================================*/
void InfoNES_Wait() {}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_MessageBox() : Print System Message            */
/*                                                                   */
/*===================================================================*/
void InfoNES_MessageBox( char *pszMsg, ... )
{
//    va_list args;
//    va_start( args, pszMsg );
//    printf( pszMsg, args );	
//    va_end( args );
}


