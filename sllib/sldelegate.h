///ί�м�����ģ��
/**
 * ���ﶨ����һ��ί�кͺ������󡣶������������10����
 * ί��û�з���ֵ������ͨ�����ò������ء����ί�ж���û�ж��壬�򲻻���ã�ֱ�ӷ��ء������׳��κ��쳣��
 *
 * ���������������û��ָ��ĺ��������õ�ʱ�򣬻��׳�ָ��Խ����ʡ�ʹ�õ�ʱ����ע�⡣
 *
 * ��ʹ�ö��̳еĺ���ί�У������this���Ե�����Ϊ�˽��������⡣����ʹ���˿�Դ��FastDelegate���滻�������е�
 *
 * ���ڿƼ� 2010-2011 ��Ȩ����
 *
 * @file lzdelegate.h
 * @author rex
 * @version 1.1
 * @date 2011-03-09
 * company ���ڿƼ�
 */
/**********************************************************************
    �����б�  //��Ҫ��������
    1��ί�ж���ģ��
    2����������ģ��

    ��ʷ:       //��Ҫ�޸ļ�¼
    [author]    [time]      [version]       [descriptor]
    rex     2011-03-09  1.0             ������ʼ�ļ�
    rex     2011-05-04  1.1             ����FastDelegate
***********************************************************************/

#ifndef _SL_DELEGATE_H__
#define _SL_DELEGATE_H__
#include "slconfig.h"
#include "sltype.h"
#include "FastDelegate.h"
namespace sl
{
    /* FastDelegate Sample
     @code
        class A
        {
        public:
            void test()
            {
                cout<<"hello A"<<endl;
            }
            void test1(int a_1, int a_2)
            {
                cout<<"hello A:"<<a_1<<","<<a_2<<endl;
            }
        };   
        int main(int argc, char * argv[])
        {
            A a;
            //��ͨ�﷨ί��
            fastdelegate::FastDelegate0<> f;
            fastdelegate::FastDelegate2<int,int> f1;
            //���﷨ί��
            fastdelegate::FastDelegate<void()> f2;
            fastdelegate::FastDelegate<void(int,int)> f3;

            f.bind(&a,&A::test);
            f1.bind(&a,&A::test1);
            f2.bind(&a,&A::test);
            f3.bind(&a,&A::test1);

            if (!f.empty() )
            {
                f();
            }
            if (!f1.empty() )
            {
                f1(1,2);
            }
            return 0;
        }
     @endcode
     */
    ///���¼�����
    class CNULL_Event
    {
    private:
        CNULL_Event(){}
    };
    ///ί�к���ָ�����Ͷ���
    template<class ClassType, 
    class P1 = CNULL_Event, class P2 = CNULL_Event, class P3 = CNULL_Event, 
    class P4 = CNULL_Event, class P5 = CNULL_Event, class P6 = CNULL_Event, 
    class P7 = CNULL_Event, class P8 = CNULL_Event, class P9 = CNULL_Event, 
    class P10 = CNULL_Event>
    struct SClassFunctionDefine
    {
        typedef void (ClassType::*F0)();
        typedef void (ClassType::*F1)(P1);
        typedef void (ClassType::*F2)(P1,P2);
        typedef void (ClassType::*F3)(P1,P2,P3);
        typedef void (ClassType::*F4)(P1,P2,P3,P4);
        typedef void (ClassType::*F5)(P1,P2,P3,P4,P5);
        typedef void (ClassType::*F6)(P1,P2,P3,P4,P5,P6);
        typedef void (ClassType::*F7)(P1,P2,P3,P4,P5,P6,P7);
        typedef void (ClassType::*F8)(P1,P2,P3,P4,P5,P6,P7,P8);
        typedef void (ClassType::*F9)(P1,P2,P3,P4,P5,P6,P7,P8,P9);
        typedef void (ClassType::*F10)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10);
    };
    ///����ָ�����Ͷ���
    template<class R = void, 
    class P1 = CNULL_Event, class P2 = CNULL_Event, class P3 = CNULL_Event, 
    class P4 = CNULL_Event, class P5 = CNULL_Event, class P6 = CNULL_Event, 
    class P7 = CNULL_Event, class P8 = CNULL_Event, class P9 = CNULL_Event, 
    class P10 = CNULL_Event>
    struct SFunctionDefine
    {
        typedef R (*F0)();
        typedef R (*F1)(P1);
        typedef R (*F2)(P1,P2);
        typedef R (*F3)(P1,P2,P3);
        typedef R (*F4)(P1,P2,P3,P4);
        typedef R (*F5)(P1,P2,P3,P4,P5);
        typedef R (*F6)(P1,P2,P3,P4,P5,P6);
        typedef R (*F7)(P1,P2,P3,P4,P5,P6,P7);
        typedef R (*F8)(P1,P2,P3,P4,P5,P6,P7,P8);
        typedef R (*F9)(P1,P2,P3,P4,P5,P6,P7,P8,P9);
        typedef R (*F10)(P1,P2,P3,P4,P5,P6,P7,P8,P9,P10);
    };
    ///�޲�������ָ��
    template<class R>
    class CFunction0
    {
    public:
        typedef typename SFunctionDefine<R>::F0 F; 
        typedef CFunction0<R> ThisClass;
    public:
        CFunction0()
            :m_Function(NULL)
        {}
        CFunction0(F paramF)
            :m_Function(paramF)
        {}
        ThisClass & operator = (const ThisClass & paramF)
        {
            m_Function = paramF.m_Function;
            return *this;
        }
        ThisClass & operator = (F paramF)
        {
            m_Function = paramF;
            return *this;
        }
        R operator()() const
        {
            return m_Function();
        }
        R Invoke() const
        {
            return m_Function();
        }
        bool isBinded() const
        {
            return m_Function != NULL;
        }
        void Bind(F paramF)
        {
            m_Function = paramF;
        }
        void UnBind()
        {
            m_Function = NULL;
        }
    private:
        F m_Function;
    };

    ///һ��������ָ��
    template<class R,class P1>
    class CFunction1
    {
    public:
        typedef typename SFunctionDefine<R,P1>::F1 F; 
        typedef CFunction1<R,P1> ThisClass;
    public:
        CFunction1()
            :m_Function(NULL)
        {}
        CFunction1(F paramF)
            :m_Function(paramF)
        {}
        ThisClass & operator = (const ThisClass & paramF)
        {
            m_Function = paramF.m_Function;
            return *this;
        }
        ThisClass & operator = (F paramF)
        {
            m_Function = paramF;
            return *this;
        }
        R operator()(P1 paramP1) const
        {
            return m_Function(paramP1);
        }
        R Invoke(P1 paramP1) const
        {
            return m_Function(paramP1);
        }
        bool isBinded() const
        {
            return m_Function != NULL;
        }
        void Bind(F paramF)
        {
            m_Function = paramF;
        }
        void UnBind()
        {
            m_Function = NULL;
        }
    private:
        F m_Function;
    };
    ///����������ָ��
    template<class R,class P1,class P2>
    class CFunction2
    {
    public:
        typedef typename SFunctionDefine<R,P1,P2>::F2 F; 
        typedef CFunction2<R,P1,P2> ThisClass;
    public:
        CFunction2()
            :m_Function(NULL)
        {}
        CFunction2(F paramF)
            :m_Function(paramF)
        {}
        ThisClass & operator = (const ThisClass & paramF)
        {
            m_Function = paramF.m_Function;
            return *this;
        }
        ThisClass & operator = (F paramF)
        {
            m_Function = paramF;
            return *this;
        }
        R operator()(P1 paramP1,P2 paramP2) const
        {
            return m_Function(paramP1,paramP2);
        }
        R Invoke(P1 paramP1,P2 paramP2) const
        {
            return m_Function(paramP1,paramP2);
        }
        bool isBinded() const
        {
            return m_Function != NULL;
        }
        void Bind(F paramF)
        {
            m_Function = paramF;
        }
        void UnBind()
        {
            m_Function = NULL;
        }
    private:
        F m_Function;
    };
    ///����������ָ��
    template<class R,class P1,class P2,class P3>
    class CFunction3
    {
    public:
        typedef typename SFunctionDefine<R,P1,P2,P3>::F3 F; 
        typedef CFunction3<R,P1,P2,P3> ThisClass;
    public:
        CFunction3()
            :m_Function(NULL)
        {}
        CFunction3(F paramF)
            :m_Function(paramF)
        {}
        ThisClass & operator = (const ThisClass & paramF)
        {
            m_Function = paramF.m_Function;
            return *this;
        }
        ThisClass & operator = (F paramF)
        {
            m_Function = paramF;
            return *this;
        }
        R operator()(P1 paramP1,P2 paramP2,P3 paramP3) const
        {
            return m_Function(paramP1,paramP2,paramP3);
        }
        R Invoke(P1 paramP1,P2 paramP2,P3 paramP3) const
        {
            return m_Function(paramP1,paramP2,paramP3);
        }
        bool isBinded() const
        {
            return m_Function != NULL;
        }
        void Bind(F paramF)
        {
            m_Function = paramF;
        }
        void UnBind()
        {
            m_Function = NULL;
        }
    private:
        F m_Function;
    };

    ///�Ĳ�������ָ��
    template<class R,class P1,class P2,class P3,class P4>
    class CFunction4
    {
    public:
        typedef typename SFunctionDefine<R,P1,P2,P3,P4>::F4 F; 
        typedef CFunction4<R,P1,P2,P3,P4> ThisClass;
    public:
        CFunction4()
            :m_Function(NULL)
        {}
        CFunction4(F paramF)
            :m_Function(paramF)
        {}
        ThisClass & operator = (const ThisClass & paramF)
        {
            m_Function = paramF.m_Function;
            return *this;
        }
        ThisClass & operator = (F paramF)
        {
            m_Function = paramF;
            return *this;
        }
        R operator()(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4);
        }
        R Invoke(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4);
        }
        bool isBinded() const
        {
            return m_Function != NULL;
        }
        void Bind(F paramF)
        {
            m_Function = paramF;
        }
        void UnBind()
        {
            m_Function = NULL;
        }
    private:
        F m_Function;
    };

    ///���������ָ��
    template<class R,class P1,class P2,class P3,class P4,class P5>
    class CFunction5
    {
    public:
        typedef typename SFunctionDefine<R,P1,P2,P3,P4,P5>::F5 F; 
        typedef CFunction5<R,P1,P2,P3,P4,P5> ThisClass;
    public:
        CFunction5()
            :m_Function(NULL)
        {}
        CFunction5(F paramF)
            :m_Function(paramF)
        {}
        ThisClass & operator = (const ThisClass & paramF)
        {
            m_Function = paramF.m_Function;
            return *this;
        }
        ThisClass & operator = (F paramF)
        {
            m_Function = paramF;
            return *this;
        }
        R operator()(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4,P5 paramP5) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4,paramP5);
        }
        R Invoke(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4,P5 paramP5) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4,paramP5);
        }
        bool isBinded() const
        {
            return m_Function != NULL;
        }
        void Bind(F paramF)
        {
            m_Function = paramF;
        }
        void UnBind()
        {
            m_Function = NULL;
        }
    private:
        F m_Function;
    };

    ///����������ָ��
    template<class R,class P1,class P2,class P3,class P4,class P5,class P6>
    class CFunction6
    {
    public:
        typedef typename SFunctionDefine<R,P1,P2,P3,P4,P5,P6>::F6 F; 
        typedef CFunction6<R,P1,P2,P3,P4,P5,P6> ThisClass;
    public:
        CFunction6()
            :m_Function(NULL)
        {}
        CFunction6(F paramF)
            :m_Function(paramF)
        {}
        ThisClass & operator = (const ThisClass & paramF)
        {
            m_Function = paramF.m_Function;
            return *this;
        }
        ThisClass & operator = (F paramF)
        {
            m_Function = paramF;
            return *this;
        }
        R operator()(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4,P5 paramP5,P6 paramP6) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4,paramP5,paramP6);
        }
        R Invoke(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4,P5 paramP5,P6 paramP6) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4,paramP5,paramP6);
        }
        bool isBinded() const
        {
            return m_Function != NULL;
        }
        void Bind(F paramF)
        {
            m_Function = paramF;
        }
        void UnBind()
        {
            m_Function = NULL;
        }
    private:
        F m_Function;
    };

    ///�߲�������ָ��
    template<class R,class P1,class P2,class P3,class P4,class P5,class P6,class P7>
    class CFunction7
    {
    public:
        typedef typename SFunctionDefine<R,P1,P2,P3,P4,P5,P6,P7>::F7 F; 
        typedef CFunction7<R,P1,P2,P3,P4,P5,P6,P7> ThisClass;
    public:
        CFunction7()
            :m_Function(NULL)
        {}
        CFunction7(F paramF)
            :m_Function(paramF)
        {}
        ThisClass & operator = (const ThisClass & paramF)
        {
            m_Function = paramF.m_Function;
            return *this;
        }
        ThisClass & operator = (F paramF)
        {
            m_Function = paramF;
            return *this;
        }
        R operator()(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4,P5 paramP5,P6 paramP6,P7 paramP7) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4,paramP5,paramP6,paramP7);
        }
        R Invoke(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4,P5 paramP5,P6 paramP6,P7 paramP7) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4,paramP5,paramP6,paramP7);
        }
        bool isBinded() const
        {
            return m_Function != NULL;
        }
        void Bind(F paramF)
        {
            m_Function = paramF;
        }
        void UnBind()
        {
            m_Function = NULL;
        }
    private:
        F m_Function;
    };

    ///�˲�������ָ��
    template<class R,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8>
    class CFunction8
    {
    public:
        typedef typename SFunctionDefine<R,P1,P2,P3,P4,P5,P6,P7,P8>::F8 F; 
        typedef CFunction8<R,P1,P2,P3,P4,P5,P6,P7,P8> ThisClass;
    public:
        CFunction8()
            :m_Function(NULL)
        {}
        CFunction8(F paramF)
            :m_Function(paramF)
        {}
        ThisClass & operator = (const ThisClass & paramF)
        {
            m_Function = paramF.m_Function;
            return *this;
        }
        ThisClass & operator = (F paramF)
        {
            m_Function = paramF;
            return *this;
        }
        R operator()(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4,P5 paramP5,P6 paramP6,P7 paramP7,P8 paramP8) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4,paramP5,paramP6,paramP7,paramP8);
        }
        R Invoke(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4,P5 paramP5,P6 paramP6,P7 paramP7,P8 paramP8) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4,paramP5,paramP6,paramP7,paramP8);
        }
        bool isBinded() const
        {
            return m_Function != NULL;
        }
        void Bind(F paramF)
        {
            m_Function = paramF;
        }
        void UnBind()
        {
            m_Function = NULL;
        }
    private:
        F m_Function;
    };

    ///�Ų�������ָ��
    template<class R,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8,class P9>
    class CFunction9
    {
    public:
        typedef typename SFunctionDefine<R,P1,P2,P3,P4,P5,P6,P7,P8,P9>::F9 F; 
        typedef CFunction9<R,P1,P2,P3,P4,P5,P6,P7,P8,P9> ThisClass;
    public:
        CFunction9()
            :m_Function(NULL)
        {}
        CFunction9(F paramF)
            :m_Function(paramF)
        {}
        ThisClass & operator = (const ThisClass & paramF)
        {
            m_Function = paramF.m_Function;
            return *this;
        }
        ThisClass & operator = (F paramF)
        {
            m_Function = paramF;
            return *this;
        }
        R operator()(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4,P5 paramP5,P6 paramP6,P7 paramP7,P8 paramP8,P9 paramP9) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4,paramP5,paramP6,paramP7,paramP8,paramP9);
        }
        R Invoke(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4,P5 paramP5,P6 paramP6,P7 paramP7,P8 paramP8,P9 paramP9) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4,paramP5,paramP6,paramP7,paramP8,paramP9);
        }
        bool isBinded() const
        {
            return m_Function != NULL;
        }
        void Bind(F paramF)
        {
            m_Function = paramF;
        }
        void UnBind()
        {
            m_Function = NULL;
        }
    private:
        F m_Function;
    };

    ///ʮ��������ָ��
    template<class R,class P1,class P2,class P3,class P4,class P5,class P6,class P7,class P8,class P9,class P10>
    class CFunction10
    {
    public:
        typedef typename SFunctionDefine<R,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10>::F10 F; 
        typedef CFunction10<R,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10> ThisClass;
    public:
        CFunction10()
            :m_Function(NULL)
        {}
        CFunction10(F paramF)
            :m_Function(paramF)
        {}
        ThisClass & operator = (const ThisClass & paramF)
        {
            m_Function = paramF.m_Function;
            return *this;
        }
        ThisClass & operator = (F paramF)
        {
            m_Function = paramF;
            return *this;
        }
        R operator()(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4,P5 paramP5,P6 paramP6,P7 paramP7,P8 paramP8,P9 paramP9,P10 paramP10) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4,paramP5,paramP6,paramP7,paramP8,paramP9,paramP10);
        }
        R Invoke(P1 paramP1,P2 paramP2,P3 paramP3,P4 paramP4,P5 paramP5,P6 paramP6,P7 paramP7,P8 paramP8,P9 paramP9,P10 paramP10) const
        {
            return m_Function(paramP1,paramP2,paramP3,paramP4,paramP5,paramP6,paramP7,paramP8,paramP9,paramP10);
        }
        bool isBinded() const
        {
            return m_Function != NULL;
        }
        void Bind(F paramF)
        {
            m_Function = paramF;
        }
        void UnBind()
        {
            m_Function = NULL;
        }
    private:
        F m_Function;
    };
    //ί�еĻ���
    class CDelegateBase
    {
    private:
        //����ṹ�����������bind�ĺ����У�������ָ��ת��Ϊvoid *
        template<class FunctionPointer>
        struct SFunctionPointer
        {
            FunctionPointer Pointer;
        };
    protected:
        void * m_DelegateObject;        ///<ί�еĶ���
        void * m_DelegateFunction;      ///<ί�еĺ���
    public:
        ///���캯��
        CDelegateBase()
            :m_DelegateObject(NULL),m_DelegateFunction(NULL)
        {}
        ///ί�а�
        template<class DelegateObjectType,class DelegateFunctionPointer>
        void Bind(DelegateObjectType * aDelegateObject, DelegateFunctionPointer aDelegateFunction)
        {
            SFunctionPointer<DelegateFunctionPointer> FunctionPtr = {aDelegateFunction };
            m_DelegateObject = (void *)aDelegateObject;
            m_DelegateFunction = *(void **)&FunctionPtr;  
        }
        ///�����
        void UnBind() 
        {
            m_DelegateFunction = NULL;
            m_DelegateObject = NULL;
        }
        ///�ж��Ƿ��Ѿ�����
        bool isBinded() const
        {
            return m_DelegateFunction != NULL;
        }
    };
    //�޲�������ί����
    template<class T>
    class CDelegate0 : public CDelegateBase
    {
    public:
        typedef typename SClassFunctionDefine<CNULL_Event>::F0 Function;
        void Invoke()
        {
            if (m_DelegateFunction != NULL )
            {
                Function Fun;
                *(void **)&Fun = m_DelegateFunction;
                (((CNULL_Event *)m_DelegateObject)->*Fun)();
            }

        }
    };
    //һ����������ί����
    template<class P1>
    class CDelegate1 : public CDelegateBase
    {
    public:
        typedef typename SClassFunctionDefine<CNULL_Event,P1>::F1 Function;
        void Invoke(P1 aP1)
        {
            if (m_DelegateFunction != NULL )
            {
                Function Fun;
                *(void **)&Fun = m_DelegateFunction;
                (((CNULL_Event *)m_DelegateObject)->*Fun)(aP1);
            }
        }
    };

    //������������ί����
    template<class P1,class P2>
    class CDelegate2 : public CDelegateBase
    {
    public:
        typedef typename SClassFunctionDefine<CNULL_Event,P1,P2>::F2 Function;
        void Invoke(P1 aP1,P2 aP2)
        {
            if (m_DelegateFunction != NULL )
            {
                Function Fun;
                *(void **)&Fun = m_DelegateFunction;
                (((CNULL_Event *)m_DelegateObject)->*Fun)(aP1, aP2);
            }
        }
    };

    //������������ί����
    template<class P1,class P2,class P3>
    class CDelegate3 : public CDelegateBase
    {
    public:
        typedef typename SClassFunctionDefine<CNULL_Event,P1,P2,P3>::F3 Function;
        void Invoke(P1 aP1,P2 aP2,P3 aP3)
        {
            if (m_DelegateFunction != NULL )
            {
                Function Fun;
                *(void **)&Fun = m_DelegateFunction;
                (((CNULL_Event *)m_DelegateObject)->*Fun)(aP1, aP2, aP3);
            }
        }
    };

    //�ĸ���������ί����
    template<class P1,class P2,class P3, class P4>
    class CDelegate4 : public CDelegateBase
    {
    public:
        typedef typename SClassFunctionDefine<CNULL_Event,P1,P2,P3,P4>::F4 Function;
        void Invoke(P1 aP1,P2 aP2,P3 aP3,P4 aP4)
        {
            if (m_DelegateFunction != NULL )
            {
                Function Fun;
                *(void **)&Fun = m_DelegateFunction;
                (((CNULL_Event *)m_DelegateObject)->*Fun)(aP1, aP2, aP3,aP4);
            }
        }
    };

    //�����������ί����
    template<class P1,class P2,class P3, class P4, class P5>
    class CDelegate5 : public CDelegateBase
    {
    public:
        typedef typename SClassFunctionDefine<CNULL_Event,P1,P2,P3,P4,P5>::F5 Function;
        void Invoke(P1 aP1,P2 aP2,P3 aP3,P4 aP4, P5 aP5)
        {
            if (m_DelegateFunction != NULL )
            {
                Function Fun;
                *(void **)&Fun = m_DelegateFunction;
                (((CNULL_Event *)m_DelegateObject)->*Fun)(aP1, aP2, aP3, aP4, aP5);
            }
        }
    };
    //������������ί����
    template<class P1,class P2,class P3, class P4, class P5, class P6>
    class CDelegate6 : public CDelegateBase
    {
    public:
        typedef typename SClassFunctionDefine<CNULL_Event,P1,P2,P3,P4,P5,P6>::F6 Function;
        void Invoke(P1 aP1,P2 aP2,P3 aP3,P4 aP4, P5 aP5, P6 aP6)
        {
            if (m_DelegateFunction != NULL )
            {
                Function Fun;
                *(void **)&Fun = m_DelegateFunction;
                (((CNULL_Event *)m_DelegateObject)->*Fun)(aP1, aP2, aP3, aP4, aP5, aP6);
            }
        }
    };

    //�߸���������ί����
    template<class P1,class P2,class P3, class P4, class P5, class P6, class P7>
    class CDelegate7 : public CDelegateBase
    {
    public:
        typedef typename SClassFunctionDefine<CNULL_Event,P1,P2,P3,P4,P5,P6,P7>::F7 Function;
        void Invoke(P1 aP1,P2 aP2,P3 aP3,P4 aP4, P5 aP5, P6 aP6, P7 aP7)
        {
            if (m_DelegateFunction != NULL )
            {
                Function Fun;
                *(void **)&Fun = m_DelegateFunction;
                (((CNULL_Event *)m_DelegateObject)->*Fun)(aP1, aP2, aP3, aP4, aP5, aP6, aP7);
            }
        }
    };
    //�˸���������ί����
    template<class P1,class P2,class P3, class P4, class P5, class P6, class P7,class P8>
    class CDelegate8 : public CDelegateBase
    {
    public:
        typedef typename SClassFunctionDefine<CNULL_Event,P1,P2,P3,P4,P5,P6,P7,P8>::F8 Function;
        void Invoke(P1 aP1,P2 aP2,P3 aP3,P4 aP4, P5 aP5, P6 aP6, P7 aP7,P8 aP8)
        {
            if (m_DelegateFunction != NULL )
            {
                Function Fun;
                *(void **)&Fun = m_DelegateFunction;
                (((CNULL_Event *)m_DelegateObject)->*Fun)(aP1, aP2, aP3, aP4, aP5, aP6, aP7, aP8);
            }
        }
    };
    //�Ÿ���������ί����
    template<class P1,class P2,class P3, class P4, class P5, class P6, class P7,class P8, class P9>
    class CDelegate9 : public CDelegateBase
    {
    public:
        typedef typename SClassFunctionDefine<CNULL_Event,P1,P2,P3,P4,P5,P6,P7,P8,P9>::F9 Function;
        void Invoke(P1 aP1,P2 aP2,P3 aP3,P4 aP4, P5 aP5, P6 aP6, P7 aP7,P8 aP8,P9 aP9)
        {
            if (m_DelegateFunction != NULL )
            {
                Function Fun;
                *(void **)&Fun = m_DelegateFunction;
                (((CNULL_Event *)m_DelegateObject)->*Fun)(aP1, aP2, aP3, aP4, aP5, aP6, aP7, aP8,aP9);
            }
        }
    };
    //ʮ����������ί����
    template<class P1,class P2,class P3, class P4, class P5, class P6, class P7,class P8, class P9,class P10>
    class CDelegate10 : public CDelegateBase
    {
    public:
        typedef typename SClassFunctionDefine<CNULL_Event,P1,P2,P3,P4,P5,P6,P7,P8,P9,P10>::F10 Function;
        void Invoke(P1 aP1,P2 aP2,P3 aP3,P4 aP4, P5 aP5, P6 aP6, P7 aP7,P8 aP8,P9 aP9,P10 aP10)
        {
            if (m_DelegateFunction != NULL )
            {
                Function Fun;
                *(void **)&Fun = m_DelegateFunction;
                (((CNULL_Event *)m_DelegateObject)->*Fun)(aP1, aP2, aP3, aP4, aP5, aP6, aP7, aP8, aP9, aP10);
            }
        }
    };
}
#endif // _LZ_DELEGATE_H__
