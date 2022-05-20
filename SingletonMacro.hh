/*
copyright(c) 2020 Arty (arty.quantum)       
*/
#define __SINGLETON__(CLASS)                  \
private:                                      \
    CLASS() = default;                        \
    static auto &getInstance()                \
    {                                         \
        static CLASS instance;                \
        return instance;                      \
    }                                         \
                                              \
public:                                       \
    CLASS(const CLASS &) = delete;            \
    CLASS &operator=(const CLASS &) = delete; \
    CLASS(CLASS &&) = delete;                 \
    CLASS &operator=(CLASS &&) = delete;      \
    static CLASS *getInstancePtr()            \
    {                                         \
        return &(getInstance());              \
    }

#define __SINGLETON_I__(CLASS, InitFunction)  \
private:                                      \
    CLASS() = default;                        \
    static auto &getInstance()                \
    {                                         \
        static CLASS instance;                \
        instance.InitFunction();              \
        return instance;                      \
    }                                         \
                                              \
public:                                       \
    CLASS(const CLASS &) = delete;            \
    CLASS &operator=(const CLASS &) = delete; \
    CLASS(CLASS &&) = delete;                 \
    CLASS &operator=(CLASS &&) = delete;      \
    static CLASS *getInstancePtr()            \
    {                                         \
        return &(getInstance());              \
    }

/*
Usage: 
class Foo{
  __SINGLETON(Foo);
};
Foo* foo = Foo::getInstancePtr();
-------------------------------

class Foo{
  __SINGLETON_I(Foo, initFunction)
 public: 
    initFunction()
    {
      // any initialization etc. 
    }
};

Foo *foo = Foo::getInstancePtr();
*/
