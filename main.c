/*
    Program to convert .tex to .txt via extracting only sections,
    subsections, subsubsections, and their text.
*/
#include "local.h"
int main(int argc,char *argv[])
{
    /* arg guard */
    if(argc!=3)
    {
        fprintf(stderr,"ERROR; incorrect usage. Usage is:\nla2xt path_to_texfile name_of_txtfile\n");
        exit(EXIT_FAILURE);
    }
    /* see if files exist*/
    struct stat *stat_buf=(struct stat *)calloc(sizeof(struct stat),1);
    if(stat_buf==NULL)
    {
        fprintf(stderr,"ERROR; calloc returns NULL for i_stat_buf.\n");
        exit(EXIT_FAILURE);
    }
    stat(argv[2],stat_buf);
    if(stat_buf->st_size!=0)
    {
        fprintf(stderr,"ERROR; output file already exists. Program terminating.\n");
        exit(EXIT_FAILURE);
    }
    if(stat(argv[1],stat_buf)==-1)
    {
        if(stat_buf->st_size==0)
        {
            fprintf(stderr,"ERROR; input file does not exist. Program terminating.\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            fprintf(stderr,"ERROR; stat returned -1 for input file; do you have appropriate permissions? Program terminating.\n");
            exit(EXIT_FAILURE);
        }
    }
    /* Load the .tex file into memory */
    FILE *fp=fopen(argv[1],"r");
    if(fp==NULL)
    {
        fprintf(stderr,"ERROR: fopen returns NULL for %s.\n",argv[1]);
        exit(EXIT_FAILURE);
    }
    char *char_buf=calloc(sizeof(char),stat_buf->st_size);
    if(char_buf==NULL)
    {
        fprintf(stderr,"ERROR: calloc returns NULL for char_buf.\n");
        exit(EXIT_FAILURE);
    }
    for(size_t i=0;i<stat_buf->st_size;i++)
    {
        *(char_buf+i)=fgetc(fp);
    }
    fclose(fp);
    /* Write to .txt file */
    fp=fopen(argv[2],"w");
    if(fp==NULL)
    {
        fprintf(stderr,"ERROR: fopen returns NULL for %s.\n",argv[2]);
        exit(EXIT_FAILURE);
    }
    /*
        Find fragments that start with \ and see if they match the list in local.h.
        If so, extract the text inside them, and in the headings.
    */
    size_t max_len=0;
    if(NWORDS==0)
    {
        fprintf(stderr,"ERROR: NWORDS==0; set keywords in header.\n");
        exit(EXIT_FAILURE);
    }
    for(size_t i=0;i<NWORDS;i++)
    {
        size_t compare=strnlen(keywords[i].word,20);
        if(compare>max_len)
        {
            max_len=compare;
        }
    }
    if(max_len==0)
    {
        fprintf(stderr,"ERROR: max_len==0; set keywords in header.\n");
        exit(EXIT_FAILURE);
    }
    char *word_buf=calloc(sizeof(char),1+max_len);
    if(word_buf==NULL)
    {
        fprintf(stderr,"ERROR: calloc returns NULL for word_buf.\n");
        exit(EXIT_FAILURE);
    }
    bool in_block=false;
    bool in_comment=false;
    for(size_t i=0;i<stat_buf->st_size;i++)
    {
        if(*(char_buf+i)=='\\')
        {
            in_block=false;
            if(i+max_len>stat_buf->st_size)
            {
                fprintf(stderr,"ERROR: check .tex formatting.\n");
                exit(EXIT_FAILURE);
            }
            i++;
            for(size_t j=0;j<max_len;j++)
            {
                if(*(char_buf+i)=='{')
                {
                    break;
                }
                else
                {
                    *(word_buf+j)=*(char_buf+i);
                }
                i++;           
            }
            for(size_t j=0;j<NWORDS;j++)
            {
                if(strcmp(word_buf,keywords[j].word)==0)
                {
                    in_block=true;
                }
            }
            for(size_t j=0;j<max_len;j++)
            {
                *(word_buf+j)=0;
            }
        }
        if(in_block==true)
        {
            if(in_comment==false)
            {
                if(*(char_buf+i)=='%')
                {
                    in_comment=true;
                }
                else if(*(char_buf+i)=='{'||*(char_buf+i)=='}')
                {
                    continue;
                }
                else if(fputc(*(char_buf+i),fp)==EOF)
                {
                    fprintf(stderr,"ERROR: fputc returned EOF.\n");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                if(*(char_buf+i)=='\n')
                {
                    in_comment=false;
                }
            }            
        }
    }
    fclose(fp);
    printf("Input .tex file %s was converted to %s\n",argv[1],argv[2]);
    free(word_buf);
    free(stat_buf);
    free(char_buf);
    return 0;
}
