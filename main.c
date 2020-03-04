#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include<string.h>

  typedef struct { unsigned char R;
                   unsigned char G;
                   unsigned char B;}pixel;

  typedef struct { unsigned int x;
                   unsigned int y;
                   float prag;
                   pixel culoare;
                  }detectie;


  /* calculam latimea imaginii sursa*/

    unsigned int latime_imagine  ( char * nume_fisier_sursa)
   {
       FILE * fin;
       fin=fopen(nume_fisier_sursa,"rb");
       if(fin==NULL)
       {

           return 0 ;

       }

       unsigned int latime_imagine;
       fseek(fin,18,SEEK_SET);
       fread(&latime_imagine,sizeof(unsigned int),1,fin);

       fclose(fin);

       return latime_imagine;


   }

   /*calculam inaltimea imaginii sursa */

    unsigned int inaltime_imagine  (char * nume_fisier_sursa)
   {
       FILE * fin;
       fin=fopen(nume_fisier_sursa,"rb");
       if(fin==NULL)
       {

           return 0;

       }

       unsigned int inaltime_imagine;
       fseek(fin,22,SEEK_SET);
       fread(&inaltime_imagine,sizeof(unsigned int),1,fin);

       fclose(fin);

       return inaltime_imagine;
   }

   /* salvam intr-un vector header ul imaginii sursa*/

    void header_imagine_sursa ( char * nume_fisier_sursa, unsigned char **H)
   {

       FILE * fin;
       fin=fopen(nume_fisier_sursa,"rb");
       if(fin==NULL)
       {
           printf("Nu am gasit imaginea sursa din care citesc pentru pastrarea header-ului.");
           return ;
       }


       (*H)=(unsigned char *)malloc(54*sizeof(unsigned char ));
       if((*H)==NULL)
       {
           printf("Nu s-a putut aloca memoriei pentru construirea vectorului ce salveaza header-ul");
           return ;
       }


       int i;
       for(i=0;i<54;i++)
       {
           fread(&(*H)[i],sizeof(unsigned char),1,fin);

       }

       fclose(fin);

   }
   /* liniarizam imaginea sursa, construind o matrice*/

    void transform_matrice (char * nume_fisier_sursa, pixel ***M, unsigned int H_imagine, int W_imagine)
   {
       FILE *fin;
       fin=fopen(nume_fisier_sursa,"rb");
       if(fin==NULL)
       {
           printf("Nu am gasit imaginea sursa din care citesc pentru pastrarea imaginii intr-o matrice.");
           return ;
       }


       fseek(fin,54,SEEK_SET);


       *M=(pixel **)malloc(H_imagine*sizeof(pixel*));
       if((*M)==NULL)
       {
           printf("Nu s-a putut aloca memorie pentru matrice.");
           return ;
       }

       int padding;
       if(W_imagine%4!=0)
        padding=4-(3*W_imagine)%4;

       else

            padding=0;

       int i,j;
       pixel x;
       for(i=H_imagine-1;i>=0;i--)

       {  (*M)[i]=(pixel *)malloc(W_imagine*sizeof(pixel));
           for(j=0;j<W_imagine;j++)
           {

            //citesc fiecare pixel din imaginea sursa pentru fiecare canal de culoare
            fread(&x.B, 1,1,fin);
            fread(&x.G, 1,1,fin);
            fread(&x.R, 1,1,fin);
            (*M)[i][j]=x;
           }

        fseek(fin,padding,SEEK_CUR);
       }
       fclose(fin);

   }

   /*deliniarizam imaginea cu ajutorul vectorului de liniarizare si cu vectorul in care am pastrat header-ul imaginii sursa*/

    void deliniarizare_matrice( char *nume_fisier_sursa, char * nume_fisier_destinatie, pixel **M ,unsigned int H_imagine, unsigned int W_imagine )
   {
       FILE *fout;
       fout=fopen(nume_fisier_destinatie,"wb");

       unsigned char *H;
       header_imagine_sursa(nume_fisier_sursa,&H);

       int i,j;
       for(i=0;i<54;i++)
        fwrite(&H[i],sizeof(unsigned char),1,fout);

       int padding;
       if(W_imagine%4!=0)
        padding=4-(3*W_imagine)%4;
    else
        padding=0;

     pixel black;
     black.B=black.G=black.R=0;

       for(i=H_imagine-1;i>=0;i--)
       {int p=padding;

           for(j=0;j<W_imagine;j++)
           {


            fwrite(&M[i][j].B,sizeof(char),1,fout);
            fwrite(&M[i][j].G,sizeof(char),1,fout);
            fwrite(&M[i][j].R,sizeof(char),1,fout);
       }
           while(p!=0)/*afisam padding-ul sub forma unor pixeli negri */
            {fwrite(&black.B,1,1,fout);
             p--;}


   }
   fclose(fout);
   free(H);/* eliberam memoria in care pastram headerul */
   }

   /* transformarea imaginii color in una grayscale */
    void grayscale (char * nume_fisier_sursa, char * nume_fisier_destinatie, unsigned int W_imagine, unsigned int H_imagine)
    {

        FILE *fin, * fout;
        fin=fopen(nume_fisier_sursa,"rb");
        fout=fopen(nume_fisier_destinatie,"wb");
        if(fin==NULL)
        {
            printf("Eroare la deschiderea fisierului ce contine imaginea color.");
            return ;
        }
        if(fout==NULL)
        {
            printf("Eroare la deschiderea fisierului ce va contine imaginea grayscale.");
            return ;
        }

        unsigned char *H;
        header_imagine_sursa(nume_fisier_sursa,&H);
        int i,j;
        for(i=0;i<54;i++)
            fwrite(&H[i],sizeof(unsigned char),1,fout);

        fseek(fin,54,SEEK_SET);

        int padding;
       if(W_imagine%4!=0)
        padding=4-(3*W_imagine)%4;
       else
        padding=0;

        pixel x;
        unsigned char pixel_gri;
        pixel black;
        black.B=black.G=black.R=0;
        int p;

        for(i=0;i<H_imagine;i++)

        {  p=padding;
            for(j=0;j<W_imagine;j++)
            {

                fread(&x.B,1,1,fin);
                fread(&x.G,1,1,fin);
                fread(&x.R,1,1,fin);
                pixel_gri = 0.299*x.R+ 0.587*x.G + 0.114*x.B;
                x.B=x.G=x.R=pixel_gri;
                fwrite(&x.B,1,1,fout);
                fwrite(&x.G,1,1,fout);
                fwrite(&x.R,1,1,fout);
            }

            fseek(fin,padding,SEEK_CUR);
            while(p!=0)
            {
                fwrite(&black.B,1,1,fout);
                p--;
            }


        }
        fclose(fin);
        fclose(fout);
        free(H);

    }

    /* media valorilor intensitatilor grayscale a pixelilor din fereastra fI */
    float medie_intensitati_fI(pixel **M, unsigned i, unsigned j, unsigned W_sablon, unsigned H_sablon)
    {

        unsigned int p,q;
        float Suma=0,mediefI;
        for(p=i;p<(H_sablon+i);p++)
        {
            for(q=j;q<(W_sablon+j);q++)
                Suma+=M[p][q].R;

        }
        mediefI=Suma/(W_sablon*H_sablon);
        return mediefI;

    }

    /* deviatia standard a valorilor intensitatilor grayscale a pixelilor din fereastra fI */
    float  deviatia_standard_fI ( pixel **M, unsigned int i, unsigned int j, unsigned int W_sablon, unsigned int H_sablon)

    {

        unsigned int p, q;
        float suma=0,deviatiafI;
        float media=medie_intensitati_fI(M,i,j,W_sablon,H_sablon);
        for(p=i;p<(H_sablon+i);p++)
        {
            for(q=j;q<(W_sablon+j);q++)
                suma=suma +((M[p][q].R-media)*(M[p][q].R-media));
        }

        deviatiafI=sqrt( suma/(W_sablon*H_sablon));
         return deviatiafI;
    }
    /* media valorilor intensitatilor grayscale a pixelilor din fereastra sablonului */
    float medie_intensitati_sablon( pixel **S, unsigned int W_sablon, unsigned int H_sablon)
    {

        unsigned int i,j;
        float Suma=0,medieS;
        for(i=0;i<H_sablon;i++)
        {
            for(j=0;j<W_sablon;j++)
                Suma+=S[i][j].R;
        }
        medieS=Suma/(W_sablon*H_sablon);
        return medieS;
    }


   /* deviatia  standard a valorilio intensitatilor pixelilor */
    float deviatia_standard_sablon (pixel **S, unsigned int W_sablon, unsigned int H_sablon)

   {
       float media;
       media=medie_intensitati_sablon(S,W_sablon,H_sablon);

       unsigned int i,j;
       float suma=0;
       for(i=0;i<H_sablon;i++)
       {

           for(j=0;j<W_sablon;j++)
           {
            suma=suma+((S[i][j].R-media)*(S[i][j].R-media));
            }

       }
       float deviatiaS;
       deviatiaS=sqrt( suma/(W_sablon*H_sablon-1));
        return deviatiaS;



   }


    float corelatie ( pixel **M, pixel **S, unsigned int W_sablon, unsigned int H_sablon,unsigned int i, unsigned int j)
   {
       float corelatie;
       float suma=0;

       float deviatiaS=deviatia_standard_sablon(S,W_sablon,H_sablon);
       float deviatiafI;
       float medieS=medie_intensitati_sablon(S,W_sablon,H_sablon);
       float mediefI;
       deviatiafI=deviatia_standard_fI(M,i,j,W_sablon,H_sablon);
       mediefI=medie_intensitati_fI(M,i,j,W_sablon,H_sablon);
       unsigned int p,q;
       for(p=0;p<H_sablon;p++)

       {
           for(q=0;q<W_sablon;q++)

               suma=suma +(((M[p+i][q+j].R-mediefI)*(S[p][q].R-medieS))/(deviatiafI*deviatiaS));

   }
   corelatie=suma/(W_sablon*H_sablon);
   return corelatie;
   }


   /* template matching */
    void  template_matching(pixel **I, unsigned int W_imagine, unsigned int H_imagine,
                         pixel **S, unsigned int W_sablon, unsigned int H_sablon, float prag_furnizat,
                         detectie **D, unsigned int *k, pixel C, char * cale_sablon)

   {   transform_matrice(cale_sablon,&S,H_sablon,W_sablon);
       unsigned int i,j;
       detectie *aux;
       float corel;
       for(i=0;i<(H_imagine-H_sablon);i++)
       {
           for(j=0;j<(W_imagine-W_sablon);j++)
           {
               corel=corelatie(I,S,W_sablon,H_sablon,i,j);

               if(corel>prag_furnizat)

                {   (*k)++;
                    aux=(detectie *)realloc((*D),(*k)*sizeof(detectie));
                    if(aux==NULL)
                    {
                        printf("Nu s-a putut realoca memorie");
                        return ;
                    }
                    else
                    {
                        (*D)=aux;
                        (*D)[(*k)-1].x=i;
                        (*D)[*k-1].y=j;
                        (*D)[*k-1].prag=corel;
                        (*D)[*k-1].culoare.B=C.B;
                        (*D)[*k-1].culoare.G=C.G;
                        (*D)[*k-1].culoare.R=C.R;
                    }

                }

           }
       }
   }

       /* functia coloreaza pentru o o fereastra pentru care stim coordonatele i si j, linia, respectiv, coloana */
    void colorare (pixel **M, unsigned int i, unsigned int j,pixel C, unsigned int W_sablon, unsigned int H_sablon)
{
           unsigned p,q;
         for(p=i;p<(H_sablon+i+1);p++)
        {
           M[p][j]=C;
           M[p][j+W_sablon]=C;
        }

        for(q=j;q<(W_sablon+j+1);q++)
        {
                M[i][q]=C;
                M[i+H_sablon][q]=C;
        }
       }

    int cmpdescrescator( const void *a, const void *b)
      {
           detectie va=*(detectie *)a;
           detectie vb=*(detectie *)b;
           if(va.prag<vb.prag)
            return 1;
           if(va.prag>vb.prag)
            return -1;
           return 0;
       }


    unsigned int arie_intersectie(unsigned int i, unsigned int j, unsigned int p , unsigned int q, unsigned int W_sablon, unsigned int H_sablon)
{
      unsigned int lat1,lat2,A_intersectie;

    if(abs(i-p)>=H_sablon||abs(q-j)>=W_sablon)
        return 0;

    if(i==p)
    {
         if( j==q)
        {
         lat1=W_sablon;
         lat2=H_sablon;
        }
    else
        if( q>j)
       {
        lat1=j+W_sablon-q;
        lat2=H_sablon;
       }
    else
         if( q<j)
    {

        lat1=q+W_sablon-j;
        lat2=H_sablon;
    }}


        if(j==q )
          {if(i<p)
    {

        lat1=W_sablon;
        lat2=i+H_sablon-p;
    }
    else

           if( i>p)
    {

        lat1=W_sablon;
        lat2=p+H_sablon-i;
    }}

        if(i<p )
          {if( j<q)
    {
        lat1=j+W_sablon-q;
        lat2=i+H_sablon-p;
    }

    else
        if( j>q)
    {
        lat1=q+W_sablon-j;
        lat2=i+H_sablon-p;
    }}

        if(i>p )
           {if( j>q)
    {
        lat1=q+W_sablon-j;
        lat2=p+H_sablon-i;
    }
    else

           if( j<q)
    {

        lat1=j+W_sablon-q;
        lat2=p+H_sablon-i;
    }}


    A_intersectie=lat1*lat2;
    return A_intersectie;
  }

    float suprapunere(unsigned int i,unsigned int j, unsigned int p, unsigned int q, unsigned int W_sablon, unsigned int H_sablon)
{


    float s;
    unsigned int A_reuniune, A_intersectie;
    A_intersectie=arie_intersectie(i,j,p,q,W_sablon,H_sablon);
    if(A_intersectie==0)
        return 0;

    A_reuniune=(W_sablon*H_sablon*2)-A_intersectie;
    s=(float)A_intersectie/A_reuniune;
    return s;

}

    void eliminare_non_maxime(detectie **D, unsigned int *k, unsigned int W_sablon, unsigned int H_sablon)
{

    unsigned int i,j,s;
    detectie * aux;
    aux=NULL;
     for(i=0;i<*k-1;i++)
     {
         for(j=i+1;j<*k;j++)
            {
             if(suprapunere((*D)[i].x,(*D)[i].y,(*D)[j].x,(*D)[j].y,W_sablon,H_sablon)>0.2)
                {
                    for(s=j;s<*k;s++)
                        (*D)[s]=(*D)[s+1];
                    (*k)--;
                    j--;
                    aux=(detectie *)realloc((*D),(*k)*sizeof(detectie));
                    if(aux==NULL)
                    {
                        printf("Nu s-a putut realoca memorie pentru vectorul D.");
                        return ;
                    }
                    else
                    {
                        (*D)=aux;
                    }
                }

     }
     }




}

    void coloreaza (detectie *D, unsigned int k, pixel **M,unsigned int W_sablon,unsigned int H_sablon)
{
     unsigned int i;
   for(i=0;i<k;i++)
     colorare(M,D[i].x,D[i].y,D[i].culoare,W_sablon,H_sablon);



}
    void eliberare_memorie (detectie **D, pixel ***M,pixel ***A, pixel ***S, pixel ***P, unsigned int W_imagine, unsigned int H_imagine,
                            unsigned int W_sablon, unsigned int H_sablon)
{
    free(*D);
   unsigned int i;
   for(i=0;i<H_imagine;i++)
   {
       free((*M)[i]);
       free((*P)[i]);
       free((*A)[i]);
   }
   for(i=0;i<H_sablon;i++)
    free((*S)[i]);
   free(*S);
   free(*M);
   free(*P);
   free(*A);
   *M=NULL;
   *P=NULL;
   *D=NULL;
   *S=NULL;
   *A=NULL;
}

int main()
{  char nume_fisier_sursa[101];
   char nume_imagine_grayscale[]="testgrayscale.bmp";
   char nume_cifra0[101];
   char nume_cifra1[101];
   char nume_cifra2[101];
   char nume_cifra3[101];
   char nume_cifra4[101];
   char nume_cifra5[101];
   char nume_cifra6[101];
   char nume_cifra7[101];
   char nume_cifra8[101];
   char nume_cifra9[101];
   char nume_cifra0grayscale[]="cifra0grayscale.bmp";
   char nume_cifra1grayscale[]="cifra1grayscale.bmp";
   char nume_cifra2grayscale[]="cifra2grayscale.bmp";
   char nume_cifra3grayscale[]="cifra3grayscale.bmp";
   char nume_cifra4grayscale[]="cifra4grayscale.bmp";
   char nume_cifra5grayscale[]="cifra5grayscale.bmp";
   char nume_cifra6grayscale[]="cifra6grayscale.bmp";
   char nume_cifra7grayscale[]="cifra7grayscale.bmp";
   char nume_cifra8grayscale[]="cifra8grayscale.bmp";
   char nume_cifra9grayscale[]="cifra9grayscale.bmp";
   char nume_fisier_text[]="fisier.txt";
   char nume_destinatie[]="coloraretotala.bmp";
   char nume_colorare[]="nonmaxime.bmp";

   FILE *fin;
   fin=fopen(nume_fisier_text,"r");
   if(fin==NULL)
    printf("Eroare la deschiderea fisierului text");
   printf("Numele imaginii : ");

   fgets(nume_fisier_sursa,101,fin);
   nume_fisier_sursa[strlen(nume_fisier_sursa)-1]='\0';
   fputs(nume_fisier_sursa,stdout);
   printf("\n");

   printf("Numele celor 10 sabloane :\n");
   fgets(nume_cifra0,101,fin);
   nume_cifra0[strlen(nume_cifra0)-1]='\0';
   fputs(nume_cifra0,stdout);
   printf("\n");

    fgets(nume_cifra1,101,fin);
    nume_cifra1[strlen(nume_cifra1)-1]='\0';
    fputs(nume_cifra1,stdout);
    printf("\n");

    fgets(nume_cifra2,101,fin);
    nume_cifra2[strlen(nume_cifra2)-1]='\0';
    fputs(nume_cifra2,stdout);
    printf("\n");

    fgets(nume_cifra3,101,fin);
    nume_cifra3[strlen(nume_cifra3)-1]='\0';
    fputs(nume_cifra3,stdout);
    printf("\n");

    fgets(nume_cifra4,101,fin);
    nume_cifra4[strlen(nume_cifra4)-1]='\0';
    fputs(nume_cifra4,stdout);
    printf("\n");

    fgets(nume_cifra5,101,fin);
    nume_cifra5[strlen(nume_cifra5)-1]='\0';
    fputs(nume_cifra5,stdout);
    printf("\n");

    fgets(nume_cifra6,101,fin);
    nume_cifra6[strlen(nume_cifra6)-1]='\0';
    fputs(nume_cifra6,stdout);
    printf("\n");

    fgets(nume_cifra7,101,fin);
    nume_cifra7[strlen(nume_cifra7)-1]='\0';
    fputs(nume_cifra7,stdout);
    printf("\n");

    fgets(nume_cifra8,101,fin);
    nume_cifra8[strlen(nume_cifra8)-1]='\0';
    fputs(nume_cifra8,stdout);
    printf("\n");

    fgets(nume_cifra9,101,fin);
    nume_cifra9[strlen(nume_cifra9)-1]='\0';
    fputs(nume_cifra9,stdout);
    printf("\n");


    unsigned int W_imagine,H_imagine;
    W_imagine=latime_imagine(nume_fisier_sursa);
    H_imagine=inaltime_imagine(nume_fisier_sursa);
    if(W_imagine==0||H_imagine==0)
    {
        printf("Eroare la deschiderea fisierului pentru calculul inaltimii sau latimii.");

    }
        unsigned int W_sablon,H_sablon;
    W_sablon=latime_imagine(nume_cifra0);
    H_sablon=inaltime_imagine(nume_cifra0);
    if(W_sablon==0||H_sablon==0)
   {
       printf("Eroare la deschiderea fisierului pentru calculul inaltimii sau latimii sablonului.");

   }
       /* transformam imaginea sursa intr-o imaginea grayscale */
    grayscale(nume_fisier_sursa,nume_imagine_grayscale,W_imagine,H_imagine);
    grayscale(nume_cifra0,nume_cifra0grayscale,W_sablon,H_sablon);
    grayscale(nume_cifra1,nume_cifra1grayscale,W_sablon,H_sablon);
    grayscale(nume_cifra2,nume_cifra2grayscale,W_sablon,H_sablon);
    grayscale(nume_cifra3,nume_cifra3grayscale,W_sablon,H_sablon);
    grayscale(nume_cifra4,nume_cifra4grayscale,W_sablon,H_sablon);
    grayscale(nume_cifra5,nume_cifra5grayscale,W_sablon,H_sablon);
    grayscale(nume_cifra6,nume_cifra6grayscale,W_sablon,H_sablon);
    grayscale(nume_cifra7,nume_cifra7grayscale,W_sablon,H_sablon);
    grayscale(nume_cifra8,nume_cifra8grayscale,W_sablon,H_sablon);
    grayscale(nume_cifra9,nume_cifra9grayscale,W_sablon,H_sablon);

    pixel **M,**P,**S,**A;/* salvam intr-o matrice pixelii imaginilor grayscale */
    transform_matrice(nume_imagine_grayscale,&M,H_imagine,W_imagine);
    transform_matrice(nume_fisier_sursa,&P,H_imagine,W_imagine);
    transform_matrice(nume_cifra0grayscale,&S,H_sablon,W_sablon);
    transform_matrice(nume_fisier_sursa,&A,H_imagine,W_imagine);

   /*rularea operatia de template matching pentru toate sabloanelor */
   detectie *D;
   D=NULL;
   unsigned int k=0,op=0;
   pixel C;
   float prag_furnizat=0.50;
           switch(op)
           {
           case 0:
                  C.B=C.G=0;
                  C.R=255;
                  template_matching(M,W_imagine,H_imagine,S,W_sablon,H_sablon,prag_furnizat,&D,&k,C,nume_cifra0grayscale);

           case 1:
                  C.B=0;
                  C.G=255;
                  C.R=255;
                  template_matching(M,W_imagine,H_imagine,S,W_sablon,H_sablon,prag_furnizat,&D,&k,C,nume_cifra1grayscale);


           case 2:
                  C.B=0;
                  C.G=255;
                  C.R=0;
                  template_matching(M,W_imagine,H_imagine,S,W_sablon,H_sablon,prag_furnizat,&D,&k,C,nume_cifra2grayscale);

           case 3:
                  C.B=255;
                  C.G=255;
                  C.R=0;
                  template_matching(M,W_imagine,H_imagine,S,W_sablon,H_sablon,prag_furnizat,&D,&k,C,nume_cifra3grayscale);

           case 4:
                  C.B=255;
                  C.G=0;
                  C.R=255;
                  template_matching(M,W_imagine,H_imagine,S,W_sablon,H_sablon,prag_furnizat,&D,&k,C,nume_cifra4grayscale);

           case 5:
                  C.B=255;
                  C.G=0;
                  C.R=0;
                  template_matching(M,W_imagine,H_imagine,S,W_sablon,H_sablon,prag_furnizat,&D,&k,C,nume_cifra5grayscale);

           case 6:
                  C.B=192;
                  C.G=192;
                  C.R=192;
                  template_matching(M,W_imagine,H_imagine,S,W_sablon,H_sablon,prag_furnizat,&D,&k,C,nume_cifra6grayscale);

           case 7:
                  C.B=0;
                  C.G=140;
                  C.R=255;
                  template_matching(M,W_imagine,H_imagine,S,W_sablon,H_sablon,prag_furnizat,&D,&k,C,nume_cifra7grayscale);

           case 8:
                  C.B=128;
                  C.G=0;
                  C.R=128;
                  template_matching(M,W_imagine,H_imagine,S,W_sablon,H_sablon,prag_furnizat,&D,&k,C,nume_cifra8grayscale);

           case 9:
                  C.B=0;
                  C.G=0;
                  C.R=128;
                  template_matching(M,W_imagine,H_imagine,S,W_sablon,H_sablon,prag_furnizat,&D,&k,C,nume_cifra9grayscale);

           }

    /* coloram imaginea initiala dupa ce am rulat toate cele 10 sabloane */
    coloreaza(D,k,A,W_sablon,H_sablon);
    deliniarizare_matrice(nume_fisier_sursa,nume_destinatie,A,H_imagine,W_imagine);

    /* sortam descrescator dupa pragul de corelatie */
    qsort(D,k,sizeof(detectie),cmpdescrescator);
    /* eliminam non-maximele */
    eliminare_non_maxime(&D,&k,W_sablon,H_sablon);
    /* coloram dupa eliminarea non-maximelor */
    coloreaza(D,k,P,W_sablon,H_sablon);
    deliniarizare_matrice(nume_fisier_sursa,nume_colorare,P,H_imagine,W_imagine);

    eliberare_memorie(&D,&M,&A,&S,&P,W_imagine,H_imagine,W_sablon,H_sablon);

    int s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,st,verificare_stergere;
    /* stergem fisierele grayscale de pe hard disk */
    s0=remove(nume_cifra0grayscale);
    s1=remove(nume_cifra1grayscale);
    s2=remove(nume_cifra2grayscale);
    s3=remove(nume_cifra3grayscale);
    s4=remove(nume_cifra4grayscale);
    s5=remove(nume_cifra5grayscale);
    s6=remove(nume_cifra6grayscale);
    s7=remove(nume_cifra7grayscale);
    s8=remove(nume_cifra8grayscale);
    s9=remove(nume_cifra9grayscale);
    st=remove(nume_imagine_grayscale);
    verificare_stergere=s0+s1+s2+s3+s4+s5+s6+s7+s8+s9+st;
    if(verificare_stergere==0)
        printf("Fisierele grayscale au fost sterse cu succes.");
    else
        printf("Eroare la stergerea unui fisier sau a mai multor fisiere grayscale.");


    return 0;
}
