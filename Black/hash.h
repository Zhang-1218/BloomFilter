#pragma once 


size_t BKDRHash(const char *str)
{
	register size_t hash = 0;
	while (size_t ch = (size_t)*str++)
	{
		hash = hash * 131 + ch;
	}
	return hash;
}


size_t SDBMHash(const char *str)
{
	register size_t hash = 0;
	while (size_t ch = (size_t)*str++)
	{
		hash = 65599 * hash + ch;
	}
	return hash;
}

size_t RSHash(const char * str)
{
	size_t hash = 0;
	size_t magic = 63689;
	while (size_t ch = (size_t)*str++)
	{
		hash = hash * magic + ch;
		magic *= 378551;
	}
	return hash;
}


size_t APHash(const char *str)
{
	register size_t hash = 0;
	size_t ch;
	for (long i = 0; ch = (size_t)*str++; i++)
	{
		if ((i & 1) == 0)
		{
			hash ^= ((hash << 7) ^ ch ^ (hash >> 3));
		}
		else
		{
			hash ^= (~((hash << 11) ^ ch ^ (hash >> 5)));
		}
	}
	return hash;
}


size_t JSHash(const char* str)
{
	if (!*str)
	{
		return 0;
	}
	size_t hash = 1315423911;
	while (size_t ch = (size_t)*str++)
	{
		hash ^= ((hash << 5) + ch + (hash >> 2));
	}
	return hash;
}


size_t Hash1(const char *str)
{
	register size_t hash = BKDRHash(str) + SDBMHash(str);
	return hash;
}


size_t Hash2(const char *str)
{
	register size_t hash = BKDRHash(str) + 2 * SDBMHash(str);
	return hash;
}

size_t Hash3(const char *str)
{
	register size_t hash = BKDRHash(str) + 3 * SDBMHash(str);
	return hash;
}

size_t Hash4(const char *str)
{
	register size_t hash = BKDRHash(str) + 4 * SDBMHash(str);

	return hash;
}

size_t Hash5(const char *str)
{
	register size_t hash = BKDRHash(str) + 5 * SDBMHash(str);
	return hash;
}