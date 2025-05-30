/******************************************************************************/
/*! @file test.cc
    @brief test program for libapolloron
    @author Masashi Astro Tachibana, Apolloron Project.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "apolloron.h"

using namespace apolloron;


int test1();
int test2();
int test3();
int test4();
int test5();
int test6();
int test7();
int test8();
int test9();
int test10();
int test11();
int test12();
int example1();
int example2();


/*! Main  Calling Test functions
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int main(int argc, char *argv[]) {
    int status;

    fprintf(stderr, "Starting Test1  Setting A Value Into String Object ... ");
    status = test1();
    if (status != 0) {
        return -1;
    }
    fprintf(stderr, "OK\n");

    fprintf(stderr, "Starting Test2  Comparing String And Using Sub-String ... ");
    status = test2();
    if (status != 0) {
        return -1;
    }
    fprintf(stderr, "OK\n");

    fprintf(stderr, "Starting Test3  String Encodings ... ");
    status = test3();
    if (status != 0) {
        return -1;
    }
    fprintf(stderr, "OK\n");

    fprintf(stderr, "Starting Test4  Converting Character-Set ... ");
    status = test4();
    if (status != 0) {
        return -1;
    }
    fprintf(stderr, "OK\n");

    fprintf(stderr, "Starting Test5  Regular Expression ... ");
#if __REGEX == 1
    status = test5();
    if (status != 0) {
        return -1;
    }
    fprintf(stderr, "OK\n");
#else
    fprintf(stderr, "SKIP\n");
#endif

    fprintf(stderr, "Starting Test6  Eval Text ... ");
    status = test6();
    if (status != 0) {
        return -1;
    }
    fprintf(stderr, "OK\n");

    fprintf(stderr, "Starting Test7  Creating SMF (Standard MIDI File) ... ");
    status = test7();
    if (status != 0) {
        return -1;
    }
    fprintf(stderr, "OK\n");

    fprintf(stderr, "Starting Test8  List Class ... ");
    status = test8();
    if (status != 0) {
        return -1;
    }
    fprintf(stderr, "OK\n");

    fprintf(stderr, "Starting Test9  Keys Class ... ");
    status = test9();
    if (status != 0) {
        return -1;
    }
    fprintf(stderr, "OK\n");

    fprintf(stderr, "Starting Test10 Sheet Class ... ");
    status = test10();
    if (status != 0) {
        return -1;
    }
    fprintf(stderr, "OK\n");

    fprintf(stderr, "Starting Test11 DateTime Class ... ");
    status = test11();
    if (status != 0) {
        return -1;
    }
    fprintf(stderr, "OK\n");

    fprintf(stderr, "Starting Test12 MIMEHeader Class ... ");
    status = test12();
    if (status != 0) {
        return -1;
    }
    fprintf(stderr, "OK\n");

//  example1();
//  example2();

    return 0;
}


/*! Test1  Setting A Value Into String Object
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int test1() {
    // Declare Strings
    String str_a, str_b, str_c, str_d, str_e;

    // Set Values
    str_a = "You get married";                         // Set String
    str_b = 100 * 10000;                               // Set String By Number
    str_c = "Wow " + str_a + " and have " + str_b + " kids.";  // Set String Combination
    str_d.sprintf("%S %s", &str_b, "kids");            // Set Format String
#if __MD5 == 1
    str_e = str_c.crypt();
#endif

    // Expected Results
    // str_a = You get married
    // str_b = 1000000
    // str_c = Wow You get married and have 1000000 kids.
    // str_d = 1000000 kids
    // str_d.len() = 12
    // str_e = (crypted string)

    if (strcmp(str_a.c_str(), "You get married") != 0) {
        fprintf(stderr, "Error: Test1 #1\n");
        return -1;
    }

    if (strcmp(str_b.c_str(), "1000000") != 0) {
        fprintf(stderr, "Error: Test1 #2\n");
        return -1;
    }

    if (strcmp(str_c.c_str(), "Wow You get married and have 1000000 kids.") != 0) {
        fprintf(stderr, "Error: Test1 #3\n");
        return -1;
    }

    if (strcmp(str_d.c_str(), "1000000 kids") != 0) {
        fprintf(stderr, "Error: Test1 #4\n");
        return -1;
    }

    if (str_d.len() != 12) {
        fprintf(stderr, "Error: Test1 #5\n");
        return -1;
    }

    if (!str_a.isWcMatchAll("You*d")) {
        fprintf(stderr, "Error: Test1 #6\n");
        return -1;
    }

    if (!str_a.isWcMatchAll("Y*ma*")) {
        fprintf(stderr, "Error: Test1 #7\n");
        return -1;
    }

#if __MD5 == 1
    if (strcmp(str_e.decrypt().c_str(), str_c.c_str()) != 0) {
        fprintf(stderr, "Error: Test1 #8\n");
        return -1;
    }
#endif

    // Clear Allocated Memories (option)
    str_b.clear();
    str_c.clear();
    str_d.clear();
#if __MD5 == 1
    str_e.clear();
#endif

    return 0;
}


/*! Test2  Comparing String And Using Sub-String
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int test2() {
    // Declare Strings
    String str_a, str_b, str_c;

    // Set Values
    str_a = "Linux Penguin Attack";                    // Set String
    str_b = str_a.right(6);                            // Get Right Side 6 Chars
    str_c = str_a.upper();                             // To Upper-Case

    // Expected Results
    // str_a = Linux Penguin Attack
    // str_b = Attack
    // str_c = LINUX PENGUIN ATTACK
    // str_c[4] = X

    if (strcmp(str_a.c_str(), "Linux Penguin Attack") != 0) {
        fprintf(stderr, "Error: Test2 #1\n");
        return -1;
    }

    if (strcmp(str_b.c_str(), "Attack") != 0) {
        fprintf(stderr, "Error: Test2 #2\n");
        return -1;
    }

    if (strcmp(str_c.c_str(), "LINUX PENGUIN ATTACK") != 0) {
        fprintf(stderr, "Error: Test2 #3\n");
        return -1;
    }

    if (str_c[4] != 'X') {
        fprintf(stderr, "Error: Test2 #4\n");
        return -1;
    }

    // Clear Allocated Memories (option)
    str_a.clear();
    str_b.clear();
    str_c.clear();

    return 0;
}


/*! Test3  String Encodings
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int test3() {
    // Declare Strings
    String str_a, str_b, str_c, str_d, str_e, str_f, str_g, str_h, str_i, str_j,
           str_k, str_l, str_m, str_n, str_o, str_p, str_q, str_r, str_s, str_t;
    String name, user, domain;

    // Set Values
    str_a = "This is a pen.";                          // Set String
    str_b = str_a.encodeURL();                         // URL Encode
    str_b += "%u65E5%u672C%u8A9E";                     // Append Japanese String
    str_c = str_b.decodeURL("AUTODETECT_JP", "EUC-JP");// URL Decode
    str_d = str_a.encodeBASE64();                      // BASE64 Encode
    str_e = str_d.decodeBASE64();                      // BASE64 Decode
    str_f = "He is \xA5\xDA\xA5\xF3\xA5\xDA\xA5\xF3 the <penguin>."; // Set EUC-JP String
    str_g = str_f.encodeMIME("AUTODETECT_JP", "ISO-2022-JP"); // MIME Encode
    str_h = str_g.decodeMIME("AUTODETECT_JP", "EUC-JP"); // MIME Decode
    str_i = str_f.escapeHTML("AUTODETECT_JP", "EUC-JP"); // HTML Escape
#if __MD5 == 1
    str_j = str_a.md5();                               // Get MD5 Diget Key
#endif
    str_k = "hello&quot&#34;&#x22&copyworld";          // HTML encoded String
    str_l = str_k.unescapeHTML();                      // Decoded String
    str_m = "abc\r\ndef\r";                            // String with a return code
    str_n = str_m.changeReturnCode("ABC");             // change return codes
#if __MD5 == 1
    // CRAM-MD5 responce
    str_o = "password";
    str_o = str_o.cramMd5("account",
                          "PEMzN0QxRDA5QkREMURBRjQ5NjI5REFCQzE0QzdCMDJBQHZpc2lvbj4=");
#endif
#if __SHA1 == 1
    str_p = "Hello~";
    str_p = str_p.sha1();
    str_q = "what do ya want for nothing?"; // Data
    str_q = str_q.hmacSha1("Jefe"); // Key="Jefe"
#endif

    // Expected Results
    // str_a = This is a pen.
    // str_b = This%20is%20a%20pen.%u65E5%u672C%u8A9E
    // str_c = This is a pen.\xC6\xFC\xCB\xDC\xB8\xEC
    // str_d = VGhpcyBpcyBhIHBlbi4=
    // str_e = This is a pen.
    // str_f = He is \xA5\xDA\xA5\xF3\xA5\xDA\xA5\xF3 the <penguin>.
    // str_g = =?ISO-2022-JP?B?SGUgaXMgGyRCJVolcyVaJXMbKEIgdGhlIHBlbmd1aW4u?=
    // str_h = He is \xA5\xDA\xA5\xF3\xA5\xDA\xA5\xF3 the <penguin>.
    // str_i = He is \xA5\xDA\xA5\xF3\xA5\xDA\xA5\xF3 the &lt;penguin&gt;.
    // str_j = 26df8d1ff25025dbc9205416a75c7a80
    // str_k = hello&quot&#34;&#x22&copyworld
    // str_l = hello"""\xC2\xA9world
    // str_m = abc\r\ndef\r
    // str_n = abcABCdefABC
    // str_o = YWNjb3VudCA4MTk3M2Q3NTllYWRjMjBjYTNhMGRmZDgxMDQwMDg1ZQ==
    // str_p = 70b6fef3be85ab78eae4b9ba38c74f5b07252341
    // str_q = effcdf6ae5eb2fa2d27416d5f184df9c259a7c79

    if (strcmp(str_a.c_str(), "This is a pen.") != 0) {
        fprintf(stderr, "Error: Test3 #1\n");
        return -1;
    }

    if (strcmp(str_b.c_str(), "This%20is%20a%20pen.%u65E5%u672C%u8A9E") != 0) {
        fprintf(stderr, "Error: Test3 #2\n");
        return -1;
    }

    if (strcmp(str_c.c_str(), "This is a pen.\xC6\xFC\xCB\xDC\xB8\xEC") != 0) {
        fprintf(stderr, "Error: Test3 #3\n");
        return -1;
    }

    if (strcmp(str_d.c_str(), "VGhpcyBpcyBhIHBlbi4=") != 0) {
        fprintf(stderr, "Error: Test3 #4\n");
        return -1;
    }

    if (strcmp(str_e.c_str(), "This is a pen.") != 0) {
        fprintf(stderr, "Error: Test3 #5\n");
        return -1;
    }

    if (strcmp(str_f.c_str(), "He is \xA5\xDA\xA5\xF3\xA5\xDA\xA5\xF3 the <penguin>.") != 0) {
        fprintf(stderr, "Error: Test3 #6\n");
        return -1;
    }

    if (strcmp(str_g.c_str(), "=?ISO-2022-JP?B?SGUgaXMgGyRCJVolcyVaJXMbKEIgdGhlIDxwZW5ndWluPi4=?=") != 0) {
        fprintf(stderr, "Error: Test3 #7\n");
        return -1;
    }

    if (strcmp(str_h.c_str(), "He is \xA5\xDA\xA5\xF3\xA5\xDA\xA5\xF3 the <penguin>.") != 0) {
        fprintf(stderr, "Error: Test3 #8\n");
        return -1;
    }

    if (strcmp(str_i.c_str(), "He is \xA5\xDA\xA5\xF3\xA5\xDA\xA5\xF3 the &lt;penguin&gt;.") != 0) {
        fprintf(stderr, "Error: Test3 #9\n");
        return -1;
    }

#if __MD5 == 1
    if (strcmp(str_j.c_str(), "26df8d1ff25025dbc9205416a75c7a80") != 0) {
        fprintf(stderr, "Error: Test3 #10\n");
        return -1;
    }
#endif

    if (strcmp(str_l.c_str(), "hello\"\"\"\xC2\xA9world") != 0) {
        fprintf(stderr, "Error: Test3 #11\n");
        return -1;
    }

    if (strcmp(str_n.c_str(), "abcABCdefABC") != 0) {
        fprintf(stderr, "Error: Test3 #12\n");
        return -1;
    }

#if __MD5 == 1
    if (strcmp(str_o.c_str(), "YWNjb3VudCA4MTk3M2Q3NTllYWRjMjBjYTNhMGRmZDgxMDQwMDg1ZQ==") != 0) {
        fprintf(stderr, "Error: Test3 #13\n");
        return -1;
    }
#endif

#if __SHA1 == 1
    if (strcmp(str_p.c_str(), "70b6fef3be85ab78eae4b9ba38c74f5b07252341") != 0) {
        fprintf(stderr, "Error: Test3 #14\n");
        return -1;
    }

    if (strcmp(str_q.c_str(), "effcdf6ae5eb2fa2d27416d5f184df9c259a7c79") != 0) {
        fprintf(stderr, "Error: Test3 #15\n");
        return -1;
    }
#endif

    str_r = "=?ISO-2022-JP?B?GyRCJWYhPCU2GyhCMQ==?= <user3@example.com>";
    str_r.parseEmail(name, user, domain);
    if (strcmp(name.c_str(), "=?ISO-2022-JP?B?GyRCJWYhPCU2GyhCMQ==?=") != 0) {
        fprintf(stderr, "Error: Test3 #16\n");
        return -1;
    }
    if (strcmp(user.c_str(), "user3") != 0) {
        fprintf(stderr, "Error: Test3 #17\n");
        return -1;
    }
    if (strcmp(domain.c_str(), "example.com") != 0) {
        fprintf(stderr, "Error: Test3 #18\n");
        return -1;
    }

    str_r = " \"Nakamura, Tarou\" <nakamura@example.com > ";
    str_r.parseEmail(name, user, domain);
    if (strcmp(name.c_str(), "Nakamura, Tarou") != 0) {
        fprintf(stderr, "Error: Test3 #19\n");
        return -1;
    }
    if (strcmp(user.c_str(), "nakamura") != 0) {
        fprintf(stderr, "Error: Test3 #20\n");
        return -1;
    }
    if (strcmp(domain.c_str(), "example.com") != 0) {
        fprintf(stderr, "Error: Test3 #21\n");
        return -1;
    }

    str_s = "=?utf-8?q?ABC=E3=81=82=E3=81=84=E3=81=86=E3=81=88=E3=81=8A=E3=81="
            "8B=E3=81=8D=E3=81=8F=E3=81=91=E3=81=93?=\r\n"
            " a=?ISO-2022-JP?B?SGUgaXMgGyRCJVolcyVaJXMbKEIgdGhlIDxwZW5ndWluPi4=?=";
    if (strcmp(str_s.decodeMIME("AUTODETECT", "UTF-8").c_str(),
               "ABC\xE3\x81\x82\xE3\x81\x84\xE3\x81\x86\xE3\x81\x88\xE3\x81\x8A\xE3\x81"
               "\x8B\xE3\x81\x8D\xE3\x81\x8F\xE3\x81\x91\xE3\x81\x93 aHe is \xE3\x83"
               "\x9A\xE3\x83\xB3\xE3\x83\x9A\xE3\x83\xB3 the <penguin>.") != 0) {
        fprintf(stderr, "Error: Test3 #22\n");
        return -1;
    }

    // Clear Allocated Memories (option)
    str_a.clear();
    str_b.clear();
    str_c.clear();
    str_d.clear();
    str_e.clear();
    str_f.clear();
    str_g.clear();
    str_h.clear();
#if __MD5 == 1
    str_j.clear();
#endif
    str_k.clear();
    str_l.clear();
    str_m.clear();
    str_n.clear();
#if __MD5 == 1
    str_o.clear();
#endif
#if __SHA1 == 1
    str_p.clear();
    str_q.clear();
#endif
    str_r.clear();
    str_s.clear();
    str_t.clear();

    return 0;
}


/*! Test4  Converting Character-Set
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int test4() {
    // Declare Strings
    String str_a, str_b, str_c, str_d;

    // Set Values
    str_a = "\xA4\xA2\xA4\xA4\xA4\xA6\xA4\xA8\xA4\xAA"
            "\xA5\xA2\xA5\xA4\xA5\xA6\xA5\xA8\xA5\xAA"
            "ABC";                                     // Set Japanese String
    str_b = str_a.detectCharSet();                     // Detect Character-Set
    str_c = str_a.strconv("AUTODETECT", "UTF-8");      // Convert To UTF-8
    str_d = str_a.changeWidth("ajk", "EUC-JP", "EUC-JP"); // Convert To Half-Width

    // Expected Results
    // str_a = \xA4\xA2\xA4\xA4\xA4\xA6\xA4\xA8\xA4\xAA
    //         \xA5\xA2\xA5\xA4\xA5\xA6\xA5\xA8\xA5\xAAABC
    // str_b = EUC-JP
    // str_c.detectCharSet() = UTF-8
    // str_d = \x8E\xB1\x8E\xB2\x8E\xB3\x8E\xB4\x8E\xB5
    //         \x8E\xB1\x8E\xB2\x8E\xB3\x8E\xB4\x8E\xB5ABC
    // str_d.width("EUC-JP") = 13

    if (strcmp(str_a.c_str(), "\xA4\xA2\xA4\xA4\xA4\xA6\xA4\xA8\xA4\xAA"
               "\xA5\xA2\xA5\xA4\xA5\xA6\xA5\xA8\xA5\xAA"
               "ABC") != 0) {
        fprintf(stderr, "Error: Test4 #1\n");
        return -1;
    }

    if (strcmp(str_b.c_str(), "EUC-JP") != 0) {
        fprintf(stderr, "Error: Test4 #2\n");
        return -1;
    }

    if (strcmp(str_c.detectCharSet(), "UTF-8") != 0) {
        fprintf(stderr, "Error: Test4 #3\n");
        return -1;
    }

    if (strcmp(str_d.c_str(), "\x8E\xB1\x8E\xB2\x8E\xB3\x8E\xB4\x8E\xB5"
               "\x8E\xB1\x8E\xB2\x8E\xB3\x8E\xB4\x8E\xB5"
               "ABC") != 0) {
        fprintf(stderr, "Error: Test4 #4\n");
        return -1;
    }

    if (str_d.width("EUC-JP") != 13) {
        fprintf(stderr, "Error: Test4 #5\n");
        return -1;
    }

    // Clear Allocated Memories (option)
    str_a.clear();
    str_b.clear();
    str_c.clear();
    str_d.clear();

    return 0;
}


#if __REGEX == 1
/*! Test5  Regular Expression
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int test5() {
    // Declare Strings
    String str_a, str_b, str_c, str_d;

    // Set Values
    str_a = "12345/ABCDEF";                            // Set String
    str_b = str_a.reMatch("^.*(2.*/A).*$");            // Get Sub-String
    str_c = str_a.reSubst("2.*C", "_test_");           // Replace Sub-String
    str_d = str_a.reTrans("234", "bcd");               // Replace Sub-String

    // Expected Results
    // str_a = 12345/ABCDEF
    // str_b = 2345/A
    // str_c = 1_test_DEF
    // str_d = 1bcd5/ABCDEF

    if (strcmp(str_a.c_str(), "12345/ABCDEF") != 0) {
        fprintf(stderr, "Error: Test5 #1\n");
        return -1;
    }

    if (strcmp(str_b.c_str(), "2345/A") != 0) {
        fprintf(stderr, "Error: Test5 #2\n");
        return -1;
    }

    if (strcmp(str_c.c_str(), "1_test_DEF") != 0) {
        fprintf(stderr, "Error: Test5 #3\n");
        return -1;
    }

    if (strcmp(str_d.c_str(), "1bcd5/ABCDEF") != 0) {
        fprintf(stderr, "Error: Test5 #4\n");
        return -1;
    }

    // Clear Allocated Memories (option)
    str_a.clear();
    str_b.clear();
    str_c.clear();
    str_d.clear();

    return 0;
}
#endif


/*! Test6  Eval Text
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int test6() {
    // Declare Strings and Keys
    String html, disp_html1, disp_html2, disp_html3, disp_html4, disp_html5, disp_html6, disp_html7;
    Keys html_keys;

    // Set Values
    html_keys["KEY1"] = "abc";
    html = "<html><body>&($KEY1);</body></html>";
    disp_html1 = html.evalText(html_keys);

    html_keys["KEY1"] = "xyz&XYZ";
    html = "<html><body>&($KEY1::html);</body></html>";
    disp_html2 = html.evalText(html_keys);

    html_keys["KEY1"] = "";
    html_keys["KEY2"] = "xyz&XYZ";
    html = "<html><body>&($KEY1?$KEY2:\"KEY1 is \\\"NOT\\\" set\"::html);</body></html>";
    disp_html3 = html.evalText(html_keys);

    html_keys["KEY1"] = "on";
    html_keys["KEY2"] = "on";
    html = "<html><body>&($KEY1==$KEY2?$KEY2:\"not match\"::html);</body></html>";
    disp_html4 = html.evalText(html_keys);

    html_keys["KEY1"] = "";
    html_keys["KEY2"] = "on";
    html = "{{#if $KEY1}};key1{{#elif $KEY2}}key2{{#else}}else{{#endif}}";
    disp_html5 = html.evalText(html_keys);

    html_keys["KEY1"] = "";
    html_keys["KEY2"] = "on";
    html = "{{#if $KEY1==\"on\"}}key1{{#elif $KEY2==\"on\"}}key2{{#else}}else{{#endif}}";
    disp_html6 = html.evalText(html_keys);

    html_keys["KEY1"] = "";
    html_keys["KEY2"] = "on";
    html_keys["KEY3"] = "off";
    html = "{{#if $KEY1==\"on\"}}key1{{#elif $KEY2==\"on\"}}{{#if $KEY3==\"off\"}}key3{{#else}}key2{{#endif}}{{#else}}else{{#endif}}";
    disp_html7 = html.evalText(html_keys);

    // Expected Results
    // disp_html1 = <html><body>abc</body></html>
    // disp_html2 = <html><body>xyz&amp;XYZ</body></html>
    // disp_html3 = <html><body>KEY1 is "NOT" set</body></html>

    if (strcmp(disp_html1.c_str(), "<html><body>abc</body></html>") != 0) {
        fprintf(stderr, "Error: Test6 #1\n");
        return -1;
    }

    if (strcmp(disp_html2.c_str(), "<html><body>xyz&amp;XYZ</body></html>") != 0) {
        fprintf(stderr, "Error: Test6 #2\n");
        return -1;
    }

    if (strcmp(disp_html3.c_str(), "<html><body>KEY1 is &quot;NOT&quot; set</body></html>") != 0) {
        fprintf(stderr, "Error: Test6 #3\n");
        return -1;
    }

    if (strcmp(disp_html4.c_str(), "<html><body>on</body></html>") != 0) {
        fprintf(stderr, "Error: Test6 #4\n");
        return -1;
    }

    if (strcmp(disp_html5.c_str(), "key2") != 0) {
        fprintf(stderr, "Error: Test6 #5\n");
        return -1;
    }

    if (strcmp(disp_html6.c_str(), "key2") != 0) {
        fprintf(stderr, "Error: Test6 #6\n");
        return -1;
    }

    if (strcmp(disp_html7.c_str(), "key3") != 0) {
        fprintf(stderr, "Error: Test6 #7\n");
        return -1;
    }

    // Clear Allocated Memories (option)
    html.clear();
    disp_html1.clear();
    disp_html2.clear();
    disp_html3.clear();

    return 0;
}


/*! Test7  Creating SMF (Standard MIDI File)
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int test7() {
    // Declare Strings
    String str_a, str_b;

    // Set Values
    str_a = "cde";                                     // Set Notes

    // Convert
    str_b = str_a.toMIDI();                            // Convert To MIDI
    //str_b.saveFile("test7.mid");                       // Save File

    // Expected Results
    // str_b.binaryLength() = 150
    // str_b = (binary)

    if (str_b.binaryLength() != 150) {
        fprintf(stderr, "Error: Test7 #1\n");
        return -1;
    }

    if (memcmp(str_b.c_str(),
               "\x4D\x54\x68\x64\x00\x00\x00\x06\x00\x01\x00\x02\x00\x78\x4D\x54"
               "\x72\x6B\x00\x00\x00\x5E\x00\xF0\x0A\x41\x10\x42\x12\x40\x00\x7F"
               "\x00\x41\xF7\x00\xFF\x58\x04\x04\x02\x18\x08\x00\xFF\x59\x02\x00"
               "\x00\x00\xFF\x51\x03\x07\x27\x0E\x00\xFF\x2F\x00\x00\x00\x00\x00"
               "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
               "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
               "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
               "\x00\x00\x00\x00\x4D\x54\x72\x6B\x00\x00\x00\x1A\x7F\xC0\x00\x00"
               "\x90\x3C\x64\x78\x3C\x00\x00\x3E\x64\x78\x3E\x00\x00\x40\x64\x78"
               "\x40\x00\x00\xFF\x2F\x00", 150) != 0) {
        fprintf(stderr, "Error: Test7 #2\n");
        return -1;
    }

    // Clear Allocated Memories (option)
    str_a.clear();
    str_b.clear();

    return 0;
}


/*! Test8  List Class
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int test8() {
    // Declare Strings
    String str_a, str_b, str_c;
    List list1, list2;

    // Set Values
    str_a = "Tarou";                                // Set String
    str_b = "is";                                   // Set String
    str_c = "friend";                               // Set String
    list1.add(str_a);                               // Append String Into List
    list1.add(str_b);                               // Append String Into List
    list1.add(str_c);                               // Append String Into List

    // Insert Into List
    list1.insert(2, "my");                          // Insert Before 3rd Element

    // Delete From List
    list1.remove(1);                                // Delete 2nd Element

    // Replace List Element
    list1[0] += "'s";                               // Replace 1st Element String

    // Expected Results
    // str_a = Tarou
    // str_b = is
    // str_c = friend
    // list1.max() = 3
    // list1[0] = Tarou's
    // list1[1] = my
    // list1[2] = friend

    if (strcmp(str_a.c_str(), "Tarou") != 0) {
        fprintf(stderr, "Error: Test8 #1\n");
        return -1;
    }

    if (strcmp(str_b.c_str(), "is") != 0) {
        fprintf(stderr, "Error: Test8 #2\n");
        return -1;
    }

    if (strcmp(str_c.c_str(), "friend") != 0) {
        fprintf(stderr, "Error: Test8 #3\n");
        return -1;
    }

    if (list1.max() != 3) {
        fprintf(stderr, "Error: Test8 #4\n");
        return -1;
    }

    if (strcmp(list1[0].c_str(), "Tarou's") != 0) {
        fprintf(stderr, "Error: Test8 #5\n");
        return -1;
    }

    if (strcmp(list1[1].c_str(), "my") != 0) {
        fprintf(stderr, "Error: Test8 #6\n");
        return -1;
    }

    if (strcmp(list1[2].c_str(), "friend") != 0) {
        fprintf(stderr, "Error: Test8 #7\n");
        return -1;
    }

    list2.setEmails("user1@example.com, <user2@example.com> , \r\n"
                    "\t=?ISO-2022-JP?B?GyRCJWYhPCU2GyhCMQ==?= <user3@example.com>,"
                    "    a: =?UTF-8?B?7Lus65+s?= <kara@example.com>,\r\n"
                    "    \"Nakamura, Tarou\" <nakamura@example.com> ;");

    if (list2.max() != 5) {
        fprintf(stderr, "Error: Test8 #8\n");
        return -1;
    }

    if (list2[0] != "user1@example.com") {
        fprintf(stderr, "Error: Test8 #9\n");
        return -1;
    }

    if (list2[1] != "<user2@example.com>") {
        fprintf(stderr, "Error: Test8 #10\n");
        return -1;
    }

    if (list2[2] != "=?ISO-2022-JP?B?GyRCJWYhPCU2GyhCMQ==?= <user3@example.com>") {
        fprintf(stderr, "Error: Test8 #11\n");
        return -1;
    }

    if (list2[3] != "=?UTF-8?B?7Lus65+s?= <kara@example.com>") {
        fprintf(stderr, "Error: Test8 #12\n");
        return -1;
    }

    if (list2[4] != "\"Nakamura, Tarou\" <nakamura@example.com>") {
        fprintf(stderr, "Error: Test8 #13\n");
        return -1;
    }

    // Clear Allocated Memories (option)
    str_a.clear();
    str_b.clear();
    str_c.clear();
    list1.clear();
    list2.clear();

    return 0;
}


/*! Test9  Keys Class
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int test9() {
    // Declare Strings
    String str_a;
    Keys keys;

    // Set Values
    str_a = "name";                                   // Set String
    keys[str_a] = "Tarou";                            // Set Key
    keys["nickname"] = "Hunhun";                      // Set Key
    keys["age"] = "5";                                // Set Key
    keys[str_a] = "Tarou G Banana";                   // Replace Value Of Key

    // Expected Results
    // str_a = name
    // keys.max() = 3
    // keys.key(0) = age
    // keys.value(0) = 5
    // keys.key(1) = name
    // keys.value(1) = Tarou G Banana
    // keys.key(2) = nickname
    // keys.value(2) = Hunhun
    // keys["name"] = Tarou G Banana
    // keys["nickname"] = Hunhun
    // keys["age"] = 5

    if (strcmp(str_a.c_str(), "name") != 0) {
        fprintf(stderr, "Error: Test9 #1\n");
        return -1;
    }

    if (keys.max() != 3) {
        fprintf(stderr, "Error: Test9 #2\n");
        return -1;
    }

    if (strcmp(keys.key(0), "age") != 0) {
        fprintf(stderr, "Error: Test9 #3\n");
        return -1;
    }

    if (strcmp(keys.value(0).c_str(), "5") != 0) {
        fprintf(stderr, "Error: Test9 #4\n");
        return -1;
    }

    if (strcmp(keys.key(1), "name") != 0) {
        fprintf(stderr, "Error: Test9 #5\n");
        return -1;
    }

    if (strcmp(keys.value(1).c_str(), "Tarou G Banana") != 0) {
        fprintf(stderr, "Error: Test9 #6\n");
        return -1;
    }

    if (strcmp(keys.key(2), "nickname") != 0) {
        fprintf(stderr, "Error: Test9 #7\n");
        return -1;
    }

    if (strcmp(keys.value(2).c_str(), "Hunhun") != 0) {
        fprintf(stderr, "Error: Test9 #8\n");
        return -1;
    }

    if (strcmp(keys["name"].c_str(), "Tarou G Banana") != 0) {
        fprintf(stderr, "Error: Test9 #9\n");
        return -1;
    }

    if (strcmp(keys["nickname"].c_str(), "Hunhun") != 0) {
        fprintf(stderr, "Error: Test9 #10\n");
        return -1;
    }

    if (strcmp(keys["age"].c_str(), "5") != 0) {
        fprintf(stderr, "Error: Test9 #11\n");
        return -1;
    }

    Keys keys1, keys2;
    long i;
    for (i = 0; i < 10000; i++) {
        keys1[String(i)] = "aaa";
    }
    if (keys1.max() != 10000) {
        fprintf(stderr, "Error: Test9 #12\n");
        return -1;
    }
    for (i = 50; i < 500; i++) {
        keys1.delKey(String(i));
    }
    if (keys1.max() != 10000 - (500-50)) {
        fprintf(stderr, "Error: Test9 #13\n");
        return -1;
    }

    keys1.saveFile("test9.txt");
    keys2.loadFile("test9.txt");
    if (keys2.max() != 10000 - (500-50)) {
        fprintf(stderr, "Error: Test9 #13\n");
        return -1;
    }

    // Clear Allocated Memories (option)
    str_a.clear();
    keys.clear();
    keys1.clear();
    keys2.clear();

    return 0;
}


/*! Test10  Sheet Class
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int test10() {
    // Declare Sheet
    Sheet sheet1, sheet2;

    // Set Values
    sheet1.addCol("COL1");
    sheet1.addCol("COL2");
    sheet1.addRow();
    sheet1.setValue("COL1", 0, "abc");
    sheet1.setValue("COL2", 0, "123");
    sheet1.addRow();
    sheet1.setValue("COL1", 1, "A");
    sheet1.setValue("COL2", 1, "1");
    sheet1.delRow(1);
    sheet1.addRow();
    sheet1.setValue("COL1", 1, "b\"cd");
    sheet1.setValue("COL2", 1, "345");
    sheet1.insertRow(1);
    sheet1.setValue("COL1", 1, "xyz");
    sheet1.setValue("COL2", 1, "987");

    // Expected Results
    // sheet1(COL1, 0) == "abc"
    // sheet1(COL2, 0) == "123"
    // sheet1(COL1, 1) == "xyz"
    // sheet1(COL2, 1) == "987"
    // sheet1(COL1, 2) == "b\"cd"
    // sheet1(COL2, 2) == "345"

    if (sheet1.getValue("COL1", 0) != "abc") {
        fprintf(stderr, "Error: Test10 #1\n");
        return -1;
    }

    if (sheet1.getValue("COL2", 0) != "123") {
        fprintf(stderr, "Error: Test10 #2\n");
        return -1;
    }

    if (sheet1.getValue("COL1", 1) != "xyz") {
        fprintf(stderr, "Error: Test10 #3\n");
        return -1;
    }

    if (sheet1.getValue("COL2", 1) != "987") {
        fprintf(stderr, "Error: Test10 #4\n");
        return -1;
    }

    if (sheet1.getValue("COL1", 2) != "b\"cd") {
        fprintf(stderr, "Error: Test10 #5\n");
        return -1;
    }

    if (sheet1.getValue("COL2", 2) != "345") {
        fprintf(stderr, "Error: Test10 #6\n");
        return -1;
    }

    sheet1.setValue("COL2", 2, "0456");

    if (sheet1.getValue("COL2", 2) != "0456") {
        fprintf(stderr, "Error: Test10 #7\n");
        return -1;
    }

    sheet1.sort("COL1");

    if (sheet1.getValue("COL2", 2) != "987") {
        fprintf(stderr, "Error: Test10 #8\n");
        return -1;
    }

    sheet1.sortnumr("COL2");
    if (sheet1.getValue("COL2", 2) != "123") {
        fprintf(stderr, "Error: Test10 #9\n");
        return -1;
    }

    if (sheet1.getCSV() != "COL1,COL2\nxyz,987\n\"b\"\"cd\",\"0456\"\nabc,123\n") {
        fprintf(stderr, "Error: Test10 #10\n");
        return -1;
    }

    sheet2.setCSV(sheet1.getCSV());
    if (sheet1 != sheet2) {
        fprintf(stderr, "Error: Test10 #11\n");
        return -1;
    }

    // Clear Allocated Memories (option)
    sheet1.clear();

    return 0;
}


/*! Test11  DateTime Class
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int test11() {
    // Declare Strings
    String str_a, str_b, str_c, str_d, str_e;
    DateTime datetime1, datetime2, datetime3;

    // Set Values
    str_a = "Wed, 26 Dec 2007 14:15:19 +0900 (JST)";  // Set String
    datetime1.set(str_a);
    str_b = datetime1.toString("%04Y/%02m/%02d %02H:%02M:%02S %Z");
    datetime2.set("20071226141520+0900");
    str_c = datetime2.toString(DATEFORMAT_RFC822);
    datetime3 = datetime2;
    datetime3.adjustDate(10);
    str_d = datetime3.toString("%04Y-%02m-%02dT%02H:%02M:%02S%Z (%A)");
    str_e = datetime3.getMessage("DATETIME_WDAY_SEC", "en");

    // Expected Results
    // str_a = Wed, 26 Dec 2007 14:15:19 +0900 (JST)
    // str_b = 2007/12/26 14:15:19 +09:00
    // str_c = Wed, 26 Dec 2007 14:15:20 +0900
    // str_d = 2008-01-05T14:15:20+09:00 (Saturday)
    // str_e = 14:15:20 Sat January 5, 2008

    if (strcmp(str_b.c_str(), "2007/12/26 14:15:19 +09:00") != 0) {
        fprintf(stderr, "Error: Test11 #1\n");
        return -1;
    }

    if (strcmp(str_c.c_str(), "Wed, 26 Dec 2007 14:15:20 +0900") != 0) {
        fprintf(stderr, "Error: Test11 #2\n");
        return -1;
    }

    if (datetime1 >= datetime2) {
        fprintf(stderr, "Error: Test11 #3\n");
        return -1;
    }

    if (strcmp(str_d.c_str(), "2008-01-05T14:15:20+09:00 (Saturday)") != 0) {
        fprintf(stderr, "Error: Test11 #4\n");
        return -1;
    }

    if (strcmp(str_e.c_str(), "14:15:20 Sat January 5, 2008") != 0) {
        fprintf(stderr, "Error: Test12 #4\n");
        return -1;
    }

    // Clear Allocated Memories (option)
    str_a.clear();
    str_b.clear();
    str_c.clear();
    str_d.clear();
    str_e.clear();
    datetime1.clear();
    datetime2.clear();
    datetime3.clear();

    return 0;
}


/*! Test12  MIMEHeader Class
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int test12() {
    // Declare Strings
    String str_a;
    MIMEHeader header;

    // Set Values
    str_a = "Content-Type: text/plain; charset=UTF-8\r\n"
            "MIME-Version: 1.0\r\n"
            "Spam-Check: 1\r\n"
            " checked\r\n";

    // Expected Results
    // header.getValue("CONTENT-TYPE") == "text/plain; charset=UTF-8"
    // header.getValue("mime-version") == "1.0"
    // header.getValue("Spam-Check") == "1\r\n checked"

    header.set(str_a);

    if (strcmp(header.getValue("CONTENT-TYPE").c_str(), "text/plain; charset=UTF-8") != 0) {
        fprintf(stderr, "Error: Test12 #1\n");
        return -1;
    }

    if (strcmp(header.getValue("mime-version").c_str(), "1.0") != 0) {
        fprintf(stderr, "Error: Test12 #2\n");
        return -1;
    }

    if (strcmp(header.getValue("Spam-Check").c_str(), "1\r\n checked") != 0) {
        fprintf(stderr, "Error: Test12 #3\n");
        return -1;
    }

    if (strcmp(header.toString(), "Content-Type: text/plain; charset=UTF-8\r\n"
               "MIME-Version: 1.0\r\n"
               "Spam-Check: 1\r\n"
               " checked\r\n") != 0) {
        fprintf(stderr, "Error: Test12 #4\n");
        return -1;
    }

    // Clear Allocated Memories (option)
    str_a.clear();
    header.clear();

    return 0;
}


/*! Example1  Socket Class
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int example1() {
    // Declare Strings
    String str_a;
    Socket strsock;

    // Set Timeout
    strsock.setTimeout(5);

    // Connect
    strsock.connect("www.kantei.go.jp", "http");

    // Send Data
    strsock.send("GET / HTTP/1.1\r\n");
    strsock.send("HOST: www.kantei.go.jp\r\n");
    strsock.send("CONNECTION: close\r\n");
    strsock.send("\r\n");

    // Receive Data
    str_a = "";
    while (strsock.error() == 0 && 0 < strsock.receiveLine().len()) {
        str_a += strsock.receivedData();
    }

    // Disconnect
    strsock.disconnect();

    // Change Character-Set
    str_a = str_a.strconv("SHIFT_JIS", "UTF-8");

    // Output
    printf("[example1]\n");
    printf("str_a = %s\n",str_a.c_str());

    // Clear Allocated Memories (option)
    str_a.clear();
    strsock.clear();

    return 0;
}


/*! Example2  Convering GBK Character-Set
    @param  void
    @retval 0  success
    @retval -1 failure
 */
int example2() {
    // Declare Strings
    String str_a;

    // Load Text File That Encoded In GBK
    str_a.loadFile("gbk.txt");

    // Output
    printf("[example2]\n");
    printf("str_a.len() = %ld\n",str_a.len());

    // Save Text in UTF-8
    str_a.strconv("GBK", "UTF-8").saveFile("utf8.txt");

    // Load File And Save in GBK
    str_a.loadFile("utf8.txt");
    str_a.strconv("UTF-8", "GBK").saveFile("gbk-reconvert.txt");

    str_a.clear();

    return 0;
}
