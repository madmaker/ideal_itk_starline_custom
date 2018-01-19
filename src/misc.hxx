#ifndef MISC_HXX_
#define MISC_HXX_

#define STR_INIT(str) ((str)[0]='\0')
#define STR_EMPTY(str) ((str)[0]=='\0')
#define WRITE_LOG WriteToSyslog
#define IFERR_THROW(X) (ThrowException((char*) __FILE__, __LINE__, (char*) #X, X))

#include <string>
#include <tc/tc.h>

template<class T> class auto_itk_mem_free {
public:
	auto_itk_mem_free(T * t = NULL) :
			m_t(t) {
	}
	virtual ~ auto_itk_mem_free() {
		if (m_t) {
			MEM_free(m_t);
			m_t = NULL;
		}
	}
	operator T *() {
		return m_t;
	}
	operator T **() {
		return &m_t;
	}
private:
	T * m_t;
};

bool StartsWith(const std::string & param, const std::string & what);
void AppendToResult(char*, char*, char*, int);
char *StrCpy(char*, int, const char*);
char *StrUpr(char *StrInOut);
int ShowTCMessage(int, char*, int, char*);
void WriteToSyslog(char * format,...);
void ThrowException(char*, int, char*, int);

#endif /* MISC_HXX_ */
