#ifndef			___DEF_H_CRASHRPT_FACE_EXP_CREATE_20111126_1428_DEF_H__
#define			___DEF_H_CRASHRPT_FACE_EXP_CREATE_20111126_1428_DEF_H__


#ifdef CRASHRPT_FACE_EXPORTS
#   define CRASHRPT_FACE_DEF __declspec(dllexport)
#else
#   define CRASHRPT_FACE_DEF __declspec(dllimport)
#endif

extern "C" CRASHRPT_FACE_DEF void CrashRptFace();



#endif