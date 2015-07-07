/*
 *  messages.c
 *  LIME, The versatile 3D line modeling environment
 *
 *  Created by Christian Brinch on 29/10/08.
 *  Copyright 2006-2014, Christian Brinch,
 *  <brinch@nbi.dk>
 *  Niels Bohr institutet
 *  University of Copenhagen
 *	All rights reserved.
 *
 */

#include "lime.h"
#include <curses.h>
#include <time.h>

void
greetings(){
#ifdef DEBUG

  printf("*** LIME, The versatile line modeling engine, Ver. %s\n", VERSION);
#ifdef TEST
  printf(">>> NOTE! Test flag is set in the Makefile. <<<\n");
#endif
#ifdef FASTEXP
  printf(">>> NOTE! Fast-exponential routine is enabled. <<<\n");
#endif

#else

  initscr();
  printw("*** LIME, The versatile line modeling engine, Ver. %s\n", VERSION);
#ifdef TEST
  printw(">>> NOTE! Test flag is set in the Makefile. <<<\n");
#endif
#ifdef FASTEXP
  printw(">>> NOTE! Fast-exponential routine is enabled. <<<\n");
#endif
  refresh();

#endif
}

void
greetings_parallel(numThreads){
#ifdef DEBUG

  if (numThreads>1){
    printf("*** LIME, The versatile line modeling engine, Ver. %s (parallel running, %d threads)\n", VERSION, numThreads);
  } else {
    printf("*** LIME, The versatile line modeling engine, Ver. %s\n", VERSION);
  }
#ifdef TEST
  printf(">>> NOTE! Test flag is set in the Makefile. <<<\n");
#endif
#ifdef FASTEXP
  printf(">>> NOTE! Fast-exponential routine is enabled. <<<\n");
#endif

#else

  initscr();
  if (numThreads>1){
    printw("*** LIME, The versatile line modeling engine, Ver. %s (parallel running, %d threads)\n", VERSION, numThreads);
  } else {
    printw("*** LIME, The versatile line modeling engine, Ver. %s\n", VERSION);
  }
#ifdef TEST
  printw(">>> NOTE! Test flag is set in the Makefile. <<<\n");
#endif
#ifdef FASTEXP
  printw(">>> NOTE! Fast-exponential routine is enabled. <<<\n");
#endif
  refresh();

#endif
}

void
screenInfo(){
#ifndef DEBUG
  move(4,4);  printw("Building grid      :");
  move(4,51); printw("|");
  move(5,4);  printw("Smoothing grid     :");
  move(5,51); printw("|");
  move(7,4);  printw("Statistics         :");
  move(9,4);  printw("Iterations         :");
  move(10,4); printw("Propagating photons:");
  move(10,51);printw("|");
  move(13,4); printw("Ray-tracing model  :");
  move(13,51);printw("|");
  move(4,60); printw("|      Molecular data");
  move(5,60); printw("|");
  move(6,60); printw("|");
  move(7,60); printw("|");
  move(8,60); printw("|");
  move(9,60); printw("|");
  move(10,60); printw("|");
  move(11,60); printw("|");
  move(12,60); printw("|");
  move(13,60); printw("|");
  move(14,60); printw("|");
  refresh();	
#endif
}

void
done(int line){
#ifdef DEBUG
  if (line == 4)
    printf(  "   Building grid: DONE                               \n\n"); 
  else if (line == 5)
    printf(  "   Building grid: DONE                               \n\n");
  else if (line == 10)
    printf(  "   Propagating photons: DONE                         \n\n");
  else if (line == 13)
    printf(  "   Raytracing model: DONE                            \n\n");
  else if (line == 15)
    printf("\n   Writing fits file: DONE                           \n\n");

#else
  move(line,52); printw(" [ok]");
  refresh();
#endif
}

void
progressbar(double percent, int line){
#ifdef DEBUG
  if (line == 4)
    printf("   Building grid: %.2f percent done\r", percent * 100.);
  else if (line == 5)
    printf("   Smoothing grid: %.2f percent done\r", percent * 100.);
  else if (line == 10)
    printf("   Propagating photons: %.2f percent done\r", percent * 100.);
  else if (line == 13)
    printf("   Raytracing model: %.2f percent done\r", percent * 100.);
  else if (line == 15)
    printf("   Writing fits file\n");

#else
  int i;
  for(i=0;i<(int)(percent*25.);i++){
    move(line,25+i);
    printw("#");
  }
  refresh();
#endif
}

void
progressbar2(int prog, double percent, double minsnr, double median){
#ifndef DEBUG
  move(7,38); printw("                    ");            
  move(8,38); printw("                    ");
  if(minsnr<1000){
    move(7,25); printw("Min(SNR)    %3.3f", minsnr);
  } else {
    move(7,25); printw("Min(SNR)    %.3e", minsnr);
  }
  if(median<1000){
    move(8,25);	printw("Median(SNR) %3.3f", median);
  } else {
    move(8,25); printw("Median(SNR) %.3e", median);
  }
  move(9,25+prog); printw("#");
  if(percent<100) {
    move(10,25);	 printw("                         ");
  }
  refresh();	
#endif
}

void
progressbar2_new(int flag, int prog, double percent, double minsnr, double median){
#ifdef DEBUG
  if (flag == 1){
    if (minsnr < 1000)
      printf("      Statistics: Min(SNR)    %3.3f                     \n", minsnr); 
    else 
      printf("      Statistics: Min(SNR)    %.3e                      \n", minsnr);

    if (median < 1000)
      printf("      Statistics: Median(SNR) %3.3f                     \n", median);
    else 
      printf("      Statistics: Median(SNR) %.3e                      \n", median);

    printf("  Iteration %i / %i: DONE\n\n", prog, NITERATIONS + 1);
  } else if (flag == 0)
    printf("  Iteration %i / %i: Starting\n", prog + 1, NITERATIONS + 1);

#else
  move(7,38); printw("                    ");            
  move(8,38); printw("                    ");
  if(minsnr<1000){
    move(7,25); printw("Min(SNR)    %3.3f", minsnr);
  } else {
    move(7,25); printw("Min(SNR)    %.3e", minsnr);
  }
  if(median<1000){
    move(8,25);	printw("Median(SNR) %3.3f", median);
  } else {
    move(8,25); printw("Median(SNR) %.3e", median);
  }
  move(9,25+prog); printw("#");
  if(percent<100) {
    move(10,25);	 printw("                         ");
  }
  refresh();	
#endif
}

void
goodnight(int initime, char filename[80]){
  int runtime=time(0)-initime;
#ifdef DEBUG
  printf("Output written to %s", filename);
  printf("*** Program ended successfully               \n");
  printf("    Runtime: %3dh %2dm %2ds\n\n", runtime / 3600, runtime / 60 % 60, runtime % 60);

#else
  move(14,4); printw("Output written to %s", filename);
  move(22,0); printw("*** Program ended successfully               ");
  move(22,58); printw("runtime: %3dh %2dm %2ds", runtime/3600, runtime/60%60, runtime%60);
  move(23,0); printw("*** [Press any key to quit]");
  refresh();
  getch();
  endwin();
#endif
}

void
quotemass(double mass){
#ifdef DEBUG
  printf("  Total mass contained in model: %3.2e solar masses", mass);
#else
  move(21,6); printw("Total mass contained in model: %3.2e solar masses", mass);
  refresh();
#endif
}



void
warning(char message[80]){
#ifdef DEBUG
  if(strlen(message)>0)
    {
      printf("Warning : %s\n", message );
    }
#else
  move(22,0); printw("*** %s\n",message);
  refresh();
#endif
}

void
bail_out(char message[80]){
#ifdef DEBUG
  printf("Error : %s\n", message );
#else
  move(22,0); printw("*** %s",message);
  move(23,0); printw("*** [Press any key to quit]");
  refresh();
  getch();
  endwin();
#endif
}

void
collpartmesg(char molecule[90], int partners){//, int specnumber){
#ifdef DEBUG
  printf("   Molecule: %.25s\n", molecule);
  if (partners==1)
    printf("   %d collision partner:\n", partners);
  else
    printf("   %d collision partners:\n", partners);

#else
  move(6,63); printw("%.25s", molecule);
  move(7,63);
  if (partners==1)
    printw("%d collision partner:", partners);
  else
    printw("%d collision partners:", partners);

  refresh();
#endif
}

void
collpartmesg2(char name[10], int partner){
#ifdef DEBUG
  printf("      %s\n ", name);
#else
  move(8,63); printw("%s ",name);
  refresh();
#endif
}

void
collpartmesg3(int number, int flag){
#ifdef DEBUG
  if (number==1)
    printf("   Model provides: %d density profile\n\n", number);
  else
    printf("   Model provides: %d density profiles\n\n", number);

  if(flag==1) printf("*** Warning! ***: Too few density profiles");  
#else
  move(10,63); printw("Model provides:");
  move(11,63);
  if (number==1)
    printw("%d density profile", number);
  else
    printw("%d density profiles", number);

  if(flag==1) {
    move(13,63); printw("*** Warning! ***");
    move(14,63); printw("Too few density profiles");
  }
  refresh();
#endif
}