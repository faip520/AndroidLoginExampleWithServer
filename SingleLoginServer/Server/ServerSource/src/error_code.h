#ifndef ERROR_CODE_INCLUDED
#define ERROR_CODE_INCLUDED

// 简单的错误码映射
#define ERR_CODE_OK 0
#define ERR_CODE_USER_EXIST 10001
#define ERR_CODE_SYSTEM_ERROR 10002
#define ERR_CODE_USER_NAME_EMPTY 10003
#define ERR_CODE_USER_PSW_EMPTY 10004
#define ERR_CODE_USER_PSW_OR_USER_NAME_ERROR 10005
#define ERR_CODE_TOKEN_OUTDATED 10006
#define ERR_CODE_USER_NAME_OR_PASSWORD_MALFORMED 10007
#define ERR_CODE_USER_NAME_TOKEN_MALFORMED 10008

// 错误码，初始化错误 1000开头
#define ERROR_CODE_INIT_GET_SELF_DIR_ERROR 10000001           // 初始化获取自身所在目录失败
// 错误码，注册错误 1001开头
#define ERROR_CODE_REGISTER_SAVE_SALT_FAILED 10010001         // 给用户生成并保存随机盐失败

#endif // ERROR_CODE_INCLUDED