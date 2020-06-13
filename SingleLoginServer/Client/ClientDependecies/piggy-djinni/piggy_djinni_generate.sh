base_dir=$(cd "`dirname "$loc"`" && pwd)

#echo $base_dir

# 配置djinni声明文件
piggy_djinni_file="piggy.djinni"
# 配置djinni可执行文件
djinni_executable="$base_dir/../djinni-master/src/run-assume-built"
# 配置输出的java包名
piggy_djinni_java_package="com.faip.piggy.djinni"
# 配置c++的namespace
piggy_native_namespace="ns_piggy_native"

# 配置djinni输出文件目录
piggy_cpp_out="$base_dir/../app/src/main/djinni_generated_src/cpp"
piggy_jni_out="$base_dir/../app/src/main/djinni_generated_src/jni"
piggy_objc_out="$base_dir/../app/src/main/djinni_generated_src/objc"
piggy_java_out="$base_dir/../app/src/main/djinni_generated_src/java/com/faip/piggy/djinni"

# 清理现有文件
rm -r "$base_dir/../app/src/main/djinni_generated_src"

# 执行djinni编译
"$djinni_executable" \
    --idl $piggy_djinni_file \
    --java-out $piggy_java_out \
    --java-class-access-modifier "package" \
    --java-package $piggy_djinni_java_package \
    --java-cpp-exception DbxException \
    --cpp-out $piggy_cpp_out \
    --jni-out $piggy_jni_out \
    --cpp-namespace $piggy_native_namespace \
    --ident-java-field mFooBar \
    --ident-jni-class NativeFooBar \
    --ident-jni-file NativeFooBar
