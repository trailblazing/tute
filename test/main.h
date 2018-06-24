#ifndef main_h
#define main_h

#include "utility/lease.h"
#include <future>
#include <iostream>
#include <thread>
using namespace std;
class base {
public:
    base(float i)
        : jake{ i }
    {
    }
    float jake = 0.1f;
};
class test_intrusive : public base
    //        , public boost::intrusive_ref_counter<test_intrusive, boost::thread_safe_counter>
    ,
                       public sd::renter {
public:
    test_intrusive(int i)
        : base(0.2f)
        , value(i)
    {
    }
    int value;
};

struct container {
    container()
        : test_obj{
            sd::intrusive_ptr<test_intrusive>(new test_intrusive(29))
            //              [&] {
            //            //		    test_obj = nullptr;
            //            //test_obj = sd::make_intrusive<test>(29);	// make swap crash at rhs connection recreate
            //            auto _test_obj = sd::intrusive_ptr<test>(new test(29));;
            ////            [&] // sd::make_intrusive<test>(29); //auto _test_obj = std::move(sd::make_intrusive<test>(29));

            ////            {
            ////                return sd::intrusive_ptr<test>(new test(29)); //
            ////                //                    //		    std::forward<sd::intrusive_ptr<element_type>>
            ////                //                    //		    std::move
            ////                //                    ( //sd::intrusive_ptr<element_type>(
            ////                //                        new element_type( //std::forward<
            ////                //                            //typename std::remove_reference<args>::type
            ////                //                            //>
            ////                //                            (args_)...) //, true//)
            ////                //                    ); //std::forward<sd::intrusive_ptr<element_type>>(sd::intrusive_ptr<element_type>(new element_type(std::forward<args>(std::remove_const<args>(args_))...), true));
            ////            }();

            //            return _test_obj; //return std::move(_test_obj);
            //        }()
        }
    {
        cout << "in container::container() test_obj->value = " << test_obj->value << endl;
    }
    void print()
    {
        static int count = 0;
        cout << "in container::print test_obj->value = " << (test_obj->value += count) << "\tcount = " << count << endl;
        count++;
    }
    sd::intrusive_ptr<test_intrusive> test_obj;
};

#endif //main_h
