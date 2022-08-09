/*+--------------------------------------------------------------------
  *             UNIFAL − Universidade Federal de Alfenas
  *              Bacharelado em Ciência da Computação
  *  Trabalho..: Imagem de Dados
  *  Disciplina: Processamento de Imagens
  *  Professor.: Luiz Eduardo da Silva
  *  Aluno.....: Arthur Rodrigues Proença
  *  Data......: 30/06/2022
  *  Greetings.: Luiz Eduardo da Silva for the imagelib.c file.
  +----------------------------------------------------------------------*/
#define CREATOR "# CREATOR: Image Processing using C-Ansi - ByDu\n"

typedef int *image;
typedef unsigned short int ui16;

#define ERROR(x, y) \
    if (x)          \
    y

enum
{
    BW = 1,
    GRAY,
    COLOR
};

#if defined(_WIN32) || defined(__WIN64__) || defined(__CYGWIN__)
#define VIEW "./i_view32"
#elif defined(__linux__)
#define VIEW "eog"
#endif

void errormsg(char *, ...);
image img_alloc(int, int);
int img_free(image);
void img_name(char *name, char *in, char *out, int tp);
image img_get(char *name, int *nr, int *nc, int *ml, int tp);
void img_put(image img, char *name, int nr, int nc, int ml, int tp);
char *img_getcompressed(char *name, int *nr, int *nc);
ui16 *lzw_decode(char *lzw_decode, int size);