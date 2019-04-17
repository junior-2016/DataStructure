//
// Created by junior on 19-4-15.
//

#ifndef DATASTRUCTURE_DATASTRUCTURE_H
#define DATASTRUCTURE_DATASTRUCTURE_H

#include "MultiArray.h"
#include "List.h"
#include "variant.h"
#include "any.h"
#include "Util.h"

namespace DS {
    template<typename T, size_t N, size_t ...Ns>
    class MultiArray;

    class List;

    void test();
}

#endif //DATASTRUCTURE_DATASTRUCTURE_H

// 对于模板类的代码,一般所有的实现(即定义)和声明都放在头文件里,
// 如果要把实现和声明分开成.cpp和.h,就要在.h文件最后导入实现的.cpp文件.
// 否则就会找不到定义
#include "DataStructure.cpp"
