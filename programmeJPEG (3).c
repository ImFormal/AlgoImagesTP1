

// HOUCHI_BRANIS  et THUBERT MAXIME

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define N 8

double Q[8][8] = {
    {16, 11, 10, 16, 24, 40, 51, 61},     {12, 12, 14, 19, 26, 58, 60, 55},
    {14, 13, 16, 24, 40, 57, 69, 56},     {14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 69, 103, 77},    {24, 35, 55, 64, 81, 104, 113, 92},
    {49, 64, 78, 87, 103, 121, 120, 101}, {72, 92, 95, 98, 112, 100, 103, 99}};

struct RGB_t {

  unsigned char R, G, B;
};
typedef struct RGB_t RGB;

struct picture_t {

  unsigned char type;
  unsigned int hauteur, largeur;
  unsigned char value_max;

  union {

    unsigned char **pixels; // pour PGM
    RGB **pixels_rgb;       // pour PPM
  };
};
typedef struct picture_t picture;

picture *get_picture(char *file_name) {

  FILE *f = fopen(file_name, "r");
  picture *image = (picture *)malloc(sizeof(picture));

  if (f == NULL)
    printf("Erreur : Le fichier n'a pas été ouvert\n");

  fseek(f, 0, SEEK_SET); // Se place au début du fichier texte

  char c1 =
      fgetc(f); // c1 est le premier caractère du fichier donc logiquement 'P'
  char c2 = fgetc(f); // c2 est le deuxième caractère du fichier donc
                      // logiquement entre un chiffre entre 1-6

  if (c1 != 'P')
    printf("Erreur de syntaxe : Le mode P doit être renseigné au début\n");

  if (c2 == '1' || c2 == '2' || c2 == '3' || c2 == '4' || c2 == '5' ||
      c2 == '6') {

    if (c2 == '1' || c2 == '4')
      printf("Erreur : Le fichier ne peut pas être en PBM\n");

    if (c2 == '2' || c2 == '5')
      image->type = '0';

    if (c2 == '3' || c2 == '6')
      image->type = '1';

  }

  else
    printf("Erreur : Veuillez choisir un mode existant\n");

  if (c2 != '1' && c2 != '4') {

    fseek(f, 1, SEEK_CUR); // Se place sur la ligne du premier commentaire

    char c = fgetc(f);

    while (c == '#') { // Tant qu'on a des comms on les sautes
      while (c != '\n') {
        c = fgetc(f);
      }
      c = fgetc(f);
    }

    fseek(f, -2, SEEK_CUR); // Se place un caractère avant afin de récupérer
                            // correctement la largeur

    fscanf(f, "%d", &(image->largeur));
    fscanf(f, "%d", &(image->hauteur));

    int value_max;
    fscanf(f, "%d", &value_max); // Prendre la valeur max qui est 255;
    image->value_max = (unsigned char)value_max;

    if (image->type == '0') {

      image->pixels = (unsigned char**)malloc(sizeof(unsigned char *) * image->hauteur);
      for (unsigned int i = 0; i < image->hauteur; i++)
        image->pixels[i] = (unsigned char*)malloc(sizeof(unsigned char) * image->largeur);

      if (c2 == '2') {

        for (unsigned int i = 0; i < image->hauteur; i++) {
          for (unsigned int j = 0; j < image->largeur; j++)
            fscanf(f, "%hhu", &image->pixels[i][j]);
        }

      }

      else {

        for (unsigned int i = 0; i < image->hauteur; i++)
          fread(image->pixels[i], 1, image->largeur, f);
      }

    }

    else {

      image->pixels_rgb = (RGB **)malloc(sizeof(RGB *) * image->hauteur);
      for (unsigned int i = 0; i < image->hauteur; i++)
        image->pixels_rgb[i] = (RGB *)malloc(sizeof(RGB) * image->largeur);

      if (c2 == '3') {

        for (unsigned int i = 0; i < image->hauteur; i++) {
          for (unsigned int j = 0; j < image->largeur; j++)
            fscanf(f, "%hhu %hhu %hhu", &image->pixels_rgb[i][j].R,
                   &image->pixels_rgb[i][j].G, &image->pixels_rgb[i][j].B);
        }

      }

      else {

        for (unsigned int i = 0; i < image->hauteur; i++)
          fread(&image->pixels_rgb[i], 1, sizeof(RGB), f);
      }
    }

    fclose(f);
  }
  return image;
}

void free_picture(picture *image) {

  if (image->type == '0') {

    for (unsigned int i = 0; i < image->hauteur; i++)
      free(image->pixels[i]);
    free(image->pixels);

  }

  else {

    for (unsigned int i = 0; i < image->hauteur; i++)
      free(image->pixels_rgb[i]);
    free(image->pixels_rgb);
  }

  free(image);
}

double get_Y_component_from_RGB(RGB pixel) {

  // Y = 0.299*R + 0.587*G + 0.114*B
  return 0.299 * pixel.R + 0.587 * pixel.G + 0.114 * pixel.B;
}

picture *ppm_to_pgm(picture *image) {

  if (image->type != '1') {
    printf("Erreur : L'image n'est pas en PPM\n");
    return NULL;
  }

  picture *image_pgm = (picture *)malloc(sizeof(picture));

  if (image_pgm == NULL) {

    printf("Erreur : Problème allocation de mémoire");
    return NULL;
  }

  image_pgm->type = '0';
  image_pgm->largeur = image->largeur;
  image_pgm->hauteur = image->hauteur;
  image_pgm->value_max = image->value_max;
  image_pgm->pixels = (unsigned char**)malloc(sizeof(unsigned char *) * image_pgm->hauteur);

  if (image_pgm->pixels == NULL) {

    printf("Erreur : Problème d'allocation de mémoire");
    free(image_pgm);
    return NULL;
  }

  for (unsigned int i = 0; i < image_pgm->hauteur; i++) {

    image_pgm->pixels[i] = (unsigned char*)malloc(sizeof(unsigned char) * image_pgm->largeur);

    if (image_pgm->pixels[i] == NULL) {

      printf("Erreur : Problème d'allocation");
      return NULL;
    }

    for (unsigned int j = 0; j < image_pgm->largeur; j++) {
      image_pgm->pixels[i][j] = (unsigned char)round(
          get_Y_component_from_RGB(image->pixels_rgb[i][j]));
    }
  }

  return image_pgm;
}

void write_picture(picture *image, char *file_name, int binary) {

  FILE *f;
  int i, j;

  f = fopen(file_name, "wb");
  if (f == NULL) {
    printf("Impossible d'ouvrir le fichier %s\n", file_name);
    return;
  }

  if (image->type == '1') {

    if (binary == 1) {
      fprintf(f, "P6\n%d %d\n%d\n", image->largeur, image->hauteur,
              image->value_max);
      for (i = 0; i < (int)image->hauteur; i++) {
        for (j = 0; j < (int)image->largeur; j++) {
          fwrite(&image->pixels_rgb[i][j].R, sizeof(image->pixels_rgb[i][j].R),
                 1, f);
          fwrite(&image->pixels_rgb[i][j].G, sizeof(image->pixels_rgb[i][j].G),
                 1, f);
          fwrite(&image->pixels_rgb[i][j].B, sizeof(image->pixels_rgb[i][j].B),
                 1, f);
        }
      }
    } else {
      fprintf(f, "P3\n%d %d\n%d\n", image->largeur, image->hauteur,
              image->value_max);
      for (i = 0; i < (int)image->hauteur; i++) {
        for (j = 0; j < (int)image->largeur; j++) {
          fprintf(f, "%d %d %d\n", image->pixels_rgb[i][j].R,
                  image->pixels_rgb[i][j].G, image->pixels_rgb[i][j].B);
        }
      }
    }
  }

  else {

    if (binary == 1) {
      fprintf(f, "P5\n%d %d\n%d\n", image->largeur, image->hauteur,
              image->value_max);
      for (i = 0; i < (int)image->hauteur; i++) {
        for (j = 0; j < (int)image->largeur; j++) {
          fwrite(&image->pixels[i][j], sizeof(image->pixels[i][j]), 1, f);
        }
      }
    } else {
      fprintf(f, "P2\n%d %d\n%d\n", image->largeur, image->hauteur,
              image->value_max);
      for (i = 0; i < (int)image->hauteur; i++) {
        for (j = 0; j < (int)image->largeur; j++) {
          fprintf(f, "%d  ", image->pixels[i][j]);
        }
        fprintf(f, "\n");
      }
    }
  }

  fclose(f);
}

void print_image(picture *image) {

  if (image->type == '1') {

    int i = 0;
    int j;
    while (i != (int)image->hauteur) {
      j = 0;
      while (j != (int)image->largeur) {

        printf("%hhu %hhu %hhu\n", image->pixels_rgb[i][j].R,
               image->pixels_rgb[i][j].G, image->pixels_rgb[i][j].B);
        j++;
      }
      i++;
    }

  }

  else {

    int i = 0;
    int j;
    while (i != (int)image->hauteur) {
      j = 0;
      while (j != (int)image->largeur) {

//         printf("%hhu\t", image->pixels[i][j]);
        j++;
      }
      i++;
//       printf("\n");
    }
  }
}

void extract_bloc(picture *image, double bloc[N][N], int i, int j) {
  int z = 0, k = 0;
  for (int l = i; l < i + 8; l++)
  {
    for (int r = j; r < j + 8; r++)
    {
      bloc[z][k] = image->pixels[l][r];
      k++;
    }
    k = 0;
    z++;
  }
}

void DCT(double bloc[N][N]) {
  double temp[N][N];
  double sum = 0;
  float firstconst = 0, secondconst = 0;

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {

          if (i == 0)
            firstconst = 1 / sqrt(2);
          else
            firstconst = 1;

          if (j == 0)
            secondconst = 1 / sqrt(2);
          else
            secondconst = 1;

          sum += firstconst * secondconst * bloc[x][y] * cos((2 * x + 1) * i * M_PI / (2. * N)) *
                 cos((2 * y + 1) * j * M_PI / (2. * N));
        }
      }
      temp[i][j] = 2. * sum / N;
      sum = 0;
    }
  }

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++){
      bloc[i][j] = temp[i][j];
//       printf("%.2lf ", bloc[i][j]);
    }
  }
  
}

void quantify(double bloc[N][N]) {

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++){
      bloc[i][j] /= Q[j][i];
//       printf("%.2lf ", bloc[i][j]);
    }
  }
  
}

void zigzag_extraction(double bloc[N][N], int zigzag[N * N]) {

  int i = 0, j = 0, k = 0, croiss = 0;

  while (i < N && j < N) {

    zigzag[k] = round(bloc[i][j]);
    k++;
    if (i == 0 || i == 7) {
      if (j == 7) {
        j = j - 1;
        i = i + 1;
      }
      j = j + 1;
      zigzag[k] = round(bloc[i][j]);
      k++;
    } else {
      if (j == 0 || j == 7) {
        if (i == 7) {
          i = i - 1;
          j = j + 1;
        }
        i = i + 1;
        zigzag[k] = round(bloc[i][j]);
        k++;
      }
    }

    if (i == 0 || j == 7) {
      croiss = 0;
    }
    if (j == 0 || i == 7) {
      croiss = 1;
    }

    if (croiss == 1) {
      i = i - 1;
      j = j + 1;
    } else {
      i = i + 1;
      j = j - 1;
    }
  }

  int q = 0;
  for (int h = 0; h < N; h++) {
    for (int d = 0; d < N; d++) {
      q++;
    }
  }
  int u = 0;
  for(int i =0; i<N; i++){
    for(int j =0; j<N; j++){
//       printf("%d ", zigzag[u]);
      u++;
    }
  }

}

void compress_RLE(FILE *f, int zigzag[N * N]) {
  int i = 0, n = 0;
  while (i < N * N) {

    if (zigzag[i] != 0)
      fprintf(f, "%d\n", zigzag[i]);
    else
      n++;

    i++;
  }
  if (n >= 2)
    fprintf(f, "@%d\n", n);

  else
    fprintf(f, "0\n");
}

void jpeg_compression(picture *image, char *file_name) {

  FILE *f = fopen(file_name, "w");
  
  if (f == NULL)
    printf("Erreur : Le fichier n'a pas été ouvert\n");
  
  double bloc[N][N];
  int zigzag[N * N];

  fprintf(f, "JPEG\n%d %d\n", image->largeur, image->hauteur);
  for (int i = 0; i < (int) image->hauteur; i += N) { // i += N pour sauter un bloc de 8
    for (int j = 0; j < (int) image->largeur; j += N) { // j += N pour sauter un bloc de 8
      extract_bloc(image, bloc, i, j);
      DCT(bloc);
      quantify(bloc);
      zigzag_extraction(bloc, zigzag);
      compress_RLE(f, zigzag);
    }
  }

  fclose(f);
}

unsigned int file_size(char *file_name) {

  FILE *f = fopen(file_name, "r");

  if (f == NULL)
    printf("Erreur : Le fichier n'a pas été ouvert\n");

     unsigned int size = 0;

  while (fgetc(f) != EOF)
    size++;

  fclose(f);

  return size;
}

int main(int argv, char **argc) {

  if (argv != 2) {
    printf("Utilise ./main images/'nom_image.ppm'");
    return 1;
  }

  picture *image = get_picture((argc[1]));
  picture *image_PGM = ppm_to_pgm(image);
  
  int l = 0;
  for (int i = 0; argc[1][i]; i++)
    l++;
  
  argc[1][l-2] = 'g';
  char *file_name_pgm = argc[1];
  
  write_picture(image_PGM, file_name_pgm, 0);

  printf("Le fichier %s pèse %d octets\n", file_name_pgm, file_size(file_name_pgm));
  float taillepgm=(float)file_size(file_name_pgm);
  argc[1][l + 1] = '\0';
  argc[1][l] = 'g';
  argc[1][l - 1] = 'e';
  argc[1][l - 2] = 'p';
  argc[1][l - 3] = 'j';
  char *file_name_jpeg = argc[1];
  jpeg_compression(image, file_name_jpeg);
  
  printf("Le fichier %s pèse %d octets\n", file_name_jpeg, file_size(file_name_jpeg));
   float taillejpeg  = (float)file_size(file_name_jpeg);
   float res=taillejpeg/taillepgm;
  printf("Le taux de compression de ce fichier est de %f \n", res);

  

// double bloc[N][N];
//   int zigzag[N * N];
//
//   extract_bloc(image_PGM, bloc, 0, 31);
//   printf("\n");
//   DCT(bloc);
//   printf("\n");
//   quantify(bloc);
//   printf("\n");
//   zigzag_extraction(bloc, zigzag);
//   printf("\n");
//   FILE *f = fopen("test", "w");
//   compress_RLE(f, zigzag);

  free_picture(image_PGM);
  free_picture(image);

  return 0;
}
