/* 
 * Copyright 2003, 2004, 2005, 2006 PathScale, Inc.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * Further, this software is distributed without any warranty that it is
 * free of the rightful claim of any third person regarding infringement 
 * or the like.  Any license provided herein, whether implied or 
 * otherwise, applies only to this software file.  Patent licenses, if 
 * any, provided herein do not apply to combinations of this program with 
 * other software, or any other product whatsoever.  
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * File modified June 20, 2003 by PathScale, Inc. to update Open64 C/C++ 
 * front-ends to GNU 3.2.2 release.
 */


/* 
   Copyright (C) 2001 Tensilica, Inc.  All Rights Reserved.
   Revised to support Tensilica processors and to improve overall performance
 */

#include <values.h>
#include <sys/types.h>
#include <elf.h>
#include "defs.h"
#include "config.h"
#include "config_debug.h"
#include "config_list.h"
#include "config_TARGB.h"
#include "controls.h"
#include "erglob.h"
#include "erlib.h"
#include "file_util.h"
#include "flags.h"
#include "glob.h"
#include "mempool.h"
#include "tracing.h"
#include "util.h"
#include "errors.h"
// #include "cmd_line.h"
#include "err_host.tab"
#include <stdarg.h>
#include "gnu_config.h"
extern "C" {
#include "gnu/system.h"
#include "gnu/tree.h"
}
#ifdef KEY
#include "gnu/flags.h"
#endif
#include "wn.h"
#include "wn_util.h"
#include "wn_simp.h"
#include "symtab.h"
#include "pu_info.h"
#include "ir_reader.h"
#include "ir_bwrite.h"
#include "wfe_decl.h"
#include "wfe_expr.h"
#include "wfe_dst.h"
#include "wfe_misc.h"
#include "wfe_stmt.h"
#include "c_int_model.h"
#ifdef KEY
#include "tree_symtab.h"
#endif

int WFE_Keep_Zero_Length_Structs = TRUE;

extern int optimize;

PU_Info *PU_Tree_Root = NULL;
int      wfe_invoke_inliner = FALSE;

extern void Initialize_IRB (void);	/* In lieu of irbutil.h */
extern char *asm_file_name;		/* from toplev.c */

int trace_verbose = FALSE;
// an_error_severity error_threshold = es_warning;

static BOOL Prepare_Source (void);
static void WFE_Stmt_Stack_Init (void);
static void WFE_Stmt_Stack_Free (void);
#ifdef KEY
static void WFE_Guard_Var_Init();
// When region optimization is enabled using -foptimize-regions, we try not
// to close a region as soon as a call stmt finishes. We try to keep it open
// and include as many calls as possible.
// If we got an opportunity but did not close a region in WFE_Stmt_Append,
// we set the following flag.
bool Did_Not_Terminate_Region = FALSE;
#endif
#ifndef KEY
// The following taken from gnu/flags.h
// our #include of flags.h gets common/util/flags.h instead
enum debug_info_level
{
  DINFO_LEVEL_NONE,     /* Write no debugging info.  */
  DINFO_LEVEL_TERSE,    /* Write minimal info to support tracebacks only.  */
  DINFO_LEVEL_NORMAL,   /* Write info for all declarations (and line table). */
  DINFO_LEVEL_VERBOSE   /* Write normal info plus #define/#undef info.  */
};
#endif // !KEY

/* Specify how much debugging info to generate.  */
extern enum debug_info_level debug_info_level;
// End gnu/flags.h data decl



/* ====================================================================
 *
 * Local data.
 *
 * ====================================================================
 */

/*       MAX_DEBUG_LEVEL	2  :: Defined in flags.h */
# define DEF_DEBUG_LEVEL	0
INT8 Debug_Level = DEF_DEBUG_LEVEL;	/* -gn:	debug level */
# define MAX_MSG_LEVEL 2
# define DEF_MSG_LEVEL 2

#ifdef MONGOOSE_CIF
mUINT32 Cif_Level = 0;       	/* CIF level */
#define MAX_CIF_LEVEL 3 
#define DEF_CIF_LEVEL 2 
#endif /* MONGOOSE_CIF */

/* Default file	extensions: */
#define	IRB_FILE_EXTENSION ".B"	/* ACIR file */
#define	IRD_FILE_EXTENSION ".D"	/* Intermediate data file */
#define	ERR_FILE_EXTENSION ".e"	/* Error file */
#define	LST_FILE_EXTENSION ".l"	/* Listing file */
#define	TRC_FILE_EXTENSION ".t"	/* Trace file */
#define DSTDUMP_FILE_EXTENSION ".fe.dst" /* DST dump-file extension */

/* Static data:	command	line information: */
static INT32 Argc;		/* Copy of argc */
static char **Argv;		/* Copy of argv */
static INT32 Source_Arg = 0;	/* Number of current source arg */
static INT32 Src_Count = 0;	/* Number of source files seen */
static char Dash [] = "-";

/* Internal flags: */
static BOOL Echo_Flag =	FALSE;	/* Echo command	lines */
static BOOL Delete_IR_File = FALSE;	/* Delete SGIR file when done */


/* ====================================================================
 *
 * Cleanup_Files
 *
 * Close all per-source	files involved in a compilation	and prepare the
 * global variables for	the next source.  This routine is externalized
 * for signal cleanup; the report parameter allows suppressing of error
 * reporting during such cleanup.
 *
 * ====================================================================
 */

void
Cleanup_Files (	BOOL report, BOOL delete_dotofile )
{
  /* No	current	line number for	errors:	*/
  Set_Error_Line (ERROR_LINE_UNKNOWN);

  /* Close source file:	*/
  if ( Src_File	!= NULL	&& Src_File != stdin &&	fclose (Src_File) ) {
    if ( report	) ErrMsg ( EC_Src_Close, Src_File_Name,	errno );
  }
  Src_File = NULL;

  /* Close and delete SGIR file: */
  if ( IR_File != NULL && fclose (IR_File) ) {
    if ( report	) ErrMsg ( EC_IR_Close,	IR_File_Name, errno );
  }
  IR_File = NULL;
  if ( Delete_IR_File && unlink	(IR_File_Name) ) {
    if ( report	) ErrMsg ( EC_IR_Delete, IR_File_Name, errno );
  }

  /* Close listing file: */
  if ( Lst_File	!= NULL	&& Lst_File != stdout && fclose	(Lst_File) ) {
    if ( report	) ErrMsg ( EC_Lst_Close, Lst_File_Name,	errno );
  }
  Lst_File = NULL;

  /* Close trace file: */
  Set_Trace_File ( NULL	);

  /* Disable timing file: */
  Tim_File = NULL;

  /* Finally close error file: */
  Set_Error_File ( NULL	);
  Set_Error_Source ( NULL );
}

/* ====================================================================
 *
 * Terminate
 *
 * Do any necessary cleanup and	terminate the program with the given
 * status.
 *
 * ====================================================================
 */

void
Terminate ( INT status )
{
  /* Close and delete files as necessary: */
  Cleanup_Files	( FALSE, FALSE);

  exit (status);
}

/* ====================================================================
 *
 * Prepare_Source
 *
 * Process the next source argument and	associated file	control	flags
 * from	the command line.  Pre-process the source file unless
 * suppressed, and initialize output files as required.	 Return	TRUE
 * iff we have a successfully pre-processed source file	left to
 * compile.
 *
 * ====================================================================
 */

static BOOL
Prepare_Source ( void )
{
  INT16	i;
  char *cp;
  char *fname;
  INT16 len;
  BOOL  dashdash_flag = FALSE;

  /* Initialize error handler: */
  Init_Error_Handler ( 100 );
  Set_Error_Line ( ERROR_LINE_UNKNOWN );
  Set_Error_File ( NULL );
  Set_Error_Phase ( "Front End Driver" );

  /* Clear file names: */
  Src_File_Name = NULL;	/* Source file */
  IR_File_Name = NULL;	/* SGIR file */
  Irb_File_Name = NULL;	/* ACIR file */
  Err_File_Name = Dash;	/* Error file */
  Lst_File_Name = NULL;	/* Listing file */
  Trc_File_Name = NULL;	/* Trace file */
  DSTdump_File_Name = NULL; /* DST dump */

  Delete_IR_File = FALSE;
  
  /* Check the command line flags for -f? and source file names: */
  while ( ++Source_Arg <= Argc ) {
    i = Source_Arg;

    /* Null argument => end of list: */
    if ( Argv[i] == NULL ) return FALSE;

    if ( !dashdash_flag && (*(Argv[i]) == '-' )) {
      cp = Argv[i]+1;	/* Pointer to next flag character */

      /* -oname or -o name are passed to the linker: */
      if ( *cp == 'o' ) {
	++cp;
	if ( *cp == 0 ) {
	  /* Link file name is next command line argument: */
	  ++Source_Arg;
	}
	continue;
      }

      /* process as command-line option group */
      if (strncmp(cp, "OPT:", 4) == 0) { 
	Process_Command_Line_Group (cp, Common_Option_Groups);
    	continue;
      }
    } 
    else {
      Src_Count++;
      dashdash_flag = FALSE;

      /* Copy the given source name: */
      len = strlen ( Argv[i] );
      Src_File_Name = (char *) malloc (len+5);
      strcpy ( Src_File_Name, Argv[i] );

      /* We've got a source file name -- open other files.
       * We want them to be created in the current directory, so we
       * strip off the filename only from Src_File_Name for use:
       */
      fname = Last_Pathname_Component ( Src_File_Name );

      /* Error file first to get error reports: */
      if ( Err_File_Name == NULL ) {
	/* Replace source file extension to get error file: */
	Err_File_Name = New_Extension
			    ( fname, ERR_FILE_EXTENSION	);
      } else if ( *Err_File_Name == '-' ) {
	/* Disable separate error file: */
	Err_File_Name = NULL;
      }
      Set_Error_File ( Err_File_Name );

      /* Trace file next: */
      if ( Trc_File_Name == NULL ) {
	if ( Tracing_Enabled ) {
	  /* Replace source file extension to get trace file: */
	  Trc_File_Name = New_Extension
			    ( fname, TRC_FILE_EXTENSION	);
	}
      } else if ( *Trc_File_Name == '-' ) {
	/* Leave trace file on stdout: */
	Trc_File_Name = NULL;
      }
      Set_Trace_File ( Trc_File_Name );
      if ( Get_Trace (TKIND_INFO, TINFO_TIME) ) Tim_File = TFile;

      /* We're ready to pre-process: */
      IR_File_Name = Src_File_Name;

      /* Open the IR file for compilation: */
      if ( Irb_File_Name == NULL ) {
	if (asm_file_name == NULL) {
		/* Replace source file extension to get listing file: */
		Irb_File_Name = New_Extension (	fname, IRB_FILE_EXTENSION );
	}
	else {
		Irb_File_Name = asm_file_name;
	}
      }

	if ( (Irb_File = fopen ( Irb_File_Name, "w" )) == NULL ) {
	  ErrMsg ( EC_IR_Open, IR_File_Name, errno );
	  Cleanup_Files ( TRUE, FALSE );	/* close opened files */
	  return Prepare_Source ();
	} else {
	  if ( Get_Trace ( TP_MISC, 1) ) {
	    fprintf ( TFile, 
	      "\n%sControl Values: Open_Dot_B_File\n%s\n", DBar, DBar );
	    Print_Controls ( TFile, "", TRUE );
	  }
	}

      /* Configure internal options for this source file */
      Configure_Source ( Src_File_Name );

      return TRUE;
    }
  }

  return FALSE;
}

#ifdef KEY
extern void WFE_Omp_Init (void);
#endif // KEY


void
WFE_Init (INT argc, char **argv, char **envp )
{
  Set_Error_Tables ( Phases, host_errlist );
#ifdef KEY
  Initialize_C_Int_Model();
#endif
  MEM_Initialize();
  Handle_Signals();

  /* Perform preliminary command line processing: */
  Set_Error_Line ( ERROR_LINE_UNKNOWN );
  Set_Error_Phase ( "Front End Driver" );
  Preconfigure ();
#ifdef TARG_MIPS
  ABI_Name = "n64";
#endif
#ifdef TARG_IA64
  ABI_Name = "i64";
#endif
#ifdef TARG_IA32
  ABI_Name = "ia32";
#endif
#if defined(TARG_IA32) || defined(TARG_X8664)
  if (TARGET_64BIT)
    ABI_Name = "n64";
  else ABI_Name = "n32";
#endif
  Init_Controls_Tbl();
  Argc = argc;
  Argv = argv;
  Configure ();
  Initialize_C_Int_Model();
  IR_reader_init();
  Initialize_Symbol_Tables (TRUE);
  WFE_Stmt_Stack_Init ();
  WFE_Stmt_Init ();
#ifdef KEY
  WFE_Omp_Init ();
#endif
  WFE_Expr_Init ();
  WHIRL_Mldid_Mstid_On = TRUE;
  WN_Simp_Fold_LDA = TRUE;  // fold (LDA offset) + const to LDA (offset+const)
			    // since the static initialization code relies on it
  WHIRL_Keep_Cvt_On = TRUE; // so simplifier won't I8I4CVT
  Opt_Level = optimize;

  // This is not right: we should match what gnu does
  // and this is only an approximation.
  Debug_Level = (debug_info_level >= DINFO_LEVEL_NORMAL)? 2:0;

#ifdef KEY
  WFE_Guard_Var_Init ();
#endif
} /* WFE_Init */

void
WFE_File_Init (INT argc, char **argv)
{
  /* Process each source file: */
  Prepare_Source();
  MEM_POOL_Push (&MEM_src_pool);

  Restore_Cmd_Line_Ctrls();

  /* If the user forgot to specify sources, complain: */
  if ( Src_Count == 0 ) {
    ErrMsg ( EC_No_Sources );
  }

  Open_Output_Info ( Irb_File_Name );
  DST_build(argc, argv);	// do initial setup of dst
}

void
WFE_File_Finish (void)
{
    Verify_SYMTAB (GLOBAL_SYMTAB);
    Write_Global_Info (PU_Tree_Root);
    Close_Output_Info ();
    IR_reader_finish ();
    MEM_POOL_Pop (&MEM_src_pool);
}

void
WFE_Finish ()
{
  WFE_Stmt_Stack_Free ();
}

void
WFE_Check_Errors (int *error_count, int *warning_count, BOOL *need_inliner)
{
  
  /* If we've seen errors, note them and terminate: */
  Get_Error_Count ( error_count, warning_count);
  *need_inliner = wfe_invoke_inliner;
}

#define ENLARGE(x) (x + (x >> 1))
#define WN_STMT_STACK_SIZE 32

typedef struct wn_stmt {
  WN            *wn;
  WFE_STMT_KIND  kind;
} WN_STMT;

static WN_STMT *wn_stmt_stack;
static WN_STMT *wn_stmt_sp;
static WN_STMT *wn_stmt_stack_last;
static INT      wn_stmt_stack_size;

char * WFE_Stmt_Kind_Name [wfe_stmk_last+1] = {
  "'unknown'",
  "'function entry'",
  "'function pragma'",
  "'function body'",
  "'region pragmas'",
#ifdef KEY
  "'region body'",
  "'region exits'",
  "'call region pragmas'",
  "'call region body'",
  "'call region exits'",
#endif // KEY
  "'scope'",
  "'if condition'",
  "'if then clause'",
  "'if else clause'",
  "'while condition'",
  "'while body'",
  "'dowhile condition'",
  "'dowhile body'",
  "'for condition'",
  "'for body'",
  "'switch'",
  "'comma'",
  "'rcomma'",
#ifdef KEY
  "'temp_cleanup'",
  "'dummy'",
#endif // KEY
  "'last'"
};

static void
WFE_Stmt_Stack_Init (void)
{
  wn_stmt_stack_size = WN_STMT_STACK_SIZE;
  wn_stmt_stack      = (WN_STMT *) malloc (sizeof (WN_STMT) *
                                           wn_stmt_stack_size );
  wn_stmt_sp         = wn_stmt_stack - 1;
  wn_stmt_stack_last = wn_stmt_stack + wn_stmt_stack_size - 1;
} /* WFE_Stmt_Stack_Init */

static void
WFE_Stmt_Stack_Free (void)
{
  free (wn_stmt_stack);
  wn_stmt_stack = NULL;
} /* WFE_Stmt_stack_free */

void
WFE_Stmt_Push (WN* wn, WFE_STMT_KIND kind, SRCPOS srcpos)
{
  INT new_stack_size;

#ifdef KEY
  // Close any existing EH region before we push a new stmt, since we don't
  // know what the new stmt offers, and may have difficulty closing the region
  // then.
  if (opt_regions && wn_stmt_sp)
    Check_For_Call_Region ();
#endif

  if (wn_stmt_sp == wn_stmt_stack_last) {
    new_stack_size = ENLARGE(wn_stmt_stack_size);
    wn_stmt_stack =
      (WN_STMT *) realloc (wn_stmt_stack, new_stack_size * sizeof (WN_STMT));
    wn_stmt_sp = wn_stmt_stack + wn_stmt_stack_size - 1;
    wn_stmt_stack_size = new_stack_size;
    wn_stmt_stack_last = wn_stmt_stack + wn_stmt_stack_size - 1;
  }
  ++wn_stmt_sp;
  wn_stmt_sp->wn   = wn;
  wn_stmt_sp->kind = kind;

  if (srcpos)
    WN_Set_Linenum ( wn, srcpos );
} /* WFE_Stmt_Push */

WN*
WFE_Stmt_Top (void)
{
  FmtAssert (wn_stmt_sp >= wn_stmt_stack,
             ("no more entries on stack in function WFE_Stmt_Top"));

  return (wn_stmt_sp->wn);
} /* WFE_Stmt_Top */

#ifdef KEY
// A region has started before a call stmt, and it seems its difficult 
// to close the region cleanly. For the time being, we have this function 
// that closes the call region.
// Return 1 if we did close a region.
bool
Check_For_Call_Region (void)
{
  if (key_exceptions) {
	if (wn_stmt_sp->kind == wfe_stmk_call_region_body) {
		Setup_EH_Region();
		return TRUE;
	}
  }
  return FALSE;
}
#endif // KEY

void
WFE_Stmt_Append (WN* wn, SRCPOS srcpos)
{
  WN * body;
  WN * last;

  if (srcpos) {
    WN_Set_Linenum ( wn, srcpos );
    if (WN_operator(wn) == OPR_BLOCK && WN_first(wn) != NULL)
    	WN_Set_Linenum ( WN_first(wn), srcpos );
  }

  body = WFE_Stmt_Top ();

  if (body) {

    last = WN_last(body);
    WN_INSERT_BlockAfter (body, last, wn);
  }

#ifdef KEY
// This should not ideally be mixed with this function code, but ...
  if (!opt_regions)
    Check_For_Call_Region();
  else if (wn_stmt_sp->kind == wfe_stmk_call_region_body)
    Did_Not_Terminate_Region = TRUE;
#endif // KEY
} /* WFE_Stmt_Append */


WN*
WFE_Stmt_Last (void)
{
  WN * body;

  body = WFE_Stmt_Top ();
  return (WN_last(body));
} /* WFE_Stmt_Last */


WN *
WFE_Stmt_Delete ()
{
  WN * body;
  WN * last;
  WN * prev;

  body = WFE_Stmt_Top ();
  last = WN_last(body);
  prev = WN_prev(last);
  if (prev)
    WN_next(prev)  = NULL;
  else
    WN_first(body) = NULL;
  WN_last(body) = prev;
  WN_prev(last) = NULL;

  return last;
} /* WFE_Stmt_Delete */


WN*
WFE_Stmt_Pop (WFE_STMT_KIND kind)
{
  WN * wn;

  FmtAssert (wn_stmt_sp >= wn_stmt_stack,
             ("no more entries on stack in function WFE_Stmt_Pop"));

#ifdef KEY
// another hack.
  WN * to_be_pushed = 0;
  if (key_exceptions && wn_stmt_sp->kind != kind)
  {
    if (!opt_regions || !Did_Not_Terminate_Region)
    {
  	FmtAssert (wn_stmt_sp->kind == wfe_stmk_call_region_body,
             ("mismatch in statements: expected %s, got %s\n",
              WFE_Stmt_Kind_Name [kind],
              WFE_Stmt_Kind_Name [wn_stmt_sp->kind]));

	to_be_pushed = WFE_Stmt_Pop (wfe_stmk_call_region_body);
    }
    else
    { // If we got an opportunity but did not close the region earlier in
      // WFE_Stmt_Append, then close it now.
    	Check_For_Call_Region ();
	Did_Not_Terminate_Region = FALSE;
    }
  }
#endif // KEY

  FmtAssert (wn_stmt_sp->kind == kind,
             ("mismatch in statements: expected %s, got %s\n",
              WFE_Stmt_Kind_Name [kind],
              WFE_Stmt_Kind_Name [wn_stmt_sp->kind]));

  wn = wn_stmt_sp->wn;
  wn_stmt_sp--;

#ifdef KEY
  if (to_be_pushed) 
  	WFE_Stmt_Push (to_be_pushed, wfe_stmk_call_region_body, Get_Srcpos()); 
#endif

  return (wn);
} /* WFE_Stmt_Pop */

/*
void process_diag_override_option(an_option_kind kind,
                                  char          *opt_arg)
{
}
*/

#ifdef KEY
// Stack of VAR_DECLs representing guard variables.  A NULL_TREE means the
// guard variable is needed but not yet allocated.
std::vector<tree> guard_vars;

static void
WFE_Guard_Var_Init()
{
  // Clear the stack.
  guard_vars.clear();
}

// Indicate a new guard variable is needed.
void
WFE_Guard_Var_Push()
{
  guard_vars.push_back(NULL_TREE);
}

// Get the guard variable currently in effect.  NULL_TREE if guard variable is
// needed but not yet allocated.
tree
WFE_Guard_Var_Pop()
{
  FmtAssert (!guard_vars.empty(), ("WFE_Guard_Var_Pop: no guard vars to pop"));
  tree t = guard_vars.back();
  guard_vars.pop_back();
  return t;
}

// Return the current guard variable.  Allocate one if it doesn't exist.
tree
WFE_Get_Guard_Var()
{
  tree t;

  // Empty stack means no guard variable is needed.
  if (guard_vars.empty())
    return NULL_TREE;

  // If top of stack is not NULL_TREE, then it is a valid guard variable.
  t = guard_vars.back();
  if (t != NULL_TREE)
    return t;

  // Top of stack is NULL_TREE.  Replace top of stack with a real guard
  // variable.
  guard_vars.pop_back();	// Pop off the NULL_TREE.
  t = build_decl(VAR_DECL, NULL_TREE, integer_type_node);
  Get_ST(t);
  guard_vars.push_back(t);	// Push new guard variable onto stack.
  return t;
}

// Currently this function is only used for searching func_entry, but it
// is intended to be a general utility function.
WN *
WFE_Find_Stmt_In_Stack (WFE_STMT_KIND kind)
{
  WN_STMT * sp = wn_stmt_sp;
  Is_True (sp, ("Null WN stack pointer"));

  while (sp->kind != wfe_stmk_func_entry)
  {
    if (sp->kind == kind)
      break;
    sp--;
    Is_True (sp, ("FUNC_ENTRY node not found"));
  }
  FmtAssert (sp->kind == kind, ("Stmt kind not found in stack"));
  Is_True (sp->wn, ("Null WN stmt in apparently valid stack location"));
  return sp->wn;
}
#endif
