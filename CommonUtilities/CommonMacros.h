#ifndef COMMONMACROS_HEADER
#define COMMONMACROS_HEADER

namespace CommonUtilities
{
	namespace Macros
	{
		#define MIN(a,b)  ((a) < (b) ? (a) : (b))
		#define MAX(a,b)  ((a) > (b) ? (a) : (b))

		#define SAFE_DELETE(a) delete (a); a=NULL;
	};
};

#endif
