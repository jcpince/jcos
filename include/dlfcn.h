#ifndef __DLFCN_H__
#define __DLFCN_H__

typedef struct
{
  __const char *dli_fname;	/* File name of defining object.  */
  void *dli_fbase;		/* Load address of that object.  */
  __const char *dli_sname;	/* Name of nearest symbol.  */
  void *dli_saddr;		/* Exact value of nearest symbol.  */
} Dl_info;

inline int dladdr(void*, Dl_info*) {
	return 0;
}

#endif /* __DLFCN_H__ */ 
