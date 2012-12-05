/*
 * PROJECT: GEM-Tools library
 * FILE: gt_suite_input_map_parser.c
 * DATE: 03/10/2012
 * DESCRIPTION: // TODO
 */

#include "gt_test.h"

gt_template* source;
gt_template* target;

void gt_template_utils_setup(void) {
  source = gt_template_new();
  target = gt_template_new();
}

void gt_template_utils_teardown(void) {
  gt_template_delete(source);
  gt_template_delete(target);
}

START_TEST(gt_test_template_merge)
{

  fail_unless(gt_input_map_parse_template(
      "ID\tACGT\t####\t1\tchr1:-:20:4",source)==0);
  fail_unless(gt_input_map_parse_template(
      "ID\tACGT\t####\t1:1\tchr1:-:20:4,chr9:+:50:2C1",target)==0);
  // merge into source
  gt_template_merge_template_mmaps(source,target);
  gt_string* string = gt_string_new(1024);
  gt_output_map_sprint_template(string, source, GT_ALL, true);
  // convert to string
  char * line = gt_string_get_string(string);
  fail_unless(gt_streq(line,"ID\tACGT\t####\t1+1\tchr1:-:20:4,chr9:+:50:2C1\n"));
}
END_TEST

START_TEST(gt_test_template_merge_hang)
{

  fail_unless(gt_input_map_parse_template(
      "HWI-962:71:D0PEYACXX:4:1101:18640:2354/2\tCGCGCGGGAGCCAGCAGGAGCACCAGCTGCGCAGGCAGGTTGAACTGCTGGCTTATAAAGTAGAGCAGGAGAAGT\t"
      "@CCFFFFFHHHHHIJIJJIJJJJJJJIIJGIDGIHHHFF6>CCCDEDDDDCBDD>CCDCC>CCCCCDDDDDBDC>\t0:0:0:0:0:0\t-",source)==0);
  fail_unless(gt_input_map_parse_template(
      "HWI-962:71:D0PEYACXX:4:1101:18640:2354/2\tCGCGCGGGAGCCAGCAGGAGCACCAGCTGCGCAGGCAGGTTGAACTGCTGGCTTATAAAGTAGAGCAGGAGAAGT\t"
      "@CCFFFFFHHHHHIJIJJIJJJJJJJIIJGIDGIHHHFF6>CCCDEDDDDCBDD>CCDCC>CCCCCDDDDDBDC>\t0:1\tchr21:+:47848466:39>1419*36",target)==0);
  // merge into source
  gt_template_merge_template_mmaps(source,target);
  gt_string* string = gt_string_new(1024);
  gt_output_map_sprint_template(string, source, GT_ALL, true);
  // convert to string
  char * line = gt_string_get_string(string);
  fail_unless(gt_streq(line,"HWI-962:71:D0PEYACXX:4:1101:18640:2354/2\tCGCGCGGGAGCCAGCAGGAGCACCAGCTGCGCAGGCAGGTTGAACTGCTGGCTTATAAAGTAGAGCAGGAGAAGT\t"
      "@CCFFFFFHHHHHIJIJJIJJJJJJJIIJGIDGIHHHFF6>CCCDEDDDDCBDD>CCDCC>CCCCCDDDDDBDC>\t0:1:0:0:0:0\tchr21:+:47848466:39>1419*36\n"));
}
END_TEST


START_TEST(gt_test_template_merge_error)
{

  fail_unless(gt_input_map_parse_template(
      "HISEQ8_0071:3:1101:19107:2010#TGACCA/1\t"
      "NGTCATGAGTGCAAAATGCAAATGCAAGTTTGGCCAGAAGTCCGGTCACCATCCAGGGGAGACTCCACCTCTCATCACCCCAGGCTCAGCCCAAAGCTGAT\t"
      "BPYcceeegegggiiiiiiiiiiiiiiighhhfgghhiiihhhifhfhhhfhhdghhiiihfbggedddeabbcdcccb`ZaW[^^abbcGW[`^`R]`BB\t"
      "0:0:0:0:1\t"
      "chr19:+:35613736:C7>78*37>316*38>132*18",source)==0);
  fail_unless(gt_input_map_parse_template(
      "HISEQ8_0071:3:1101:19107:2010#TGACCA/1\t"
      "NGTCATGAGTGCAAAATGCAAATGCAAGTTTGGCCAGAAGTCCGGTCACCATCCAGGGGAGACTCCACCTCTCATCACCCCAGGCTCAGCCCAAAGCTGAT\t"
      "BPYcceeegegggiiiiiiiiiiiiiiighhhfgghhiiihhhifhfhhhfhhdghhiiihfbggedddeabbcdcccb`ZaW[^^abbcGW[`^`R]`BB\t"
      "0:0:0:0:1+0:1\t"
      "chr19:+:35613741:(5)3>78*37>316*36(20),chr19:+:35613819:(5)CAG37>316*36(20)", target)==0);
  // merge into source
  gt_template_merge_template_mmaps(source,target);
  gt_string* string = gt_string_new(1024);
  gt_output_map_sprint_template(string, source, GT_ALL, true);
  // convert to string
  char * line = gt_string_get_string(string);
  fail_unless(gt_streq(line,
      "HISEQ8_0071:3:1101:19107:2010#TGACCA/1\t"
      "NGTCATGAGTGCAAAATGCAAATGCAAGTTTGGCCAGAAGTCCGGTCACCATCCAGGGGAGACTCCACCTCTCATCACCCCAGGCTCAGCCCAAAGCTGAT\t"
      "BPYcceeegegggiiiiiiiiiiiiiiighhhfgghhiiihhhifhfhhhfhhdghhiiihfbggedddeabbcdcccb`ZaW[^^abbcGW[`^`R]`BB\t"
      "0:0:0:0:2+0:1\t"
      "chr19:+:35613736:C7>78*37>316*38>132*18,chr19:+:35613741:(5)3>78*37>316*36(20),chr19:+:35613819:(5)CAG37>316*36(20)\n"));
}
END_TEST

START_TEST(gt_test_template_merge_inconsistent)
{

  fail_unless(gt_input_map_parse_template(
      "ID/1\t"
      "GAGAGAACAGGCCTCTGAGCCCAAGCCAAGCCATCGCATCCCCTGTGACTTGCCCGTATATATGCCCAGATGGCCTGAAGTAACTGAAGAATCACAAAAGA\t"
      "BPYcceeegegggiiiiiiiiiiiiiiighhhfgghhiiihhhifhfhhhfhhdghhiiihfbggedddeabbcdcccb`ZaW[^^abbcGW[`^`R]`BB\t"
      "0:0:0:0:1:4+27:46:45:34:12:16:3:0:1\t"
      "chr7:+:24435865:1T>5-46A8C38,chr2:+:64479547:>1-1T>1-1T>1-94,chr4:+:16999371:1T>5-35>2-9A8C38,chr9:+:90025610:1T>5-28A17A8C38,chr11:-:130623749:2>1-1T1T46A2>2-43",source)==0);
  fail_unless(gt_input_map_parse_template(
      "ID/1\t"
      "GAGAGAACAGGCCTCTGAGCCCAAGCCAAGCCATCGCATCCCCTGTGACTTGCCCGTATATATGCCCAGATGGCCTGAAGTAACTGAAGAATCACAAAAGA\t"
      "BPYcceeegegggiiiiiiiiiiiiiiighhhfgghhiiihhhifhfhhhfhhdghhiiihfbggedddeabbcdcccb`ZaW[^^abbcGW[`^`R]`BB\t"
      "0:1:0:0:0:0:1:3\t"
      "chr2:+:64479108:10>436*91,chr2:+:64479544:TTC1TGT94,chr13:+:91496959:1GA1TGT27T18A47,chr2:+:216792393:1GA1TGT46A8C38,chr2:+:64478959:AG1T1G1TT1>585*91",target)==0);
  // merge into source
  gt_template_recalculate_counters(source);
  gt_template_merge_template_mmaps(source,target);
  gt_string* string = gt_string_new(1024);
  gt_output_map_sprint_template(string, source, GT_ALL, true);
  // convert to string
  char * line = gt_string_get_string(string);
  fail_unless(gt_streq(line,
      "ID/1\t"
      "GAGAGAACAGGCCTCTGAGCCCAAGCCAAGCCATCGCATCCCCTGTGACTTGCCCGTATATATGCCCAGATGGCCTGAAGTAACTGAAGAATCACAAAAGA\t"
      "BPYcceeegegggiiiiiiiiiiiiiiighhhfgghhiiihhhifhfhhhfhhdghhiiihfbggedddeabbcdcccb`ZaW[^^abbcGW[`^`R]`BB\t"
      "0:1:0:0:1:4+1:3\t"
      "chr2:+:64479108:10>436*91,chr7:+:24435865:1T>5-46A8C38,chr2:+:64479547:(1)1T>1-1T>1-94,"
      "chr4:+:16999371:1T>5-35>2-9A8C38,chr9:+:90025610:1T>5-28A17A8C38,chr11:-:130623749:2>1-1T1T46A2>2-43,"
      "chr2:+:64479544:TTC1TGT94,chr13:+:91496959:1GA1TGT27T18A47,chr2:+:216792393:1GA1TGT46A8C38,chr2:+:64478959:AG1T1G1TT1>585*91\n"));
}
END_TEST


START_TEST(gt_test_template_to_string)
{

  fail_unless(gt_input_map_parse_template(
      "ID\tACGT\t####\t1\tchr1:-:20:4",source)==0);
  gt_string* string = gt_string_new(1024);
  gt_output_map_sprint_template(string, source, GT_ALL, true);
  // convert to string
  char * line = gt_string_get_string(string);
  fail_unless(gt_streq(line,"ID\tACGT\t####\t1\tchr1:-:20:4\n"));
}
END_TEST


START_TEST(gt_test_template_copy)
{

  // test no maps no mmaps
  fail_unless(gt_input_map_parse_template(
      "ID\tACGT\t####\t1\tchr1:-:20:4",source)==0);
  gt_template* copy = gt_template_copy(source, false, false);
  gt_string* string = gt_string_new(1024);
  gt_output_map_sprint_template(string, copy, GT_ALL, true);
  // convert to string for simple check
  char * line = gt_string_get_string(string);
  fail_unless(gt_streq(line,"ID\tACGT\t####\t0\t-\n"));
  gt_template_delete(copy);

  // test maps no mmaps
  gt_string_clear(string);
  copy = gt_template_copy(source, true, false);
  string = gt_string_new(1024);
  gt_output_map_sprint_template(string, copy, GT_ALL, true);
  // convert to string for simple check
  line = gt_string_get_string(string);
  fail_unless(gt_streq(line,"ID\tACGT\t####\t1\tchr1:-:20:4\n"));
  gt_template_delete(copy);

  // test maps no mmaps with multi map string
  fail_unless(gt_input_map_parse_template(
    "ID\tACGT\t####\t2\tchr1:-:20:4,chr2:-:40:4",source)==0);

  gt_string_clear(string);
  copy = gt_template_copy(source, true, false);
  string = gt_string_new(1024);
  gt_output_map_sprint_template(string, copy, GT_ALL, true);
  // convert to string for simple check
  line = gt_string_get_string(string);
  fail_unless(gt_streq(line,"ID\tACGT\t####\t2\tchr1:-:20:4,chr2:-:40:4\n"));
  gt_template_delete(copy);

  // test maps and mmaps
  gt_string_clear(string);
  copy = gt_template_copy(source, true, true);
  string = gt_string_new(1024);
  gt_output_map_sprint_template(string, copy, GT_ALL, true);
  // convert to string for simple check
  line = gt_string_get_string(string);
  fail_unless(gt_streq(line,"ID\tACGT\t####\t2\tchr1:-:20:4,chr2:-:40:4\n"));
  gt_template_delete(copy);

}
END_TEST

Suite *gt_template_utils_suite(void) {
  Suite *s = suite_create("gt_template_utils");

  /* String parsers test case */
  TCase *test_case = tcase_create("Template Utils");
  tcase_add_checked_fixture(test_case,gt_template_utils_setup,gt_template_utils_teardown);
  tcase_add_test(test_case,gt_test_template_merge);
  tcase_add_test(test_case,gt_test_template_merge_hang);
  tcase_add_test(test_case,gt_test_template_merge_error);
  tcase_add_test(test_case,gt_test_template_merge_inconsistent);
  tcase_add_test(test_case,gt_test_template_to_string);
  tcase_add_test(test_case,gt_test_template_copy);
  suite_add_tcase(s,test_case);

  return s;
}