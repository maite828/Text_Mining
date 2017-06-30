// Prueba de integración del Detector Textual de Idioma
// DTT 24-1-00

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>

#include "../../libs/hdrs/detector_id.h"




static char UTF8_2_ISO_8859_1_len[] =
{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* erroneous */
	2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 5, 6
};

static char UTF8_2_ISO_8859_1_mask[] = {0x3F, 0x7F, 0x1F, 0x0F, 0x07,
0x03, 0x01};



int utf8_2_isolatin1(const char *cad_utf, char **cad_iso)
{
	int res = 0;
	char *p_iso;
	char *p_utf;

	char byte_1_orig ;		
	char byte_2_orig ;			
	char byte_3_orig ;


	int max_length_iso;


	if (cad_utf == NULL)
	{
		fprintf(stderr, "Null input string to utf8_2_isolatin1\n");
		*cad_iso = NULL;
		return(-1);
	}

	max_length_iso = strlen(cad_utf)+3;

	if ((*cad_iso = (char *)malloc(max_length_iso)) == NULL)
	{
		fprintf(stderr, "Not enough memory for %d bytes in utf8_2_isolatin1\n", max_length_iso);
		return (-1);
	}
	memset(*cad_iso, 0, max_length_iso);

	p_utf = (char *) cad_utf;
	p_iso = *cad_iso;

	while (*p_utf != '\0')
	{
		int           len = UTF8_2_ISO_8859_1_len[(*p_utf >> 2) & 0x3F];
		unsigned long u   = *p_utf & UTF8_2_ISO_8859_1_mask[len];


		if (len == 0)
			len = 5;

		if (len == 3)
		{
			byte_1_orig = *p_utf;		
			byte_2_orig = *(p_utf+1);			
			byte_3_orig = *(p_utf+2);
		}


		if ((len == 3) && (byte_1_orig == '\342') && (byte_2_orig == '\200') && (byte_3_orig == '\234'))
		{
			*p_iso = '\223';
			p_iso++;
			p_utf+=3;
		}
		else if ((len == 3) && (byte_1_orig == '\342') && (byte_2_orig == '\200') && (byte_3_orig == '\235'))
		{
			*p_iso = '\224';
			p_iso++;
			p_utf+=3;
		}
		else if ((len == 3) && (byte_1_orig == '\342') && (byte_2_orig == '\200') && (byte_3_orig == '\231'))
		{
			*p_iso = '\222';
			p_iso++;
			p_utf+=3;
		}
		else if ((len == 3) && (byte_1_orig == '\342') && (byte_2_orig == '\202') && (byte_3_orig == '\254'))
		{
			*p_iso = '\244';
			p_iso++;
			p_utf+=3;
		}
		else
		{
			



			for (++p_utf; --len > 0 && (*p_utf != '\0'); ++p_utf)
			{

				if ((*p_utf & 0xC0) != 0x80)
					break;


				u = (u << 6) | (*p_utf & 0x3F);
			}
			

			if (p_iso != 0 && max_length_iso != 0)
			{

				if (res >= (max_length_iso-1))
				{
					*p_iso = '\0';
					fprintf(stderr, "Returning early because end of buffer(%d) in utf8_2_isolatin1\n", max_length_iso);
					return 0;
				}

				*p_iso++ = (u <= 0xFF ? (char)u : '_');
			}
		}
		++res;
	}

	return 0;
} 



int isolatin1_2_utf8(const char *cad_iso, char **cad_utf)
{
	char *p_iso;
	char *p_utf;

	int max_length_utf = 3*strlen(cad_iso)+1;
	int res = 0;


	if (cad_iso == NULL)
	{
		fprintf(stderr, "Null input string to isolatin1_2_utf8\n");
		*cad_utf = NULL;
		return (-1);
	}

	if ((*cad_utf = (char *)malloc(max_length_utf)) == NULL)
	{
		return (-1);
	}
	memset(*cad_utf, 0, max_length_utf);

	p_iso = (char *)cad_iso;
	p_utf = *cad_utf;
	while (*p_iso != '\0')
	{

		if ((*p_iso & 0x80) == 0) 
		{
			if (p_utf != 0 && max_length_utf != 0)
			{
				if (res >= (max_length_utf-1))
				{
					*p_utf = '\0';
					return 0;
				}

				*p_utf++ = *p_iso;
				p_iso++;
			}
			++res;
		}
		else
		{
			if (p_utf != 0 && max_length_utf != 0)
			{
				if (res+1 >= (max_length_utf-1))
				{
					*p_utf = '\0';
					return 0;
				}
				if (*p_iso == '\223')
				{
					*p_iso = '\253';
				}
				else if (*p_iso == '\224')
				{
					*p_iso = '\273';
				}
				else if (*p_iso == '\222')
				{
					*p_iso = '\'';
				}

				if (*p_iso == '\244')
				{
					*p_utf++ = '\342';
					*p_utf++ = '\202';
					*p_utf++ = '\254';
					res += 3;


				}
				else
				{

					*p_utf++ = (0xC0 | (0x03 & (*p_iso >> 6)));
					*p_utf++ = (0x80 | (0x3F & *p_iso));

					res += 2;
				}
				p_iso++;
			}
		}
	}

	return 0;
} 




int main(int argc, char *argv[])
{
  int i;
  Detector_id::Idioma_detid idioma;
  int conf = 0;

  FILE *ftexto;
  FILE *foutput;

  // Objeto Detector Textual de Idioma
  Detector_id detid;

  Detector_id::Error error;

// Número de idiomas a tratar
#define N_IDI 9

// Codódigos de los idiomas a cargar
Detector_id::Idioma_detid cod_idi[N_IDI]  = { Detector_id::CASTELLANO,
                                        Detector_id::CATALAN,
                                        Detector_id::GALLEGO,
                                        Detector_id::EUSKERA,
                                        Detector_id::INGLES,
                                        Detector_id::FRANCES,
                                        Detector_id::ALEMAN,
                                        Detector_id::BRASILENO,
					Detector_id::ITALIANO};

// Ficheros donde están los modelos de idioma a cargar
char mod_idi[N_IDI][100] = { "mod_cas",
                             "mod_cat",
                             "mod_gal",
                             "mod_eus",
                             "mod_ing",
                             "mod_fra",
                             "mod_ale",
                             "mod_bra",
                             "mod_ita"};

char mod_id_path[100];
char mod_id_file[100];

  extern char *optarg;
  int option_char;

  bool parse_dump = false;
  char separator[81];

  char *input_file;
  char *output_file;

  strcpy(mod_id_path, "../modid_bin/");
  strcpy(separator, "|");
  while ((option_char = getopt(argc, argv, "m:s:ph?")) != EOF)
  {
	  switch (option_char)
	  {
		case 'm':
			strcpy(mod_id_path, optarg);
			break;
		case 's':
			strcpy(separator, optarg);
			break;
		case 'p':
			parse_dump = true;
			break;
		case 'h':
		case '?':
			fprintf(stderr, "Usage: %s [-m path_to_bin_mods] [-s separator (def:|)] [-p parse dump text] input_file(or stdin) output_file(or stdout)\n", argv[0]);
			exit(-1);
	  }
  }

  if (argc < (optind + 2)) {
    fprintf(stderr, "Missing arguments for input and output filenames, argc:%d, optind:%d\n", argc, optind);
    fprintf(stderr, "Usage: %s [-m path_to_bin_mods] [-s separator (def:|)] [-p parse dump text] input_file(or stdin) output_file(or stdout)\n", argv[0]);
    exit(-1);
  }

  if (argc > (optind + 2)) {
    fprintf(stderr, "Too many arguments, argc:%d, optind:%d\n", argc, optind);
    fprintf(stderr, "Usage: %s [-m path_to_bin_mods] [-s separator (def:|)] [-p parse dump text] input_file(or stdin) output_file(or stdout)\n", argv[0]);
    exit(-1);
  }

  input_file = argv[optind];
  output_file = argv[optind+1];

  // Carga los modelos de idioma
  for(i=0;i<N_IDI;i++)
  {
    sprintf(mod_id_file, "%s/%s", mod_id_path, mod_idi[i]);
    if((error = detid.Carga(cod_idi[i],mod_id_file))!=Detector_id::OK)
    {
      fprintf(stderr,"ERROR:%d al cargar '%s'\n", (int)error, mod_id_file);
      exit(-1);
    }
  }

  if (strcmp(input_file, "stdin") != 0)
  {
  if((ftexto = fopen(input_file, "r")) == NULL)
  {
	fprintf(stderr, "Cannot open input file:'%s' errno:%d\n", input_file, errno);
       	exit(-1);
  }
  }
  else
  {
	  ftexto = stdin;
  }

  if (strcmp(output_file, "stdout") != 0)
  {
  if((foutput = fopen(output_file, "w")) == NULL)
  {
	fprintf(stderr, "Cannot open output file:'%s' errno:%d\n", output_file, errno);
       	exit(-1);
  }
  }
  else
  {
	  foutput = stdout;
  }

#define MAX_LEN_LINE 64*1024
  char line_utf8[MAX_LEN_LINE + 1];
  char *line_iso;

  bool detect_line = true;
  if (parse_dump == true)
  {
    detect_line = false;
  }
  while (fgets(line_utf8, MAX_LEN_LINE, ftexto) != NULL)
  {
	  if (parse_dump && (strcmp(line_utf8, "ParseText::\n") == 0))
	  {
		  detect_line = true;
		  continue;
	  }
	  if (detect_line)
	  {
		  if (line_utf8[strlen(line_utf8)-1] == '\n') 
		  {
			  line_utf8[strlen(line_utf8)-1] = '\0';
		  }
		  if (line_utf8[strlen(line_utf8)-1] == '\r') 
		  {
			  line_utf8[strlen(line_utf8)-1] = '\0';
		  }
	int error_conversion = utf8_2_isolatin1(line_utf8, &line_iso);
	if (error_conversion != 0)
	{
		fprintf(stderr, "Error converting line to iso:'%s'\n", line_utf8);
		continue;
	}
	idioma = detid.Detecta(line_iso, &conf);

  
  switch(idioma)
  {
    case Detector_id::CASTELLANO: fprintf(foutput, "%s%sESP%s%d\n",line_utf8,separator, separator, conf);
				  fprintf(stderr, "%s\n", line_iso);
                                   break;
    case Detector_id::CATALAN: fprintf(foutput, "%s%sCAT%s%d\n",line_utf8,separator, separator, conf);
                                   break;
    case Detector_id::GALLEGO: fprintf(foutput, "%s%sGAL%s%d\n",line_utf8,separator, separator, conf);
                                   break;
    case Detector_id::EUSKERA: fprintf(foutput, "%s%sEUS%s%d\n",line_utf8,separator, separator, conf);
                                   break;
    case Detector_id::FRANCES: fprintf(foutput, "%s%sFRA%s%d\n",line_utf8,separator, separator, conf);
                                   break;
    case Detector_id::ALEMAN: fprintf(foutput, "%s%sDEU%s%d\n",line_utf8,separator, separator, conf);
                                   break;
    case Detector_id::INGLES: fprintf(foutput, "%s%sENG%s%d\n",line_utf8,separator, separator, conf);
                                   break;
    case Detector_id::BRASILENO: fprintf(foutput, "%s%sBRA%s%d\n",line_utf8,separator, separator, conf);
                                   break;
    case Detector_id::ITALIANO: fprintf(foutput, "%s%sITA%s%d\n",line_utf8,separator, separator, conf);
                                   break;
    default: fprintf(foutput, "%s%sERR%s0.0\n",line_utf8, separator, separator);
             break;
  }
  free(line_iso);
  if (parse_dump)
  {
	  detect_line = false;
  }
	  }
  }

  // Descarga los modelos de idioma.
  for(i=0;i<N_IDI;i++)
  {
    if(detid.Descarga(cod_idi[i])!=Detector_id::OK)
    {
      fprintf(stderr,"ERROR al descargar '%s'\n",mod_idi[i]);
      exit(-1);
    }
  }

  exit(0);

}


  
  

