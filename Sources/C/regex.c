/* A Bison parser, made by GNU Bison 3.7.5.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30705

/* Bison version string.  */
#define YYBISON_VERSION "3.7.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 18 "regex.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "foma.h"
#define MAX_F_RECURSION 100
extern int yyerror();
extern int yylex();
extern int my_yyparse(char *my_string, int lineno, struct defined_networks *defined_nets, struct defined_functions *defined_funcs);
struct fsm *current_parse;
int rewrite, rule_direction;
int substituting = 0;
static char *subval1, *subval2;
struct fsmcontexts *contexts;
struct fsmrules *rules;
struct rewrite_set *rewrite_rules;
static struct fsm *fargs[100][MAX_F_RECURSION];  /* Function arguments [number][frec] */
static int frec = -1;                            /* Current depth of function recursion */
static char *fname[MAX_F_RECURSION];             /* Function names */
static int fargptr[MAX_F_RECURSION];             /* Current argument no. */
/* Variable to produce internal symbols */
unsigned int g_internal_sym = 23482342;

void add_function_argument(struct fsm *net) {
    fargs[fargptr[frec]][frec] = net;
    fargptr[frec]++;
}

void declare_function_name(char *s) {
    if (frec > MAX_F_RECURSION) {
        printf("Function stack depth exceeded. Aborting.\n");
        exit(1);
    }
    fname[frec] = xxstrdup(s);
    xxfree(s);
}

struct fsm *function_apply(struct defined_networks *defined_nets, struct defined_functions *defined_funcs) {
    int i, mygsym, myfargptr;
    char *regex;
    char repstr[13], oldstr[13];
    if ((regex = find_defined_function(defined_funcs, fname[frec],fargptr[frec])) == NULL) {
        fprintf(stderr, "***Error: function %s@%i) not defined!\n",fname[frec], fargptr[frec]);
        return NULL;
    }
    regex = xxstrdup(regex);
    mygsym = g_internal_sym;
    myfargptr = fargptr[frec];
    /* Create new regular expression from function def. */
    /* and parse that */
    for (i = 0; i < fargptr[frec]; i++) {
        sprintf(repstr,"%012X",g_internal_sym);
        sprintf(oldstr, "@ARGUMENT%02i@", (i+1));
        streqrep(regex, oldstr, repstr);
        /* We temporarily define a network and save argument there */
        /* The name is a running counter g_internal_sym */
        add_defined(defined_nets, fargs[i][frec], repstr);
        g_internal_sym++;
    }
    my_yyparse(regex,1,defined_nets, defined_funcs);
    for (i = 0; i < myfargptr; i++) {
        sprintf(repstr,"%012X",mygsym);
        /* Remove the temporarily defined network */
        remove_defined(defined_nets, repstr);
        mygsym++;
    }
    xxfree(fname[frec]);
    frec--;
    xxfree(regex);
    return(current_parse);
}

void add_context_pair(struct fsm *L, struct fsm *R) {
    struct fsmcontexts *newcontext;
    newcontext = xxcalloc(1,sizeof(struct fsmcontexts));
    newcontext->left = L;
    newcontext->right = R;
    newcontext->next = contexts;
    contexts = newcontext;
}

void clear_rewrite_ruleset(struct rewrite_set *rewrite_rules) {
    struct rewrite_set *rule, *rulep;
    struct fsmcontexts *contexts, *contextsp;
    struct fsmrules *r, *rp;
    for (rule = rewrite_rules; rule != NULL; rule = rulep) {

	for (r = rule->rewrite_rules ; r != NULL; r = rp) {
	    fsm_destroy(r->left);
	    fsm_destroy(r->right);
	    fsm_destroy(r->right2);
	    fsm_destroy(r->cross_product);
	    rp = r->next;
	    xxfree(r);
	}
	
	for (contexts = rule->rewrite_contexts; contexts != NULL ; contexts = contextsp) {

	    contextsp = contexts->next;
	    fsm_destroy(contexts->left);
	    fsm_destroy(contexts->right);
	    fsm_destroy(contexts->cpleft);
	    fsm_destroy(contexts->cpright);
	    xxfree(contexts);
	}
       	rulep = rule->next;
	//fsm_destroy(rules->cpunion);
	xxfree(rule);
    }
}

void add_rewrite_rule() {
    struct rewrite_set *new_rewrite_rule;
    if (rules != NULL) {
        new_rewrite_rule = xxmalloc(sizeof(struct rewrite_set));
        new_rewrite_rule->rewrite_rules = rules;
        new_rewrite_rule->rewrite_contexts = contexts;
        new_rewrite_rule->next = rewrite_rules;
        new_rewrite_rule->rule_direction = rule_direction;

        rewrite_rules = new_rewrite_rule;
        rules = NULL;
        contexts = NULL;
        rule_direction = 0;
    }
}

void add_eprule(struct fsm *R, struct fsm *R2, int type) {
    struct fsmrules *newrule;
    rewrite = 1;
    newrule = xxmalloc(sizeof(struct fsmrules));
    newrule->left = fsm_empty_string();
    newrule->right = R;
    newrule->right2 = R2;
    newrule->arrow_type = type;
    newrule->next = rules;
    rules = newrule;
}
 
void add_rule(struct fsm *L, struct fsm *R, struct fsm *R2, int type) {
    struct fsm *test;
    struct fsmrules *newrule;
    rewrite = 1;
    newrule = xxmalloc(sizeof(struct fsmrules));

    if ((type & ARROW_DOTTED) != 0) {
        newrule->left = fsm_minus(fsm_copy(L), fsm_empty_string());
    } else {
        newrule->left = L;
    }
    newrule->right = R;
    newrule->right2 = R2;
    newrule->next = rules;
    newrule->arrow_type = type;
    if ((type & ARROW_DOTTED) != 0) {
        newrule->arrow_type = type - ARROW_DOTTED;
    }

    rules = newrule;

    if ((type & ARROW_DOTTED) != 0) {
        /* Add empty [..] -> B for dotted rules (only if LHS contains the empty string) */
        test = fsm_intersect(L,fsm_empty_string());
        if (!fsm_isempty(test)) {
            newrule = xxmalloc(sizeof(struct fsmrules));
            newrule->left = test;
            newrule->right = fsm_copy(R);
            newrule->right2 = fsm_copy(R2);
            newrule->next = rules;
            newrule->arrow_type = type;
            rules = newrule;
        } else {
	    //fsm_destroy(test);
	}
    }
}



#line 251 "regex.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "regex.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NET = 3,                        /* NET  */
  YYSYMBOL_END = 4,                        /* END  */
  YYSYMBOL_LBRACKET = 5,                   /* LBRACKET  */
  YYSYMBOL_RBRACKET = 6,                   /* RBRACKET  */
  YYSYMBOL_LPAREN = 7,                     /* LPAREN  */
  YYSYMBOL_RPAREN = 8,                     /* RPAREN  */
  YYSYMBOL_ENDM = 9,                       /* ENDM  */
  YYSYMBOL_ENDD = 10,                      /* ENDD  */
  YYSYMBOL_CRESTRICT = 11,                 /* CRESTRICT  */
  YYSYMBOL_CONTAINS = 12,                  /* CONTAINS  */
  YYSYMBOL_CONTAINS_OPT_ONE = 13,          /* CONTAINS_OPT_ONE  */
  YYSYMBOL_CONTAINS_ONE = 14,              /* CONTAINS_ONE  */
  YYSYMBOL_XUPPER = 15,                    /* XUPPER  */
  YYSYMBOL_XLOWER = 16,                    /* XLOWER  */
  YYSYMBOL_FLAG_ELIMINATE = 17,            /* FLAG_ELIMINATE  */
  YYSYMBOL_IGNORE_ALL = 18,                /* IGNORE_ALL  */
  YYSYMBOL_IGNORE_INTERNAL = 19,           /* IGNORE_INTERNAL  */
  YYSYMBOL_CONTEXT = 20,                   /* CONTEXT  */
  YYSYMBOL_NCONCAT = 21,                   /* NCONCAT  */
  YYSYMBOL_MNCONCAT = 22,                  /* MNCONCAT  */
  YYSYMBOL_MORENCONCAT = 23,               /* MORENCONCAT  */
  YYSYMBOL_LESSNCONCAT = 24,               /* LESSNCONCAT  */
  YYSYMBOL_DOUBLE_COMMA = 25,              /* DOUBLE_COMMA  */
  YYSYMBOL_COMMA = 26,                     /* COMMA  */
  YYSYMBOL_SHUFFLE = 27,                   /* SHUFFLE  */
  YYSYMBOL_PRECEDES = 28,                  /* PRECEDES  */
  YYSYMBOL_FOLLOWS = 29,                   /* FOLLOWS  */
  YYSYMBOL_RIGHT_QUOTIENT = 30,            /* RIGHT_QUOTIENT  */
  YYSYMBOL_LEFT_QUOTIENT = 31,             /* LEFT_QUOTIENT  */
  YYSYMBOL_INTERLEAVE_QUOTIENT = 32,       /* INTERLEAVE_QUOTIENT  */
  YYSYMBOL_UQUANT = 33,                    /* UQUANT  */
  YYSYMBOL_EQUANT = 34,                    /* EQUANT  */
  YYSYMBOL_VAR = 35,                       /* VAR  */
  YYSYMBOL_IN = 36,                        /* IN  */
  YYSYMBOL_IMPLIES = 37,                   /* IMPLIES  */
  YYSYMBOL_BICOND = 38,                    /* BICOND  */
  YYSYMBOL_EQUALS = 39,                    /* EQUALS  */
  YYSYMBOL_NEQ = 40,                       /* NEQ  */
  YYSYMBOL_SUBSTITUTE = 41,                /* SUBSTITUTE  */
  YYSYMBOL_SUCCESSOR_OF = 42,              /* SUCCESSOR_OF  */
  YYSYMBOL_PRIORITY_UNION_U = 43,          /* PRIORITY_UNION_U  */
  YYSYMBOL_PRIORITY_UNION_L = 44,          /* PRIORITY_UNION_L  */
  YYSYMBOL_LENIENT_COMPOSE = 45,           /* LENIENT_COMPOSE  */
  YYSYMBOL_TRIPLE_DOT = 46,                /* TRIPLE_DOT  */
  YYSYMBOL_LDOT = 47,                      /* LDOT  */
  YYSYMBOL_RDOT = 48,                      /* RDOT  */
  YYSYMBOL_FUNCTION = 49,                  /* FUNCTION  */
  YYSYMBOL_SUBVAL = 50,                    /* SUBVAL  */
  YYSYMBOL_ISUNAMBIGUOUS = 51,             /* ISUNAMBIGUOUS  */
  YYSYMBOL_ISIDENTITY = 52,                /* ISIDENTITY  */
  YYSYMBOL_ISFUNCTIONAL = 53,              /* ISFUNCTIONAL  */
  YYSYMBOL_NOTID = 54,                     /* NOTID  */
  YYSYMBOL_LOWERUNIQ = 55,                 /* LOWERUNIQ  */
  YYSYMBOL_LOWERUNIQEPS = 56,              /* LOWERUNIQEPS  */
  YYSYMBOL_ALLFINAL = 57,                  /* ALLFINAL  */
  YYSYMBOL_UNAMBIGUOUSPART = 58,           /* UNAMBIGUOUSPART  */
  YYSYMBOL_AMBIGUOUSPART = 59,             /* AMBIGUOUSPART  */
  YYSYMBOL_AMBIGUOUSDOMAIN = 60,           /* AMBIGUOUSDOMAIN  */
  YYSYMBOL_EQSUBSTRINGS = 61,              /* EQSUBSTRINGS  */
  YYSYMBOL_LETTERMACHINE = 62,             /* LETTERMACHINE  */
  YYSYMBOL_MARKFSMTAIL = 63,               /* MARKFSMTAIL  */
  YYSYMBOL_MARKFSMTAILLOOP = 64,           /* MARKFSMTAILLOOP  */
  YYSYMBOL_MARKFSMMIDLOOP = 65,            /* MARKFSMMIDLOOP  */
  YYSYMBOL_MARKFSMLOOP = 66,               /* MARKFSMLOOP  */
  YYSYMBOL_ADDSINK = 67,                   /* ADDSINK  */
  YYSYMBOL_LEFTREWR = 68,                  /* LEFTREWR  */
  YYSYMBOL_FLATTEN = 69,                   /* FLATTEN  */
  YYSYMBOL_SUBLABEL = 70,                  /* SUBLABEL  */
  YYSYMBOL_CLOSESIGMA = 71,                /* CLOSESIGMA  */
  YYSYMBOL_CLOSESIGMAUNK = 72,             /* CLOSESIGMAUNK  */
  YYSYMBOL_ARROW = 73,                     /* ARROW  */
  YYSYMBOL_DIRECTION = 74,                 /* DIRECTION  */
  YYSYMBOL_COMPOSE = 75,                   /* COMPOSE  */
  YYSYMBOL_CROSS_PRODUCT = 76,             /* CROSS_PRODUCT  */
  YYSYMBOL_HIGH_CROSS_PRODUCT = 77,        /* HIGH_CROSS_PRODUCT  */
  YYSYMBOL_UNION = 78,                     /* UNION  */
  YYSYMBOL_INTERSECT = 79,                 /* INTERSECT  */
  YYSYMBOL_MINUS = 80,                     /* MINUS  */
  YYSYMBOL_COMPLEMENT = 81,                /* COMPLEMENT  */
  YYSYMBOL_KLEENE_STAR = 82,               /* KLEENE_STAR  */
  YYSYMBOL_KLEENE_PLUS = 83,               /* KLEENE_PLUS  */
  YYSYMBOL_REVERSE = 84,                   /* REVERSE  */
  YYSYMBOL_INVERSE = 85,                   /* INVERSE  */
  YYSYMBOL_TERM_NEGATION = 86,             /* TERM_NEGATION  */
  YYSYMBOL_YYACCEPT = 87,                  /* $accept  */
  YYSYMBOL_start = 88,                     /* start  */
  YYSYMBOL_regex = 89,                     /* regex  */
  YYSYMBOL_network = 90,                   /* network  */
  YYSYMBOL_networkA = 91,                  /* networkA  */
  YYSYMBOL_n0 = 92,                        /* n0  */
  YYSYMBOL_network1 = 93,                  /* network1  */
  YYSYMBOL_network2 = 94,                  /* network2  */
  YYSYMBOL_network3 = 95,                  /* network3  */
  YYSYMBOL_network4 = 96,                  /* network4  */
  YYSYMBOL_network5 = 97,                  /* network5  */
  YYSYMBOL_network6 = 98,                  /* network6  */
  YYSYMBOL_network7 = 99,                  /* network7  */
  YYSYMBOL_network8 = 100,                 /* network8  */
  YYSYMBOL_network9 = 101,                 /* network9  */
  YYSYMBOL_network10 = 102,                /* network10  */
  YYSYMBOL_network11 = 103,                /* network11  */
  YYSYMBOL_sub1 = 104,                     /* sub1  */
  YYSYMBOL_sub2 = 105,                     /* sub2  */
  YYSYMBOL_network12 = 106,                /* network12  */
  YYSYMBOL_fstart = 107,                   /* fstart  */
  YYSYMBOL_fmid = 108,                     /* fmid  */
  YYSYMBOL_fend = 109                      /* fend  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  105
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1180

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  87
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  23
/* YYNRULES -- Number of rules.  */
#define YYNRULES  136
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  291

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   341


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   252,   252,   253,   256,   258,   259,   260,   261,   263,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   280,   281,   282,   283,   284,   285,
     286,   287,   288,   289,   291,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   307,   308,   309,
     310,   312,   314,   316,   317,   318,   319,   320,   321,   322,
     323,   325,   326,   327,   329,   330,   331,   332,   334,   335,
     336,   337,   338,   339,   341,   342,   343,   344,   345,   347,
     349,   350,   351,   352,   353,   354,   355,   356,   357,   359,
     360,   361,   362,   364,   365,   367,   368,   369,   370,   371,
     372,   373,   374,   375,   376,   378,   379,   381,   382,   383,
     384,   385,   386,   387,   388,   389,   390,   391,   392,   393,
     394,   395,   396,   397,   398,   399,   400,   401,   402,   403,
     405,   407,   410,   411,   413,   415,   417
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "NET", "END",
  "LBRACKET", "RBRACKET", "LPAREN", "RPAREN", "ENDM", "ENDD", "CRESTRICT",
  "CONTAINS", "CONTAINS_OPT_ONE", "CONTAINS_ONE", "XUPPER", "XLOWER",
  "FLAG_ELIMINATE", "IGNORE_ALL", "IGNORE_INTERNAL", "CONTEXT", "NCONCAT",
  "MNCONCAT", "MORENCONCAT", "LESSNCONCAT", "DOUBLE_COMMA", "COMMA",
  "SHUFFLE", "PRECEDES", "FOLLOWS", "RIGHT_QUOTIENT", "LEFT_QUOTIENT",
  "INTERLEAVE_QUOTIENT", "UQUANT", "EQUANT", "VAR", "IN", "IMPLIES",
  "BICOND", "EQUALS", "NEQ", "SUBSTITUTE", "SUCCESSOR_OF",
  "PRIORITY_UNION_U", "PRIORITY_UNION_L", "LENIENT_COMPOSE", "TRIPLE_DOT",
  "LDOT", "RDOT", "FUNCTION", "SUBVAL", "ISUNAMBIGUOUS", "ISIDENTITY",
  "ISFUNCTIONAL", "NOTID", "LOWERUNIQ", "LOWERUNIQEPS", "ALLFINAL",
  "UNAMBIGUOUSPART", "AMBIGUOUSPART", "AMBIGUOUSDOMAIN", "EQSUBSTRINGS",
  "LETTERMACHINE", "MARKFSMTAIL", "MARKFSMTAILLOOP", "MARKFSMMIDLOOP",
  "MARKFSMLOOP", "ADDSINK", "LEFTREWR", "FLATTEN", "SUBLABEL",
  "CLOSESIGMA", "CLOSESIGMAUNK", "ARROW", "DIRECTION", "COMPOSE",
  "CROSS_PRODUCT", "HIGH_CROSS_PRODUCT", "UNION", "INTERSECT", "MINUS",
  "COMPLEMENT", "KLEENE_STAR", "KLEENE_PLUS", "REVERSE", "INVERSE",
  "TERM_NEGATION", "$accept", "start", "regex", "network", "networkA",
  "n0", "network1", "network2", "network3", "network4", "network5",
  "network6", "network7", "network8", "network9", "network10", "network11",
  "sub1", "sub2", "network12", "fstart", "fmid", "fend", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341
};
#endif

#define YYPACT_NINF (-42)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     750,   -42,   750,   750,    68,    68,    68,   330,    -3,   750,
     459,     0,   820,   610,   750,   750,   750,   750,   750,   750,
     750,   750,   750,   750,   750,   750,   750,   750,   750,   750,
     750,   750,   750,   750,   750,   750,   750,    68,  1001,    10,
     750,     8,   -42,    25,    15,   -42,   -42,  1100,    68,   266,
     -42,   -42,   206,   -42,   -42,   -13,   -42,   680,   750,   -42,
      66,   142,   -42,   -42,   -42,   750,   750,    43,   750,   -36,
       3,    14,   960,    17,    20,   750,   669,   160,   -12,  1069,
     171,   203,   263,   291,   361,   370,   431,   440,   501,   510,
     571,   174,   580,   893,  1048,  1053,  1056,   641,  1058,  1060,
    1064,   711,   720,   -42,   -42,   -42,   -42,   -42,   750,   750,
     750,   750,   400,   750,   260,   960,   960,   960,   960,   960,
     960,   960,   960,   960,   960,   266,    68,    68,    68,    68,
      68,   -42,   -42,   -42,   -42,   -42,   -42,   -42,  1001,   -42,
     -42,   -42,   -42,    36,   -42,   -42,   149,   165,   -42,   -42,
      25,    25,   750,   781,   -42,   -42,    68,   -42,   -42,  1073,
     890,    29,   750,    35,   -42,   -42,   -42,   -42,   -42,   -42,
     -42,   -42,   -42,   -42,   -42,   750,   -42,   750,   750,   750,
     750,   -42,   750,   750,   750,   -42,   -42,   -42,   -42,   -42,
      25,   750,   750,   350,    25,   750,   470,   750,    40,   -42,
     -42,   -42,    68,    68,    68,    68,    68,    68,    68,   -42,
     -42,   -42,   -42,   -42,   -42,    62,   -42,   -42,   -42,   -42,
      25,   -42,   -42,  1133,   790,   110,    74,   750,  1076,   851,
     860,   921,   962,  1002,  1031,  1082,    25,    25,   750,    25,
     750,   750,   181,    25,   750,   540,   750,    64,   -42,   -42,
     -42,   750,   750,   231,   750,   -42,   -42,   -42,   -42,   -42,
     -42,   750,    25,    25,    25,   750,   750,    25,   750,   750,
     348,    25,   -42,    25,    25,   750,   750,  1036,  1040,    25,
      25,    25,    25,   750,   750,    25,    25,   -42,   -42,    25,
      25
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    95,     0,     0,     0,     0,     0,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       2,     0,     5,     9,    10,    47,    51,    52,    53,    61,
      68,    74,    79,    80,    93,     0,    96,     0,     0,   107,
       0,     0,    76,    78,    77,     0,     0,    18,     0,    98,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     131,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    75,    94,     1,     3,     4,     0,     0,
       0,     0,    12,     0,    23,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    62,     0,     0,     0,     0,
       0,    85,    86,    87,    89,    92,    90,    91,     0,    81,
      82,    83,    84,     0,   104,   136,     0,     0,   100,    99,
      15,    17,     0,     0,    66,    67,    63,    64,    65,     0,
       0,     0,     0,     0,   130,   110,   108,   109,   111,   112,
     113,   114,   115,   116,   117,     0,   118,     0,     0,     0,
       0,   123,     0,     0,     0,   127,   128,     7,     6,     8,
      21,     0,     0,    11,    34,     0,    35,     0,    22,    48,
      49,    50,    59,    60,    55,    56,    54,    57,    58,    69,
      70,    71,    72,    73,    88,     0,   135,   132,   134,   133,
      19,    97,   105,     0,     0,     0,    25,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    16,    13,     0,    31,
       0,     0,    36,    33,     0,    37,     0,     0,   101,   102,
     103,     0,     0,    24,     0,   119,   120,   121,   122,   124,
     125,     0,    20,    42,    46,     0,     0,    30,     0,     0,
      38,    32,   106,    27,    29,     0,     0,     0,     0,    41,
      44,    40,    45,     0,     0,    26,    28,   129,   126,    39,
      43
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -42,   103,   -42,    -1,   -32,    -7,   -42,   -20,   -42,   -42,
     -31,   -41,    42,   -42,   -42,   -35,   -42,   -42,   -42,   -42,
     -42,   -42,   -42
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,   144,    56,
      57,    58,    59
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      67,    60,    61,   104,    68,    75,    79,   125,    69,   108,
     105,    77,   107,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   111,   143,   154,   109,
     110,   156,   115,   116,   117,   112,    62,    63,    64,   155,
     113,   111,   157,   108,   111,   158,   146,   147,   150,   151,
     112,   162,   215,   112,   225,   113,   244,   153,   113,   152,
     272,     1,   148,     2,   159,     3,   187,   188,   189,   103,
       4,     5,     6,   109,   110,   111,   245,   202,   203,   204,
     205,   206,   207,   208,   112,   199,   200,   201,   114,   113,
     251,     8,     9,   214,   190,   193,   194,   198,   227,    11,
      12,   108,   247,   114,   246,   125,   114,    14,   250,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,   109,   110,   106,     0,   220,     0,   114,   252,    37,
     149,     0,     0,     0,    38,   226,     0,   216,     0,   224,
       0,   125,   125,   125,   125,   125,   125,   125,   209,   210,
     211,   212,   213,   218,   228,   217,   229,   230,   231,   232,
       0,   233,   234,   235,   236,   237,   161,   108,   239,   242,
     243,   219,   111,     0,   108,     0,     0,   164,     0,     0,
     175,   112,     0,     0,     0,   108,   113,   265,     0,     0,
     108,   165,     0,     0,     0,     0,   108,   109,   110,   108,
     253,   131,   132,   133,   109,   110,     0,   134,   135,   136,
     137,   262,     0,   263,   264,   109,   110,   267,   270,   271,
     109,   110,   111,     0,   273,   274,   109,   110,   108,   109,
     110,   112,     0,   277,   114,   266,   113,   275,   279,   280,
     278,   281,   282,     1,     0,     2,     0,     3,   285,   286,
       0,   166,     4,     5,     6,     0,   289,   290,   109,   110,
       7,     0,     0,   138,   126,   127,   195,     0,   139,   140,
     141,   142,     0,     8,     9,    10,   128,   129,   130,   167,
       0,    11,    12,     0,   114,   276,   196,    13,   108,    14,
       0,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     1,   197,     2,   108,     3,   109,   110,
       0,    37,     4,     5,     6,     0,    38,     0,     0,     0,
       7,     0,     0,     0,     0,    65,    66,     0,     0,   111,
       0,   111,     0,     8,     9,    10,   109,   110,   112,   168,
     112,    11,    12,   113,   283,   113,   238,    13,   169,    14,
       0,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     1,     0,     2,   108,     3,     0,     0,
       0,    37,     4,     5,     6,   108,    38,     0,     0,     0,
       7,   114,   284,   114,     0,   191,   192,     0,     0,     0,
       0,     0,     0,     8,     9,    10,   109,   110,     0,   170,
       0,    11,    12,     0,     0,   109,   110,    13,   171,    14,
       0,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     1,     0,     2,   108,     3,     0,     0,
       0,    37,     4,     5,     6,   108,    38,    70,    71,     0,
       7,     0,     0,     0,     0,    72,   240,     0,    73,    74,
       0,     0,     0,     8,     9,    10,   109,   110,     0,   172,
       0,    11,    12,     0,     0,   109,   110,    13,   173,    14,
       0,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     1,   241,     2,   108,     3,     0,     0,
       0,    37,     4,     5,     6,   108,    38,     0,     0,     0,
       7,     0,     0,     0,     0,     0,   268,     0,     0,     0,
       0,     0,     0,     8,     9,    10,   109,   110,     0,   174,
       0,    11,    12,     0,     0,   109,   110,    13,   176,    14,
       0,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     1,   269,     2,   108,     3,     0,     0,
       0,    37,     4,     5,     6,   108,    38,     0,     0,     0,
       7,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     8,     9,    10,   109,   110,     0,   181,
       0,    11,    12,     0,     0,   109,   110,    13,    78,    14,
       0,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     1,     0,     2,   108,     3,   145,     0,
       0,    37,     4,     5,     6,   160,    38,    70,    71,     0,
       7,     0,     0,     0,     0,    72,     0,     0,    73,    74,
       0,     0,     0,     8,     9,    10,   109,   110,     0,   185,
       0,    11,    12,     0,     0,     0,     0,    13,   186,    14,
       0,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     1,     0,     2,   108,     3,     0,     0,
       0,    37,     4,     5,     6,   108,    38,     0,     0,     0,
       7,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     8,     9,    10,   109,   110,     0,   221,
       0,    11,    12,     0,     0,   109,   110,    13,   249,    14,
       0,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     1,     0,     2,   108,     3,     0,     0,
       0,    37,     4,     5,     6,   108,    38,     0,     0,     0,
       7,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     8,     9,    76,   109,   110,     0,   255,
       0,    11,    12,     0,     0,   109,   110,    13,   256,    14,
       0,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     1,     0,     2,   108,     3,     0,     0,
       0,    37,     4,     5,     6,   108,    38,     0,     0,     0,
       7,     0,     0,     0,     0,     0,     0,     0,     0,   177,
       0,     0,     0,     8,     9,   223,   109,   110,     0,   257,
       0,    11,    12,     0,     0,   109,   110,    13,   108,    14,
       0,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     1,     0,     2,   108,     3,   109,   110,
     258,    37,     4,     5,     6,     0,    38,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     8,     9,    10,   109,   110,     0,     0,
       0,    11,    12,     0,     1,     0,     2,   108,     3,    14,
     259,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     0,     8,     9,     0,   109,   110,   260,
       0,    37,    11,    12,   287,     0,    38,   108,   288,     0,
      14,     0,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,   178,     0,   108,   109,   110,   179,
     111,   108,   180,     0,   182,   108,   183,    38,     0,   112,
     184,     0,     0,   108,   113,     0,     0,     0,   108,   222,
       0,   108,   254,   108,     0,   108,   109,   110,   261,   108,
       0,   109,   110,     0,     0,   109,   110,   163,   108,     0,
       0,   108,     0,   109,   110,     0,     0,   108,   109,   110,
       0,   109,   110,   109,   110,   109,   110,   118,   119,   109,
     110,   248,   114,   120,   121,     0,     0,     0,   109,   110,
       0,   109,   110,     0,     0,     0,     0,   109,   110,     0,
       0,    70,    71,     0,     0,     0,     0,     0,     0,    72,
       0,     0,    73,    74,     0,     0,     0,     0,   122,   123,
     124
};

static const yytype_int16 yycheck[] =
{
       7,     2,     3,    38,     7,     5,    13,    48,     9,    45,
       0,    12,     4,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    11,    50,    35,    75,
      76,    72,    27,    28,    29,    20,     4,     5,     6,    35,
      25,    11,    35,    45,    11,    35,    57,    58,    65,    66,
      20,    73,    26,    20,    35,    25,    26,    68,    25,    26,
       6,     3,     6,     5,    75,     7,   108,   109,   110,    37,
      12,    13,    14,    75,    76,    11,    46,   118,   119,   120,
     121,   122,   123,   124,    20,   115,   116,   117,    73,    25,
      26,    33,    34,   138,   111,   112,   113,   114,    73,    41,
      42,    45,    50,    73,    74,   156,    73,    49,     8,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    75,    76,    40,    -1,   152,    -1,    73,    74,    81,
       8,    -1,    -1,    -1,    86,   162,    -1,     8,    -1,   160,
      -1,   202,   203,   204,   205,   206,   207,   208,   126,   127,
     128,   129,   130,     8,   175,    26,   177,   178,   179,   180,
      -1,   182,   183,   184,   191,   192,    26,    45,   195,   196,
     197,    26,    11,    -1,    45,    -1,    -1,    26,    -1,    -1,
      26,    20,    -1,    -1,    -1,    45,    25,    26,    -1,    -1,
      45,     8,    -1,    -1,    -1,    -1,    45,    75,    76,    45,
     227,    15,    16,    17,    75,    76,    -1,    21,    22,    23,
      24,   238,    -1,   240,   241,    75,    76,   244,   245,   246,
      75,    76,    11,    -1,   251,   252,    75,    76,    45,    75,
      76,    20,    -1,   254,    73,    74,    25,    26,   265,   266,
     261,   268,   269,     3,    -1,     5,    -1,     7,   275,   276,
      -1,     8,    12,    13,    14,    -1,   283,   284,    75,    76,
      20,    -1,    -1,    77,    18,    19,    26,    -1,    82,    83,
      84,    85,    -1,    33,    34,    35,    30,    31,    32,     8,
      -1,    41,    42,    -1,    73,    74,    46,    47,    45,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,     3,    74,     5,    45,     7,    75,    76,
      -1,    81,    12,    13,    14,    -1,    86,    -1,    -1,    -1,
      20,    -1,    -1,    -1,    -1,    25,    26,    -1,    -1,    11,
      -1,    11,    -1,    33,    34,    35,    75,    76,    20,     8,
      20,    41,    42,    25,    26,    25,    26,    47,     8,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,     3,    -1,     5,    45,     7,    -1,    -1,
      -1,    81,    12,    13,    14,    45,    86,    -1,    -1,    -1,
      20,    73,    74,    73,    -1,    25,    26,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    34,    35,    75,    76,    -1,     8,
      -1,    41,    42,    -1,    -1,    75,    76,    47,     8,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,     3,    -1,     5,    45,     7,    -1,    -1,
      -1,    81,    12,    13,    14,    45,    86,    28,    29,    -1,
      20,    -1,    -1,    -1,    -1,    36,    26,    -1,    39,    40,
      -1,    -1,    -1,    33,    34,    35,    75,    76,    -1,     8,
      -1,    41,    42,    -1,    -1,    75,    76,    47,     8,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,     3,    74,     5,    45,     7,    -1,    -1,
      -1,    81,    12,    13,    14,    45,    86,    -1,    -1,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    26,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    34,    35,    75,    76,    -1,     8,
      -1,    41,    42,    -1,    -1,    75,    76,    47,     8,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,     3,    74,     5,    45,     7,    -1,    -1,
      -1,    81,    12,    13,    14,    45,    86,    -1,    -1,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    34,    35,    75,    76,    -1,     8,
      -1,    41,    42,    -1,    -1,    75,    76,    47,    48,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,     3,    -1,     5,    45,     7,     8,    -1,
      -1,    81,    12,    13,    14,    26,    86,    28,    29,    -1,
      20,    -1,    -1,    -1,    -1,    36,    -1,    -1,    39,    40,
      -1,    -1,    -1,    33,    34,    35,    75,    76,    -1,     8,
      -1,    41,    42,    -1,    -1,    -1,    -1,    47,     8,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,     3,    -1,     5,    45,     7,    -1,    -1,
      -1,    81,    12,    13,    14,    45,    86,    -1,    -1,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    34,    35,    75,    76,    -1,     8,
      -1,    41,    42,    -1,    -1,    75,    76,    47,     8,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,     3,    -1,     5,    45,     7,    -1,    -1,
      -1,    81,    12,    13,    14,    45,    86,    -1,    -1,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    34,    35,    75,    76,    -1,     8,
      -1,    41,    42,    -1,    -1,    75,    76,    47,     8,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,     3,    -1,     5,    45,     7,    -1,    -1,
      -1,    81,    12,    13,    14,    45,    86,    -1,    -1,    -1,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,
      -1,    -1,    -1,    33,    34,    35,    75,    76,    -1,     8,
      -1,    41,    42,    -1,    -1,    75,    76,    47,    45,    49,
      -1,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,     3,    -1,     5,    45,     7,    75,    76,
       8,    81,    12,    13,    14,    -1,    86,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    33,    34,    35,    75,    76,    -1,    -1,
      -1,    41,    42,    -1,     3,    -1,     5,    45,     7,    49,
       8,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    -1,    33,    34,    -1,    75,    76,     8,
      -1,    81,    41,    42,     8,    -1,    86,    45,     8,    -1,
      49,    -1,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    26,    -1,    45,    75,    76,    26,
      11,    45,    26,    -1,    26,    45,    26,    86,    -1,    20,
      26,    -1,    -1,    45,    25,    -1,    -1,    -1,    45,    26,
      -1,    45,    26,    45,    -1,    45,    75,    76,    26,    45,
      -1,    75,    76,    -1,    -1,    75,    76,    48,    45,    -1,
      -1,    45,    -1,    75,    76,    -1,    -1,    45,    75,    76,
      -1,    75,    76,    75,    76,    75,    76,    37,    38,    75,
      76,     8,    73,    43,    44,    -1,    -1,    -1,    75,    76,
      -1,    75,    76,    -1,    -1,    -1,    -1,    75,    76,    -1,
      -1,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    36,
      -1,    -1,    39,    40,    -1,    -1,    -1,    -1,    78,    79,
      80
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     5,     7,    12,    13,    14,    20,    33,    34,
      35,    41,    42,    47,    49,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    81,    86,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   106,   107,   108,   109,
      90,    90,    99,    99,    99,    25,    26,    92,     7,    90,
      28,    29,    36,    39,    40,     5,    35,    90,    48,    92,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    99,   102,     0,    88,     4,    45,    75,
      76,    11,    20,    25,    73,    27,    28,    29,    37,    38,
      43,    44,    78,    79,    80,    98,    18,    19,    30,    31,
      32,    15,    16,    17,    21,    22,    23,    24,    77,    82,
      83,    84,    85,    50,   105,     8,    90,    90,     6,     8,
      92,    92,    26,    90,    35,    35,    97,    35,    35,    90,
      26,    26,    73,    48,    26,     8,     8,     8,     8,     8,
       8,     8,     8,     8,     8,    26,     8,    26,    26,    26,
      26,     8,    26,    26,    26,     8,     8,    91,    91,    91,
      92,    25,    26,    92,    92,    26,    46,    74,    92,    94,
      94,    94,    97,    97,    97,    97,    97,    97,    97,    99,
      99,    99,    99,    99,   102,    26,     8,    26,     8,    26,
      92,     8,    26,    35,    90,    35,    92,    73,    90,    90,
      90,    90,    90,    90,    90,    90,    92,    92,    26,    92,
      26,    74,    92,    92,    26,    46,    74,    50,     8,     8,
       8,    26,    74,    92,    26,     8,     8,     8,     8,     8,
       8,    26,    92,    92,    92,    26,    74,    92,    26,    74,
      92,    92,     6,    92,    92,    26,    74,    90,    90,    92,
      92,    92,    92,    26,    74,    92,    92,     8,     8,    92,
      92
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    87,    88,    88,    89,    90,    90,    90,    90,    91,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    93,    93,    93,
      93,    94,    95,    96,    96,    96,    96,    96,    96,    96,
      96,    97,    97,    97,    97,    97,    97,    97,    98,    98,
      98,    98,    98,    98,    99,    99,    99,    99,    99,   100,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   102,   102,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   104,   105,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     107,   107,   108,   108,   109,   109,   109
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     2,     1,     3,     3,     3,     1,
       1,     3,     2,     4,     1,     3,     4,     3,     2,     4,
       5,     3,     3,     2,     5,     4,     7,     6,     7,     6,
       5,     4,     5,     4,     3,     3,     4,     4,     5,     7,
       6,     6,     5,     7,     6,     6,     5,     1,     3,     3,
       3,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     1,     2,     3,     3,     3,     3,     3,     1,     3,
       3,     3,     3,     3,     1,     2,     2,     2,     2,     1,
       1,     2,     2,     2,     2,     2,     2,     2,     3,     2,
       2,     2,     2,     1,     2,     1,     1,     4,     2,     3,
       3,     5,     5,     5,     2,     4,     4,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     5,
       5,     5,     5,     3,     5,     5,     7,     3,     3,     7,
       3,     2,     3,     3,     3,     3,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, scanner, defined_nets, defined_funcs, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YY_LOCATION_PRINT
#  if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#   define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

#  else
#   define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#  endif
# endif /* !defined YY_LOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, scanner, defined_nets, defined_funcs); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *scanner, struct defined_networks *defined_nets, struct defined_functions *defined_funcs)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (scanner);
  YY_USE (defined_nets);
  YY_USE (defined_funcs);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, void *scanner, struct defined_networks *defined_nets, struct defined_functions *defined_funcs)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, scanner, defined_nets, defined_funcs);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, void *scanner, struct defined_networks *defined_nets, struct defined_functions *defined_funcs)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), scanner, defined_nets, defined_funcs);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, scanner, defined_nets, defined_funcs); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, void *scanner, struct defined_networks *defined_nets, struct defined_functions *defined_funcs)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (scanner);
  YY_USE (defined_nets);
  YY_USE (defined_funcs);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner, struct defined_networks *defined_nets, struct defined_functions *defined_funcs)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

/* User initialization code.  */
#line 211 "regex.y"
{
    clear_quantifiers();
    rewrite = 0;
    contexts = NULL;
    rules = NULL;
    rewrite_rules = NULL;
    rule_direction = 0;
    substituting = 0;
}

#line 1657 "regex.c"

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 4: /* regex: network END  */
#line 256 "regex.y"
                                   { current_parse = (yyvsp[-1].net);              }
#line 1869 "regex.c"
    break;

  case 5: /* network: networkA  */
#line 258 "regex.y"
                  { }
#line 1875 "regex.c"
    break;

  case 6: /* network: network COMPOSE networkA  */
#line 259 "regex.y"
                                   { (yyval.net) = fsm_compose((yyvsp[-2].net),(yyvsp[0].net));         }
#line 1881 "regex.c"
    break;

  case 7: /* network: network LENIENT_COMPOSE networkA  */
#line 260 "regex.y"
                                   { (yyval.net) = fsm_lenient_compose((yyvsp[-2].net),(yyvsp[0].net)); }
#line 1887 "regex.c"
    break;

  case 8: /* network: network CROSS_PRODUCT networkA  */
#line 261 "regex.y"
                                   { (yyval.net) = fsm_cross_product((yyvsp[-2].net),(yyvsp[0].net));   }
#line 1893 "regex.c"
    break;

  case 9: /* networkA: n0  */
#line 263 "regex.y"
             { if (rewrite) { add_rewrite_rule(); (yyval.net) = fsm_rewrite(rewrite_rules); clear_rewrite_ruleset(rewrite_rules); } rewrite = 0; contexts = NULL; rules = NULL; rewrite_rules = NULL; }
#line 1899 "regex.c"
    break;

  case 10: /* n0: network1  */
#line 265 "regex.y"
             { }
#line 1905 "regex.c"
    break;

  case 11: /* n0: n0 CONTEXT n0  */
#line 266 "regex.y"
                         { (yyval.net) = NULL; add_context_pair((yyvsp[-2].net),(yyvsp[0].net));}
#line 1911 "regex.c"
    break;

  case 12: /* n0: n0 CONTEXT  */
#line 267 "regex.y"
                         { add_context_pair((yyvsp[-1].net),fsm_empty_string()); }
#line 1917 "regex.c"
    break;

  case 13: /* n0: n0 CONTEXT COMMA n0  */
#line 268 "regex.y"
                         { add_context_pair((yyvsp[-3].net),fsm_empty_string()); }
#line 1923 "regex.c"
    break;

  case 14: /* n0: CONTEXT  */
#line 269 "regex.y"
                         { add_context_pair(fsm_empty_string(),fsm_empty_string());}
#line 1929 "regex.c"
    break;

  case 15: /* n0: CONTEXT DOUBLE_COMMA n0  */
#line 270 "regex.y"
                          { add_rewrite_rule(); add_context_pair(fsm_empty_string(),fsm_empty_string());}
#line 1935 "regex.c"
    break;

  case 16: /* n0: n0 CONTEXT DOUBLE_COMMA n0  */
#line 271 "regex.y"
                             { add_rewrite_rule(); add_context_pair((yyvsp[-3].net),fsm_empty_string());}
#line 1941 "regex.c"
    break;

  case 17: /* n0: CONTEXT COMMA n0  */
#line 272 "regex.y"
                         { add_context_pair(fsm_empty_string(),fsm_empty_string());}
#line 1947 "regex.c"
    break;

  case 18: /* n0: CONTEXT n0  */
#line 273 "regex.y"
                         { add_context_pair(fsm_empty_string(),(yyvsp[0].net)); }
#line 1953 "regex.c"
    break;

  case 19: /* n0: CONTEXT n0 COMMA n0  */
#line 274 "regex.y"
                         { add_context_pair(fsm_empty_string(),(yyvsp[-2].net)); }
#line 1959 "regex.c"
    break;

  case 20: /* n0: n0 CONTEXT n0 COMMA n0  */
#line 275 "regex.y"
                         { add_context_pair((yyvsp[-4].net),(yyvsp[-2].net)); }
#line 1965 "regex.c"
    break;

  case 21: /* n0: n0 CRESTRICT n0  */
#line 276 "regex.y"
                         { (yyval.net) = fsm_context_restrict((yyvsp[-2].net),contexts); fsm_clear_contexts(contexts);}
#line 1971 "regex.c"
    break;

  case 22: /* n0: n0 ARROW n0  */
#line 277 "regex.y"
                         { add_rule((yyvsp[-2].net),(yyvsp[0].net),NULL,(yyvsp[-1].type)); if ((yyvsp[-2].net)->arity == 2) { printf("Error: LHS is transducer\n"); YYERROR;}}
#line 1977 "regex.c"
    break;

  case 23: /* n0: n0 ARROW  */
#line 278 "regex.y"
                         { add_rule((yyvsp[-1].net),NULL,NULL,(yyvsp[0].type)); }
#line 1983 "regex.c"
    break;

  case 24: /* n0: LDOT n0 RDOT ARROW n0  */
#line 280 "regex.y"
                         { add_rule((yyvsp[-3].net),(yyvsp[0].net),NULL,(yyvsp[-1].type)|ARROW_DOTTED); if ((yyvsp[0].net) == NULL) { YYERROR;}}
#line 1989 "regex.c"
    break;

  case 25: /* n0: LDOT RDOT ARROW n0  */
#line 281 "regex.y"
                      { add_eprule((yyvsp[0].net),NULL,(yyvsp[-1].type)|ARROW_DOTTED); if ((yyvsp[0].net) == NULL) { YYERROR;}}
#line 1995 "regex.c"
    break;

  case 26: /* n0: LDOT n0 RDOT ARROW n0 COMMA n0  */
#line 282 "regex.y"
                                 { add_rule((yyvsp[-5].net),(yyvsp[-2].net),NULL,(yyvsp[-3].type)|ARROW_DOTTED);}
#line 2001 "regex.c"
    break;

  case 27: /* n0: LDOT RDOT ARROW n0 COMMA n0  */
#line 283 "regex.y"
                              { add_eprule((yyvsp[-2].net),NULL,(yyvsp[-3].type)|ARROW_DOTTED);}
#line 2007 "regex.c"
    break;

  case 28: /* n0: LDOT n0 RDOT ARROW n0 DIRECTION n0  */
#line 284 "regex.y"
                                     { add_rule((yyvsp[-5].net),(yyvsp[-2].net),NULL,(yyvsp[-3].type)|ARROW_DOTTED); rule_direction = (yyvsp[-1].type);}
#line 2013 "regex.c"
    break;

  case 29: /* n0: LDOT RDOT ARROW n0 DIRECTION n0  */
#line 285 "regex.y"
                                  { add_eprule((yyvsp[-2].net),NULL,(yyvsp[-3].type)|ARROW_DOTTED); rule_direction = (yyvsp[-1].type);}
#line 2019 "regex.c"
    break;

  case 30: /* n0: n0 ARROW n0 COMMA n0  */
#line 286 "regex.y"
                       { add_rule((yyvsp[-4].net),(yyvsp[-2].net),NULL,(yyvsp[-3].type));}
#line 2025 "regex.c"
    break;

  case 31: /* n0: n0 ARROW COMMA n0  */
#line 287 "regex.y"
                    { add_rule((yyvsp[-3].net),NULL,NULL,(yyvsp[-2].type));}
#line 2031 "regex.c"
    break;

  case 32: /* n0: n0 ARROW n0 DIRECTION n0  */
#line 288 "regex.y"
                           { add_rule((yyvsp[-4].net),(yyvsp[-2].net),NULL,(yyvsp[-3].type)); rule_direction = (yyvsp[-1].type);}
#line 2037 "regex.c"
    break;

  case 33: /* n0: n0 ARROW DIRECTION n0  */
#line 289 "regex.y"
                        { add_rule((yyvsp[-3].net),NULL,NULL,(yyvsp[-2].type)); rule_direction = (yyvsp[-1].type);}
#line 2043 "regex.c"
    break;

  case 34: /* n0: n0 DOUBLE_COMMA n0  */
#line 291 "regex.y"
                      { add_rewrite_rule();}
#line 2049 "regex.c"
    break;

  case 35: /* n0: n0 ARROW TRIPLE_DOT  */
#line 293 "regex.y"
                          { add_rule((yyvsp[-2].net),fsm_empty_string(),fsm_empty_string(),(yyvsp[-1].type));}
#line 2055 "regex.c"
    break;

  case 36: /* n0: n0 ARROW TRIPLE_DOT n0  */
#line 294 "regex.y"
                          { add_rule((yyvsp[-3].net),fsm_empty_string(),(yyvsp[0].net),(yyvsp[-2].type));}
#line 2061 "regex.c"
    break;

  case 37: /* n0: n0 ARROW n0 TRIPLE_DOT  */
#line 295 "regex.y"
                          { add_rule((yyvsp[-3].net),(yyvsp[-1].net),fsm_empty_string(),(yyvsp[-2].type));}
#line 2067 "regex.c"
    break;

  case 38: /* n0: n0 ARROW n0 TRIPLE_DOT n0  */
#line 296 "regex.y"
                            { add_rule((yyvsp[-4].net),(yyvsp[-2].net),(yyvsp[0].net),(yyvsp[-3].type));}
#line 2073 "regex.c"
    break;

  case 39: /* n0: n0 ARROW n0 TRIPLE_DOT n0 COMMA n0  */
#line 297 "regex.y"
                                     { add_rule((yyvsp[-6].net),(yyvsp[-4].net),(yyvsp[-2].net),(yyvsp[-5].type));}
#line 2079 "regex.c"
    break;

  case 40: /* n0: n0 ARROW n0 TRIPLE_DOT COMMA n0  */
#line 298 "regex.y"
                                  { add_rule((yyvsp[-5].net),(yyvsp[-3].net),fsm_empty_string(),(yyvsp[-4].type));}
#line 2085 "regex.c"
    break;

  case 41: /* n0: n0 ARROW TRIPLE_DOT n0 COMMA n0  */
#line 299 "regex.y"
                                  { add_rule((yyvsp[-5].net),fsm_empty_string(),(yyvsp[-2].net),(yyvsp[-4].type));}
#line 2091 "regex.c"
    break;

  case 42: /* n0: n0 ARROW TRIPLE_DOT COMMA n0  */
#line 300 "regex.y"
                                { add_rule((yyvsp[-4].net),fsm_empty_string(),fsm_empty_string(),(yyvsp[-3].type));}
#line 2097 "regex.c"
    break;

  case 43: /* n0: n0 ARROW n0 TRIPLE_DOT n0 DIRECTION n0  */
#line 301 "regex.y"
                                         { add_rule((yyvsp[-6].net),(yyvsp[-4].net),(yyvsp[-2].net),(yyvsp[-5].type)); rule_direction = (yyvsp[-1].type);}
#line 2103 "regex.c"
    break;

  case 44: /* n0: n0 ARROW TRIPLE_DOT n0 DIRECTION n0  */
#line 302 "regex.y"
                                      { add_rule((yyvsp[-5].net),fsm_empty_string(),(yyvsp[-2].net),(yyvsp[-4].type)); rule_direction = (yyvsp[-1].type);}
#line 2109 "regex.c"
    break;

  case 45: /* n0: n0 ARROW n0 TRIPLE_DOT DIRECTION n0  */
#line 303 "regex.y"
                                      { add_rule((yyvsp[-5].net),(yyvsp[-3].net),fsm_empty_string(),(yyvsp[-4].type)); rule_direction = (yyvsp[-1].type);}
#line 2115 "regex.c"
    break;

  case 46: /* n0: n0 ARROW TRIPLE_DOT DIRECTION n0  */
#line 304 "regex.y"
                                   { add_rule((yyvsp[-4].net),fsm_empty_string(),fsm_empty_string(),(yyvsp[-3].type)); rule_direction = (yyvsp[-1].type);}
#line 2121 "regex.c"
    break;

  case 47: /* network1: network2  */
#line 307 "regex.y"
                   { }
#line 2127 "regex.c"
    break;

  case 48: /* network1: network1 SHUFFLE network2  */
#line 308 "regex.y"
                              { (yyval.net) = fsm_shuffle((yyvsp[-2].net),(yyvsp[0].net));  }
#line 2133 "regex.c"
    break;

  case 49: /* network1: network1 PRECEDES network2  */
#line 309 "regex.y"
                              { (yyval.net) = fsm_precedes((yyvsp[-2].net),(yyvsp[0].net)); }
#line 2139 "regex.c"
    break;

  case 50: /* network1: network1 FOLLOWS network2  */
#line 310 "regex.y"
                              { (yyval.net) = fsm_follows((yyvsp[-2].net),(yyvsp[0].net));  }
#line 2145 "regex.c"
    break;

  case 51: /* network2: network3  */
#line 312 "regex.y"
                   { }
#line 2151 "regex.c"
    break;

  case 52: /* network3: network4  */
#line 314 "regex.y"
                   { }
#line 2157 "regex.c"
    break;

  case 53: /* network4: network5  */
#line 316 "regex.y"
                   { }
#line 2163 "regex.c"
    break;

  case 54: /* network4: network4 UNION network5  */
#line 317 "regex.y"
                                     { (yyval.net) = fsm_union((yyvsp[-2].net),(yyvsp[0].net));                     }
#line 2169 "regex.c"
    break;

  case 55: /* network4: network4 PRIORITY_UNION_U network5  */
#line 318 "regex.y"
                                     { (yyval.net) = fsm_priority_union_upper((yyvsp[-2].net),(yyvsp[0].net));      }
#line 2175 "regex.c"
    break;

  case 56: /* network4: network4 PRIORITY_UNION_L network5  */
#line 319 "regex.y"
                                     { (yyval.net) = fsm_priority_union_lower((yyvsp[-2].net),(yyvsp[0].net));      }
#line 2181 "regex.c"
    break;

  case 57: /* network4: network4 INTERSECT network5  */
#line 320 "regex.y"
                                     { (yyval.net) = fsm_intersect((yyvsp[-2].net),(yyvsp[0].net));                 }
#line 2187 "regex.c"
    break;

  case 58: /* network4: network4 MINUS network5  */
#line 321 "regex.y"
                                     { (yyval.net) = fsm_minus((yyvsp[-2].net),(yyvsp[0].net));                     }
#line 2193 "regex.c"
    break;

  case 59: /* network4: network4 IMPLIES network5  */
#line 322 "regex.y"
                                     { (yyval.net) = fsm_union(fsm_complement((yyvsp[-2].net)),(yyvsp[0].net));     }
#line 2199 "regex.c"
    break;

  case 60: /* network4: network4 BICOND network5  */
#line 323 "regex.y"
                                     { (yyval.net) = fsm_intersect(fsm_union(fsm_complement(fsm_copy((yyvsp[-2].net))),fsm_copy((yyvsp[0].net))), fsm_union(fsm_complement(fsm_copy((yyvsp[0].net))),fsm_copy((yyvsp[-2].net)))); fsm_destroy((yyvsp[-2].net)); fsm_destroy((yyvsp[0].net));}
#line 2205 "regex.c"
    break;

  case 61: /* network5: network6  */
#line 325 "regex.y"
                    { }
#line 2211 "regex.c"
    break;

  case 62: /* network5: network5 network6  */
#line 326 "regex.y"
                    { (yyval.net) = fsm_concat((yyvsp[-1].net),(yyvsp[0].net)); }
#line 2217 "regex.c"
    break;

  case 63: /* network5: VAR IN network5  */
#line 327 "regex.y"
                    { (yyval.net) = fsm_ignore(fsm_contains(fsm_concat(fsm_symbol((yyvsp[-2].string)),fsm_concat((yyvsp[0].net),fsm_symbol((yyvsp[-2].string))))),union_quantifiers(),OP_IGNORE_ALL); }
#line 2223 "regex.c"
    break;

  case 64: /* network5: VAR EQUALS VAR  */
#line 329 "regex.y"
                    { (yyval.net) = fsm_logical_eq((yyvsp[-2].string),(yyvsp[0].string)); }
#line 2229 "regex.c"
    break;

  case 65: /* network5: VAR NEQ VAR  */
#line 330 "regex.y"
                    { (yyval.net) = fsm_complement(fsm_logical_eq((yyvsp[-2].string),(yyvsp[0].string))); }
#line 2235 "regex.c"
    break;

  case 66: /* network5: VAR PRECEDES VAR  */
#line 331 "regex.y"
                    { (yyval.net) = fsm_logical_precedence((yyvsp[-2].string),(yyvsp[0].string)); }
#line 2241 "regex.c"
    break;

  case 67: /* network5: VAR FOLLOWS VAR  */
#line 332 "regex.y"
                    { (yyval.net) = fsm_logical_precedence((yyvsp[0].string),(yyvsp[-2].string)); }
#line 2247 "regex.c"
    break;

  case 68: /* network6: network7  */
#line 334 "regex.y"
                   { }
#line 2253 "regex.c"
    break;

  case 69: /* network6: network6 IGNORE_ALL network7  */
#line 335 "regex.y"
                                         { (yyval.net) = fsm_ignore((yyvsp[-2].net),(yyvsp[0].net), OP_IGNORE_ALL);          }
#line 2259 "regex.c"
    break;

  case 70: /* network6: network6 IGNORE_INTERNAL network7  */
#line 336 "regex.y"
                                         { (yyval.net) = fsm_ignore((yyvsp[-2].net),(yyvsp[0].net), OP_IGNORE_INTERNAL);     }
#line 2265 "regex.c"
    break;

  case 71: /* network6: network6 RIGHT_QUOTIENT network7  */
#line 337 "regex.y"
                                         { (yyval.net) = fsm_quotient_right((yyvsp[-2].net),(yyvsp[0].net));                 }
#line 2271 "regex.c"
    break;

  case 72: /* network6: network6 LEFT_QUOTIENT network7  */
#line 338 "regex.y"
                                         { (yyval.net) = fsm_quotient_left((yyvsp[-2].net),(yyvsp[0].net));                  }
#line 2277 "regex.c"
    break;

  case 73: /* network6: network6 INTERLEAVE_QUOTIENT network7  */
#line 339 "regex.y"
                                         { (yyval.net) = fsm_quotient_interleave((yyvsp[-2].net),(yyvsp[0].net));            }
#line 2283 "regex.c"
    break;

  case 74: /* network7: network8  */
#line 341 "regex.y"
                   { }
#line 2289 "regex.c"
    break;

  case 75: /* network7: COMPLEMENT network7  */
#line 342 "regex.y"
                              { (yyval.net) = fsm_complement((yyvsp[0].net));       }
#line 2295 "regex.c"
    break;

  case 76: /* network7: CONTAINS network7  */
#line 343 "regex.y"
                              { (yyval.net) = fsm_contains((yyvsp[0].net));         }
#line 2301 "regex.c"
    break;

  case 77: /* network7: CONTAINS_ONE network7  */
#line 344 "regex.y"
                              { (yyval.net) = fsm_contains_one((yyvsp[0].net));     }
#line 2307 "regex.c"
    break;

  case 78: /* network7: CONTAINS_OPT_ONE network7  */
#line 345 "regex.y"
                              { (yyval.net) = fsm_contains_opt_one((yyvsp[0].net)); }
#line 2313 "regex.c"
    break;

  case 79: /* network8: network9  */
#line 347 "regex.y"
                   { }
#line 2319 "regex.c"
    break;

  case 80: /* network9: network10  */
#line 349 "regex.y"
                    { }
#line 2325 "regex.c"
    break;

  case 81: /* network9: network9 KLEENE_STAR  */
#line 350 "regex.y"
                                        { (yyval.net) = fsm_kleene_star(fsm_minimize((yyvsp[-1].net))); }
#line 2331 "regex.c"
    break;

  case 82: /* network9: network9 KLEENE_PLUS  */
#line 351 "regex.y"
                                        { (yyval.net) = fsm_kleene_plus((yyvsp[-1].net)); }
#line 2337 "regex.c"
    break;

  case 83: /* network9: network9 REVERSE  */
#line 352 "regex.y"
                                        { (yyval.net) = fsm_determinize(fsm_reverse((yyvsp[-1].net))); }
#line 2343 "regex.c"
    break;

  case 84: /* network9: network9 INVERSE  */
#line 353 "regex.y"
                                        { (yyval.net) = fsm_invert((yyvsp[-1].net)); }
#line 2349 "regex.c"
    break;

  case 85: /* network9: network9 XUPPER  */
#line 354 "regex.y"
                                        { (yyval.net) = fsm_upper((yyvsp[-1].net)); }
#line 2355 "regex.c"
    break;

  case 86: /* network9: network9 XLOWER  */
#line 355 "regex.y"
                                        { (yyval.net) = fsm_lower((yyvsp[-1].net)); }
#line 2361 "regex.c"
    break;

  case 87: /* network9: network9 FLAG_ELIMINATE  */
#line 356 "regex.y"
                                        { (yyval.net) = flag_eliminate((yyvsp[-1].net), NULL); }
#line 2367 "regex.c"
    break;

  case 88: /* network9: network9 HIGH_CROSS_PRODUCT network10  */
#line 357 "regex.y"
                                        { (yyval.net) = fsm_cross_product((yyvsp[-2].net),(yyvsp[0].net)); }
#line 2373 "regex.c"
    break;

  case 89: /* network9: network9 NCONCAT  */
#line 359 "regex.y"
                          { (yyval.net) = fsm_concat_n((yyvsp[-1].net),atoi((yyvsp[0].string))); }
#line 2379 "regex.c"
    break;

  case 90: /* network9: network9 MORENCONCAT  */
#line 360 "regex.y"
                          { (yyval.net) = fsm_concat(fsm_concat_n(fsm_copy((yyvsp[-1].net)), atoi((yyvsp[0].string))),fsm_kleene_plus(fsm_copy((yyvsp[-1].net)))); fsm_destroy((yyvsp[-1].net)); }
#line 2385 "regex.c"
    break;

  case 91: /* network9: network9 LESSNCONCAT  */
#line 361 "regex.y"
                          { (yyval.net) = fsm_concat_m_n((yyvsp[-1].net),0,atoi((yyvsp[0].string))-1); }
#line 2391 "regex.c"
    break;

  case 92: /* network9: network9 MNCONCAT  */
#line 362 "regex.y"
                          { (yyval.net) = fsm_concat_m_n((yyvsp[-1].net),atoi((yyvsp[0].string)),atoi(strstr((yyvsp[0].string),",")+1)); }
#line 2397 "regex.c"
    break;

  case 93: /* network10: network11  */
#line 364 "regex.y"
                     { }
#line 2403 "regex.c"
    break;

  case 94: /* network10: TERM_NEGATION network10  */
#line 365 "regex.y"
                          { (yyval.net) = fsm_term_negation((yyvsp[0].net)); }
#line 2409 "regex.c"
    break;

  case 95: /* network11: NET  */
#line 367 "regex.y"
               { (yyval.net) = (yyvsp[0].net);}
#line 2415 "regex.c"
    break;

  case 96: /* network11: network12  */
#line 368 "regex.y"
            { (yyval.net) = (yyvsp[0].net); }
#line 2421 "regex.c"
    break;

  case 97: /* network11: UQUANT LPAREN network RPAREN  */
#line 369 "regex.y"
                               { (yyval.net) = fsm_complement(fsm_substitute_symbol(fsm_intersect(fsm_quantifier((yyvsp[-3].string)),fsm_complement((yyvsp[-1].net))),(yyvsp[-3].string),"@_EPSILON_SYMBOL_@")); purge_quantifier((yyvsp[-3].string)); }
#line 2427 "regex.c"
    break;

  case 98: /* network11: EQUANT network  */
#line 370 "regex.y"
                 {  (yyval.net) = fsm_substitute_symbol(fsm_intersect(fsm_quantifier((yyvsp[-1].string)),(yyvsp[0].net)),(yyvsp[-1].string),"@_EPSILON_SYMBOL_@"); purge_quantifier((yyvsp[-1].string)); }
#line 2433 "regex.c"
    break;

  case 99: /* network11: LPAREN network RPAREN  */
#line 371 "regex.y"
                        { if (count_quantifiers()) (yyval.net) = (yyvsp[-1].net); else {(yyval.net) = fsm_optionality((yyvsp[-1].net));} }
#line 2439 "regex.c"
    break;

  case 100: /* network11: LBRACKET network RBRACKET  */
#line 372 "regex.y"
                            { (yyval.net) = (yyvsp[-1].net); }
#line 2445 "regex.c"
    break;

  case 101: /* network11: SUCCESSOR_OF VAR COMMA VAR RPAREN  */
#line 373 "regex.y"
                                    {(yyval.net) = fsm_concat(fsm_universal(),fsm_concat(fsm_symbol((yyvsp[-3].string)),fsm_concat(fsm_universal(),fsm_concat(fsm_symbol((yyvsp[-3].string)),fsm_concat(union_quantifiers(),fsm_concat(fsm_symbol((yyvsp[-1].string)),fsm_concat(fsm_universal(),fsm_concat(fsm_symbol((yyvsp[-1].string)),fsm_universal())))))))); }
#line 2451 "regex.c"
    break;

  case 102: /* network11: SUCCESSOR_OF VAR COMMA network RPAREN  */
#line 374 "regex.y"
                                        {(yyval.net) = fsm_concat(fsm_universal(),fsm_concat(fsm_symbol((yyvsp[-3].string)),fsm_concat(fsm_universal(),fsm_concat(fsm_symbol((yyvsp[-3].string)),fsm_concat(fsm_ignore((yyvsp[-1].net),union_quantifiers(),OP_IGNORE_ALL),fsm_universal()))))); }
#line 2457 "regex.c"
    break;

  case 103: /* network11: SUCCESSOR_OF network COMMA VAR RPAREN  */
#line 375 "regex.y"
                                        {(yyval.net) = fsm_concat(fsm_universal(),fsm_concat(fsm_ignore((yyvsp[-3].net),union_quantifiers(),OP_IGNORE_ALL),fsm_concat(fsm_symbol((yyvsp[-1].string)),fsm_concat(fsm_universal(),fsm_concat(fsm_symbol((yyvsp[-1].string)),fsm_universal()))))); }
#line 2463 "regex.c"
    break;

  case 104: /* network11: sub1 sub2  */
#line 376 "regex.y"
            {(yyval.net) = fsm_substitute_symbol((yyvsp[-1].net),subval1,subval2); substituting = 0; xxfree(subval1); xxfree(subval2); subval1 = subval2 = NULL;}
#line 2469 "regex.c"
    break;

  case 105: /* sub1: SUBSTITUTE LBRACKET network COMMA  */
#line 378 "regex.y"
                                        { (yyval.net) = (yyvsp[-1].net); substituting = 1;                      }
#line 2475 "regex.c"
    break;

  case 106: /* sub2: SUBVAL COMMA SUBVAL RBRACKET  */
#line 379 "regex.y"
                                        { subval1 = (yyvsp[-2].string); subval2 = (yyvsp[0].string); }
#line 2481 "regex.c"
    break;

  case 107: /* network12: fend  */
#line 381 "regex.y"
                   { (yyval.net) = (yyvsp[0].net); }
#line 2487 "regex.c"
    break;

  case 108: /* network12: ISIDENTITY network RPAREN  */
#line 382 "regex.y"
                                        { (yyval.net) = fsm_boolean(fsm_isidentity((yyvsp[-1].net)));   }
#line 2493 "regex.c"
    break;

  case 109: /* network12: ISFUNCTIONAL network RPAREN  */
#line 383 "regex.y"
                                        { (yyval.net) = fsm_boolean(fsm_isfunctional((yyvsp[-1].net))); }
#line 2499 "regex.c"
    break;

  case 110: /* network12: ISUNAMBIGUOUS network RPAREN  */
#line 384 "regex.y"
                                        { (yyval.net) = fsm_boolean(fsm_isunambiguous((yyvsp[-1].net))); }
#line 2505 "regex.c"
    break;

  case 111: /* network12: NOTID network RPAREN  */
#line 385 "regex.y"
                                        { (yyval.net) = fsm_extract_nonidentity(fsm_copy((yyvsp[-1].net))); }
#line 2511 "regex.c"
    break;

  case 112: /* network12: LOWERUNIQ network RPAREN  */
#line 386 "regex.y"
                                        { (yyval.net) = fsm_lowerdet(fsm_copy((yyvsp[-1].net))); }
#line 2517 "regex.c"
    break;

  case 113: /* network12: LOWERUNIQEPS network RPAREN  */
#line 387 "regex.y"
                                        { (yyval.net) = fsm_lowerdeteps(fsm_copy((yyvsp[-1].net))); }
#line 2523 "regex.c"
    break;

  case 114: /* network12: ALLFINAL network RPAREN  */
#line 388 "regex.y"
                                        { (yyval.net) = fsm_markallfinal(fsm_copy((yyvsp[-1].net))); }
#line 2529 "regex.c"
    break;

  case 115: /* network12: UNAMBIGUOUSPART network RPAREN  */
#line 389 "regex.y"
                                        { (yyval.net) = fsm_extract_unambiguous(fsm_copy((yyvsp[-1].net)));      }
#line 2535 "regex.c"
    break;

  case 116: /* network12: AMBIGUOUSPART network RPAREN  */
#line 390 "regex.y"
                                        { (yyval.net) = fsm_extract_ambiguous(fsm_copy((yyvsp[-1].net)));        }
#line 2541 "regex.c"
    break;

  case 117: /* network12: AMBIGUOUSDOMAIN network RPAREN  */
#line 391 "regex.y"
                                        { (yyval.net) = fsm_extract_ambiguous_domain(fsm_copy((yyvsp[-1].net))); }
#line 2547 "regex.c"
    break;

  case 118: /* network12: LETTERMACHINE network RPAREN  */
#line 392 "regex.y"
                                        { (yyval.net) = fsm_letter_machine(fsm_copy((yyvsp[-1].net))); }
#line 2553 "regex.c"
    break;

  case 119: /* network12: MARKFSMTAIL network COMMA network RPAREN  */
#line 393 "regex.y"
                                                  { (yyval.net) = fsm_mark_fsm_tail((yyvsp[-3].net),(yyvsp[-1].net)); }
#line 2559 "regex.c"
    break;

  case 120: /* network12: MARKFSMTAILLOOP network COMMA network RPAREN  */
#line 394 "regex.y"
                                                      { (yyval.net) = fsm_add_loop((yyvsp[-3].net),(yyvsp[-1].net),1); }
#line 2565 "regex.c"
    break;

  case 121: /* network12: MARKFSMMIDLOOP network COMMA network RPAREN  */
#line 395 "regex.y"
                                                     { (yyval.net) = fsm_add_loop((yyvsp[-3].net),(yyvsp[-1].net),0); }
#line 2571 "regex.c"
    break;

  case 122: /* network12: MARKFSMLOOP network COMMA network RPAREN  */
#line 396 "regex.y"
                                                  { (yyval.net) = fsm_add_loop((yyvsp[-3].net),(yyvsp[-1].net),2); }
#line 2577 "regex.c"
    break;

  case 123: /* network12: ADDSINK network RPAREN  */
#line 397 "regex.y"
                                { (yyval.net) = fsm_add_sink((yyvsp[-1].net),1); }
#line 2583 "regex.c"
    break;

  case 124: /* network12: LEFTREWR network COMMA network RPAREN  */
#line 398 "regex.y"
                                               { (yyval.net) = fsm_left_rewr((yyvsp[-3].net),(yyvsp[-1].net)); }
#line 2589 "regex.c"
    break;

  case 125: /* network12: FLATTEN network COMMA network RPAREN  */
#line 399 "regex.y"
                                              { (yyval.net) = fsm_flatten((yyvsp[-3].net),(yyvsp[-1].net)); }
#line 2595 "regex.c"
    break;

  case 126: /* network12: SUBLABEL network COMMA network COMMA network RPAREN  */
#line 400 "regex.y"
                                                             { (yyval.net) = fsm_substitute_label((yyvsp[-5].net), fsm_network_to_char((yyvsp[-3].net)), (yyvsp[-1].net)); }
#line 2601 "regex.c"
    break;

  case 127: /* network12: CLOSESIGMA network RPAREN  */
#line 401 "regex.y"
                                      { (yyval.net) = fsm_close_sigma(fsm_copy((yyvsp[-1].net)), 0); }
#line 2607 "regex.c"
    break;

  case 128: /* network12: CLOSESIGMAUNK network RPAREN  */
#line 402 "regex.y"
                                      { (yyval.net) = fsm_close_sigma(fsm_copy((yyvsp[-1].net)), 1); }
#line 2613 "regex.c"
    break;

  case 129: /* network12: EQSUBSTRINGS network COMMA network COMMA network RPAREN  */
#line 403 "regex.y"
                                                                 { (yyval.net) = fsm_equal_substrings((yyvsp[-5].net),(yyvsp[-3].net),(yyvsp[-1].net)); }
#line 2619 "regex.c"
    break;

  case 130: /* fstart: FUNCTION network COMMA  */
#line 406 "regex.y"
{ frec++; fargptr[frec] = 0 ;declare_function_name((yyvsp[-2].string)) ; add_function_argument((yyvsp[-1].net)); }
#line 2625 "regex.c"
    break;

  case 131: /* fstart: FUNCTION network  */
#line 408 "regex.y"
{ frec++; fargptr[frec] = 0 ;declare_function_name((yyvsp[-1].string)) ; add_function_argument((yyvsp[0].net)); }
#line 2631 "regex.c"
    break;

  case 132: /* fmid: fstart network COMMA  */
#line 410 "regex.y"
                               { add_function_argument((yyvsp[-1].net)); }
#line 2637 "regex.c"
    break;

  case 133: /* fmid: fmid network COMMA  */
#line 411 "regex.y"
                               { add_function_argument((yyvsp[-1].net)); }
#line 2643 "regex.c"
    break;

  case 134: /* fend: fmid network RPAREN  */
#line 414 "regex.y"
{ add_function_argument((yyvsp[-1].net)); if (((yyval.net) = function_apply(defined_nets, defined_funcs)) == NULL) YYERROR; }
#line 2649 "regex.c"
    break;

  case 135: /* fend: fstart network RPAREN  */
#line 416 "regex.y"
{ add_function_argument((yyvsp[-1].net)); if (((yyval.net) = function_apply(defined_nets, defined_funcs)) == NULL) YYERROR; }
#line 2655 "regex.c"
    break;

  case 136: /* fend: fstart RPAREN  */
#line 418 "regex.y"
{ if (((yyval.net) = function_apply(defined_nets, defined_funcs)) == NULL) YYERROR;}
#line 2661 "regex.c"
    break;


#line 2665 "regex.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (&yylloc, scanner, defined_nets, defined_funcs, YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, scanner, defined_nets, defined_funcs);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, scanner, defined_nets, defined_funcs);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, scanner, defined_nets, defined_funcs, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, scanner, defined_nets, defined_funcs);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, scanner, defined_nets, defined_funcs);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 420 "regex.y"

