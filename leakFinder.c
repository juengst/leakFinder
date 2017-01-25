/*
    leakFinder.c

    Copyright © 2016 Stuart Juengst. All Rights Reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
    to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,      and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifndef NULL
#define NULL (void *)0
#endif

typedef struct listEntry
{
    void *p;
    char *file;
    int line;
    struct listEntry *next;
} listEntry_t;

listEntry_t head;

void addToList(void *p, char *file, int line)
{
    listEntry_t *entry = &head;
    listEntry_t *newEntry;

    while(entry->next != NULL)
    {
        entry = entry->next;
    }
    if((newEntry = malloc(sizeof(listEntry_t))) != NULL)
    {
        newEntry->p = p;
        newEntry->file = malloc(strlen(file)+1);
        strcpy(newEntry->file,file);
        newEntry->line = line;
        newEntry->next = NULL;
        entry->next = newEntry;
    }
    else
        printf("couldn't malloc new entry!\n");
}

void removeFromList(void *p, char *s, int line)
{
    listEntry_t *entry = &head;
    listEntry_t *deadEntry;

    while(entry->next != NULL)
    {
        if(entry->next->p == p)
        {
            deadEntry = entry->next;
            entry->next = entry->next->next;
            free(deadEntry->file);
            free(deadEntry);
        }
        else
            entry = entry->next;
    }
}

void printList(void)
{
    listEntry_t *entry = head.next;

    while(entry != NULL)
    {
        printf("0x%x %s line %d\n",(unsigned int)(unsigned long)(entry->p),entry->file,entry->line);
        entry = entry->next;
    }
}

int main(int argc, char **argv)
{
    char *logFile = argv[1];
    FILE *infp;
    int i;
    char buffer[128];
    void *p;
    char file[128];
    int line;

    if(argc <2)
        printf("Usage: leakFinder <logFileName>\n");
    else
    {
        head.p = NULL;
        head.next = NULL;
        if((infp = fopen(logFile,"r")) != NULL)
        {
            // Find all the mallocs
            while(!feof(infp))
            {
                fgets(buffer,128,infp);
                switch(buffer[0])
                {
                    case 'M':
                        if((i=sscanf(buffer,"M 0x%x %s %d\n",(unsigned int *)&p,file,&line)) == 3)
                        {
                            addToList(p,file,line);
                        }
                        else
                            printf("scanf failed to parse %s: %d\n",buffer,i);
                        break;
                    case 'F':
                        if((i=sscanf(buffer,"F 0x%x %s %d\n",(unsigned int *)&p,file,&line)) == 3)
                        {
                            removeFromList(p,file,line);
                        }
                        else
                            printf("scanf failed to parse %s: %d\n",buffer,i);
                        break;
                }
            }
            // print out the resulting list
            printList();
        }
        else
            printf("Couldn't open %s: %d\n",logFile, errno);
    }
}

