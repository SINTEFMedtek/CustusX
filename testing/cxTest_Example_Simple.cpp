#include <iostream>

/**Simple proof-of-concept test for ctest-style unit test.
 * 
 * Return value is used to evaluate success.
 */
int main(int argc, char **argv)
{
	std::cout << "testing 2+2=4" << std::endl;

	int b = 2;
	int a = b + b;
	bool success = (a==4);

	return !success;
}
