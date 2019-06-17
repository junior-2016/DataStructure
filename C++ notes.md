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
返回了T的一个对象,如果T是int,double,char*...这些,可以认为是返回了一个对应的数值),
比如 struct A() {}; std::declval<A>()可以认为返回一个A的对象,
std::declval<int>()可以认为是返回一个int&&,即数值.
但是你不能认为std::declval<T>()可以帮你创建匿名对象,实际上它是需要与decltype一起
使用的,比如decltype(std::declval< std::string >()) str = "string";
或者 struct A { A(const A&){..} int foo() const {return 1;} };
如果你用 decltype(A().foo()) a = 1 是非法的,因为A没有默认的无参构造函数,
而使用 decltype(std::declval< A >().foo()) a = 1 就可以了.

#### std::move纠正
std::move(T&)仅仅是将类型T&变成了T&&，以便于调用右值版本的构造器,但是移动后
的原对象依然可以使用和析构. 至于调用的右值版本构造器是否需要将原对象储存的内容move过来,还是只
move一部分,都取决于用户自己的实现,不是由std::move来决定的.

#### std ::this_thread ::yield() 和 std ::this_thread ::sleep_for(time)
yield和sleep都是暂时交出当前线程的时间片,但是两者有本质的差别:
sleep是固定好了需要交出的时间片(休眠范围),而且必须让出cpu时间片;
而yield更像是一种尝试让出时间片的操作,如果有其他需要利用cpu的线程当然就让出时间片了,
如果没有则依然会占用cpu(只有当前线程在运行的时候),另外yield让出的时间片大小也是不固定的,
cpu什么时候重新调度当前线程也是不确定的。yield的一个主要应用是:在循环判断
或者循环获取一个状态的时候,不去过多占用cpu的时间,而是隔一段时间再判断一次,
即: while(is_get_state()) std::this_thread ::yield(); 
 
#### using namespace std::chrono_literals;
这个名称空间可以提供一个时间单位符号的索引,在调用sleep_for或者其他
需要时间范围限制的锁操作,可以直接使用常用的时间单位,比如s,ms等.

#### C++ condition variable 的一些笔记
- cv.notify_all() 或者 cv.notify_one() 之前必须解锁.
最好是用lock_guard<Mutex>或者unique_lock<Mutex>进行自动解锁,即: 
```c++
{ 
    lock_guard<Mutex> lock; // 或者unique_lock<Mutex> lock;
    ... ;
}(离开{...}scope后自动解锁) 
cv.notify_all()
```
- void cv.wait(std::unique_lock<Mutex>&lock);  
在cv.wait之前已经持有互斥量的锁lock,调用cv.wait后,将锁释放(通过lock.unlock()),
同时堵塞当前线程,所谓堵塞当前线程,可以理解为将当前线程的寄存器(临时变量,以及唤醒线程后需要执行
的代码首地址等等)保存在栈里,也就是保存现场,然后将当前线程的id号塞进等待队列,
一旦有其他线程调用cv.notify_all()或者cv.notify_one(),等待线程就会唤醒.
如果是cv.notify_all(), 那么所有在等待队列的线程都会唤醒,所有等待的线程都会调用
自己持有的lock对象的lock()函数,如果其中一个线程拿到了锁,那么互斥量就是它的了,其他线程
只能堵塞在自己lock对象的lock()函数上,等拿到锁的线程释放锁以后,剩下的线程继续抢夺互斥量的
锁,直到所有的线程都经历这么一段[拿到锁 -> 其他操作 -> 释放锁]的过程后,程序就结束了; 
如果是cv.notify_one(),就简单很多,操作系统会随机从等待队列选择某个线程,把它唤醒,
它当然就会调用lock对象的lock()函数持有锁,然后接着其他操作,最后释放锁.(
为什么是操作系统来做这件事,因为C++其实不过是把各个OS自带的原生线程模型封装后提供一个
统一的接口而已,内部操作包括线程调度等都是OS在做,并非由语言自己实现).


- void cv.wait(std::unique_lock<Mutex>&lock,Pred_func pred)  
内部实现:
```c++
while(!pred()){
    wait(lock);
}
```
这里和前面的cv.wait不同在于加入了bool函数pred的判断,也就是说,如果其他线程
调用notify_*将当前线程唤醒,并且当前线程调用lock对象的lock()函数成功获得了互斥量的锁,
它还需要判断一下pred()函数返回的是不是true,如果不是,那么就调用wait重新进入等待状态,
释放获得的锁,同时进入堵塞状态... 直到当前线程被notify_*唤醒并且pred()也是返回true的时候,
才能继续执行当前线程剩下的代码.

- cv_status cv.wait_until(std::unique_lock<Mutex>&lock,time_point)  
这个和前面cv.wait的不同在于:除了可以通过notify_*唤醒线程外,还可以设置一个
时间点time_point,如果等待时间超过这个时间点,线程就会自动唤醒(不需要notify_*唤醒). 
这里有几点需要讨论一下:如果等待线程在time_point到来之前就被其他线程通过notify_*唤醒了,
那么它会调用线程lock对象的lock函数尝试锁住互斥量,如果失败就会一直堵塞在lock函数那里,
直到锁住互斥量为止,最后返回cv_status::no_timeout; 如果线程一直等到time_point
那一刻还没有收到notify, 就会选择自动唤醒,同样的,线程会调用lock对象的lock函数尝试
锁住互斥量,如果不成功就堵塞住,直到成功拿到互斥量的锁为止,最后返回cv::status::time_out.

- bool cv.wait_until(std::unique_lock<Mutex>&lock,time_point,Pred_func pred)  
内部实现:
```c++
while (!pred()) {
    if (wait_until(lock, time_point) == std::cv_status::timeout) {
        return pred();
    }
}
return true;
```
加入pred()函数判断,跟上一个不同在于,如果时间点到来之前,等待线程被其他线程的notify
唤醒,那么等待线程尝试锁住互斥量(如果不成功就堵塞),成功锁住后返回cv_status::no_timeout,
此时上面的代码中if(wait_until...)判断失败,进入while()判断,如果判断结果pred()是true,
那么线程就直接返回true了; 如果pred()是false,那么线程会再次调用wait_until进入等待状态,
同时解锁自己得到的互斥量. 注意就算再次调用wait_until,时间点设置和之前还是一样的.
如果一直到time_point时刻(之前的notify都没有成功让线程返回true),那么线程会自动唤醒,
调用lock对象的lock()函数得到互斥量的锁(不成功就堵塞),同时返回cv_status::timeout,
此时if(wait until...)判断成功,线程返回pred()函数此时的状态.

- cv.wait_for  
cv.wait_for 和 cv_wait_until是差不多的,只是从时间点设置(time_point)变成了
从当前开始的一段时间内(time_interval),通过 now_time() + time_interval 就可以把
cv.wait_for变成cv.wait_until,所以这里就不细讲了.

- 常用工具函数:
using namespace std::chrono_literals; => 可以使用 s/ms/.. 作为单位
std::chrono ::system_clock ::now();   => 当前时间点
   
#### C++ std::bind
```
struct multiple{
   inline int operator()(int a,int times){
       return a*times; 
   }
}
int main(){
    using namespace std;
    using namespace std::placeholders;
    auto f = bind(multiple(),_2,10);
    // 说明一下: std::bind(functional_object, Args&& ...)
    // 将一个函数对象与某些数值(实参)绑定成生成一个可以运行的函数f,
    // 这些实参对应函数对象执行所需要的形参,第一个实参 _2 对应形参a,第二个实参10对应形参times.
    // 这些实参可以是具体的数值,可以是通过std::ref(),std::cref()转换的引用,
    // 也可以是placeholders. placeholders的形式是: _N, 这是指bind生成的f
    // 在后面调用时传递给f的第N个实参,假如后面调用f是这样: f(1,2,3,4,../*其他参数*/);
    // 那么这里的_2就是f的第二个实参2.
    cout<<f(1,2,3,4); // => 输出20(_2为2,结果是2*10=20)
    // PS:如果前面代码是: bind(functional_object,_10,10);但f实际调用时是
    // f(1,2,3,4),只有4个实参,不存在第10个实参_10,那么编译器就会报错.. 
}
```
更复杂的binder例子
```
void remove_example(){
    vector<int> v1 = {1,2,3,4,5,6,7,8,9};
    replace_if(v1.begin(),
              v1.end(),
              bind(logical_and<bool>(),
          /* f= */ bind(greater_equal<int>(),_1,3), 
             // 内部调用 f(vector_element) => greater_equal(vector_element,3) => return e>=3;
          /* f= */ bind(less_equal<int>(),_1,6))   
             // 内部调用 f(vector_element) => less_equal(vector_element,6) => return e<=6;
              ,10);
    for_each(v1.begin(),v1.end(),[](int a){cout<<a<<" ";});
    // 输出 1 2 10 10 10 10 7 8 9 
}
```
std::bind对类函数和类成员的绑定 : 对于类函数,是可以直接绑定后调用的;
对于类成员,只能绑定后对其访问,而不能绑定后修改成员,如果要修改成员就写一个类函数来修改,
然后用bind绑定这个类函数.
```c++
struct S{
    void func(int){}
    double d = 1.0;
};
int main(){
    using namespace std::placeholders;
    S s;
    auto f = bind(&S::func, &s, _1);
    f(1);
    auto f1 = bind(&S::d, _1);
    std::cout<<f1(s)<<"\n";                      // => 1.0
    std::cout<<f1(std::make_shared<S>(s))<<"\n"; // => 1.0
}
```

#### std::result_of<F(Args...)> ::type && decltype(f(args...)) && 
std::invoke_result_t<F,Args...>的等价用法 
std::result_of<F(Args...)>::type 返回一个函数签名F(Args...)的返回值类型；  
decltype(f(args...)) 返回一个具体函数f(args...)的返回值类型.  
std::invoke_result_t<F,Args...>是C++17引入的.
下面是两个example,分别针对函数对象和lambda表达式给出三者的等价用法:
```c++
struct S {
    bool operator()(int a, char c) { return false; }
};
void example1() {
    decltype(S()(1, 'c')) s1 = true;
    std::result_of<S(int, char)>::type s2 = true;
    std::invoke_result_t<S,int,char> s3 = true;
    std::cout << std::boolalpha << s1 << " " << s2 <<" "<<s3<< "\n"; // 输出 true true true
}
void example2() {
    auto f = [](int a) { return a; };
    decltype(f(1)) a1 = 1;
    std::result_of<decltype(f)(int)>::type a2 = 1;
    std::invoke_result_t<decltype(f),int> a3 = 1;
    std::cout << a1 << " " << a2 <<" "<<a3<< "\n"; // 输出 1 1 1
}
```
通过模板来反映这三者直接的等价性:
```c++
template<typename F, typename ... Args>
void example(F &&f, Args &&... args) {
    using return_type_1 = typename std::result_of<F(Args...)>::type;
    using return_type_2 = decltype(f(args...));
    using return_type_3 = typename std::invoke_result_t<F,Args...>;
    std::cout << std::boolalpha << 
         std::is_same<return_type_1, return_type_2>::value << " " <<
         std::is_same<return_type_1, return_type_3>::value << " " <<
         std::is_same<return_type_2, return_type_3>::value <<"\n";
}
auto main()->int{
   example1();
   example2();
   example(S(), 1, 'c'); // 这里的S还是前面那个结构体,输出 true true true
   example([](int, char) { return std::string("str"); }, 1, 'c'); // 输出 true true true
}
```

#### C++线程池的一些技巧
通过std::bind将任意函数包装成 std::function<return_type()>,以及更进一步将任意函数
包装成 std::function<void()>
```c++
template<typename F,typename ... Args>
future submit_task(F&& f, Args&& ... args){
    using return_type = decltype(f(args...));//也可以用std::result_of<F(Args...)>::type
    
    std::function<return_type()> func = std::bind(std::forward<F>(f),std::forward<Args>(args)...)
    // 这里将函数对象(或者lambda表达式对象)与已经提供的参数包装起来,变成一个新的函数func,
    // 此时的func已经不需要任何参数(调用它只需要auto ret = func()即可),所以它的函数签名是<return_type()>.
    // 注意在bind函数对象f和参数args...时需要先用std::forward处理一下,
    // 因为这里的F&&和Args&&..都是通用引用,既可以接收左值又可以接收右值,需要forward实现正确的转发.
    
    // 有了包装后的函数,我们需要把它打包在一个packaged_task对象指针里.
    // packaged_task类似与std::promise,可以提供一个future对象,
    // 我们可以在当前线程获取task的future对象,然后调用future对象的get()方法,
    // get()方法如果没有得到值就会堵塞在那里;
    // 同时我们把task进一步包装为<void()>类型的函数存在队列里,然后让我们的线程池
    // 分配一个线程去执行队列里的task,一旦task执行完成,我们当前线程的future.get()方法就不再堵塞了,
    // 可以得到正确的返回值.这里为什么要进一步把task包装成<void()>的类型呢?第一个
    // 原因是统一task队列的元素类型,task队列里面每一个任务都是void()函数,我们就只需要
    // 一个数据结构 std::queue<std::function<void()>> 即可,如果不这样做,
    // 不同task签名中return_type完全不一样,就很难构造队列了; 第二个原因是,
    // 我们将task保存在队列里,只是为了让线程池分配线程去执行队列里的task,而不需要
    // 线程池的线程得到task的返回值,返回值是在当前线程获取future后调用get()得到的,
    // 线程池的线程只负责执行task就可以了,从这一点来说,包装成<void()>才是正确的.
    auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(func);
    std::function<void()> wrap_task = [task_ptr](){ (*task_ptr)(); }; // 进一步包装为<void()>
    task_queue.enqueue(wrap_task); // 任务入队列
    exec_task_condition_variable.notify_one(); // 通过条件变量的notify_one()唤醒一个线程去执行task.
    return task_ptr->get_future(); // 返回future供当前线程调用get()方法获得返回值
}
auto main()->int{
    auto future = summit_task([](int a){return a;},1);
    std::cout<< future.get()<< "\n";
}
```

#### lambda && functional object && std::bind 三者的等价用法
```c++
struct S {
    bool operator()(int a) const { return false; }
};
template <typename F>
void func(F f){ std::cout<<std::boolalpha<< f(1)<<"\n";}
auto main() -> int {
    func(S()); 
    // S()创建函数对象s,调用时s(1)等价于s.operator()(1)
    
    func([](int a){return false;}); 
    // 创建lambda表达式[](..){...},调用时为[](..){...}(1)
    
    func(std::bind(S(),std::placeholders::_1)); 
    // std::bind将函数对象S()与某些参数绑定生成f,后面调用f(1),
    // 则placeholders::_1为1,实际调用是 (S())(1) => (S()).operator()(1)
    return 0;
}
```

#### std::vector< std::thread>错误添加方法
```c++
std::thread th(func);
// vector.push_back(th); => 错误,因为thread的左值拷贝器被删除了.
vector.push_back(std::move(th)); => 正确
vector.push_back(std::thread(func)); => 正确
vector.emplace_back(function_object/function_pointer/lambda-expr);=>正确 

```

#### c++ 拷贝构造函数的极致写法(见[https://www.zhihu.com/question/30196513])  
```c++
struct S {
    std::string s1, s2;

    template<typename S1, typename S2 = std::string,
            typename = std::enable_if_t<std::is_convertible_v<S1, std::string>>>
    explicit S(S1 &&s1, S2 &&s2 = "default_string") {
        this->s1 = std::forward<S1>(s1);
        this->s2 = std::forward<S2>(s2);
    }
};
```

#### auto和decltype(auto)的区别:  
auto是一种decay的写法,即它会去掉所给元素类型的引用和const修饰等,而decltype(auto)
不会.实际代码中更推荐decltype(auto)的写法.例如:
```c++
const int & i = 1;
auto i1 = i;           // i1 类型为 int
const auto & i2 = i;   // i2 类型为 const int &
decltype(auto) i3 = i; // i3 类型为 const int &
```
用一个函数返回值的例子做测试(从中可以看出decltype(auto)作为函数返回类型更精确):
```c++
auto f(const int &i) {
    return i;
}

const auto &f_(const int &i) {
    return i;
}

decltype(auto) f__(const int &i) {
    return i;
}

void test() {
    std::cout << std::boolalpha << std::is_same<int, decltype(f(1))>::value << "\n";
    std::cout << std::boolalpha << std::is_same<const int &, decltype(f(1))>::value << "\n";
    std::cout << std::boolalpha << std::is_same<const int &, decltype(f_(1))>::value << "\n";
    std::cout << std::boolalpha << std::is_same<const int &, decltype(f__(1))>::value << "\n";
}
输出:
true
false
true
true
```
用模板包装任意一个函数的写法,其返回值也应该用decltype(auto)更好:
```c++
template<typename F,typename ... Args)
decltype(auto) Function(F&& func,Args&& ... args){
    return func(std::forward<Args>(args)...);
    // 或 return std::bind(func,std::forward<Args>(args)...)();
}
```

#### std::extent<array_type,dim=0>
std::extent可以打印数组类型的维度,比如 int x \[3\]\[4\]; 
std::extent< decltype(x),0 > 为 3; std::extent< decltype(x),1 > 为 4.
如果std::extent< array_type ,dim >没有提供dim,默认dim=0.
更具体的例子:
```c++
 std::cout << std::extent<int[3]>::value << '\n'; // < 默认维度为0,输出3
    std::cout << std::extent<int[3][4], 0>::value << '\n'; // 输出3
    std::cout << std::extent<int[3][4], 1>::value << '\n'; // 输出4
    std::cout << std::extent<int[3][4], 2>::value << '\n'; // 输出0,因为超过数组维度
    std::cout << std::extent<int[]>::value << '\n'; // 输出0,因为是空数组
 
    const auto ext = std::extent<int[9]>{};
    std::cout << ext << '\n'; // < 隐式转换到 std::size_t,输出9
 
    const int ints[] = {1,2,3,4};
    std::cout << std::extent<decltype(ints)>::value << '\n'; // < 数组大小,输出4 
```

#### noexcept 关键字的使用场景
noexcept只有在你明确这个函数不会抛出异常的情况下,才可以显式写noexcept(
或者noexcept(true)).如果这个函数在实现的过程中会抛出异常或者有类似抛出
异常的行为(比如C语言的pthread_exit,见[https://www.zhihu.com/question/307509340/answer/563442562]
),就不要加noexcept关键字,否则在noexcept函数抛出异常的话,
编译器会调用std::terminal()直接崩溃程序.
