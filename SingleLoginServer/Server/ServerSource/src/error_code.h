#ifndef ERROR_CODE_INCLUDED
#define ERROR_CODE_INCLUDED

// 错误码映射
#define ERR_CODE_OK 0                                         // 成功的返回码都是0
#define ERR_CODE_USER_EXIST 10001                             // 注册相关，用户已经存在
#define ERR_CODE_SYSTEM_ERROR 10002                           // 注册相关，发生系统错误
#define ERR_CODE_USER_NAME_EMPTY 10003                        // 注册相关，用户名为空
#define ERR_CODE_USER_PSW_EMPTY 10004                         // 注册相关，用户密码为空

#define ERR_CODE_USER_PSW_OR_USER_NAME_ERROR 10005            // 登录相关，用户名或者密码错误

#define ERR_CODE_TOKEN_OUTDATED 10006                         // 登录token查询相关，token过期

#define ERR_CODE_USER_NAME_OR_PASSWORD_MALFORMED 10007        // 通用错误相关，用户名或者密码格式不对
#define ERR_CODE_USER_NAME_TOKEN_MALFORMED 10008              // 通用错误相关，token格式错误

// 错误码，初始化错误 1000开头
#define ERROR_CODE_INIT_GET_SELF_DIR_ERROR 10000001           // 初始化获取自身所在目录失败
// 错误码，注册错误 1001开头
#define ERROR_CODE_REGISTER_SAVE_SALT_FAILED 10010001         // 给用户生成并保存随机盐失败

#endif // ERROR_CODE_INCLUDED