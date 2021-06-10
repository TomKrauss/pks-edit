#include "pch.h"
#include "CppUnitTest.h"

extern "C" {
#include "../include/lineoperations.h"
#include "../include/regexp.h"
#include "../include/arraylist.h"
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
			LINKED_LIST* p1 = ll_insert(&p, sizeof * p);
			Assert::IsNotNull(p);
			Assert::IsNotNull(p1);
			strcpy_s(p1->name, "p1");
			Assert::AreEqual(1l, ll_size(p));
			LINKED_LIST* p2 = ll_insert(&p, sizeof * p);
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
			LINE* pLast = (LINE*)ll_at((LINKED_LIST*)ftable.firstl, 1);
			Assert::IsNotNull(pLast);
			Assert::AreEqual(1l, ln_indexOf(&ftable, pLast));
		}
	};
	TEST_CLASS(regularExpressions)
	{
	private:
		RE_OPTIONS* createOptions(char* expression, int flags) {
			static RE_OPTIONS options;
			static char patternBuf[256];

			options.patternBuf = patternBuf;
			options.endOfPatternBuf = &patternBuf[sizeof patternBuf];
			options.eof = 0;
			options.flags = flags;
			options.expression = expression;
			return &options;
		}

	public:
		TEST_METHOD(SimpleTests)
		{
			RE_OPTIONS* options;
			RE_PATTERN pattern;
			RE_MATCH match;
			options = createOptions("^[a-z]{1,9}(.)$", RE_DOREX);
			Assert::AreEqual(1, compile(options, &pattern));
			Assert::AreEqual(1, pattern.nbrackets);
			Assert::AreEqual(1, step(&pattern, (unsigned char*)"abcX", NULL, &match));
			Assert::AreEqual(1, match.matches);
			Assert::AreEqual(0, step(&pattern, (unsigned char*)"??abcX", NULL, &match));
			Assert::AreEqual(0, match.matches);

			options = createOptions("[a-z]{2,3}", RE_DOREX);
			Assert::AreEqual(1, compile(options, &pattern));
			const char* expr = "99abcX";
			Assert::AreEqual(1, step(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(2, (int)(match.loc1 - expr));
			Assert::AreEqual(5, (int)(match.loc2 - expr));
		}
		TEST_METHOD(CompilerErrorParsing)
		{
			RE_OPTIONS* options;
			RE_PATTERN pattern;
			RE_MATCH match;
			options = createOptions("\"([^\"]+)\", line ([0-9]+): *(.*)", RE_DOREX);
			Assert::AreEqual(1, compile(options, &pattern));
			Assert::AreEqual(3, pattern.nbrackets);
			Assert::AreEqual(1, step(&pattern, (unsigned char*)"\"D:\\tom\\desktop\\readme.txt\", line 90: 0x1 0x1 -1 -1 -8 -31 531 0 1904", NULL, &match));
			char group[200];
			regex_getCapturingGroup(&match, 0, group, sizeof group);
			Assert::AreEqual("D:\\tom\\desktop\\readme.txt", group);
			regex_getCapturingGroup(&match, 1, group, sizeof group);
			Assert::AreEqual("90", group);
		}

		TEST_METHOD(MatchWithOptions)
		{
			RE_OPTIONS* options;
			RE_PATTERN pattern;
			RE_MATCH match;
			options = createOptions("(fritz|franz)", RE_DOREX | RE_IGNCASE);
			Assert::AreEqual(1, compile(options, &pattern));
			Assert::AreEqual(1, step(&pattern, (unsigned char*)"This is Fritz the cat", NULL, &match));
			char group[200];
			regex_getCapturingGroup(&match, 0, group, sizeof group);
			Assert::AreEqual("Fritz", group);
			Assert::AreEqual(1, step(&pattern, (unsigned char*)"This is FRANZ the cat", NULL, &match));
			regex_getCapturingGroup(&match, 0, group, sizeof group);
			Assert::AreEqual("FRANZ", group);

			options = createOptions("*.txt", RE_SHELLWILD | RE_IGNCASE);
			Assert::AreEqual(1, compile(options, &pattern));
			Assert::AreEqual(1, step(&pattern, (unsigned char*)"MYFILE.TXT", NULL, &match));
			Assert::AreEqual(0, step(&pattern, (unsigned char*)"myfile.bmp", NULL, &match));
		}

		TEST_METHOD(ReplaceRegex) {
			RE_OPTIONS* options;
			RE_PATTERN pattern;
			RE_MATCH match;
			options = createOptions("(ape|cat)", RE_DOREX | RE_IGNCASE);
			Assert::AreEqual(1, compile(options, &pattern));
			Assert::AreEqual(1, step(&pattern, (unsigned char*)"The Ape and the Cat are walking along the beach", NULL, &match));
			REPLACEMENT_OPTIONS reOptions;
			REPLACEMENT_PATTERN rePattern;
			reOptions.flags = RE_DOREX | RE_IGNCASE;
			reOptions.maxCaptureGroups = match.nbrackets;
			reOptions.newlineCharacter = '\n';
			reOptions.replacementPattern = "x\\u\\&";
			regex_initializeReplaceByExpressionOptions(&reOptions, &rePattern);
			unsigned char result[300];
			Assert::AreNotEqual(0, (int)rePattern.specialProcessingNeeded);
			Assert::AreEqual(4, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("xAPE", (const char*)result);

			reOptions.replacementPattern = "\\l\\&C";
			regex_initializeReplaceByExpressionOptions(&reOptions, &rePattern);
			Assert::AreEqual(4, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("apec", (const char*)result);

			reOptions.replacementPattern = "\\l\\&\\eC";
			regex_initializeReplaceByExpressionOptions(&reOptions, &rePattern);
			Assert::AreEqual(4, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("apeC", (const char*)result);

			reOptions.replacementPattern = "giraffe";
			reOptions.flags = RE_DOREX | RE_PRESERVE_CASE | RE_IGNCASE;
			regex_initializeReplaceByExpressionOptions(&reOptions, &rePattern);
			Assert::AreEqual(7, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("Giraffe", (const char*)result);

			Assert::AreEqual(1, step(&pattern, (unsigned char*)"The ApE and the Cat are walking along the beach", NULL, &match));
			Assert::AreEqual(7, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("GiRAFFE", (const char*)result);

			reOptions.replacementPattern = "lame duck";
			reOptions.flags = RE_DOREX | RE_PRESERVE_CASE | RE_IGNCASE;
			regex_initializeReplaceByExpressionOptions(&reOptions, &rePattern);
			Assert::AreEqual(9, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("LaME DUCK", (const char*)result);

			options = createOptions("ape,cat", RE_DOREX | RE_IGNCASE);
			Assert::AreEqual(1, compile(options, &pattern));
			Assert::AreEqual(1, step(&pattern, (unsigned char*)"The Ape,Cat and the lion are walking along the beach", NULL, &match));

			reOptions.replacementPattern = "bird,snake";
			reOptions.flags = RE_DOREX | RE_PRESERVE_CASE | RE_IGNCASE;
			regex_initializeReplaceByExpressionOptions(&reOptions, &rePattern);
			Assert::AreEqual(10, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("Bird,Snake", (const char*)result);

			Assert::AreEqual(1, step(&pattern, (unsigned char*)"The ApE,CAT and the lion are walking along the beach", NULL, &match));
			Assert::AreEqual(10, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("BiRD,SNAKE", (const char*)result);

			options = createOptions("der", RE_DOREX | RE_IGNCASE);
			Assert::AreEqual(1, compile(options, &pattern));
			reOptions.replacementPattern = "die";
			reOptions.flags = RE_PRESERVE_CASE | RE_IGNCASE;
			Assert::AreEqual(1, step(&pattern, (unsigned char*)"DEr", NULL, &match));
			regex_initializeReplaceByExpressionOptions(&reOptions, &rePattern);
			Assert::AreEqual(3, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("DIe", (const char*)result);

			Assert::AreEqual(1, step(&pattern, (unsigned char*)"deR", NULL, &match));
			regex_initializeReplaceByExpressionOptions(&reOptions, &rePattern);
			Assert::AreEqual(3, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("diE", (const char*)result);
		}
	};
	TEST_CLASS(arrayList)
	{

	public:
		TEST_METHOD(BasicOperations)
		{
			ARRAY_LIST* pList = arraylist_create(0);
			char* p1 = "hans";
			char* p2 = "otto";
			char* p3 = "katja";
			arraylist_add(pList, p1);
			Assert::AreEqual((size_t)1, arraylist_size(pList));
			arraylist_add(pList, p3);
			Assert::AreEqual((size_t)2, arraylist_size(pList));
			arraylist_add(pList, p2);
			Assert::AreEqual((size_t)3, arraylist_size(pList));
			for (int i = 0; i < 1000; i++) {
				arraylist_add(pList, p2);
			}
			Assert::AreEqual((size_t)1003, arraylist_size(pList));
			int idx = arraylist_indexOf(pList, p3);
			Assert::AreEqual(1, idx);
			arraylist_remove(pList, p3);
			Assert::AreEqual((size_t)1002, arraylist_size(pList));

			arraylist_remove(pList, p2);
			Assert::AreEqual((size_t)1001, arraylist_size(pList));
			arraylist_remove(pList, p2);
			Assert::AreEqual((size_t)1000, arraylist_size(pList));
			int ret = arraylist_remove(pList, p2);
			Assert::AreEqual(1, ret);
			ret = arraylist_remove(pList, "erna");
			Assert::AreEqual(0, ret);

			arraylist_remove(pList, p1);
			ARRAY_ITERATOR iterator = arraylist_iterator(pList);
			void** p = iterator.i_buffer;
			while (p < iterator.i_bufferEnd) {
				void *szString = *p++;
				Assert::AreEqual((char*)szString, p2);
			}
			arraylist_destroy(pList);

		}
	};
}
