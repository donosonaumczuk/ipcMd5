#include "./include/mainTest.h"

int main()
{
   CU_pSuite slaveSuite = NULL;
   CU_pSuite ShmBuffSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   slaveSuite = CU_add_suite("slaveSuite", initSlaveSuite, cleanSlaveSuite);
   ShmBuffSuite = CU_add_suite("ShmBuffSuite", initShmBuffSuite, cleanShmBuffSuite);
   if (NULL == slaveSuite || NULL == ShmBuffSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if((CU_add_test(slaveSuite, "test of Writting a Hash on a fifo", testWriteHashOnAFIFO) == NULL)
    || (CU_add_test(slaveSuite, "test of read a file path()", testReadAFilePath) == NULL)
    || (CU_add_test(slaveSuite, "test of read a quantity from a file()", testOfReadingTheQuantityOfFilePathsToHash) == NULL)
    || (CU_add_test(ShmBuffSuite, "test to write in share memory", testWriteInShmBuff) == NULL)
    || (CU_add_test(ShmBuffSuite, "test to read from share memory", testReadFromShmBuff) == NULL)
    || (CU_add_test(ShmBuffSuite, "test to write in share memory after read", testWriteInShmBuffAfterRead) == NULL)
    || (CU_add_test(ShmBuffSuite, "test to read in share memory after read and write", testReadInShmBuffAfterReadAndWrite) == NULL)
    || (CU_add_test(ShmBuffSuite, "test to write and read in share memory whit two process", testReadAndWriteDifferentProcess) == NULL)) {
      CU_cleanup_registry();
      return CU_get_error();
    }
   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   printf("\n");
   return CU_get_error();
}
