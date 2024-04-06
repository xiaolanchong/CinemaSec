#ifndef ELVEES_TODO__
#define ELVEES_TODO__

#if defined (_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

///////////////////////////////////////////////////////////////////////////////

// To allow #pragma TODO(blah blah) to give a 'clickable' message in the build
// window.

#define TODOSTRINGIZE(L) #L
#define TODOMAKESTRING(M,L) M(L)
#define TODOLINE TODOMAKESTRING( TODOSTRINGIZE, __LINE__) 

#define TODO(_msg) message(__FILE__ "(" TODOLINE ") : TODO : " _msg)
#define BUG(_msg)  message(__FILE__ "(" TODOLINE ") : BUG : "  _msg)
#define HACK(_msg) message(__FILE__ "(" TODOLINE ") : HACK : " _msg)

#endif // ELVEES_TODO__

