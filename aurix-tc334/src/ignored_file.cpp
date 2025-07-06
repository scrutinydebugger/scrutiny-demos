/*
None of these variables will appear in Scrutiny as they will be filtered out of the SFD during the postbuild stage.
The cmake requires to ignore that file. See: CU_IGNORE_PATTERNS
*/
static volatile int ignored_static_int = 0;
volatile int ignored_global_int = 0;
