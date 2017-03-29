#ifndef main_h
#define main_h

#include "utility/lease.h"
#include <future>
#include <iostream>
#include <thread>
using namespace std;
class base {
	float jake;
};
class test : public base, public boost::intrusive_ref_counter<test, boost::thread_safe_counter>, public sd::renter {
    public:
	test(int i)
	    : value(i) {}
	int value;
};

struct container {
	container()
	    : test_obj([&] {
		    //		    test_obj = nullptr;
		    //test_obj = sd::make_intrusive<test>(29);	// make swap crash at rhs connection recreate
		    auto _test_obj = sd::make_intrusive<test>(29); //auto _test_obj = std::move(sd::make_intrusive<test>(29));
		    return _test_obj;                              //return std::move(_test_obj);
	    }())
	{
		cout << "in container::container() test_obj->value = " << test_obj->value << endl;
	}
	void print()
	{
		static int count = 0;
		cout << "in container::print test_obj->value = " << (test_obj->value += count) << "\tcount = " << count << endl;
		count++;
	}
	sd::intrusive_ptr<test> test_obj;
};

#endif //main_h
