#include "pch.h"
#include "CppUnitTest.h"

extern "C" {
#include "../include/lineoperations.h"
#include "../include/regexp.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace pkseditTests
{
	TEST_CLASS(pksEditDocuments)
	{
	public:
		TEST_METHOD(LinkedLists)
		{
			LINKED_LIST* p = NULL;
			LINKED_LIST* p1 = ll_insert(&p, sizeof *p);
			Assert::IsNotNull(p);
			Assert::IsNotNull(p1);
			strcpy_s(p1->name, "p1");
			Assert::AreEqual(1l, ll_size(p));
			LINKED_LIST *p2 = ll_insert(&p, sizeof * p);
			strcpy_s(p2->name, "x2");
			Assert::AreEqual(2l, ll_size(p));
			p2 = ll_find(p, "x2");
			Assert::IsNotNull(p2);
			Assert::AreEqual("x2", p2->name);
			p1 = ll_at(p, 1);
			Assert::AreEqual("p1", p1->name);
			ll_delete(&p, p2);
			Assert::AreEqual(1l, ll_size(p));
			p1 = ll_at(p, 0);
			Assert::IsNotNull(p);
			ll_delete(&p, p1);
			Assert::IsNull(p);
		}

		TEST_METHOD(FileBufferLineOperations)
		{
			FTABLE ftable;
			memset(&ftable, 0, sizeof ftable);
			ln_createAndAddSimple(&ftable, "Hello world");
			Assert::IsNotNull(ftable.firstl);
			Assert::AreEqual(1l, ll_size((LINKED_LIST*)ftable.firstl));
			ln_createAndAddSimple(&ftable, "Hello world2");
			Assert::AreEqual(2l, ll_size((LINKED_LIST*)ftable.firstl));
			LINE* pLast = (LINE*) ll_at((LINKED_LIST*) ftable.firstl, 1);
			Assert::IsNotNull(pLast);
			Assert::AreEqual(1l, ln_indexOf(&ftable, pLast));
		}
	};
	TEST_CLASS(regularExpressions)
	{
	private: void capturingGroup(RE_MATCH* match, int idx, char buffer[200]) {
		size_t delta = match->braelist[idx] - match->braslist[idx];
		memcpy(buffer, match->braslist[idx], delta);
		buffer[delta] = 0;
	}
	public:
		TEST_METHOD(SimpleTests)
		{
			RE_OPTIONS options;
			RE_PATTERN pattern;
			RE_MATCH match;
			char patternBuf[256];
			options.patternBuf = patternBuf;
			options.endOfPatternBuf = &patternBuf[sizeof patternBuf];
			options.eof = 0;
			options.flags = RE_DOREX;
			options.expression = "^[a-z]{1,9}(.)$";
			Assert::AreEqual(1, compile(&options, &pattern));
			Assert::AreEqual(1, pattern.nbrackets);
			Assert::AreEqual(1, step(&pattern, (unsigned char*)"abcX", NULL, &match));
			Assert::AreEqual(1, match.matches);
			Assert::AreEqual(0, step(&pattern, (unsigned char*)"??abcX", NULL, &match));
			Assert::AreEqual(0, match.matches);

			options.expression = "[a-z]{2,3}";
			Assert::AreEqual(1, compile(&options, &pattern));
			const char* expr = "99abcX";
			Assert::AreEqual(1, step(&pattern, (unsigned char*) expr, NULL, &match));
			Assert::AreEqual(2, (int)(match.loc1 - expr));
			Assert::AreEqual(5, (int)(match.loc2 - expr));
		}
		TEST_METHOD(CompilerErrorParsing)
		{
			RE_OPTIONS options;
			RE_PATTERN pattern;
			RE_MATCH match;
			char patternBuf[256];
			options.patternBuf = patternBuf;
			options.endOfPatternBuf = &patternBuf[sizeof patternBuf];
			options.eof = 0;
			options.expression = "\"([^\"]+)\", line ([0-9]+): *(.*)";
			options.flags = RE_DOREX;
			Assert::AreEqual(1, compile(&options, &pattern));
			Assert::AreEqual(3, pattern.nbrackets);
			Assert::AreEqual(1, step(&pattern, (unsigned char*)"\"D:\\tom\\desktop\\readme.txt\", line 90: 0x1 0x1 -1 -1 -8 -31 531 0 1904", NULL, &match));
			char group[200];
			capturingGroup(&match, 0, group);
			Assert::AreEqual("D:\\tom\\desktop\\readme.txt", group);
			capturingGroup(&match, 1, group);
			Assert::AreEqual("90", group);
		}

		TEST_METHOD(MatchWithOptions)
		{
			RE_OPTIONS options;
			RE_PATTERN pattern;
			RE_MATCH match;
			char patternBuf[256];
			options.patternBuf = patternBuf;
			options.endOfPatternBuf = &patternBuf[sizeof patternBuf];
			options.eof = 0;
			options.expression = "(fritz|franz)";
			options.flags = RE_DOREX|RE_IGNCASE;
			Assert::AreEqual(1, compile(&options, &pattern));
			Assert::AreEqual(1, step(&pattern, (unsigned char*)"This is Fritz the cat", NULL, &match));
			char group[200];
			capturingGroup(&match, 0, group);
			Assert::AreEqual("Fritz", group);
			Assert::AreEqual(1, step(&pattern, (unsigned char*)"This is FRANZ the cat", NULL, &match));
			capturingGroup(&match, 0, group);
			Assert::AreEqual("FRANZ", group);

			options.expression = "*.txt";
			options.flags = RE_SHELLWILD | RE_IGNCASE;
			Assert::AreEqual(1, compile(&options, &pattern));
			Assert::AreEqual(1, step(&pattern, (unsigned char*)"MYFILE.TXT", NULL, &match));
			Assert::AreEqual(0, step(&pattern, (unsigned char*)"myfile.bmp", NULL, &match));
		}
		};
}
