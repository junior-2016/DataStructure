//
// Created by junior on 19-4-15.
//

#ifndef DATASTRUCTURE_TEST_H
#define DATASTRUCTURE_TEST_H

#include "MultiArray.h"
#include "List.h"
#include "variant.h"
#include "any.h"
#include "ThreadPool.h"
#include "KdTree.h"
#include "Util.h"

namespace DS {
    void test();
}

// 对于模板类的代码,一般所有的实现(即定义)和声明都放在头文件里,
// 如果要把实现和声明分开为两个文件,那就要分成 xxx.h(声明)以及xxx.hpp(实现)
// 或者两个都是.h文件,但是声明文件为xxx.h,实现文件为xxx_impl.h.
// 注意两个文件都要加上: #ifnedf .. #define .. #endif(以防止头文件重复包含),
// 因为本质上这两个声明和实现文件都是头文件
#include "TestImpl.h"

#endif //DATASTRUCTURE_TEST_H
