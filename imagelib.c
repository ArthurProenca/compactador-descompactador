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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "imagelib.h"
#define PER_LINE 16

char b64_content[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                      'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                      'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                      'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

/*-------------------------------------------------------------------------
 * Image allocation and free routines
 *   nr = number of rows
 *   nc = number of columns
 *-------------------------------------------------------------------------*/
image img_alloc(int nr, int nc)
{
    return (image)malloc(nr * nc * sizeof(int));
}

int img_free(image Im)
{
    free(Im);
}

/*-------------------------------------------------------------------------
 * Define input and output image name
 * Params:
 *   name = image name file
 *   in = input image name
 *   out = output image name
 *   tp = image type (BW, GRAY, COLOR)
 *-------------------------------------------------------------------------*/
void img_name(char *name, char *in, char *out, int tp)
{
    char *p = strstr(name, ".l64");
    if (p)
        *p = 0;
    sprintf(in, "%s%s", name, ".l64");
    sprintf(out, "%s-result%s", name, ".l64");
}

/*-------------------------------------------------------------------------
 * Display image information
 *   name = file name image
 *   nr = number of rows
 *   nc = number os columns
 *   ml = max grayscale level
 *   tp = image type (1, 2 ou 3)
 *-------------------------------------------------------------------------*/
void img_info(char *name, int nr, int nc, int ml, int tp)
{
    printf("\nImage Informations:");
    printf("\n--------------------------\n");
    printf("Image file name.............: %s \n", name);
    printf("Image type..................: P%d\n", tp);
    printf("Number of rows..............: %d \n", nr);
    printf("Number of columns...........: %d \n", nc);
    printf("Max intensity level.........: %d \n\n", ml);
}

/*-------------------------------------------------------------------------
 * Error message
 *   str - string message control
 *   ... - parameters
 *-------------------------------------------------------------------------*/
void errormsg(char *str, ...)
{
    char format[255];
    va_list arg;
    va_start(arg, str);
    sprintf(format, "ERROR: %s\n\n", str);
    vprintf(format, arg);
    va_end(arg);
    exit(1);
}

/*-------------------------------------------------------------------------
 * Read pnm ascii image
 * Params (in):
 *   name = image file name
 *   tp = image type (BW, GRAY or COLOR)
 * Params (out):
 *   nr = number of rows
 *   nc = number of columns
 *   ml = max grayscale level
 * Returns:
 *   image vector
 *-------------------------------------------------------------------------+*/
image img_get(char *name, int *nr, int *nc, int *ml, int tp)
{
    char lines[100];
    image img;
    FILE *fimg;
    ERROR((fimg = fopen(name, "r")) == NULL, errormsg("File open error: <%s>", name));
    /*--- PNM = "P1" or "P2" or "P3" ---*/
    fgets(lines, 80, fimg);
    ERROR((lines[0] != 'P' || (lines[1] != tp + '0')), errormsg("File type image error: <%s>", name));
    /*--- Comment lines ---*/
    fgets(lines, 80, fimg);
    while (strchr(lines, '#'))
        fgets(lines, 80, fimg);
    sscanf(lines, "%d %d", nc, nr);
    if (tp != BW)
        fscanf(fimg, "%d", ml);
    else
        *ml = 1;
    ERROR(*nc == 0 || *nr == 0 || *ml == 0, errormsg("Image dimensions error: <%s>", name));
    img = img_alloc(*nr, *nc);
    ERROR(!img, errormsg("Image allocation error: %s\n\n img_get routine", name));
    for (int i = 0; i < (*nr) * (*nc); i++)
        if (tp != COLOR)
        {
            int k;
            fscanf(fimg, "%d", &k);
            ERROR(k > *ml, errormsg("Max pixel intensity in the image error: <%s>", name));
            img[i] = k;
        }
        else
        {
            int r, g, b;
            fscanf(fimg, "%d %d %d", &r, &g, &b);
            ERROR(r > *ml || r < 0 || g > *ml || g < 0 || b > *ml || b < 0,
                  errormsg("Max intensity of color in the image error: <%s>", name));
            img[i] = (r << 16) + (g << 8) + b;
            printf("%d", img[i]);
        }
    fclose(fimg);
    img_info(name, *nr, *nc, *ml, tp);
    return img;
}

/*-------------------------------------------------------------------------
 * Write pnm image
 * Params:
 *   img = image
 *   name = image file name
 *   nr = number of rows
 *   nc = number of columns
 *   ml = max intensity level
 *   tp = image type (BW, GRAY or COLOR)
 *-------------------------------------------------------------------------*/
void img_put(image img, char *name, int nr, int nc, int ml, int tp)
{
    int count;
    FILE *fimg;
    ERROR((fimg = fopen(name, "wt")) == NULL, errormsg("Image creation error: <%s>", name));
    fprintf(fimg, "P%c\n", tp + '0');
    fputs(CREATOR, fimg);
    fprintf(fimg, "%d  %d\n", nc, nr);
    if (tp != BW)
        fprintf(fimg, "%d\n", ml);
    count = 0;
    for (int i = 0; i < nr * nc; i++)
    {
        if (tp != COLOR)
        {
            int x = img[i];
            fprintf(fimg, "%3d ", x);
        }
        else
        {
            int r = (img[i] >> 16) & 0xFF;
            int g = (img[i] >> 8) & 0xFF;
            int b = img[i] & 0xFF;
            fprintf(fimg, "%3d %3d %3d ", r, g, b);
        }
        count++;
        if (count > PER_LINE)
        {
            fprintf(fimg, "\n");
            count = 0;
        }
    }
    fclose(fimg);
}

image compact_get(char *name, int *nr, int *nc, int *ml, int tp)
{
    char lines[100];
    image img;
    FILE *fimg;
    ERROR((fimg = fopen(name, "r")) == NULL, errormsg("File open error: <%s>", name));
    /*--- PNM = "P1" or "P2" or "P3" ---*/
    fgets(lines, 80, fimg);
    ERROR((lines[0] != 'P' || (lines[1] != tp + '0')), errormsg("File type image error: <%s>", name));
    /*--- Comment lines ---*/
    fgets(lines, 80, fimg);
    while (strchr(lines, '#'))
        fgets(lines, 80, fimg);
    sscanf(lines, "%d %d", nc, nr);
    if (tp != BW)
        fscanf(fimg, "%d", ml);
    else
        *ml = 1;
    ERROR(*nc == 0 || *nr == 0 || *ml == 0, errormsg("Image dimensions error: <%s>", name));
    img = img_alloc(*nr, *nc);
    ERROR(!img, errormsg("Image allocation error: %s\n\n img_get routine", name));
    for (int i = 0; i < (*nr) * (*nc); i++)
        if (tp != COLOR)
        {
            int k;
            fscanf(fimg, "%d", &k);
            ERROR(k > *ml, errormsg("Max pixel intensity in the image error: <%s>", name));
            img[i] = k;
        }
        else
        {
            int r, g, b;
            fscanf(fimg, "%d %d %d", &r, &g, &b);
            ERROR(r > *ml || r < 0 || g > *ml || g < 0 || b > *ml || b < 0,
                  errormsg("Max intensity of color in the image error: <%s>", name));
            img[i] = (r << 16) + (g << 8) + b;
            printf("%d", img[i]);
        }
    fclose(fimg);
    img_info(name, *nr, *nc, *ml, tp);
    return img;
}

char *img_getcompressed(char *name, int *nr, int *nc)
{
    FILE *fimg;
    char lines[100];
    ERROR((fimg = fopen(name, "r")) == NULL, errormsg("File open error: <%s>", name));
    fgets(lines, 8, fimg);
    fscanf(fimg, "%d %d", nc, nr);
    ERROR(*nc == 0 || *nr == 0, errormsg("Image dimensions error: <%s>", name));
    char ch;
    char *content = malloc(sizeof(char) * (*nr * *nc));
    int i = 0;

    while (ch != EOF)
    {
        ch = fgetc(fimg);
        if (ch != '\n')
        {
            content[i] = ch;
            i++;
        }
    }
    return content;
}

int find_in_array(char r)
{
    int j = 0;
    while (j < 64 && r != b64_content[j])
    {
        j++;
    }
    return j;
}

char *convert(int n)
{
    long long bin = 0;
    int rem, i = 1;

    while (n != 0)
    {
        rem = n % 2;
        n /= 2;
        bin += rem * i;
        i *= 10;
    }
    char *str = malloc(sizeof(char) * 256);
    sprintf(str, "%lld", bin);
    char *str_retorno = malloc(sizeof(char) * 6);
    for (int i = 0; i < 6 - strlen(str); i++)
    {
        str_retorno[i] = '0';
    }
    strcat(str_retorno, str);
    return str_retorno;
}

int strbin_to_int(char *str)
{
    char input[12 + 1] = {'\0'};
    int i = 0;

    for (; i < 12; i++)
    {
        input[i] = str[i];
    }

    int result = 0;
    for (int j = 0; j < i; j++)
    {
        // EDIT1: move shift statement to create proper result
        result <<= 1;

        if (input[j] == '1')
        {
            result |= 1;
        }
    }

    return result;
}

ui16 *lzw_decode(char *lzw_decode, int size)
{
    char cumulator[2];
    int cont = 0;
    ui16 *lzw_decoded = malloc(sizeof(ui16) * size);
    for (int i = 0; i < strlen(lzw_decode); i += 2)
    {
        if (i + 3 > strlen(lzw_decode))
            break;
        cumulator[0] = lzw_decode[i];
        cumulator[1] = lzw_decode[i + 1];

        char *s, *s1;

        s = convert(find_in_array(cumulator[0]));
        s1 = convert(find_in_array(cumulator[1]));
        strcat(s, s1);
       //printf("%s", s);
        lzw_decoded[cont] = strbin_to_int(s);
        cont++;
    }

    // printf("%d", lzw_decoded[2]);
    // int a, b;

    // a = find_in_array('E');
    // b = find_in_array('A');
    // printf("%i %i\n", a, b);
    // char *s = convert(b);
    // char *s1 = convert(a);
    // strcat(s1, s);
    // printf("%i", strbin_to_int(s1));

    return lzw_decoded;
}