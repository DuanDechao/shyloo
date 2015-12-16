/**
* 一个简单的委托类.
*
* 通过模板+接口继承的方式来简化委托, 使用方法见最后
* 方法的返回值固定是void，如果要返回数据，请用引用参数
*/
#ifndef __SL_DETEGATE_H__
#define __SL_DETEGATE_H__

#include <new>

#define SL_DECLARE_PARAMS(...) __VA_ARGS__
#define SL_DECLARE_ARGS(...) __VA_ARGS__

//0个参数的委托
#define SL_DELEGATE0(name) \
    SL_DECLARE_DELEGATE(name, SL_DECLARE_PARAMS(void), )

//1个参数的委托
#define SL_DELEGATE1(name, p1) \
    SL_DECLARE_DELEGATE( \
    name, \
    SL_DECLARE_PARAMS(p1 a), \
    SL_DECLARE_ARGS(a))

//2个参数的委托
#define SL_DELEGATE2(name, p1, p2) \
    SL_DECLARE_DELEGATE( \
    name, \
    SL_DECLARE_PARAMS(p1 a, p2 b), \
    SL_DECLARE_ARGS(a, b))

//3个参数的委托
#define SL_DELEGATE3(name, p1, p2, p3) \
    SL_DECLARE_DELEGATE( \
    name, \
    SL_DECLARE_PARAMS(p1 a, p2 b, p3 c), \
    SL_DECLARE_ARGS(a, b, c))

//4个参数的委托
#define SL_DELEGATE4(name, p1, p2, p3, p4) \
    SL_DECLARE_DELEGATE( \
    name, \
    SL_DECLARE_PARAMS(p1 a, p2 b, p3 c, p4 d), \
    SL_DECLARE_ARGS(a, b, c, d))

//5个参数的委托
#define SL_DELEGATE5(name, p1, p2, p3, p4, p5) \
    SL_DECLARE_DELEGATE( \
    name, \
    SL_DECLARE_PARAMS(p1 a, p2 b, p3 c, p4 d, p5 e), \
    SL_DECLARE_ARGS(a, b, c, d, e))

//6个参数的委托
#define SL_DELEGATE6(name, p1, p2, p3, p4, p5, p6) \
    SL_DECLARE_DELEGATE( \
    name, \
    SL_DECLARE_PARAMS(p1 a, p2 b, p3 c, p4 d, p5 e, p6 f), \
    SL_DECLARE_ARGS(a, b, c, d, e, f))

//7个参数的委托
#define SL_DELEGATE7(name, p1, p2, p3, p4, p5, p6, p7) \
    SL_DECLARE_DELEGATE( \
    name, \
    SL_DECLARE_PARAMS(p1 a, p2 b, p3 c, p4 d, p5 e, p6 f, p7 g), \
    SL_DECLARE_ARGS(a, b, c, d, e, f, g))

//8个参数的委托
#define SL_DELEGATE8(name, p1, p2, p3, p4, p5, p6, p7, p8) \
    SL_DECLARE_DELEGATE( \
    name, \
    SL_DECLARE_PARAMS(p1 a, p2 b, p3 c, p4 d, p5 e, p6 f, p7 g, p8 h), \
    SL_DECLARE_ARGS(a, b, c, d, e, f, g, h))

#define SL_DECLARE_DELEGATE(name, params, args)                                         \
namespace lz                                                                            \
{                                                                                       \
    namespace detail                                                                    \
    {                                                                                   \
        class CDelegateBase##name                                                       \
        {                                                                               \
        public:                                                                         \
            virtual ~CDelegateBase##name() {}                                           \
            virtual void Invoke(params) = 0;                                            \
        };                                                                              \
        template<typename T>                                                            \
        class CDelegateImpl##name : public CDelegateBase##name                          \
        {                                                                               \
        public:                                                                         \
            CDelegateImpl##name(T* pType, void(T::*pFunc)(params)):                     \
                m_pType(pType), m_pFunc(pFunc) {}                                       \
            void Invoke(params) { (m_pType->*m_pFunc)(args); }                          \
        private:                                                                        \
            T* m_pType;                                                                 \
            void (T::*m_pFunc)(params);                                                 \
        };                                                                              \
        template<>                                                                      \
        class CDelegateImpl##name<void> : public CDelegateBase##name                    \
        {                                                                               \
        public:                                                                         \
            CDelegateImpl##name(void*, void(*pFunc)(params)) : m_pFunc(pFunc) {}        \
            void Invoke(params) { return (*m_pFunc)(args); }                            \
        private:                                                                        \
            void (*m_pFunc)(params);                                                    \
        };                                                                              \
    }                                                                                   \
    class name                                                                          \
    {                                                                                   \
    public:                                                                             \
        name() : m_empty(true), m_static(false) {}                                      \
        template<typename X, typename Y>                                                \
        void bind(X* pType, void(Y::*pFunc)(params))                                    \
        {                                                                               \
            new (m_buf) detail::CDelegateImpl##name<X>(pType, pFunc);                   \
            m_empty = false;                                                            \
            m_static = false;                                                           \
        }                                                                               \
        void bind(void(*pFunc)(params))                                                 \
        {                                                                               \
            new (m_buf) detail::CDelegateImpl##name<void>(NULL, pFunc);                 \
            m_empty = false;                                                            \
            m_static = true;                                                            \
        }                                                                               \
        void operator()(params)                                                         \
        {                                                                               \
            if (!m_empty)                                                               \
            {                                                                           \
                ((detail::CDelegateBase##name *) m_buf)->Invoke(args);                  \
            }                                                                           \
        }                                                                               \
        bool isEmpty() const { return m_empty; }                                        \
        bool isStatic() const { return m_static; }                                      \
    protected:                                                                          \
        bool m_empty;                                                                   \
        bool m_static;                                                                  \
        char m_buf[32];                                                                 \
    };                                                                                  \
}
#endif

/**  sample **

#include <stdio.h>
#include "lzdelegate2.h"

class SomeClass
{
public:
    virtual void some_member_func(int a, char * b)
    {
        printf("In SomeClass: a=%d b=%s\n", a, b);
    }
};

class DerivedClass : public SomeClass
{
public:
    virtual void some_member_func(int a, char * b)
    {
        printf("In DerivedClass: a=%d b=%s\n", a, b);
    };
};

void test_func(int a, char* b)
{
    printf("In test_func: a=%d b=%s\n", a, b);
}

SL_DELEGATE2(CTestDelegate, int, char*);

int main()
{
    lz::CTestDelegate w1, w2, w3;

    DerivedClass a;
    w1.bind(&a, &DerivedClass::some_member_func);
    w1(1, NULL);

    w2 = w1;
    w2(2, NULL);

    w3.bind(test_func);
    w3(3, NULL);

    return 0;
}

*/
