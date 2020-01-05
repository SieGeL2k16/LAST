#include <exec/execbase.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/rdargs.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <stdlib.h>
#include <strings.h>
#include "struct_ex.h"
#include "global_defines.h"
#include <clib/utility_protos.h>
#include <proto/utility.h>
#include "proto.h"
#include "struct_ex.h"

void BuiltBull(char *); 	// erzeugt Bull !
void ParseFlags(void);		// Scanned die User-Flags durch

void ParseFlags(void)
	{
	char	buf[200],tocheck[200],*token,y2kbuf[200];
	int		i=0;
	long	zeit=0L,std=0L;

	lc.Flags[6]='G';							// Standardmaessig auf GURU-ENDE :)
	strcpy(lc.Totaltime,"--:--");	// Default: Keine Zeit
	while(1)
		{
		*buf='\0';token=NULL;lczeit=NULL;
		i=0;
		FAMEStrFil(buf,'\0',198);
   	if(!(FGets(loc,buf,199))) return;
    strcpy(tocheck,buf);
    if(!(token=strtok(buf,":"))) continue;
		if(!(Stricmp(token,"DOWNLOADING"))) {	lc.Flags[3]='D';continue;};
		if(!(Stricmp(token,"DOWNLOAD FAIL"))) { lc.Flags[3]='d';continue;};
		if(!(Stricmp(token,"UPLOADING"))) {	lc.Flags[4]='U';continue;};
		if(!(Stricmp(token,"UPLOAD FAIL"))) { lc.Flags[4]='u';continue;};
		if(!(Stricmp(token,"PAGED SYSOP AT")))
			{
      switch(tocheck[strlen(tocheck)-2])
				{
				case 'K':
				case 'L':	lc.Flags[2]='p';
                  break;
				default:	lc.Flags[2]='P';
                  break;
				};
			lcheader.TodayPages++;
			continue;
			};
		if(!(Stricmp(token,"<+>ACCOUNT EDITING ON"))) lc.Flags[5]='S';
    if(!(Stricmp(token,"UPLOADSTAT")))
			{
      if(!(token=strtok(NULL,","))) continue;
			lc.Ulfiles+=MyAtol(token);
			if(!(token=strtok(NULL," "))) continue;
			lc.Ulbytes+=MyAtol(token);
      continue;
			};
		if(!(Stricmp(token,"DOWNLOADSTAT")))
			{
      if(!(token=strtok(NULL,","))) continue;
      lc.Dlfiles+=MyAtol(token);
			if(!(token=strtok(NULL," "))) continue;
			lc.Dlbytes+=MyAtol(token);
			continue;
			};
    strcpy(y2kbuf,token);
		strcat(y2kbuf,":");
    if(!(Stricmp(token,"OFFDATE")))
			{
			if(!(token=strtok(NULL," "))) continue;
			strcpy(offdate,token);
			if(!(token=strtok(NULL,":"))) continue;
			if(!(token=strtok(NULL," "))) continue;
			strcpy(lc.Totaltime,token);
			if(!(token=strtok(NULL,":"))) continue;
			if(!(token=strtok(NULL," "))) continue;
      if(!(Stricmp(token,"USER"))||(!(Stricmp(token,"NO"))))
				{
				lc.Flags[1]='C';
				lcheader.TodayLC++;
        };
      lc.Flags[6]='-';zeit=0;std=0;
			zeit=makeallsec(lc.Conntime,lc.Totaltime);
			lcheader.TodayBusy+=zeit;
			lczeit=zeit;
			*lc.Totaltime='\0';
			if(zeit>=60)
        {
				while(zeit>=60)
					{
					std++;
					zeit-=60;
					if(zeit<0)
            {
						zeit+=60;
						std--;
						break;
						};
					};
        };
			SPrintf(lc.Totaltime,"%2.2ld:%2.2ld",std,zeit);
			i=0;
			while(i<4) {if(lc.Totaltime[i]==' ') lc.Totaltime[i]='0';i++;};
      break;
			};
		};
	}

void BuiltBull(char *wohin)
	{
	BPTR 	te=NULL,data=NULL,testptr=NULL;
	char	ub[15],db[15],n[2],mybuf[256];
	int		i=0;
	long	st=0,s=0;
	long	myheader,mycallers;

	myheader=sizeof(struct LCHeader);
	mycallers=sizeof(struct LastCallers);
	if(!(te=Open(wohin,MODE_NEWFILE))) return;

#ifdef DEBUGIT

	if(!(data=Open(DEBUGDATA,MODE_OLDFILE))) { Close(te); return;};

#else

	if(!(data=Open(DATAFILE,MODE_OLDFILE))) { Close(te); return;};

#endif

	if(!(testptr=Open("PROGDIR:HEADER.TXT",MODE_OLDFILE)))
		{
		FPrintf(te,"\f\n[15C[35mlAST [33m%ld [35mcALLERS [mv%s [36mbY SieGeL [m([36mtRSi[m/[36mF[m-[36miNNOVATiON[m)\n\n",anzusers,LAST_VER);
		}
	else
		{
		while(FGets(testptr,mybuf,255)) FPuts(te,mybuf);
		Close(testptr);
		}
	FPuts(te,BULL_HEADER1);
	FPuts(te,BULL_HEADER2);
  while(i<anzusers)
		{
    Seek(data,(myheader+mycallers*i),OFFSET_BEGINNING);
		Read(data,&lc,mycallers);
		switch(lc.NewUser)
			{
			case TRUE:  *n='*';
									break;
			case FALSE:	*n=' ';
									break;
			};
		n[1]='\0';
		if(lc.Dlbytes>9999) { lc.Dlbytes/=1024;SPrintf(db,"%3.3ldMB\0",lc.Dlbytes);}
		else SPrintf(db,"%5.5ld\0",lc.Dlbytes);
		if(lc.Ulbytes>9999) { lc.Ulbytes/=1024;SPrintf(ub,"%3.3ldMB\0",lc.Ulbytes);}
		else SPrintf(ub,"%5.5ld\0",lc.Ulbytes);
		FPrintf(te,"[31m%4.4s [32m%2ld[31m%s[33m%-14.14s [36m%-19.19s\r[43C[34m%8.8s [33m[[35m%7.7s[33m] [36m%5.5s [32m%5.5s %5.5s\n",lc.Baud,lc.Node,n,lc.Handle,lc.Location,lc.Conntime,lc.Flags,lc.Totaltime,ub,db);
		i++;
		memset(&lc,0,mycallers);
		};
	FPuts(te,BULL_HEADER2);
	st=0;s=0;
	st=lcheader.TodayBusy;
	if(st>=60) s=st/60;
	FPrintf(te," [36mCalls[33m: [32m%4ld [34m/ [36mY'day[33m: [32m%4ld [34m| [36mU[33m: [32m%3ld[34m / [32m%6ld[36m kB [34m| [36mD[33m: [32m%3ld[34m / [32m%6ld[36m kB [34m| [36mHrs[33m: [32m%ld\n",lcheader.TodayCalls,lcheader.LastCalls,lcheader.TodayUpFiles,lcheader.TodayUpBytes,lcheader.TodayDlFiles,lcheader.TodayDlBytes,s);
	s=0;st=0;
	st=lcheader.BestBusy;
	if(st>=60) s=st/60;
	FPrintf(te," [36mThe Records [34m- [36mCalls[33m: [32m%4ld [34m| [36mU[33m: [32m%3ld[34m / [32m%6ld[36m kB [34m| [36mD[33m: [32m%3ld[34m / [32m%6ld[36m kB [34m| [36mHrs[33m: [32m%ld\n",lcheader.BestCalls,lcheader.BestUpFiles,lcheader.BestUpBytes,lcheader.BestDlFiles,lcheader.BestDlBytes,s);
  FPuts(te,BULL_HEADER2);
	FPuts(te,LEGENDE1);
	FPuts(te,LEGENDE2);
	Close(te);
	Close(data);
	}
