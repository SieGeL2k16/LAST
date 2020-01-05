/*****************************************************************************
 *                  lAST-cALLERS fOR f.A.M.E. BBS sYSTEM                     *
 *                                                                           *
 *            wRITTEN bY SieGeL (tRSi/F-iNNOVATiON) 1995-2000                *
 *****************************************************************************/

#include <FAME/FAME.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/rdargs.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <stdlib.h>
#include <strings.h>
#include "global_defines.h"
#include "struct.h"
#include <clib/utility_protos.h>
#include <proto/utility.h>
#include "proto.h"
#include "struct.h"

extern struct	 ExecBase *SysBase;
extern char *_ProgramName;

STATIC const char *Ver="\0$VER:LastCallers "LAST_VER" "__AMIGADATE__"\0";
long	ArgArray[6]={0L,0L,0L,0L,0L,0L};

LONG	WriteDataFile(long);
ULONG	checklog(char *);
BOOL	checkdatafile(void);						// Prueft Vorhandensein vom datafile
void	UpdateStats(void);							// Updated das Datafile
void 	SPrintf(char *Buffer,char *ctl, ...);
void 	CloseAll(void);
LONG 	MyAtol(char *was);

void main(void)
	{
	struct	RDArgs *rda=NULL;
	char		bullpath[200];
	long		node=NULL;
	USHORT	retcode=0;

	anzusers=NULL;last=NULL;
	if(SysBase->LibNode.lib_Version<37) return;
  if(!(UtilityBase=OpenLibrary("utility.library",37))) { CloseAll();SetIoErr(ERROR_INVALID_RESIDENT_LIBRARY);return;};
	if(!(FAMEBase=(struct FAMELibrary *)OpenLibrary(FAMENAME,4))) { CloseAll(); SetIoErr(ERROR_INVALID_RESIDENT_LIBRARY);return;};
	if(!(last=(struct LastCallers *)AllocVec((long) (sizeof(struct LastCallers)*21),MEMF_PUBLIC|MEMF_CLEAR))) { SetIoErr(ERROR_NO_FREE_STORE);CloseAll();return;};
  Decrypt(regstring);
  Printf("\n\nLastCallers V%s by Sascha 'SieGeL' Pfalz\n\n",LAST_VER);
	Printf("%s\n\n",regstring);
	strcpy(lc.Flags,"-------");
	if(rda=ReadArgs("CALLERSLOG/A,NODE-NR./N/A,USERS/N/A,OUTPUT-BULL/A,TELNET/S,LOGFILE/S",ArgArray,rda))
		{
		SPrintf(CPath,"%s",ArgArray[0]);
    node=*(long *) ArgArray[1];
		anzusers=*(long *) ArgArray[2];
		SPrintf(bullpath,"%s\0",ArgArray[3]);
		if(ArgArray[4]) telnet=TRUE;
		else telnet=FALSE;
		if(ArgArray[5]) logfile=TRUE;
		else logfile=FALSE;
		FreeArgs(rda);rda=NULL;
		if(!anzusers) goto ende;
		if(anzusers>20) anzusers=20L;
		if(checkdatafile()==FALSE)
			{
    	retcode=1;
			goto ende;
			}
		if(!(checklog(CPath))) retcode=1;
		else
      {
			ParseFlags();
      switch(WriteDataFile(node))
				{
				case	1:	Print("\nUnable to Write DATAS!!!\n\n");
									retcode=1;
									break;
				case	0:	retcode=0;
									break;
				case	2:  Printf("Caller %s is already in Database, updating skipped!\n",lc.Handle);
									Close(loc);
									CloseAll();
									exit(0);

				}
      }
    }
	else retcode=1;
	ende:
  if(retcode) PrintFault(IoErr(),_ProgramName);
	if (loc) Close(loc);loc=NULL;
	if(!retcode) BuiltBull(bullpath);
	CloseAll();
	exit(0);
  }

void CloseAll(void)
	{
	if(UtilityBase) CloseLibrary(UtilityBase);UtilityBase=NULL;
  if(FAMEBase) CloseLibrary((struct Library *)FAMEBase);FAMEBase=NULL;
	if(last) FreeVec((struct LastCallers *)last);last=NULL;
	}

ULONG checklog(char *pfad)
	{
	char 	str[200];
	ULONG	laenge=NULL;
	long	found=-1,searchbytes=-2500L,mycount=0L;
	struct	FileInfoBlock __aligned fib;

	mycount=0;
	while(mycount<5)
		{
		if(!(loc=Open(pfad,MODE_OLDFILE)))
			{
			Printf("Can't lock Callers.Log (%ld), Retry #%ld !\n",IoErr(),mycount);
			Delay(50L);
			mycount++;
			}
    else
			{
			mycount=-1;
			break;
			}
		}
	if(mycount>0)
		{
		Print("ERROR OPENING CALLERSLOG!\n\n");
		return(FALSE);
		};
  ExamineFH(loc,&fib);
	laenge=fib.fib_Size;
	do
		{
		searchbytes*=2;
		if(laenge<(searchbytes*-1)) Seek(loc,0,OFFSET_BEGINNING);
		else Seek(loc,searchbytes,OFFSET_END);
		while(FGets(loc,str,199))
			{
			if(str[0]=='*'&&strlen(str)==80) found=Seek(loc,0L,OFFSET_CURRENT);
			};
		}while(found<0&&(searchbytes*-1)<laenge);
	if(found==-1) return(0L);
	Print("Found Last Caller...\n\n");
  Seek(loc,found,OFFSET_BEGINNING);
	if(!(Read_Caller()))
		{
		return(0L);
		}
	else
		{
		return(1L);
		}
	}

LONG	WriteDataFile(long no)
	{
  BPTR	data=NULL;
	BOOL	updt=FALSE;
	register int		zaehlmal=18,		// Offset-Sprung
									zaehler=19;
	register long		mycallers,myheader;

	myheader=sizeof(struct LCHeader);
	mycallers=sizeof(struct LastCallers);

	if(!Stricmp(last[0].Handle,lc.Handle) && last[0].Node==no && !Stricmp(last[0].Conntime,lc.Conntime)) return(2);
	if(!(Stricmp(ondate,lcheader.TodayDate)))
		{
    if(*offdate)
			{
			if(Stricmp(offdate,lcheader.TodayDate)||Stricmp(ondate,offdate))
				{
				UpdateStats();
				updt=TRUE;
				}
			}
    }
	else
    {
		if(*offdate)
			{
			if(Stricmp(offdate,lcheader.TodayDate))
				{
 				UpdateStats();
				updt=TRUE;
				}
			}
		}
	lcheader.TodayUpBytes+=lc.Ulbytes;
	lcheader.TodayDlBytes+=lc.Dlbytes;
	lcheader.TodayUpFiles+=lc.Ulfiles;
	lcheader.TodayDlFiles+=lc.Dlfiles;
	lcheader.TodayCalls++;
  lc.Node=no;

#ifdef DEBUGIT

	if(!(data=Open(DEBUGDATA,MODE_READWRITE))) return(1);

#else

	if(!(data=Open(DATAFILE,MODE_READWRITE))) return(1);

#endif

 	Seek(data,0L,OFFSET_BEGINNING);
	Write(data,&lcheader,myheader);
  while(zaehlmal!=-1)
		{
		CopyMem(&last[zaehlmal],&last[zaehler],mycallers);
		zaehlmal--;zaehler--;
   	};
	last[0]=lc;
  zaehlmal=0;
	while(zaehlmal<20)
		{
		Seek(data,(myheader+mycallers*zaehlmal),OFFSET_BEGINNING);
		Write(data,&last[zaehlmal],mycallers);
		zaehlmal++;
		};
	Close(data);data=NULL;
	if(logfile==FALSE) return(0);
	if(!(data=Open("PROGDIR:LastCallers.log",MODE_OLDFILE)))
		{
		if(!(data=Open("PROGDIR:LastCallers.log",MODE_NEWFILE)))
			{
			PrintFault(IoErr(),"Create LogFile()");
			return(1);
			}
		}
  Seek(data,0L,OFFSET_END);
	if(updt==TRUE)
		{
		FPrintf(data,"\n**************************** NEW DATE: %s *****************************\n\n",lcheader.TodayDate);
		}
	FPrintf(data,"NAME: %-18.18s  LOGIN: %s  ONLINE: %s NODE:%ld\n",lc.Handle,lc.Conntime,lc.Totaltime,no);
	Close(data);data=NULL;
  return(0);
	}

BOOL	checkdatafile(void)
	{
	BPTR	data=NULL;
	register int		counta=0;
	long	myheader,mycallers;

	myheader=sizeof(struct LCHeader);
	mycallers=sizeof(struct LastCallers);

#ifdef DEBUGIT

	if(!(data=Open(DEBUGDATA,MODE_OLDFILE)))

#else

	if(!(data=Open(DATAFILE,MODE_OLDFILE)))

#endif

		{
		memset(&lcheader,0,myheader);
		strcpy(lcheader.TodayDate,ondate);
		lcheader.BestUpBytes=lcheader.BestDlBytes=NULL;

#ifdef DEBUGIT

		if(!(data=Open(DEBUGDATA,MODE_NEWFILE))) return(FALSE);

#else

		if(!(data=Open(DATAFILE,MODE_NEWFILE))) return(FALSE);

#endif

		Print("Initializing the Storage file...\n");
		Write(data,&lcheader,myheader);
    strcpy(lc.Baud,"--.-");
		strcpy(lc.Handle,"--[NO ENTRY]--");
		strcpy(lc.Location,"--------------------");
		strcpy(lc.Conntime,"00:00:00");
		strcpy(lc.Totaltime,"00:00");
    strcpy(lc.Flags,"-------");
		lc.Node=lc.Ulbytes=lc.Dlbytes=lc.Ulfiles=lc.Dlfiles=NULL;
		lc.NewUser=FALSE;
    while(counta<20)
			{
      Write(data,&lc,mycallers);
			CopyMem(&lc,&last[counta],mycallers);
			counta++;
      };
		Close(data);
		return(TRUE);
		};
	Print("Working on Caller...\n\n");
  Seek(data,0L,OFFSET_BEGINNING);
	Read(data,&lcheader,myheader);
  while(counta<20)
		{
		Seek(data,(myheader+mycallers*counta),OFFSET_BEGINNING);
		Read(data,&last[counta],mycallers);
		counta++;
		};
	Close(data);
	return(TRUE);
	}

void UpdateStats(void)
	{
	Printf("\nUpdating Statistics...\n\n");
	if(*offdate) strcpy(lcheader.TodayDate,offdate);
	else strcpy(lcheader.TodayDate,ondate);
	if(lcheader.TodayUpBytes>lcheader.BestUpBytes) lcheader.BestUpBytes=lcheader.TodayUpBytes;
	if(lcheader.TodayDlBytes>lcheader.BestDlBytes) lcheader.BestDlBytes=lcheader.TodayDlBytes;
	if(lcheader.TodayUpFiles>lcheader.BestUpFiles) lcheader.BestUpFiles=lcheader.TodayUpFiles;
	if(lcheader.TodayDlFiles>lcheader.BestDlFiles) lcheader.BestDlFiles=lcheader.TodayDlFiles;
	if(lcheader.TodayBusy-lczeit>lcheader.BestBusy) lcheader.BestBusy=lcheader.TodayBusy-lczeit;
	if(lcheader.TodayCalls>lcheader.BestCalls) lcheader.BestCalls=lcheader.TodayCalls;
	lcheader.LastCalls=lcheader.TodayCalls;
	lcheader.TodayUpBytes=lcheader.TodayDlBytes=lcheader.TodayUpFiles=lcheader.TodayDlFiles=NULL;
	lcheader.TodayCalls=lcheader.TodayPages=lcheader.TodayLC=lcheader.TodayNew=NULL;
	lcheader.TodayHacks=lcheader.TodayBusy=NULL;
	lcheader.TodayBusy+=lczeit;
	}

void SPrintf(char *Buffer,char *ctl, ...)
 	{
	RawDoFmt(ctl, (long *)(&ctl + 1), (void (*))"\x16\xc0\x4e\x75",Buffer);
	}

LONG MyAtol(char *was)
	{
	LONG dummy;

	StrToLong(was,&dummy);
	return(dummy);
	}
