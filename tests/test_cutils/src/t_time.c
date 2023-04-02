#include "t_time.h"

#include "c_time.h"

#include "test.h"

TEST(time_int)
{
	START;

	unsigned long long start, end;

	start = c_time();

	c_sleep(1000);

	end = c_time();

	unsigned int eps = (unsigned int)(end - start - 1000);

	EXPECT_GE(eps, 0);
	EXPECT_LT(eps, 100);

	END;
}

TEST(time_str)
{
	START;

	char buf[C_TIME_BUF_SIZE] = { 0 };

	c_time_str(buf);

	unsigned int y, m, d, H, M, S, U;

	EXPECT_FMT(buf, 7, "%4u-%2u-%2u %2u:%2u:%2u.%3u", &y, &m, &d, &H, &M, &S, &U);

	EXPECT_GT(y, 2022);
	EXPECT_LE(m, 12);
	EXPECT_LE(d, 31);
	EXPECT_LT(H, 24);
	EXPECT_LT(M, 60);
	EXPECT_LT(S, 60);
	EXPECT_LE(U, 999);

	END;
}

STEST(time)
{
	SSTART;
	RUN(time_int);
	RUN(time_str);
	SEND;
}
