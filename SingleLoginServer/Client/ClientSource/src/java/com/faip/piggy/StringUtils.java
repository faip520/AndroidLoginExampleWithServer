package com.faip.piggy;

import android.text.TextUtils;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

class StringUtils {

    // 判断是否只有字母和数字的正则表达式
    private static final String REGEX_PATTERN_ONLY_NUMBER_AND_LETTER = "[a-zA-Z0-9]*";

    /**
     * 返回String字符串是否只包含字母和数字
     *
     * @param text 待检测字符串
     * @return 如果输入字符串只包含数字和字母，返回true，否则返回false
     */
    static boolean isStringOnlyNumberAndLetter(String text) {
        if (TextUtils.isEmpty(text)) {
            return false;
        }

        Pattern pattern = Pattern.compile(REGEX_PATTERN_ONLY_NUMBER_AND_LETTER);
        Matcher matcher = pattern.matcher(text);

        return matcher.matches();
    }
}
