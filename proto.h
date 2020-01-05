/*
 *  --- Protos fuer LastCallers ---
 */

extern 	STRPTR	__asm	StrStr(register __a0 STRPTR, register __a1 STRPTR);

extern  VOID  __asm Decrypt(register __a0 STRPTR);
extern ULONG 	__asm CheckRegCRC(register __d2 UWORD);

extern void 	BuiltBull(char *); 											// erzeugt Bulletin
extern LONG		makeallsec(char *,char *);
extern BOOL 	Read_Caller(void);											// Liest Callerdaten ein
extern void		ParseFlags(void);												// Erzeugt Userflags
extern void 	SPrintf(char *Buffer,char *ctl, ...);
extern LONG		MyAtol(char *);
