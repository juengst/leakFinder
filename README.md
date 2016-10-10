# leakFinder
A tool to find memory leaks.

I've written this tool several times throughout my career, and it's an easy way to find memory leaks.  I'm publishing it here in hopes it will help somebody someday (also so I don't have to rewrite it ever again!).

This implementation only tracks malloc() and free(). If you use calloc(), realloc(), etc, you'll want to extend this code to handle them too.  Similarly, the techniques used here could be adapted for other memory-allocation systems (new/delete, RTOS allocators, etc).

#usage:

1. Globally search and replace all calls in your application to malloc() with MALLOC(), and calls to free() with FREE().  

2. Include leakFinder_wrappers.h in all your files that call MALLOC() and/or FREE().

3. Add leakFinder_wrappers.c to your build.

4. Exercise your application with complete code coverage.  All mallocs and frees will be logged to stdout.  You'll want to redirect the output to a file.

5. Compile the stand-alone leakFinder tool: gcc leakFinder.c -o leakFinder

6. Run leakFinder on your log file and it will print out all the mallocs which were never freed, by source-file name and line number.
