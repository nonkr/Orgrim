### C++的std::thread相对于pthread的优缺点：
* 优点
	1. 简单，易用
    2. 跨平台，pthread只能用在POSIX系统上（其他系统有其独立的thread实现）
    3. 提供了更多高级功能，比如future
    4. 更加C++（跟匿名函数，std::bind，RAII等C++特性更好的集成）
* 缺点
	1. 没有RWlock。有一个类似的shared_mutex，不过它属于C++14,你的编译器很有可能不支持。
    2. 操作线程和Mutex等的API较少。毕竟为了跨平台，只能选取各原生实现的子集。如果你需要设置某些属性，需要通过API调用返回原生平台上的对应对象，再对返回的对象进行操作。