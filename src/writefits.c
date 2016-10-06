/*
 *  writefits.c
 *  This file is part of LIME, the versatile line modeling engine
 *
 *  Copyright (C) 2006-2014 Christian Brinch
 *  Copyright (C) 2015-2016 The LIME development team
 *
 */

#include "lime.h"

void writeFits(const char *fits_filename, configInfo *par, molData *m, image *img, const int im, const int unit){
  if((img[im].doline == 1 || (img[im].doline==0 && par->polarization)) && img[im].imgunits[unit] != 5){
    write3Dfits(fits_filename, par, m, img, im, unit);
  }
  else if(img[im].doline == 0 || img[im].imgunits[unit] == 5){
    write2Dfits(fits_filename, par, m, img, im, unit);
  }
  else{
    if(!silent) bail_out("FITS output unclear");
  }
}

void fitsFilename(char* fits_filename, configInfo *par, image *img, const int im, const int unit){
  char temp_filename[200];
  static char* unit_names[] = {"Kelvin", "Jansky-per-px", "SI", "LSun-per-px", "Tau", "#Rays"};

  /* Check unit number has a corresponding unit name */
  if(img[im].numunits < 0 || img[im].numunits > sizeof(unit_names)/sizeof(*unit_names) - 1){
    if(!silent) bail_out("Image unit number does not have a corresponding unit name");
    exit(0);
  }

  /* Append unit name to temporary filename */
  strcpy(temp_filename, img[im].filename);
  strcat(temp_filename, "_");
  strcat(temp_filename, unit_names[img[im].imgunits[unit]]);
  strcat(temp_filename, ".fits");

  /* Update image filename from temporary filename */
  strcpy(fits_filename, temp_filename);
}

void 
write3Dfits(const char *fits_filename, configInfo *par, molData *m, image *img, const int im, const int unit){
  double bscale,bzero,epoch,lonpole,equinox,restfreq;
  double cdelt1,crpix1,crval1,cdelt2,crpix2,crval2;
  double cdelt3,crpix3,crval3,ru3,scale;
  int velref;
  float *row;
  int px,py,ichan;
  fitsfile *fptr;
  int status = 0;
  int naxis=3, bitpix=-32;
  long naxes[3];
  long int fpixels[3],lpixels[3];
  char negfile[100]="! ";
  unsigned long ppi;

  row = malloc(sizeof(*row)*img[im].pxls);

  naxes[0]=img[im].pxls;
  naxes[1]=img[im].pxls;
  if(img[im].doline==1) naxes[2]=img[im].nchan;
  else if(img[im].doline==0 && par->polarization) naxes[2]=3;

  fits_create_file(&fptr, fits_filename, &status);

  if(status!=0){
    if(!silent) warning("Overwriting existing fits file                   ");
    status=0;
    strcat(negfile, fits_filename);
    fits_create_file(&fptr, negfile, &status);
  }

  /* Write FITS header */ 
  fits_create_img(fptr, bitpix, naxis, naxes, &status);
  epoch   =2.0e3;
  lonpole =1.8e2;
  equinox =2.0e3;
  restfreq=img[im].freq;
  velref  =257;
  cdelt1  =-1.8e2*img[im].imgres/PI;
  crpix1  =(double) img[im].pxls/2+0.5;
  crval1  =0.0e0;
  cdelt2  =1.8e2*img[im].imgres/PI;
  crpix2  =(double) img[im].pxls/2+0.5;
  crval2  =0.0e0;
  cdelt3  =img[im].velres;
  crpix3  =(double) (img[im].nchan-1)/2.+1;
  crval3  =0.0e0;
  bscale  =1.0e0;
  bzero   =0.0e0;

  fits_write_key(fptr, TSTRING, "OBJECT  ", &"LIMEMDL ",    "", &status);
  fits_write_key(fptr, TDOUBLE, "EPOCH   ", &epoch,         "", &status);
  fits_write_key(fptr, TDOUBLE, "LONPOLE ", &lonpole,       "", &status);
  fits_write_key(fptr, TDOUBLE, "EQUINOX ", &equinox,       "", &status);
  fits_write_key(fptr, TSTRING, "SPECSYS ", &"LSRK    ",    "", &status);
  fits_write_key(fptr, TDOUBLE, "RESTFREQ", &restfreq,      "", &status);
  fits_write_key(fptr, TINT,    "VELREF  ", &velref,        "", &status);
  fits_write_key(fptr, TSTRING, "CTYPE1  ", &"RA---SIN",    "", &status);
  fits_write_key(fptr, TDOUBLE, "CDELT1  ", &cdelt1,        "", &status);
  fits_write_key(fptr, TDOUBLE, "CRPIX1  ", &crpix1,        "", &status);
  fits_write_key(fptr, TDOUBLE, "CRVAL1  ", &crval1,        "", &status);
  fits_write_key(fptr, TSTRING, "CUNIT1  ", &"DEG     ",    "", &status);	
  fits_write_key(fptr, TSTRING, "CTYPE2  ", &"DEC--SIN",    "", &status);
  fits_write_key(fptr, TDOUBLE, "CDELT2  ", &cdelt2,        "", &status);
  fits_write_key(fptr, TDOUBLE, "CRPIX2  ", &crpix2,        "", &status);  	
  fits_write_key(fptr, TDOUBLE, "CRVAL2  ", &crval2,        "", &status);
  fits_write_key(fptr, TSTRING, "CUNIT2  ", &"DEG     ",    "", &status);
  fits_write_key(fptr, TSTRING, "CTYPE3  ", &"VELO-LSR",    "", &status);  
  fits_write_key(fptr, TDOUBLE, "CDELT3  ", &cdelt3,        "", &status);
  fits_write_key(fptr, TDOUBLE, "CRPIX3  ", &crpix3,        "", &status);
  fits_write_key(fptr, TDOUBLE, "CRVAL3  ", &crval3,        "", &status);
  fits_write_key(fptr, TSTRING, "CUNIT3  ", &"M/S     ",    "", &status);
  fits_write_key(fptr, TDOUBLE, "BSCALE  ", &bscale,        "", &status);
  fits_write_key(fptr, TDOUBLE, "BZERO   ", &bzero,         "", &status);
  if(img[im].imgunits[unit]==0) fits_write_key(fptr, TSTRING, "BUNIT", &"K       ", "", &status);
  if(img[im].imgunits[unit]==1) fits_write_key(fptr, TSTRING, "BUNIT", &"JY/PIXEL", "", &status);
  if(img[im].imgunits[unit]==2) fits_write_key(fptr, TSTRING, "BUNIT", &"WM2HZSR ", "", &status);
  if(img[im].imgunits[unit]==3) fits_write_key(fptr, TSTRING, "BUNIT", &"Lsun/PX ", "", &status);
  if(img[im].imgunits[unit]==4) fits_write_key(fptr, TSTRING, "BUNIT", &"        ", "", &status);

  if(     img[im].imgunits[unit]==0)
    scale=(CLIGHT/img[im].freq)*(CLIGHT/img[im].freq)/2./KBOLTZ;
  else if(img[im].imgunits[unit]==1)
    scale=1e26*img[im].imgres*img[im].imgres;
  else if(img[im].imgunits[unit]==2)
    scale=1.0;
  else if(img[im].imgunits[unit]==3) {
    ru3 = img[im].distance/1.975e13;
    scale=4.*PI*ru3*ru3*img[im].freq*img[im].imgres*img[im].imgres;
  }
  else if(img[im].imgunits[unit]!=4) {
    if(!silent) bail_out("Image unit number invalid");
    exit(0);
  }

  /* Write FITS data */
  for(ichan=0;ichan<img[im].nchan;ichan++){
    for(py=0;py<img[im].pxls;py++){
      for(px=0;px<img[im].pxls;px++){
        ppi = py*img[im].pxls + px;

        if(img[im].imgunits[unit]>-1 && img[im].imgunits[unit]<4)
          row[px]=(float) img[im].pixel[ppi].intense[ichan]*scale;
        else if(img[im].imgunits[unit]==4)
          row[px]=(float) img[im].pixel[ppi].tau[ichan];
        else {
          if(!silent) bail_out("Image unit number invalid");
          exit(0);
        }
        if (fabs(row[px])<(float) eps) row[px]=(float)eps;
      }
      fpixels[0]=1;
      fpixels[1]=py+1;
      fpixels[2]=ichan+1;
      lpixels[0]=img[im].pxls;
      lpixels[1]=py+1;
      lpixels[2]=ichan+1;
      fits_write_subset(fptr, TFLOAT, fpixels, lpixels, row, &status);
    }
  }
  fits_close_file(fptr, &status);

  free(row);

  if(!silent) printDone(13);
}

void 
write2Dfits(const char *fits_filename, configInfo *par, molData *m, image *img, const int im, const int unit){
  double bscale,bzero,epoch,lonpole,equinox,restfreq;
  double cdelt1,crpix1,crval1,cdelt2,crpix2,crval2;
  double ru3,scale;
  int velref;
  float *row,minVal;
  int px,py;
  fitsfile *fptr;
  int status = 0;
  int naxis=2, bitpix=-32;
  long naxes[2];
  long int fpixels[2],lpixels[2];
  char negfile[100]="! ";
  unsigned long ppi;

  row = malloc(sizeof(*row)*img[im].pxls);

  naxes[0]=img[im].pxls;
  naxes[1]=img[im].pxls;

  fits_create_file(&fptr, fits_filename, &status);

  if(status!=0){
    if(!silent) warning("Overwriting existing fits file                   ");
    status=0;
    strcat(negfile,fits_filename);
    fits_create_file(&fptr, negfile, &status);
  }

  /* Write FITS header */ 
  fits_create_img(fptr, bitpix, naxis, naxes, &status);
  epoch   =2.0e3;
  lonpole =1.8e2;
  equinox =2.0e3;
  restfreq=img[im].freq;
  velref  =257;
  cdelt1  =-1.8e2*img[im].imgres/PI;
  crpix1  =(double) img[im].pxls/2+0.5;
  crval1  =0.0e0;
  cdelt2  =1.8e2*img[im].imgres/PI;
  crpix2  =(double) img[im].pxls/2+0.5;
  crval2  =0.0e0;
  bscale  =1.0e0;
  bzero   =0.0e0;

  fits_write_key(fptr, TSTRING, "OBJECT  ", &"LIMEMDL ",    "", &status);
  fits_write_key(fptr, TDOUBLE, "EPOCH   ", &epoch,         "", &status);
  fits_write_key(fptr, TDOUBLE, "LONPOLE ", &lonpole,       "", &status);
  fits_write_key(fptr, TDOUBLE, "EQUINOX ", &equinox,       "", &status);
  fits_write_key(fptr, TSTRING, "SPECSYS ", &"LSRK    ",    "", &status);
  fits_write_key(fptr, TDOUBLE, "RESTFREQ", &restfreq,      "", &status);
  fits_write_key(fptr, TINT,    "VELREF  ", &velref,        "", &status);
  fits_write_key(fptr, TSTRING, "CTYPE1  ", &"RA---SIN",    "", &status);
  fits_write_key(fptr, TDOUBLE, "CDELT1  ", &cdelt1,        "", &status);
  fits_write_key(fptr, TDOUBLE, "CRPIX1  ", &crpix1,        "", &status);
  fits_write_key(fptr, TDOUBLE, "CRVAL1  ", &crval1,        "", &status);
  fits_write_key(fptr, TSTRING, "CUNIT1  ", &"DEG     ",    "", &status);	
  fits_write_key(fptr, TSTRING, "CTYPE2  ", &"DEC--SIN",    "", &status);
  fits_write_key(fptr, TDOUBLE, "CDELT2  ", &cdelt2,        "", &status);
  fits_write_key(fptr, TDOUBLE, "CRPIX2  ", &crpix2,        "", &status);  	
  fits_write_key(fptr, TDOUBLE, "CRVAL2  ", &crval2,        "", &status);
  fits_write_key(fptr, TSTRING, "CUNIT2  ", &"DEG     ",    "", &status);
  fits_write_key(fptr, TDOUBLE, "BSCALE  ", &bscale,        "", &status);
  fits_write_key(fptr, TDOUBLE, "BZERO   ", &bzero,         "", &status);
  if(unit==0) fits_write_key(fptr, TSTRING, "BUNIT", &"K       ", "", &status);
  if(unit==1) fits_write_key(fptr, TSTRING, "BUNIT", &"JY/PIXEL", "", &status);
  if(unit==2) fits_write_key(fptr, TSTRING, "BUNIT", &"WM2HZSR ", "", &status);
  if(unit==3) fits_write_key(fptr, TSTRING, "BUNIT", &"Lsun/PX ", "", &status);
  if(unit==4) fits_write_key(fptr, TSTRING, "BUNIT", &"        ", "", &status);
  if(unit==5) fits_write_key(fptr, TSTRING, "BUNIT", &"N_RAYS  ", "", &status);

  if(     img[im].imgunits[unit]==0)
    scale=(CLIGHT/img[im].freq)*(CLIGHT/img[im].freq)/2./KBOLTZ;
  else if(img[im].imgunits[unit]==1)
    scale=1e26*img[im].imgres*img[im].imgres;
  else if(img[im].imgunits[unit]==2)
    scale=1.0;
  else if(img[im].imgunits[unit]==3) {
    ru3 = img[im].distance/1.975e13;
    scale=4.*PI*ru3*ru3*img[im].freq*img[im].imgres*img[im].imgres;
  }
  else if(img[im].imgunits[unit]!=4 && img[im].imgunits[unit]!=5) {
    if(!silent) bail_out("Image unit number invalid");
    exit(0);
  }

  if(img[im].imgunits[unit]<5)
    minVal = eps;
  else
    minVal = 0.0;

  /* Write FITS data */
  for(py=0;py<img[im].pxls;py++){
    for(px=0;px<img[im].pxls;px++){
      ppi = py*img[im].pxls + px;

      if(img[im].imgunits[unit]>-1 && img[im].imgunits[unit]<4)
        row[px]=(float) img[im].pixel[ppi].intense[0]*scale;
      else if(img[im].imgunits[unit]==4)
        row[px]=(float) img[im].pixel[ppi].tau[0];
      else if(img[im].imgunits[unit]==5)
        row[px]=(float) img[im].pixel[ppi].numRays;
      else {
        if(!silent) bail_out("Image unit number invalid");
        exit(0);
      }
      if (fabs(row[px])<minVal) row[px]=minVal;

      fpixels[0]=1;
      fpixels[1]=py+1;
      lpixels[0]=img[im].pxls;
      lpixels[1]=py+1;
      fits_write_subset(fptr, TFLOAT, fpixels, lpixels, row, &status);
    }
  }
  fits_close_file(fptr, &status);

  free(row);

  if(!silent) printDone(13);
}

