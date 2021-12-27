
#ifdef __cplusplus__
extern "C" {}
#endif

const char* tmstr(char str[25]);


#ifndef NDEBUG
#define DBG_LOG(msg) do { std::cout << tmstr(__tmstr) << ":" << __FILE__ << ":" << __LINE__ << "): " << msg << '\n'; } while(false)
#else
#define DEB_LOG(msg) do {} while {false}
#endif


#ifdef __cplusplus__
extern }
#endif
