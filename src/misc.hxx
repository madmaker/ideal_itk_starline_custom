#ifndef MISC_HXX_
#define MISC_HXX_

#define STR_INIT(str) ((str)[0]='\0')
#define STR_EMPTY(str) ((str)[0]=='\0')
#define WRITE_LOG WriteToSyslog
#define IFERR_THROW(X) (ThrowException((char*) __FILE__, __LINE__, (char*) #X, X))
#define IFNULLTAG_THROW(X) (ThrowExceptionIfNullTag((char*) __FILE__, __LINE__, (char*) #X, X))
#define NULLTAG_FOUND ITK_invalid_nulltag

#include <string>
#include <tc/tc.h>
#include <tc/tc_errors.h>

bool StartsWith(const std::string & param, const std::string & what);
void AppendToResult(char*, char*, char*, int);
char *StrCpy(char*, int, const char*);
char *StrUpr(char *StrInOut);
int ShowTCMessage(int, char*, int, char*);
void WriteToSyslog(char * format,...);
void ThrowException(char*, int, char*, int);
void ThrowExceptionIfNullTag(char* file, int line, char* var, tag_t toChkTag);
int cmpfunc(const void *, const void *);

#endif
