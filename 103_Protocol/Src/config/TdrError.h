/* This file is generated by tdr. */
/* No manual modification is permitted. */

/* creation time: Tue Feb 10 17:59:35 2015 */
/* tdr version: 2.4.21914, build at 20110728 */

#ifndef _TDR_ERROR_H_
#define _TDR_ERROR_H_

namespace tsf4g_tdr
{


class TdrError
{
    public:
        enum ErrorType
        {
            TDR_NO_ERROR = 0,
            TDR_ERR_SHORT_BUF_FOR_WRITE        =  -1,
            TDR_ERR_SHORT_BUF_FOR_READ         =  -2,
            TDR_ERR_STR_LEN_TOO_BIG            =  -3,
            TDR_ERR_STR_LEN_TOO_SMALL          =  -4,
            TDR_ERR_STR_LEN_CONFLICT           =  -5,
            TDR_ERR_MINUS_REFER_VALUE          =  -6,
            TDR_ERR_REFER_SURPASS_COUNT        =  -7,
            TDR_ERR_ARG_POINTER_IS_NULL        =  -8,
            TDR_ERR_CUTVER_TOO_SMALL           =  -9,
            TDR_ERR_CUTVER_CONFILICT           = -10,
            TDR_ERR_PARSE_TDRIP_FAILED         = -11,
            TDR_ERR_INVALID_TDRIP_VALUE        = -12,
            TDR_ERR_INVALID_TDRTIME_VALUE      = -13,
            TDR_ERR_INVALID_TDRDATE_VALUE      = -14,
            TDR_ERR_INVALID_TDRDATETIME_VALUE  = -15,
            TDR_ERR_FUNC_LOCALTIME_FAILED      = -16,
            TDR_ERR_INVALID_HEX_STR_LEN        = -17,
            TDR_ERR_INVALID_HEX_STR_FORMAT     = -18,
            TDR_ERR_NULL_POINTER_PARAMETER     = -19,
            TDR_ERR_NET_CUTVER_INVALID         = -20,
        };

    public:
        static const char* getErrorString(ErrorType errorCode);
};

}
#endif
