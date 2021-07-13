#include "pch.h"
#include "CppUnitTest.h"

extern "C" {
#include "../include/lineoperations.h"
#include "../include/regexp.h"
#include "../include/arraylist.h"
#include "../include/linkedlist.h"

extern int string_matchFilename(char* string, char* pattern);
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
			static char patternBuf[512];

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

			options = createOptions("(a|b|c|d)", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			const char* expr = "c";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(0, (int)(match.loc1 - expr));
			Assert::AreEqual(1, (int)(match.loc2 - expr));

			options = createOptions("^[a-z]{1,9}(.)$", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, pattern.nbrackets);
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"abcX", NULL, &match));
			Assert::AreEqual(1, match.matches);
			Assert::AreEqual(0, regex_match(&pattern, (unsigned char*)"??abcX", NULL, &match));
			Assert::AreEqual(0, match.matches);

			options = createOptions("[+-]?[0-9]+(e[+-]?\\d+|b[01]+|x[a-fA-F0-9]+)?", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			expr = "99";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(0, (int)(match.loc1 - expr));
			Assert::AreEqual(2, (int)(match.loc2 - match.loc1));

			expr = "0b011";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(0, (int)(match.loc1 - expr));
			Assert::AreEqual(5, (int)(match.loc2 - match.loc1));

			options = createOptions("'(\\\\'|\\\\u\\d+|[^'])'", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			expr = "'x'";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(0, (int)(match.loc1 - expr));
			Assert::AreEqual(3, (int)(match.loc2 - expr));
			expr = "'\\''";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(0, (int)(match.loc1 - expr));
			Assert::AreEqual(4, (int)(match.loc2 - expr));
			expr = "'\\u9321'";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(0, (int)(match.loc1 - expr));
			Assert::AreEqual(8, (int)(match.loc2 - expr));

			options = createOptions("[a-z]{2,3}", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			expr = "99abcX";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(2, (int)(match.loc1 - expr));
			Assert::AreEqual(5, (int)(match.loc2 - expr));

			options = createOptions("abcd+", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			expr = "abcddddd";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(0, (int)(match.loc1 - expr));
			Assert::AreEqual(8, (int)(match.loc2 - expr));

			options = createOptions("([a-z]{2,3})\\1", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			expr = "xxababc";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(2, (int)(match.loc1 - expr));
			expr = "xxabfabc";
			options = createOptions("([a-z]{2,3}).*\\1", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(2, (int)(match.loc1 - expr));

			options = createOptions("/\\*", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			char extracted[30];
			Assert::AreEqual(1, regex_getPatternString(extracted, &pattern));
			Assert::AreEqual("/*", extracted);
		}
		TEST_METHOD(RegularExpressionsOtherFunctions) {
			RE_OPTIONS* options;
			RE_PATTERN pattern;
			options = createOptions("\"([^\"])+\"", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(3, regex_getMinimumMatchLength(&pattern));

			options = createOptions("abcdefg{3,7}a+", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(10, regex_getMinimumMatchLength(&pattern));

			options = createOptions("abcd(efg){3,7}\\d+", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(14, regex_getMinimumMatchLength(&pattern));

			options = createOptions("<[-+]?[0-9.]+>", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_matchesFirstChar(&pattern, '-'));
			Assert::AreEqual(1, regex_matchesFirstChar(&pattern, '9'));

			options = createOptions("REM.*", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_matchesFirstChar(&pattern, 'R'));
		}

		TEST_METHOD(RegularExpressionsWithGroups) {
			char group[200];
			RE_OPTIONS* options;
			RE_PATTERN pattern;
			RE_MATCH match;
			options = createOptions("\"([^\"])+\"", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, pattern.nbrackets);
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"\"hhhh\"", NULL, &match));
			Assert::AreEqual(0, regex_match(&pattern, (unsigned char*)"\"abc\"", NULL, &match));
			options = createOptions("\"(?:[^\"])+\"", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"\"abc\"", NULL, &match));
			options = createOptions("\"(?:[^a-zX?]){3,}?\"", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			options = createOptions("\"(?:a|bb)+\"\\1", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			// Bad back-reference
			options = createOptions("\"(?:a|bb)+\"\\2", RE_DOREX);
			Assert::AreEqual(0, regex_compile(options, &pattern));
			// Unclosed bracket
			options = createOptions("(x", RE_DOREX);
			Assert::AreEqual(0, regex_compile(options, &pattern));
			// Unclosed bracket
			options = createOptions("[a-z", RE_DOREX);
			Assert::AreEqual(0, regex_compile(options, &pattern));
			options = createOptions("[a-z]+", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			const char* expr = "hello";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(5, (int)(match.loc2 - match.loc1));

			expr = "  123hello123  ";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(5, (int)(match.loc2 - match.loc1));
			Assert::AreEqual(5, (int)(match.loc1 - expr));

			options = createOptions("\\s{1,2}[0-9]+[a-z]+", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(0, (int)(match.loc1 - expr));
			Assert::AreEqual(10, (int)(match.loc2 - match.loc1));

			// lazy versus greedy match of last number
			options = createOptions("\\s{1,2}[0-9]+?", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(0, (int)(match.loc1 - expr));
			Assert::AreEqual(3, (int)(match.loc2 - match.loc1));

			// lazy versus greedy match of last number in combination with range spec.
			options = createOptions("\\s{1,2}[0-9]+[a-z]+[0-9]{2,}?", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(0, (int)(match.loc1 - expr));
			Assert::AreEqual(12, (int)(match.loc2 - match.loc1));

			// ignore case
			options = createOptions("a+b+c+", RE_DOREX|RE_IGNCASE);
			expr = "  Abc  ";
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(2, (int)(match.loc1 - expr));
			Assert::AreEqual(3, (int)(match.loc2 - match.loc1));

			// character classes
			options = createOptions("\\d+\\w+", RE_DOREX);
			expr = "  0123abc  ";
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(2, (int)(match.loc1 - expr));
			Assert::AreEqual(7, (int)(match.loc2 - match.loc1));

			options = createOptions("\\S+", RE_DOREX);
			expr = "  0123abc  ";
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(2, (int)(match.loc1 - expr));
			Assert::AreEqual(7, (int)(match.loc2 - match.loc1));

			// groups
			options = createOptions("(\\d+ )+", RE_DOREX);
			expr = "  0123 0123 xxxxx";
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(2, (int)(match.loc1 - expr));
			Assert::AreEqual(10, (int)(match.loc2 - match.loc1));

			options = createOptions("(ab+)+", RE_DOREX);
			expr = "  ababbabb  ";
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(2, (int)(match.loc1 - expr));
			Assert::AreEqual(4, (int)(match.loc2 - match.loc1));

			options = createOptions("(?:ab+)+", RE_DOREX);
			expr = "  ababbabb  ";
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(2, (int)(match.loc1 - expr));
			Assert::AreEqual(8, (int)(match.loc2 - match.loc1));

			// alternatives
			options = createOptions("(aa+|bb)", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			expr = "  aaabbabb  ";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			regex_getCapturingGroup(&match, 0, group, sizeof group);
			Assert::AreEqual("aaa", group);

			expr = "  ababbabb  ";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(5, (int)(match.loc1 - expr));
			Assert::AreEqual(2, (int)(match.loc2 - match.loc1));
			regex_getCapturingGroup(&match, 0, group, sizeof group);
			Assert::AreEqual("bb", group);

			// alternatives with non-capturing groups
			options = createOptions("(\"(?:\\\"|[^\"])+\")", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			expr = "xx\"abc\\\"def\"xxxxx";
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)expr, NULL, &match));
			Assert::AreEqual(2, (int)(match.loc1 - expr));
			Assert::AreEqual(10, (int)(match.loc2 - match.loc1));
			regex_getCapturingGroup(&match, 0, group, sizeof group);
			Assert::AreEqual("\"abc\\\"def\"", group);
		}
		TEST_METHOD(CompilerErrorParsing) {
			RE_OPTIONS* options;
			RE_PATTERN pattern;
			RE_MATCH match;
			options = createOptions("\"([^\"]+)\", line ([0-9]+): *(.*)", RE_DOREX);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(3, pattern.nbrackets);
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"\"D:\\tom\\desktop\\readme.txt\", line 90: 0x1 0x1 -1 -1 -8 -31 531 0 1904", NULL, &match));
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
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"This is Fritz the cat", NULL, &match));
			char group[200];
			regex_getCapturingGroup(&match, 0, group, sizeof group);
			Assert::AreEqual("Fritz", group);
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"This is FRANZ the cat", NULL, &match));
			regex_getCapturingGroup(&match, 0, group, sizeof group);
			Assert::AreEqual("FRANZ", group);

			options = createOptions("*.txt", RE_SHELLWILD | RE_IGNCASE);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"MYFILE.TXT", NULL, &match));
			Assert::AreEqual(0, regex_match(&pattern, (unsigned char*)"myfile.bmp", NULL, &match));

			options = createOptions("???", RE_IGNCASE);
			Assert::AreEqual(1, regex_compile(options, &pattern));

			options = createOptions("a??", RE_IGNCASE);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"A??", NULL, &match));
			Assert::AreEqual(0, regex_match(&pattern, (unsigned char*)"xxx", NULL, &match));

			options = createOptions("a??", 0);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"a??", NULL, &match));
			Assert::AreEqual(3, (int)(match.loc2 - match.loc1));
			Assert::AreEqual(0, regex_match(&pattern, (unsigned char*)"A??", NULL, &match));
			Assert::AreEqual(0, regex_match(&pattern, (unsigned char*)"xxx", NULL, &match));

			options = createOptions("a\\1", RE_IGNCASE);
			Assert::AreEqual(1, regex_compile(options, &pattern));
		}

		TEST_METHOD(ReplaceRegex) {
			RE_OPTIONS* options;
			RE_PATTERN pattern;
			RE_MATCH match;
			options = createOptions("(ape|cat)", RE_DOREX | RE_IGNCASE);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"The Ape and the Cat are walking along the beach", NULL, &match));
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

			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"The ApE and the Cat are walking along the beach", NULL, &match));
			Assert::AreEqual(7, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("GiRAFFE", (const char*)result);

			reOptions.replacementPattern = "lame duck";
			reOptions.flags = RE_DOREX | RE_PRESERVE_CASE | RE_IGNCASE;
			regex_initializeReplaceByExpressionOptions(&reOptions, &rePattern);
			Assert::AreEqual(9, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("LaME DUCK", (const char*)result);

			options = createOptions("ape,cat", RE_DOREX | RE_IGNCASE);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"The Ape,Cat and the lion are walking along the beach", NULL, &match));

			reOptions.replacementPattern = "bird,snake";
			reOptions.flags = RE_DOREX | RE_PRESERVE_CASE | RE_IGNCASE;
			regex_initializeReplaceByExpressionOptions(&reOptions, &rePattern);
			Assert::AreEqual(10, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("Bird,Snake", (const char*)result);

			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"The ApE,CAT and the lion are walking along the beach", NULL, &match));
			Assert::AreEqual(10, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("BiRD,SNAKE", (const char*)result);

			options = createOptions("der", RE_DOREX | RE_IGNCASE);
			Assert::AreEqual(1, regex_compile(options, &pattern));
			reOptions.replacementPattern = "die";
			reOptions.flags = RE_PRESERVE_CASE | RE_IGNCASE;
			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"DEr", NULL, &match));
			regex_initializeReplaceByExpressionOptions(&reOptions, &rePattern);
			Assert::AreEqual(3, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("DIe", (const char*)result);

			Assert::AreEqual(1, regex_match(&pattern, (unsigned char*)"deR", NULL, &match));
			regex_initializeReplaceByExpressionOptions(&reOptions, &rePattern);
			Assert::AreEqual(3, regex_replaceSearchString(&rePattern, result, sizeof result, &match));
			Assert::AreEqual("diE", (const char*)result);
		}
	};
	TEST_CLASS(filePattern)
	{

	public:
		TEST_METHOD(matching)
		{
			int result = string_matchFilename("test.ipch", "*.ipch");
			Assert::AreEqual(1, result);
			result = string_matchFilename("test.ipch", "!*.ipch");
			Assert::AreEqual(0, result);
			result = string_matchFilename("test.ipch", "!*.pch;!*.ipch");
			Assert::AreEqual(0, result);
			result = string_matchFilename("test.x", "!*.pch;!*.ipch");
			Assert::AreEqual(1, result);
			result = string_matchFilename("test.x", "*;!*.x");
			Assert::AreEqual(0, result);
			result = string_matchFilename("3sfdsdfs.xxx", "![0-9]*");
			Assert::AreEqual(0, result);
			result = string_matchFilename("23234242", "!*[0-9][0-9][0-9]");
			Assert::AreEqual(0, result);
			result = string_matchFilename("23234242", "*.txt;*.c");
			Assert::AreEqual(0, result);
			result = string_matchFilename("x.cpp", "*.c");
			Assert::AreEqual(0, result);
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
