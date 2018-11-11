#ifndef CCXML_FILE_EXPROT_H_
#define CCXML_FILE_EXPROT_H_

#pragma warning(disable: 4005)

#   if defined( XML_SCRIPT_EXPORTS )
#       define XML_SCRIPT_EXPORTS __declspec( dllexport )
#   else
#       define XML_SCRIPT_EXPORTS __declspec( dllimport )
#   endif


#endif