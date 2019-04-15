//
// Created by junior on 19-4-15.
//

#ifndef DATASTRUCTURE_STACK_H
#define DATASTRUCTURE_STACK_H
//  带混合类型的堆栈
//  引自: https://zhuanlan.zhihu.com/p/20029820
/*  template<typename Allocator> // if need
 *  class Stack {
 *    private:
 *      char* top;
 *      char* end;
 *    public:
 *      template<typename T>
 *      FORCE_INLINE T* push(size_t count = 1){
 *          if( top+sizeof(T)*count >= end) { expand_stack(); }
 *          T * ret = reinterpret_cast<T>(top);
 *          top += sizeof(T)*count; // 开辟所需空间
 *          return ret;
 *      }
 *      FORCE_INLINE template<typename T> T* push(size_t count){...}
 *      FORCE_INLINE template<typename T> T* pop(size_t count=1){..}
 *      FORCE_INLINE template<typename T> T* pop(size_t count){...}
 *      ~Stack(){
 *          //...
 *      }
 *  }
 *  usage:
 *  Stack<Allocator> stack_();
 *  // 泛型push
 *  bool addValue(){
 *      // 下面使用了 placement new 特性, 就是不分配内存，由使用者给予内存空间来构建对象.
 *      // 即:
 *      // new ( ptr //使用者提供的内存空间指针,要确保指针指向的空间可以容纳对应的对象//) construct_func(Args...);
 *      new (stack_.template push<ValueType>()) ValueType(Args ...);
 *      return true;
 *  }
 *  // 一般push
 *  *(stack_.push<int>()) = 1;
 *  *(stack_.push<char>()) = 'c';
 *  .... 考虑到内存对齐,可能需要用 memcpy 把数据拷贝到push()提供的内存地址上.
 *
 *  // 释放:
 *  while(!stack_.empty()) { (stack_.template pop<ValueType>())->~ValueType(); }
 */
#endif //DATASTRUCTURE_STACK_H
