#include "pch.h"
#include "CppUnitTest.h"

extern "C" {
#include "../include/lineoperations.h"
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
}
