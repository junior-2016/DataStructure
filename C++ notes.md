#### C++ 模板参数推断和调用:
对于下面的情况:
```c++
template<typename T>
class A{
   A(const T & a){...}    // (1)
   
   template<typename T1>
   A(const T1 & a){...}   // (2)
   
   template<typename T>
   void f1(){}            
   
   template<typename T1>
   void f2(const T1 & t1){}    (3) 
   
   template<typename T1>
   void f2(T1 && t){          (4) 
       other_function(std::forward<T1>(t));
       // if t is lvalue,then t pass to other_function is also lvalue.
       // if t is rvalue,then t pass to other_function is also rvalue.
   }
   
   template<typename T1>
   void f3(int a){}            
   
   template<typename T1>
   T1& f4(){}              
};               
```
我们来看一下对于上面的代码,正确的调用方式:
- A< int > a(1);   => 编译期确定类型 T 为int(不需要推断),调用构造函数(1)
- A< int > b(1.23f); => 编译期确定类型 T 为int(不需要推断),但是构造函数使用的参数
是浮点类型,所以调用的是构造函数(2),需要推断出类型 T1为 float
- a.f1(); => 类型T为int在写代码A< int >...时已经确定,无需推断,直接调用即可.
- string s="str"; a.f2(s); => 调用(3),(3)的参数是const Type&,
既可以接受左值也可以接受右值,而s是左值,因此调用(3).根据传递的参数推断出类型T1是string.注意这里不需要显式
写成a.f2< string >(s),因为类型T1的推断可以根据参数得到.
- a.f2("str"); => 调用(4),因为传递的是右值,虽然(3)也可以接收右值,但是(4)的参数是Type&&,更为匹配,
此时函数(4)中传递给other_function的也是一个右值(利用std::forward)
- string s="str"; a.f2(std::move(s)) => 调用(4),因为使用std::move(s)强制将s从左值变成了
右值,此时函数(4)中传递给other_function的也是一个右值.需要注意s在进行std::move后就
不能使用了,因为强制转为右值意味着它的内存已经不归它管理.
- a.f2(string("str")) => 调用(4),因为a.f2(string("str"))传递的是一个临时构造对象,依旧
可以认为是右值,所谓左值就是存在内存的变量,也就是可以用&取到地址的变量,右值就是不能取到地址的变量,
这里构造的临时对象无法显式地取到地址,所以认为是右值,调用的是函数(4).但需要警惕的是,对于
类似T&&t这样的参数,并且T是需要推断出来的,我们把它叫做通用引用,也就是我们这里函数(4)的情况.
通用引用和右值引用的最大区别是,通用引用的类型是需要推断的,但是右值引用的类型是确定的,
比如 template< typename T > A{ void ff(T&&t);} 但你创建A< int >a时,ff的参数就确定是
int&&t了,此时就是右值引用而非通用引用. 通用引用不仅可以传递右值,其实也可以传递左值(不过我们
这里因为函数(3)的存在,所有的左值传递会被(3)匹配,所以这一点无法体现),所以通用引用T&&t的参数
t在进一步转发到其他函数other_function时,都会默认当做左值变量,调用other_function的左值
版本,如果我们要消除这种情况,让a.f2(string("str"))转发时依旧将临时变量当做右值,就需要
std::forward的帮助. 就上面的代码来说,由于转发给other_function时使用了std::forward,
所以a.f2(string("str"))内调用的other_function()依然是右值版本.
- a.f3< int >(5) => f3调用需要明确类型,因为无法根据参数推断类型 T1
- auto ret = a.f4< int > () => f4调用依旧需要明确类型,同样无法根据参数推断 T1

#### std::enable_if<>
std::enable_if<>是SFINAE(Substitution Failure Is Not An Error)一个体现,
SFINAE就是说模板匹配失败不算错误,可以继续寻找新的匹配,如果匹配都失败了才算错误.
std::enable_if<bool,type>的实现:
```c++
template<bool,typename T>
struct enable_if{};
template<typename T>
struct enable_if<true,T>{ typedef T type; }
```
也就是说当enable_if<bool,type>里的bool常量表达式为true,enable_if才能持有一个type类型,
即enable_if<true,T>::type合法,并且enable_if<true,T>::type代表的类型为T;
如果enable_if<bool,type>里的bool常量表达式为false,那么调用enable<false,T>::type
非法,编译器会直接报错。这一点在实现 variant 控制类型范围的时候就用到了,使用一个模板
is_one_of<T,Ts...>判断T是否在Ts...里,如果是返回true,那么std::enable_if(
is_one_of<T,Ts....>,void)::type就是合法的且type为void,也不会干扰原来的模板函数,
如果T不在Ts...,is_one_of<T,Ts...>返回false,那么std::enable_if<>::type直接非法,
编译器报错.
 
#### std::declval<T>()和decltype()的联合应用
std::declval<T>()会返回一个T的一个右值(如果T是类,你可以认为
放回了T的一个对象,如果T是int,double,char*...这些,可以认为是返回了一个对应的数值),
比如 struct A() {}; std::declval<A>()可以认为返回一个A的对象,
std::declval<int>()可以认为是返回一个int&&,即数值.
但是你不能认为std::declval<T>()可以帮你创建匿名对象,实际上它是需要与decltype一起
使用的,比如decltype(std::declval< std::string >()) str = "string";
或者 struct A { A(const A&){..} int foo() const {return 1;} };
如果你用 decltype(A().foo()) a = 1 是非法的,因为A没有默认的无参构造函数,
而使用 decltype(std::declval<A>().foo()) a = 1 就可以了.


