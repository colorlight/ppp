/*
 * Miscellaneous definitions for PPP STREAMS modules.
 */

#if defined(osf) || defined(__osf__)
#define OSF1
#endif

/*
 * Macros for allocating and freeing kernel memory.
 */
#ifdef SVR4			/* SVR4, including Solaris 2 */
#include <sys/kmem.h>
#define ALLOC_SLEEP(n)		kmem_alloc((n), KM_SLEEP)
#define ALLOC_NOSLEEP(n)	kmem_alloc((n), KM_NOSLEEP)
#define FREE(p, n)		kmem_free((p), (n))
#endif

#ifdef SUNOS4
#include <sys/kmem_alloc.h>	/* SunOS 4.x */
#define ALLOC_SLEEP(n)		kmem_alloc((n), KMEM_SLEEP)
#define ALLOC_NOSLEEP(n)	kmem_alloc((n), KMEM_NOSLEEP)
#define FREE(p, n)		kmem_free((p), (n))
#endif /* SunOS 4 */

#ifdef OSF1
#include <kern/kalloc.h>	/* OSF/1 */
#undef FREE
#define ALLOC_SLEEP(n)		kalloc((n))
#define ALLOC_NOSLEEP(n)	kalloc((n))
#define FREE(p, n)		kfree((p), (n))
#endif /* OSF/1 */

#ifdef AIX4
#define ALLOC_SLEEP(n)		xmalloc((n), 0, pinned_heap)	/* AIX V4.x */
#define ALLOC_NOSLEEP(n)	xmalloc((n), 0, pinned_heap)	/* AIX V4.x */
#define FREE(p, n)		xmfree((p), pinned_heap)
#endif /* AIX */

/*
 * Macros for printing debugging stuff.
 */
#ifdef DEBUG
#ifdef SVR4
#define DPRINT(f)		cmn_err(CE_CONT, f)
#define DPRINT1(f, a1)		cmn_err(CE_CONT, f, a1)
#define DPRINT2(f, a1, a2)	cmn_err(CE_CONT, f, a1, a2)
#define DPRINT3(f, a1, a2, a3)	cmn_err(CE_CONT, f, a1, a2, a3)
#else
#define DPRINT(f)		printf(f)
#define DPRINT1(f, a1)		printf(f, a1)
#define DPRINT2(f, a1, a2)	printf(f, a1, a2)
#define DPRINT3(f, a1, a2, a3)	printf(f, a1, a2, a3)
#endif /* SVR4 */

#else
#define DPRINT(f)		0
#define DPRINT1(f, a1)		0
#define DPRINT2(f, a1, a2)	0
#define DPRINT3(f, a1, a2, a3)	0
#endif /* DEBUG */

#ifndef SVR4
typedef unsigned char uchar_t;
typedef unsigned short ushort_t;
typedef int minor_t;
#endif

/*
 * If we don't have multithreading support, define substitutes.
 */
#ifndef D_MP
# define qprocson(q)
# define qprocsoff(q)
# define put(q, mp)	((*(q)->q_qinfo->qi_putp)((q), (mp)))
# define canputnext(q)	canput((q)->q_next)
# define qwriter(q, mp, func, scope)	(func)((q), (mp))
#endif

#ifdef D_MP
/* Use msgpullup if we have other multithreading support. */
#define PULLUP(mp, len)				\
    do {					\
	mblk_t *np = msgpullup((mp), (len));	\
	freemsg((mp));				\
	mp = np;				\
    } while (0)

#else
/* Use pullupmsg if we don't have any multithreading support. */
#define PULLUP(mp, len)			\
    do {				\
	if (!pullupmsg((mp), (len))) {	\
	    freemsg((mp));		\
	    mp = 0;			\
	}				\
    } while (0)
#endif

/*
 * How to declare the open and close procedures for a module.
 */
#ifdef SVR4
#define MOD_OPEN_DECL(name)	\
static int name __P((queue_t *, dev_t *, int, int, cred_t *))

#define MOD_CLOSE_DECL(name)	\
static int name __P((queue_t *, int, cred_t *))

#define MOD_OPEN(name)				\
static int name(q, devp, flag, sflag, credp)	\
    queue_t *q;					\
    dev_t *devp;				\
    int flag, sflag;				\
    cred_t *credp;

#define MOD_CLOSE(name)		\
static int name(q, flag, credp)	\
    queue_t *q;			\
    int flag;			\
    cred_t *credp;

#define OPEN_ERROR(x)		return (x)
#define DRV_OPEN_OK(dev)	return 0

#define NOTSUSER()		(drv_priv(credp))

#else	/* not SVR4 */
#define MOD_OPEN_DECL(name)	\
static int name __P((queue_t *, int, int, int))

#define MOD_CLOSE_DECL(name)	\
static int name __P((queue_t *, int))

#define MOD_OPEN(name)		\
static int name(q, dev, flag, sflag)	\
    queue_t *q;				\
    int dev;				\
    int flag, sflag;

#define MOD_CLOSE(name)		\
static int name(q, flag)	\
    queue_t *q;			\
    int flag;

#define OPEN_ERROR(x)		{ u.u_error = (x); return OPENFAIL; }
#define DRV_OPEN_OK(dev)	return (dev)

#ifdef OSF1
#include <sys/proc.h>
#define NOTSUSER()	(suser(u.u_procp->p_rcred, &u.u_acflag))
#else
#define NOTSUSER()	(suser()? 0: EPERM)
#endif

#endif	/* SVR4 */
