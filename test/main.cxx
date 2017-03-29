
//using namespace std;

//int main(int argc, char *argv[])
//{
//    cout << "Hello World!" << endl;
//    return 0;
//}
#include "main.h"
void foo(sd::intrusive_ptr<test> _int0)
{

	auto _int1 = sd::make_intrusive<test>(1003);
	_int0 = _int1; //sd::make_intrusive<test>(17);
	container ct;
	std::thread(&container::print, &ct).join();
	std::thread(&container::print, &ct).join();
	std::thread(&container::print, &ct).join();
	cout << _int0->value << endl; //assert fail when without "_int0 = sd::make_intrusive<test>(17);"

	cout << "auto _int1 = sd::make_intrusive<test>(); _int1->value  = " << _int1->value << endl;
	cout << "ct.test_obj->value = " << ct.test_obj->value << endl;
	cout << "in foo:: _int0 = _int1; _int0->value  = " << _int0->value << endl;
	cout << "in foo:: _int0 = _int1; _int0.get()->value = " << _int0.get()->value << endl;
}

int main(int argc, char* argv[])
{
	cout << "typeid(int).name() = " << typeid(int).name() << endl; //check rtti
	sd::intrusive_ptr<test> _int0;                                 //= nullptr;
	//	_int0 = sd::make_intrusive<test>(2997);
	foo(_int0);
	cout << "in main:: _int0 = _int1; _int0->value  = " << _int0->value << endl;
	cout << "in main:: _int0 = _int1; _int0.get()->value = " << _int0.get()->value << endl;
	//	cin.get();
	return 0;
}
