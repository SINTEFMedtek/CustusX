kernel void test(void)
{
	size_t i = get_global_id(0);
	i++;
}
