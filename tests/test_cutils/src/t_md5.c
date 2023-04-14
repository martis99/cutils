#include "t_md5.h"

#include "c_md5.h"
#include "cstr.h"

#include "test.h"

TEST(format)
{
	START;

	byte buf[256] = { 0 };
	char res[37]  = { 0 };

	c_md5(CSTR("test"), buf, sizeof(buf), res, sizeof(res));

	EXPECT_STR(res, "098F6BCD-4621-D373-CADE-4E832627B4F6");

	END;
}

STEST(md5)
{
	SSTART;
	RUN(format);
	SEND;
}
