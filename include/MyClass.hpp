#ifndef __MYCLASS_HPP__
#define __MYCLASS_HPP__

class Myclass
{
public:
	Myclass();
	~Myclass();

	void test(char c);
	void test_throw();

private:
	int mypriv_data;
	int myvideoidx;

	void priv_test();
};

#endif /* __MYCLASS_HPP__ */
